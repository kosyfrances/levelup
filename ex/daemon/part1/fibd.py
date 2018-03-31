#!/usr/bin/env python3
"""
A re-implementation of fibd.c in python3
An example SystemV daemon.
This was assembled from information containedin The Linux Programming Interface, by Michael Kerrisk.
See chapter 37 and section 55.6.
"""

import os
import sys
import stat
import fcntl
import struct
from time import sleep
from signal import signal, SIGHUP, SIGTERM
from syslog import syslog, LOG_ERR, LOG_INFO

STDIN_FILENO = sys.stdin.fileno()
STDOUT_FILENO  = sys.stdout.fileno()
STDERR_FILENO = sys.stderr.fileno()

# Whether the daemon should continue running.
running = False

def daemonize():
    """Daemonize the current process."""

    # fork the parent process
    pid = os.fork()
    if pid < 0:
        print("failed to fork parent process\n", file=sys.stderr)
        sys.exit(1)
    elif pid > 0:
        # we are the parent process so we exit. the child process continues
        # running under the init process and independent of any shell the
        # parent was invoked in
        sys.exit(0)

    # start a new session and disconnect from parent's controlling terminal
    try:
        os.setsid()
    except OSError as e:
        print(e)
        syslog(LOG_ERR, "failed to create a new session")
        sys.exit(1)

    # fork the parent process again so any terminal devices we open will not
    # become the controlling terminal
    pid = os.fork()
    if pid < 0:
        syslog(LOG_ERR, "failed to fork parent process for the second time")
        sys.exit(1)
    elif pid > 0:
        # we are the parent process, so quit
        sys.exit(0)

    # clear the process umask so any files or directories we create will have
    # the requested permissions
    os.umask(0)

    # change the working directory to the root directory to avoid locking any
    # non-root filesystem thereby preventing it from being unmounted
    os.chdir("/")

    # close all open file descriptors to remove any references to our original
    # terminal and to avoid locking any non-root filesystems
    open_fd = os.listdir("/proc/{}/fd".format(os.getpid()))
    fd_low = int(open_fd[0])
    fd_high = int(open_fd[len(open_fd) - 1])
    os.closerange(fd_low, fd_high + 1)

    # close the standard output stream and re-open it using /dev/null
    # note that because we closed all open file descriptors, the next time
    # we open a file the descriptor should start with 0 which is STDIN_FILENO
    sys.stdin.close()
    nullfd = os.open(os.devnull, os.O_RDWR)

    if nullfd != STDIN_FILENO:
        syslog(LOG_ERR, "failed to open /dev/null as stdin")
        sys.exit(1)

    # duplicate the stdin file descriptor for stdout and stderr
    os.dup2(STDIN_FILENO, STDOUT_FILENO)
    os.dup2(STDIN_FILENO, STDERR_FILENO)

def create_lock_file(lock_file_path):
    """Create a lock file with the current process ID as it's contents.

    Arguments:
        lock_file_path {string} -- path to the lock file
    Returns:
        lock file descriptor
    """

    # open or create the lock file in rw mode and if creating it
    # set the read and write user permission flags
    fd = os.open(lock_file_path, os.O_RDWR | os.O_CREAT, stat.S_IRUSR | stat.S_IWUSR)

    if fd < 0:
        syslog(LOG_ERR, "failed to open lock file: {}".format(lock_file_path))
        sys.exit(1)

    # set the close-on-exec file descriptor flag which will close the file
    # descriptor the next time exec() is called, useful for daemons that
    # reload themselves by re-execing
    flags = fcntl.fcntl(fd, fcntl.F_GETFD)
    if flags < 0:
        syslog(LOG_ERR, "failed to open lock file: {}".format(lock_file_path))
        sys.exit(1)

    flags |= fcntl.FD_CLOEXEC
    if fcntl.fcntl(fd, fcntl.F_SETFD, flags) < 0:
        syslog(LOG_ERR, "failed to open lock file: {}".format(lock_file_path))
        sys.exit(1)

    # lock the entire contents of the lock file to prevent another process
    # from writing to it while we're running
    try:
        fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except (IOError, OSError):
        syslog("Cannot lock {}".format(lock_file_path))
        sys.exit(1)

    # truncate the contents of the lock file
    os.truncate(fd, 0)

    # write the current process ID to the lock file
    with open(lock_file_path, 'w') as f:
        f.write('{}'.format(os.getpid()))

    # return the lock file descriptor
    return fd

def handle_signal(signum, frame):
    """Process a received signal.

    Arguments:
        signum {int} -- sent by process manager to indicate we should reload
    """

    if signum == SIGHUP:
        # reload daemon configuration files and reopen file descriptor
        syslog(LOG_INFO, "ignoring SIGHUP because there is nothing to do")
    elif signum == SIGTERM:
        syslog(LOG_INFO, "received SIGTERM and clearing the run flag")
        global running
        running = False


if __name__ == "__main__":
    # daemonize the process
    daemonize()

    # create the lock file
    create_lock_file("/var/run/fibd.pid")

    # set the running flag
    running = True

    # register the signal handler for relevant signals
    signal(SIGHUP, handle_signal)
    signal(SIGTERM, handle_signal)

    # report our status
    syslog(LOG_INFO, "starting")

    # do useful work
    a = 0
    b = 1
    while running:
        # sleep for a while
        sleep(1)

        # compute the next term
        sum = a + b
        a = b
        b = sum

        # report the term
        syslog(LOG_INFO, "computed term: {}".format(a))

        # Break the loop if running changes to false
        if not running:
            break

    # report out status
    syslog(LOG_INFO, "stopping")

    # remove the lock file
    os.unlink("/var/run/fibd.pid")

    # exit cleanly
    sys.exit(0)
