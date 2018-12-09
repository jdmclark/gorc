# Gorc

[![Build Status](https://travis-ci.org/jdmclark/gorc.svg?branch=develop)](https://travis-ci.org/jdmclark/gorc) [![Coverage Status](https://coveralls.io/repos/jdmclark/gorc/badge.svg?branch=develop&service=github)](https://coveralls.io/github/jdmclark/gorc?branch=develop) [![Coverity Scan Build Status](https://scan.coverity.com/projects/2223/badge.svg)](https://scan.coverity.com/projects/2223)

Gorc is an early-development Dark Forces II game engine recreation. The goal of the Gorc project is to replace the original game executable, allowing the original singleplayer and multiplayer levels to be played on new platforms.

## Developing

### Dependencies

* Standard C++14 compiler (GCC 7)
* CMake 3.8 or better
* SFML 2.1
* [Boost](http://boost.org)
* [Gcovr 3.2](http://gcovr.com)
* Dark Forces II

### Quick Start

#### Building

* Build the entire project from the project root:

```
cmake -DCMAKE_BUILD_TYPE=Release .
make -j4
```

### Testing

The test harness is an executable named `boc`. It was built as part of the previous step, and is
located in the `pkg/bin` directory.

* Run all tests: run `boc test` from the project root.

or

* Run a subset of tests: run `boc test` from a component source directory.

or

* Run a specific test: run `boc` from the test directory.

### Test Coverage

Gorc uses the `gcov` coverage tool. The Gorc build system provides special options for coverage
report generation. Use the following command to quickly generate a complete coverage report with an
instrumented build.

* Run `boc test coverage-report` from the project root.

The Gorc project has adopted a 100% line coverage policy to encourage the development of highly
testable code. The test coverage report must show 100% line coverage before any code changes will
be accepted.

## Running

### One-Time Setup

Before running Gorc, you must copy your Dark Forces II original `episode` and `resource`
directories into the `game` directory in the root of this repository. Gorc will not work without
these directories. This step only needs to be done once.

### Run Client

Gorc does not currently allow you to play through the levels normally. You must manually specify an
episode GOB and a level filename on the command line.

For example, to play the first level you would run:

```
pkg/bin/client --episode game/episode/jk1.gob --level 01narshadda.jkl
```

Other episodes and levels can be played by using appropriate `--episode` and `--level` options.

If you do not know the level filenames inside an episode, you can use the included `gob` utility to
list them:

```
pkg/bin/gob --file game/episode/jk1.gob --extract episode.jk
```
