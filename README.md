# levem (Linux Event Monitor)
A lightweight CLI utility to track input events (e.g., keyboard, mouse) from `/dev/input/event*` devices in real-time.

## Description:
Monitors specified input devices and prints event messages to stdout. Spawns a separate process for each device and exits if process creation fails.

## Features:
- Multi-device monitoring
- Real-time event printing
- Low-overhead (uses `libevdev`)

## Build:
### Dependencies:
- `pkg-config`
- `libevdev` (install with `sudo apt install libevdev-dev` on Debian/Ubuntu or `sudo pacman -S libevdev` on arch)

### Build Command:
- This command builds and dynamically links the executable 
```shell
./build.sh
```