# Genrerate the documentation of py_aff3ct

Prerequisite:

	$ sudo apt install sphinx

Generate the documentation:

	$ cd doc
	$ PYTHONPATH=../build/lib sphinx-autogen source/index.rst
	$ make html

Now the py_aff3ct documentation has been built in the `doc/build` folder.