# Prerequisites #

* GNU Autotools
* OpenGL 3.0
* C++11 compiler (tested with GCC 4.8.3+)
* [Boost](http://www.boost.org/)
* [GLEW](http://glew.sourceforge.net/)
* [SDL2](https://www.libsdl.org/)
* [GLM](http://glm.g-truc.net/)

On Fedora 22 or later you can install these using a single command (as root):

> $ yum install boost-* glew-devel SDL2_* glm-devel

# Building #

After cloning the source code or extracting a distributed archive, change to the
directory where the source code is:

``` bash
$ autoreconf -i
$ ./configure
$ make
```

Alternatively, if you'd like to build the project in debug mode use:

> $ make CXXFLAGS=-DDEBUG

# Running #

The build process should create a binary that can be executed as follows:

> $ ./src/shaderexample

See

> $ ./src/shaderexample --help

for usage instructions.

# Boost.Python testing #

This test is based off the example given here: https://en.wikibooks.org/wiki/Python_Programming/Extending_with_C%2B%2B

To build the "hello world" test library "hello.so", in the src file, run:

> $ python setup.py build

This will create hello.so in /build/lib-linux-... which can be run from there
(or moved to somewhere else), and then tested with (in the same directory):

> $ python
>>> import hello
>>> hello.greet()

