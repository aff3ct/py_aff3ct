#include "py_aff3ct.hpp"


PYBIND11_MODULE(py_aff3ct, m){
	// Split in two following https://pybind11.readthedocs.io/en/stable/advanced/misc.html#avoiding-c-types-in-docstrings
	// for enhancing python doc
	std::vector<aff3ct::wrapper::Wrapper_py*> wrappers;

	py::module_ m1 = m.def_submodule("module");

	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_socket       (new aff3ct::wrapper::Wrapper_Socket         (m1));
	wrappers.push_back(wrapper_socket.get());

	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_task         (new aff3ct::wrapper::Wrapper_Task           (m1));
	wrappers.push_back(wrapper_task.get());

	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_module       (new aff3ct::wrapper::Wrapper_Module         (m1));
	wrappers.push_back(wrapper_module.get());

	py::module_ m2 = m1.def_submodule("py_module");
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_py_module    (new aff3ct::wrapper::Wrapper_Py_Module      (m2));
	wrappers.push_back(wrapper_py_module.get());

{other_wrappers}

	for (size_t i = 0; i < wrappers.size(); i++)
		wrappers[i]->definitions();
}