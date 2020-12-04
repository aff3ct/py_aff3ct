#ifndef PY_AFF3CT_HPP_
#define PY_AFF3CT_HPP_
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>

#include <aff3ct.hpp>

#include "Wrapper_py/Wrapper_py.hpp"
#include "Wrapper_py/Module/Module.hpp"
#include "Wrapper_py/Module/Socket.hpp"
#include "Wrapper_py/Module/Task.hpp"
#include "Wrapper_py/Module/Py_Module/Py_Module.hpp"
#include "Wrapper_py/Tools/Gaussian_noise_generator_implem/Gaussian_noise_generator_implem.hpp"
#include "Wrapper_py/Tools/Sequence/Sequence.hpp"
#include "Wrapper_py/Tools/Monitor_reduction/Monitor_reduction_BFER.hpp"
#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator.hpp"
#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_BEC/Frozenbits_generator_BEC.hpp"
#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_GA/Frozenbits_generator_GA.hpp"
#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_GA_Arikan/Frozenbits_generator_GA_Arikan.hpp"
#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_file/Frozenbits_generator_file.hpp"
#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_file/Frozenbits_generator_5G/Frozenbits_generator_5G.hpp"
#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_file/Frozenbits_generator_TV/Frozenbits_generator_TV.hpp"
#include "Wrapper_py/Module/Monitor/Monitor.hpp"
#include "Wrapper_py/Module/Monitor/Monitor_BFER_AR/Monitor_BFER_AR.hpp"
#include "Wrapper_py/Module/Monitor/Monitor_BFER/Monitor_BFER.hpp"
#include "Wrapper_py/Module/Monitor/Monitor_EXIT/Monitor_EXIT.hpp"
#include "Wrapper_py/Module/Monitor/Monitor_MI/Monitor_MI.hpp"
{other_includes}

#endif //PY_AFF3CT_HPP_