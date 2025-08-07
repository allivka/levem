#include <errno.h>
#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <device>\n", argv[0]);
    exit(1);
  }
  char *device = argv[1];

  int fd = open(device, O_RDONLY);
  if (fd < 0) {
    perror("Failed to open device");
    exit(1);
  }

  struct libevdev *dev = NULL;
  int rc = libevdev_new_from_fd(fd, &dev);
  if (rc < 0) {
    fprintf(stderr, "Failed to init libevdev: %s\n", strerror(-rc));
    close(fd);
    exit(1);
  }

  printf("Succesfully initialized libevdev\n");

  struct input_event event;
  while (1) {
    rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_BLOCKING, &event);
    if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
      printf(
          "Event captured: Type: %hu(%s) Code: %hu(%s) Value: %d(%s)\n",
          event.type, libevdev_event_type_get_name(event.type), event.code,
          libevdev_event_code_get_name(event.type, event.code), event.value,
          libevdev_event_value_get_name(event.type, event.code, event.value));
    } else if (rc == LIBEVDEV_READ_STATUS_SYNC) {
      fprintf(stderr, "Synchronization lost, handling SYN_DROPPED.\n");
      while (rc == LIBEVDEV_READ_STATUS_SYNC) {
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_SYNC, &event);
      }
    } else {
      if (rc == -ENODEV) {
        fprintf(stderr, "Device disconnected.\n");
        break;
      }
      perror("Failed to get event");
      break;
    }
  }

  libevdev_free(dev);
  close(fd);
  return 0;
}
