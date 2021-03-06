#include "{path}/{short_name}.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

{medium_template}
Wrapper_{short_name}{short_template}
::Wrapper_{short_name}(py::handle scope)
: Wrapper_py(),
  py::class_<{name}{short_template}{parent}{dtor_trick}>(scope, "{short_name}")
{
}

{medium_template}
void Wrapper_{short_name}{short_template}
::definitions()
{{init_lines}{def_lines}};

{footer}