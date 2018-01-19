# Ork Resource Kernel
---
[![License](https://img.shields.io/github/license/calsev/ork.svg)](https://github.com/calsev/ork)
[![GitHub Tag](https://img.shields.io/github/tag/calsev/ork.svg)](https://github.com/calsev/ork)
[![Commit Activity](https://img.shields.io/github/commit-activity/y/calsev/ork.svg)](https://github.com/calsev/ork)
[![GitHub last commit](https://img.shields.io/github/last-commit/calsev/ork.svg)](https://github.com/calsev/ork)
[![GitHub repo size in bytes](https://img.shields.io/github/repo-size/calsev/ork.svg)](https://github.com/calsev/ork)

The Ork Resource Kernel supports rapid application development by providing two primary components:

* Ork library
* Ork Build System

Either component be used in isolation, but typically the library is built using the build system.

# Ork Library
---

The Ork library provides the four core, mutually-dependent resources that constitute the minimum kernel of an application:

* File Handling
* Text Handling
* Logging
* Error Handling

Simply include `ork/ork.hpp` to use the library.  This header is comprehesive for the core library yet well-isolated, using forward declarations and including a minimal set of STL headers.  See the Ork Build System below for details on how to build and link with the Ork Library.  See Additional Libraries below for information about other library features.

## Examples

```C++
```

# Ork Build System
---

The Ork Build System can be used to build the Ork Library, but is frequently used to build a self-contained dependency 'distributution' from sources.  In a language like C++, with no de facto standard module manager, a new software project faces a question of how to handle dependencies and build the system.  An ideal build system satisfies a few properties.

1. No environmental requirements
    1. Dependencies beyond the operating system and compiler/runtime
    1. Variable configuration (PATH, LD_LIBRARY_PATH, BOOST_ROOT, etc.)
1. No external code or binaries committed to the repository
1. One-click build
1. No full-time build system maintainer

These characteristics are typically in tension, as giving up one makes the others easy to satisfy.

The Ork Build System addresses 1. and 2. by working with Git submodules to build a dependency tree from sources.  To do this, a recent CMake installation is required.  The Ork Build System provides a strong base for satifying 3. and 4., as explained in the example below.

## Limitations

The Ork Build System has been geared to resource-heavy, medium-sized CAD and visualization applications. Only x64, DLL-based builds are provided for third-party libraries.  The build for the Ork Library supports static linking.  Only Visual Studio/Windows and GCC/Linux builds are provided.  There are no plans to support x86 or emulation layers (MinGW, Cygwin).

## Example

Several projects that have used the Ork Build System have been organized similar to below.

```
project_root
|bin
|src
-|3p
--|boost
--|bz2
--|ft2
--|glm
--|json
--|oce
--|ork
--|png
--|pugi
--|tbb
--|yaml
--|zlib
-|bin
--|inc
--|lib
-|build
-|inc
-|src
-|build_3p.bat
-|build_3p.sh
|.gitmodules
```

In the example above, the `/3p` directory contains Git submodule repositories for all dependencies.  To build the dependencies for the above OpenCascade-based application would require one CMakeList.txt.

```CMake

```

With this, the project is dangerously close to a one-click build using a simple script.

```batch
REM Add initialization logic, e.g. git submodule update --init, if desired



REM Depedency solution exists, run MSBuild here if desired
REM Add application build logic here
```

After building, all headers would be properly organized in /src/bin/inc, all stub libraries would be in /src/bin/lib, and all dependency DLLs would be in /bin.  Therefore the application itself would need to be configure for only one external include directory and one external link directory.

