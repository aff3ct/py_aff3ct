# Python to AFF3CT

This repository contains a code example for integrating a `Python` module inside
an AFF3CT sequence.

## Using AFF3CT as a library for your codes

The first step is to compile AFF3CT into a library.

Get the AFF3CT library:

	$ git submodule update --init --recursive

Compile the library on Linux/MacOS/MinGW:

	$ cd lib/aff3ct
	$ mkdir build
	$ cd build
	$ cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-funroll-loops -march=native" -DAFF3CT_COMPILE_EXE="OFF" -DAFF3CT_COMPILE_STATIC_LIB="ON" -DAFF3CT_COMPILE_SHARED_LIB="ON"
	$ make -j4

Now the AFF3CT library has been built in the `lib/aff3ct/build` folder.

## Compile your code

Copy the `CMake` configuration files from the AFF3CT build

	$ mkdir cmake && mkdir cmake/Modules
	$ cp lib/aff3ct/build/lib/cmake/aff3ct-*/* cmake/Modules

Install the `Python` dependencies on Ubuntu 20.04 LTS (at this time, using `pip`
is not working to install the `pybind11` package, see
[this post](https://github.com/pybind/pybind11/issues/1379))

	$ sudo apt install python3-pip python3-pybind11
	$ pip3 install --user -r requirements.txt

Compile the code on Linux/MacOS/MinGW:

	$ mkdir build
	$ cd build
	$ cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-funroll-loops -march=native"
	$ make

The compiled binary is in `build/bin/my_project`.

For the binary to find your `Python` sources, you should add them to your
`PYTHONPATH` (in this example, the Python source files are located in the
`python_src` folder)

	$ export PYTHONPATH=$PYTHONPATH:"path_to_your_python_src"
