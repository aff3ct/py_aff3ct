# Calling Python from C++ AFF3CT code

You should have first compiled the AFF3CT library and the `py_aff3ct` wrapper as
explained in the `README.md` file at the root of this repository.

## Compile your code

Copy the `CMake` configuration files from the AFF3CT build:

	$ mkdir cmake && mkdir cmake/Modules
	$ cp ../../lib/aff3ct/build/lib/cmake/aff3ct-*/* cmake/Modules

Compile the code on Linux/MacOS/MinGW:

	$ mkdir build && cd build
	$ cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-Wall -funroll-loops -march=native -fvisibility=hidden -fvisibility-inlines-hidden"
	$ make -j4

The compiled binary is in `build/bin/python_in_cpp`.

For the binary to find your `Python` sources, you should add them to your
`PYTHONPATH` (in this example, the Python source files are located in the
`python_src` folder)

	$ export PYTHONPATH=$PYTHONPATH:"path_to_your_python_src"
