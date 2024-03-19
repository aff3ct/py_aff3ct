#include "py_aff3ct.hpp"

using namespace aff3ct;

PYBIND11_MODULE(py_aff3ct, m){
	// Split in two following https://pybind11.readthedocs.io/en/stable/advanced/misc.html#avoiding-c-types-in-docstrings
	// for enhancing python doc
	setControlMode(rang::control::Off);

	m.doc() =
R"pbdoc(
        py_aff3ct python bindings for AFF3CT library.

        .. autosummary::
           :toctree:
           :template: custom-module-template.rst

           tools
           module
)pbdoc";

	std::vector<wrapper::Wrapper_py*> wrappers;
	py::module_ m0 = m.def_submodule("tools");
	std::unique_ptr<wrapper::Wrapper_py> wrapper_gngi       (new wrapper::Wrapper_Gaussian_noise_generator_implem(m0));
	wrappers.push_back(wrapper_gngi.get());

	//m.def("enable_colors", [](){setControlMode(rang::control::Force);});
	//m.def("disable_colors",[](){setControlMode(rang::control::Off);});
	std::string doc_m0 =
R"pbdoc(
        Bindings for AFF3CT tools.

        .. autosummary::
           :toctree:
           :template: custom-module-template.rst

           sequence
           pipeline
           frozenbits_generator
)pbdoc";

	py::module_ m_sequence = m0.def_submodule("sequence");
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_sequence(new aff3ct::wrapper::Wrapper_Sequence(m_sequence));
	wrappers.push_back(wrapper_sequence.get());

	py::module_ m_pipeline = m0.def_submodule("pipeline");
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_pipeline(new aff3ct::wrapper::Wrapper_Pipeline(m_pipeline));
	wrappers.push_back(wrapper_pipeline.get());

	py::module_ mod_frozenbits_generator = m0.def_submodule("frozenbits_generator");
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_frozenbits_generator(new aff3ct::wrapper::Wrapper_Frozenbits_generator(mod_frozenbits_generator));
	wrappers.push_back(wrapper_frozenbits_generator.get());

	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_frozenbits_generator_bec(new aff3ct::wrapper::Wrapper_Frozenbits_generator_BEC(mod_frozenbits_generator));
	wrappers.push_back(wrapper_frozenbits_generator_bec.get());

	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_frozenbits_generator_ga(new aff3ct::wrapper::Wrapper_Frozenbits_generator_GA(mod_frozenbits_generator));
	wrappers.push_back(wrapper_frozenbits_generator_ga.get());

	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_frozenbits_generator_ga_arikan(new aff3ct::wrapper::Wrapper_Frozenbits_generator_GA_Arikan(mod_frozenbits_generator));
	wrappers.push_back(wrapper_frozenbits_generator_ga_arikan.get());

	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_frozenbits_generator_file(new aff3ct::wrapper::Wrapper_Frozenbits_generator_file(mod_frozenbits_generator));
	wrappers.push_back(wrapper_frozenbits_generator_file.get());

	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_frozenbits_generator_5g(new aff3ct::wrapper::Wrapper_Frozenbits_generator_5G(mod_frozenbits_generator));
	wrappers.push_back(wrapper_frozenbits_generator_5g.get());

	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_frozenbits_generator_tv(new aff3ct::wrapper::Wrapper_Frozenbits_generator_TV(mod_frozenbits_generator));
	wrappers.push_back(wrapper_frozenbits_generator_tv.get());

	py::module_ mod_sparse_matrix = m0.def_submodule("sparse_matrix");
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_sparse_matrix(new aff3ct::wrapper::Wrapper_Sparse_matrix(mod_sparse_matrix));
	wrappers.push_back(wrapper_sparse_matrix.get());

{other_tool_wrappers}

	m0.doc() = doc_m0.c_str();

	py::module_ m1 = m.def_submodule("module");
	std::string doc_m1 =
