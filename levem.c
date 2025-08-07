#include <errno.h>
#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

char *usageMessage = "Usage: %s [--help | -h] <device1 path> <device2 path> <device... path>\n";

int processDevice(const char *path) {
    pid_t pid = fork();
    
    if (pid < 0) {
        return pid;
    }
    
    if (pid > 0) {
        return 0;
    }
    
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct libevdev *dev = NULL;
    int rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        fprintf(stderr, "Failed to init libevdev for %s: %s\n", path, strerror(-rc));
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Successfully initialized libevdev for %s\n", path);
    
    struct input_event event;
    while (1) {
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_BLOCKING, &event);
        if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
            printf(
                "Event captured from %s:\n\t Type: %hu(%s) Code: %hu(%s) Value: %d(%s)\n", path,
                event.type, libevdev_event_type_get_name(event.type), event.code,
                libevdev_event_code_get_name(event.type, event.code), event.value,
                libevdev_event_value_get_name(event.type, event.code, event.value));
        } else if (rc == LIBEVDEV_READ_STATUS_SYNC) {
            fprintf(stderr, "Synchronization with %s lost, handling SYN_DROPPED\n", path);
            while (rc == LIBEVDEV_READ_STATUS_SYNC) {
                rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_SYNC, &event);
            }
        } else {
            if (rc == -ENODEV) {
                fprintf(stderr, "%s disconnected\n", path);
                break;
            }
            fprintf(stderr, "Failed to get event from %s: %s\n", path, strerror(-rc));
            break;
        }
    }

    libevdev_free(dev);
    close(fd);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, usageMessage, argv[0]);
        exit(EXIT_FAILURE);
    }
    
    if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
        printf(usageMessage, argv[0]);
        exit(EXIT_SUCCESS);
    }
    
    for (int i = 1; i < argc; i++) {
        if (processDevice(argv[i]) < 0) {
            perror("Failed creating a process for input event monitoring");
            exit(EXIT_FAILURE);
        }
    }
    
    while (wait(NULL) > 0);
    
    exit(EXIT_SUCCESS);
}