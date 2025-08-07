# levem

## Description:
simple linux cli utility for events tracking from event input device

## Build:
- To build simply run the following shell script:
```shell
./build.sh
```
- For successful build you need pkg-config utility and libevdev c library installed on your machine(The executable is dynamically linked by default)

- To use just add levem executable to any folder located in your PATH environment variable e.g:
```shell
mv levem /usr/bin/levem
```

## Usage:
```shell
levem <event device path>
```