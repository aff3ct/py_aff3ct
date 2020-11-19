#include "py_aff3ct.hpp"

using namespace aff3ct;

PYBIND11_MODULE(py_aff3ct, m){
	// Split in two following https://pybind11.readthedocs.io/en/stable/advanced/misc.html#avoiding-c-types-in-docstrings
	// for enhancing python doc
	setControlMode(rang::control::Force);
	std::vector<wrapper::Wrapper_py*> wrappers;
	py::module_ m0 = m.def_submodule("tools");
	std::unique_ptr<wrapper::Wrapper_py> wrapper_gngi       (new wrapper::Wrapper_Gaussian_noise_generator_implem(m0));
	wrappers.push_back(wrapper_gngi.get());
{other_tool_wrappers}

	py::module_ m1 = m.def_submodule("module");

	std::unique_ptr<wrapper::Wrapper_py> wrapper_socket       (new wrapper::Wrapper_Socket         (m1));
	wrappers.push_back(wrapper_socket.get());

	std::unique_ptr<wrapper::Wrapper_py> wrapper_task         (new wrapper::Wrapper_Task           (m1));
	wrappers.push_back(wrapper_task.get());

	std::unique_ptr<wrapper::Wrapper_py> wrapper_module       (new wrapper::Wrapper_Module         (m1));
	wrappers.push_back(wrapper_module.get());

	py::module_ m2 = m1.def_submodule("py_module");
	std::unique_ptr<wrapper::Wrapper_py> wrapper_py_module    (new wrapper::Wrapper_Py_Module      (m2));
	wrappers.push_back(wrapper_py_module.get());

{other_module_wrappers}

	for (size_t i = 0; i < wrappers.size(); i++)
		wrappers[i]->definitions();
}