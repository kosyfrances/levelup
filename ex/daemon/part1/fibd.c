/**
 * An example SystemV daemon. This was assembled from information contained
 * in The Linux Programming Interface, by Michael Kerrisk. See chapter 37
 * and section 55.6.
 */

#include <stdbool.h>    // bool, true, false macros
#include <stdlib.h>     // exit, EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>      // fprintf
#include <errno.h>      // errno
#include <string.h>     // strlen

#include <sys/types.h>  // pid_t, mode_t
#include <sys/stat.h>   // umask
#include <unistd.h>     // fork, unlink, sleep
#include <signal.h>     // sigaction, signal
#include <fcntl.h>      // open, fcntl, F_*, AT_*
#include <syslog.h>     // openlog, closelog, syslog, LOG_*

/**
 * Whether the daemon should continue running.
 */
bool running = false;

/**
 * Daemonize the current process.
 */
void daemonize() {
  // fork the parent process
  pid_t pid = fork();
  if (pid < 0) {
    fprintf(stderr, "failed to fork parent process\n");
    exit(EXIT_FAILURE);
  }
  else if (pid > 0) {
    // we are the parent process so we exit. the child process continues
    // running under the init process and independent of any shell the
    // parent was invoked in
    exit(EXIT_SUCCESS);
  }

  // start a new session and disconnect from parent's controlling terminal
  if (setsid() < 0) {
    syslog(LOG_ERR, "failed to create a new session");
    exit(EXIT_FAILURE);
  }

  // fork the parent process again so any terminal devices we open will not
  // become the controlling terminal
  pid = fork();
  if (pid < 0) {
    syslog(LOG_ERR, "failed to fork parent process for the second time");
    exit(EXIT_FAILURE);
  }
  else if (pid > 0) {
    // we are the parent process, so quit
    exit(EXIT_SUCCESS);
  }

  // clear the process umask so any files or directories we create will have
  // the requested permissions
  umask(0);

  // change the working directory to the root directory to avoid locking any
  // non-root filesystem thereby preventing it from being unmounted
  chdir("/");

  // close all open file descriptors to remove any references to our original
  // terminal and to avoid locking any non-root filesystems
  int maxfd = sysconf(_SC_OPEN_MAX);
  if (maxfd < 0) {
    // unknown limit, use a sane default
    maxfd = 8192;
  }

  for (int fd = 0; fd < maxfd; fd++) {
    close(fd);
  }

  // close the standard output stream and re-open it using /dev/null
  // note that because we closed all open file descriptors, the next time
  // we open a file the descriptor should start with 0 which is STDIN_FILENO
  close(STDIN_FILENO);

  int nullfd = open("/dev/null", O_RDWR);
  if (nullfd != STDIN_FILENO) {
    syslog(LOG_ERR, "failed to open /dev/null as stdin");
    exit(EXIT_FAILURE);
  }

  // duplicate the stdin file descriptor for stdout and stderr
  if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) {
    syslog(LOG_ERR, "failed to duplicate stdin stream for stdout");
    exit(EXIT_FAILURE);
  }

  if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) {
    syslog(LOG_ERR, "failed to duplicate stdin stream for stderr");
    exit(EXIT_FAILURE);
  }
}

/**
 * Create a lock file with the current process ID as it's contents.
 *
 * @param lock_file_path path to the lock file
 * @returns lock file descriptor
 */
int create_lock_file(const char* lock_file_path) {
  // open or create the lock file in rw mode and if creating it set the
  // read and write user permission flags
  int fd = open(lock_file_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    syslog(LOG_ERR, "failed to open lock file: %s", lock_file_path);
    exit(EXIT_FAILURE);
  }

  // set the close-on-exec file descriptor flag which will close the file
  // descriptor the next time exec() is called, useful for daemons that
  // reload themselves by re-execing
  int flags = fcntl(fd, F_GETFD);
  if (flags < 0) {
    syslog(LOG_ERR, "failed to get PID file flags: %s", lock_file_path);
    exit(EXIT_FAILURE);
  }

  flags |= FD_CLOEXEC;

  if (fcntl(fd, F_SETFD, flags) < 0) {
    syslog(LOG_ERR, "failed to set PID file flags: %s", lock_file_path);
    exit(EXIT_FAILURE);
  }

  // lock the entire contents of the lock file to prevent another process
  // from writing to it while we're running
  struct flock fl;
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;

  if (fcntl(fd, F_SETLK, &fl) < 0) {
    if (errno == EAGAIN || errno == EACCES) {
      // pid file is already locked, probably by another instance
      syslog(LOG_ERR, "PID file is already locked: %s", lock_file_path);
      exit(EXIT_FAILURE);
    }

    // unable to lock the pid file
    syslog(LOG_ERR, "unable to lock PID file: %s", lock_file_path);
    exit(EXIT_FAILURE);
  }

  // truncate the contents of the lock file
  if (ftruncate(fd, 0) < 0) {
    syslog(LOG_ERR, "unable to truncate PID file: %s", lock_file_path);
    exit(EXIT_FAILURE);
  }

  // write the current process ID to the lock file
  char buffer[64];
  snprintf(buffer, 64, "%ld\n", (long) getpid());

  if (write(fd, buffer, strlen(buffer)) != strlen(buffer)) {
    // failed to write entire buffer to lock file
    syslog(LOG_ERR, "unable to write process ID to PID file: %s", lock_file_path);
    exit(EXIT_FAILURE);
  }

  // return the lock file descriptor
  return fd;
}

/**
 * Process a received signal.
 */
void handle_signal(int sig) {
  switch (sig) {
    // sent by process manager to indicate we should reload
    case SIGHUP:
      // XXX: reload daemon configuration files and reopen file descriptors
      syslog(LOG_INFO, "ignoring SIGHUP because there is nothing to do");
      break;

    // sent by process manager to indicate we should stop gracefully
    case SIGTERM:
      syslog(LOG_INFO, "received SIGTERM and clearing the run flag");
      running = false;
      break;
  }
}

/**
 * Application entry point.
 *
 * @param argc number of arguments
 * @param argv argument data
 * @returns exit code
 */
int main(int argc, char* argv[]) {
  // daemonize the process
  daemonize();

  // create the lock file
  int lockfd = create_lock_file("/var/run/fibd.pid");

  // set the running flag
  running = true;

  // register the signal handler for relevant signals
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handle_signal;

  if (sigaction(SIGHUP, &sa, NULL) < 0) {
    syslog(LOG_ERR, "failed to install SIGHUP signal handler");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGTERM, &sa, NULL) < 0) {
    syslog(LOG_ERR, "failed to install SIGTERM signal handler");
    exit(EXIT_FAILURE);
  }

  // report our status
  syslog(LOG_INFO, "starting");

  // do useful work
  long a = 0;
  long b = 1;

  while (running) {
    // sleep for a while
    sleep(1);

    // compute the next term
    long sum = a + b;
    a = b;
    b = sum;

    // report the term
    syslog(LOG_INFO, "computed term: %ld", a);
  }

  // report our status
  syslog(LOG_INFO, "stopping");

  // remove the lock file
  close(lockfd);
  unlink("/var/run/fibd.pid");

  // exit cleanly
  exit(EXIT_SUCCESS);
}
