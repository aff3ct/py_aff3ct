#include "Wrapper_py/Module/Switcher/Switcher.hpp"
#include <map>
#include <memory>
#include <typeindex>
#include <typeinfo>

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

/*std::map<std::string,std::type_index> type_map = {{ "int8"    , typeid(int8_t )},
                                                   { "int16"  , typeid(int16_t)},
                                                   { "int32"  , typeid(int32_t)},
                                                   { "int64"  , typeid(int64_t)},
                                                   { "float32", typeid(float  )},
                                                   { "double" , typeid(double )}};*/

Wrapper_Switcher
::Wrapper_Switcher(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::module::Switcher,aff3ct::module::Module>(scope, "Switcher")
{
}


void Wrapper_Switcher
::definitions()
{


	//this->def(py::init<const size_t, const size_t, const std::type_index>(),"n_data_sockets"_a, "n_elmts"_a, "datatype"_a, py::return_value_policy::reference);
    this->def(py::init(
        [](const size_t n_data_sockets, const size_t n_elmts, const py::object dtype){
            const std::string dtype_str (dtype.attr("__name__").cast<std::string>());
            //return new aff3ct::module::Switcher(n_data_sockets, n_elmts, type_map[dtype_str]);
            if      (dtype_str == "int8"   ) return new aff3ct::module::Switcher(n_data_sockets, n_elmts, typeid(int8_t));
            else if (dtype_str == "int16"  ) return new aff3ct::module::Switcher(n_data_sockets, n_elmts, typeid(int16_t));
            else if (dtype_str == "int32"  ) return new aff3ct::module::Switcher(n_data_sockets, n_elmts, typeid(int32_t));
            else if (dtype_str == "int64"  ) return new aff3ct::module::Switcher(n_data_sockets, n_elmts, typeid(int64_t));
            else if (dtype_str == "float32") return new aff3ct::module::Switcher(n_data_sockets, n_elmts, typeid(float  ));
            else if (dtype_str == "double" ) return new aff3ct::module::Switcher(n_data_sockets, n_elmts, typeid(double ));
        }),"n_data_sockets"_a, "n_elmts"_a, "datatype"_a, py::return_value_policy::reference);
};

