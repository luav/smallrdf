# SmallRDF

A small RDF library with minimal dependencies for embedded devices.
It works in memory and requires only memory copy and compare functions.
The library was developed for the Arduino platform, but should work also on other platforms.

## Requirements

Tests rely on google test library:
```sh
& sudo apt-get install libgtest-dev
```

## Build

```sh
$ make [<target>]
```

The resulting binaries are generated in `./bin`.

`Makefile` contains the following targets:
- `all`  - build all targets
- `test_debug`  - test application under the debug mode
- `debug`  - library (both `C` abd `C++` interfaces) under the debug mode
- `release`  - library under the *portable* release mode (without device-specific optimization)
- `release_native`  - library, optimized for the device where the build is performed (recommended, but might not be portable)
- `release_native_c`  - library with `C` only interface, optimized for the device (recommended for IoT devices, where `C++` interface is not required)

## Example

The [SmallHydra](https://github.com/bergos/smallhydra) library contains an example that show how to use SmallRDF.
