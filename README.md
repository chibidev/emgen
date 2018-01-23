# emgen

[![Build Status](https://travis-ci.org/chibidev/emgen.svg?branch=master)](https://travis-ci.org/chibidev/emgen)
A command-line tool to generate emscripten bindings to your classes based on foonathan's excellent [cppast](https://github.com/foonathan/cppast) library

## Usage

```bash
$ emgen file.cpp
```

It expects to find a clang compilation database ``compile_commands.json`` in the current directory to look up file.cpp's compiler switches. You can override the directory by specifying the ``-d directory`` parameter.
