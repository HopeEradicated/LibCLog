# LibCLog
Libc wrapper for logging fileio and memmgmt symbols

## Requirements

```ps
    GCC >= 7.1
    glibc >= 2.17
    Linux kernel >= 2.17
    CMake >= 3.1 
    POSIX API
```

## Build & run

```ps
cmake --version 
(Check for version gt when 3.1)
```

```ps
mkdir build 
cd build 
(If build directory don't exists)
```

```ps
cmake ..
cmake --build .
```

## Test

```ps
cmake --build . --target test_interceptor
```