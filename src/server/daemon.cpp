//
//  daemon.cpp
//  qrcode_detect_server
//
//  Created by king on 2024/4/10.
//

#include "daemon.hpp"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace qrcode {
bool start_daemon() {
    int fd;
    switch (fork()) {
        case -1: {
            fprintf(stderr, "fork() failed");
            return false;
        }
        case 0:
            break;
        default:
            exit(EXIT_SUCCESS);
    }

    if (setsid() == -1) {
        fprintf(stderr, "setsid() failed");
        return false;
    }

    umask(0);

    fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "open(\"/dev/null\") failed");
        return false;
    }

    if (dup2(fd, STDIN_FILENO) == -1) {
        fprintf(stderr, "dup2(STDIN) failed");
        return false;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        fprintf(stderr, "dup2(STDOUT) failed");
        return false;
    }

#if 0
        if (dup2(fd, STDERR_FILENO) == -1) {
            fprintf(stderr, "dup2(STDERR) failed");
            return false;
        }
#endif

    if (fd > STDERR_FILENO) {
        if (close(fd) == -1) {
            fprintf(stderr, "close() failed");
            return false;
        }
    }

    return true;
}
};  // namespace qrcode