R"pbdoc(
        Bindings for AFF3CT modules.

        .. autosummary::
           :toctree:
           :template: custom-module-template.rst

           py_module
           monitor
           switcher
           interleaver
		   source
)pbdoc";
	std::unique_ptr<wrapper::Wrapper_py> wrapper_socket       (new wrapper::Wrapper_Socket         (m1));
	wrappers.push_back(wrapper_socket.get());

	std::unique_ptr<wrapper::Wrapper_py> wrapper_task         (new wrapper::Wrapper_Task           (m1));
	wrappers.push_back(wrapper_task.get());

	std::unique_ptr<wrapper::Wrapper_py> wrapper_module       (new wrapper::Wrapper_Module         (m1));
	wrappers.push_back(wrapper_module.get());

	py::module_ m2 = m1.def_submodule("py_module");
	std::unique_ptr<wrapper::Wrapper_py> wrapper_py_module    (new wrapper::Wrapper_Py_Module      (m2));
	wrappers.push_back(wrapper_py_module.get());

	py::module_ mod_monitor = m1.def_submodule("monitor");
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_monitor(new aff3ct::wrapper::Wrapper_Monitor(mod_monitor));
	wrappers.push_back(wrapper_monitor.get());
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_monitor_bfer_ar(new aff3ct::wrapper::Wrapper_Monitor_BFER_AR<>(mod_monitor));
	wrappers.push_back(wrapper_monitor_bfer_ar.get());
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_monitor_bfer(new aff3ct::wrapper::Wrapper_Monitor_BFER<>(mod_monitor));
	wrappers.push_back(wrapper_monitor_bfer.get());
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_monitor_exit(new aff3ct::wrapper::Wrapper_Monitor_EXIT<>(mod_monitor));
	wrappers.push_back(wrapper_monitor_exit.get());
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_monitor_mi(new aff3ct::wrapper::Wrapper_Monitor_MI<>(mod_monitor));
	wrappers.push_back(wrapper_monitor_mi.get());
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_monitor_bfer_reduction(new aff3ct::wrapper::Wrapper_Monitor_reduction_BFER(mod_monitor));
	wrappers.push_back(wrapper_monitor_bfer_reduction.get());

	py::module_ mod_switcher = m1.def_submodule("switcher");
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_switcher(new aff3ct::wrapper::Wrapper_Switcher(mod_switcher));
	wrappers.push_back(wrapper_switcher.get());

	py::module_ mod_ilv = m1.def_submodule("interleaver");
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_interleaver_int8  (new aff3ct::wrapper::Wrapper_Interleaver<int8_t,  uint32_t>(mod_ilv, "int8"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_interleaver_int16 (new aff3ct::wrapper::Wrapper_Interleaver<int16_t, uint32_t>(mod_ilv, "int16"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_interleaver_int32 (new aff3ct::wrapper::Wrapper_Interleaver<int32_t, uint32_t>(mod_ilv, "int32"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_interleaver_int64 (new aff3ct::wrapper::Wrapper_Interleaver<int64_t, uint32_t>(mod_ilv, "int64"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_interleaver_float (new aff3ct::wrapper::Wrapper_Interleaver<float,   uint32_t>(mod_ilv, "float"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_interleaver_double(new aff3ct::wrapper::Wrapper_Interleaver<double,  uint32_t>(mod_ilv, "double"));
	wrappers.push_back(wrapper_interleaver_int8  .get());
	wrappers.push_back(wrapper_interleaver_int16 .get());
	wrappers.push_back(wrapper_interleaver_int32 .get());
	wrappers.push_back(wrapper_interleaver_int64 .get());
	wrappers.push_back(wrapper_interleaver_float .get());
	wrappers.push_back(wrapper_interleaver_double.get());

	py::module_ mod_tunnel = m1.def_submodule("tunnel");
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_tunnel_numpy_int8(new aff3ct::wrapper::Wrapper_Tunnel_numpy<int8_t>(mod_tunnel, "int8"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_tunnel_numpy_int16(new aff3ct::wrapper::Wrapper_Tunnel_numpy<int16_t>(mod_tunnel, "int16"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_tunnel_numpy_int32(new aff3ct::wrapper::Wrapper_Tunnel_numpy<int32_t>(mod_tunnel, "int32"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_tunnel_numpy_int64(new aff3ct::wrapper::Wrapper_Tunnel_numpy<int64_t>(mod_tunnel, "int64"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_tunnel_numpy_float(new aff3ct::wrapper::Wrapper_Tunnel_numpy<float>(mod_tunnel, "float"));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_tunnel_numpy_double(new aff3ct::wrapper::Wrapper_Tunnel_numpy<double>(mod_tunnel, "double"));
	wrappers.push_back(wrapper_tunnel_numpy_int8.get());
	wrappers.push_back(wrapper_tunnel_numpy_int16.get());
	wrappers.push_back(wrapper_tunnel_numpy_int32.get());
	wrappers.push_back(wrapper_tunnel_numpy_int64.get());
	wrappers.push_back(wrapper_tunnel_numpy_float.get());
	wrappers.push_back(wrapper_tunnel_numpy_double.get());

{other_module_wrappers}

	m1.doc() = doc_m1.c_str();
	for (size_t i = 0; i < wrappers.size(); i++)
		wrappers[i]->definitions();
}