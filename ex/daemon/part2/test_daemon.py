import time

import pytest
import testinfra

def test_binary_installed(host):
    binary_file = host.file("/usr/bin/fibd")
    assert binary_file.exists, "binary is not installed"
    assert binary_file.mode == 0o755, "binary permissions are incorrect"

def test_service_installed(host):
    service_file = host.file("/etc/systemd/system/fibd.service")
    assert service_file.exists, "systemd service file is not installed"
    assert service_file.mode == 0o644, "systemd service file permissions are incorrect"

def test_daemon_service(host):
    with host.sudo():
        # verify the lock file does not exist
        lock_file = host.file("/var/run/fibd.pid")
        assert not lock_file.exists, "PID file already exists"

        # start the daemon using systemd
        start_daemon = host.run("/bin/systemctl start fibd.service")
        assert start_daemon.rc == 0, "daemon failed to start"

        time.sleep(1)

        # verify the lock file was not created
        assert not lock_file.exists, "PID file was created"

        # locate the daemon process
        daemon_process = host.process.get(comm="fibd")

        # stop the daemon
        stop_daemon = host.run("/bin/systemctl stop fibd.service")
        assert stop_daemon.rc == 0

        time.sleep(2)

        # verify the process is not running anymore
        with pytest.raises(RuntimeError):
            host.process.get(comm="fibd")
