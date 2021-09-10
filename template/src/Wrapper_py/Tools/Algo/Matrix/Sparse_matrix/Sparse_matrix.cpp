#include <sstream>
#include "Wrapper_py/Tools/Algo/Matrix/Sparse_matrix/Sparse_matrix.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Sparse_matrix
::Wrapper_Sparse_matrix(py::module_& scope)
: Wrapper_py(),
py::class_<aff3ct::tools::Sparse_matrix>(scope, "array")
{
	py::module_ alist_reader = scope.def("read_from_alist", [](const std::string& path){
		std::filebuf fb;
		if (fb.open (path,std::ios::in))
		{
			std::istream stream(&fb);
			aff3ct::tools::Sparse_matrix ret = aff3ct::tools::AList::read(stream);
			return ret;
		}
	}, py::return_value_policy::copy);
	scope.def("eye", [](const size_t &size, const int64_t &d){
		aff3ct::tools::Sparse_matrix * H = new aff3ct::tools::Sparse_matrix(size, size);
		for (size_t i = 0; i<size; i++)
			if (i-d >= 0 && i-d < size)
				H->add_connection(i-d,i);
		return H;
	}, py::return_value_policy::take_ownership);
	scope.def("ones", [](py::tuple& shape){
		if (shape.size() != 2 )
		{
			std::stringstream message;
			message << "the created array should be 2-dimensional, but " << shape.size() << " indexes were given.";

			throw py::index_error(message.str());
		}
		int n_rows;
		try
		{
			n_rows = shape[0].cast<size_t>();
		}
		catch(...)
		{
			std::stringstream message;
			message << "shape should be a positive integer vector.";

			throw py::index_error(message.str());
		}
		int n_cols;
		try
		{
			n_cols = shape[1].cast<size_t>();
		}
		catch(...)
		{
			std::stringstream message;
			message << "shape should be a positive integer vector.";

			throw py::index_error(message.str());
		}
		aff3ct::tools::Sparse_matrix * H = new aff3ct::tools::Sparse_matrix(n_rows, n_cols);
		for (size_t i = 0; i<n_rows; i++)
			for (size_t j = 0; j<n_cols; j++)
				H->add_connection(i,j);
		return H;
	}, py::return_value_policy::take_ownership);
	scope.def("zeros", [](py::tuple& shape){
		if (shape.size() != 2 )
		{
			std::stringstream message;
			message << "the created array should be 2-dimensional, but " << shape.size() << " indexes were given.";

			throw py::index_error(message.str());
		}
		int n_rows;
		try
		{
			n_rows = shape[0].cast<size_t>();
		}
		catch(...)
		{
			std::stringstream message;
			message << "shape should be a positive integer vector.";

			throw py::index_error(message.str());
		}
		int n_cols;
		try
		{
			n_cols = shape[1].cast<size_t>();
		}
		catch(...)
		{
			std::stringstream message;
			message << "shape should be a positive integer vector.";

			throw py::index_error(message.str());
		}
		aff3ct::tools::Sparse_matrix * H = new aff3ct::tools::Sparse_matrix(n_rows, n_cols);
		return H;
	}, py::return_value_policy::take_ownership);
	scope.def("concatenate", [](py::tuple& arrays, size_t axis){
		aff3ct::tools::Sparse_matrix* H;
		try
		{
			H = new aff3ct::tools::Sparse_matrix(arrays[0].cast<aff3ct::tools::Sparse_matrix>());
		}
		catch(...)
		{
			std::stringstream message;
			message << "Item 0 of 'arrays' tuple cannot be casted into a py_aff3ct.tools.sparse_matrix.array.";
			throw py::value_error(message.str());
		}

		size_t n_rows = H->get_n_rows();
		size_t n_cols = H->get_n_cols();

		if (axis == 0)
		{
			for (size_t i =1; i<arrays.size(); i++)
			{
				aff3ct::tools::Sparse_matrix* H_;
				try
				{
					H_ = arrays[i].cast<aff3ct::tools::Sparse_matrix*>();
				}
				catch(...)
				{
					std::stringstream message;
					message << "Item " << i << " of 'arrays' tuple cannot be casted into a py_aff3ct.tools.sparse_matrix.array.";
					throw py::value_error(message.str());
				}

				int old_n_rows = n_rows;
				n_rows += H_->get_n_rows();
				if (H_->get_n_cols() !=  n_cols)
				{
					std::stringstream message;
					message << "all the input array dimensions for the concatenation axis must match exactly, but along dimension 1, the array at index " << i << " has size " << H_->get_n_cols() << " and the array at index " << i - 1 << " has size " << n_cols << ".";
					throw py::value_error(message.str());
				}
				H->self_resize(n_rows, n_cols, aff3ct::tools::Matrix::Origin::TOP_LEFT);
				for (size_t i = 0; i < H_->get_n_rows(); i++)
					for (size_t j = 0; j < H_->get_n_cols(); j++)
						if(H_->at(i,j) && !H->at(old_n_rows+i,j))
							H->add_connection(old_n_rows+i,j);
						else if(!H_->at(i,j) && H->at(old_n_rows+i,j))
							H->rm_connection(old_n_rows+i,j);
			}
		}
		else if (axis == 1)
		{
			for (size_t i =1; i<arrays.size(); i++)
			{
				aff3ct::tools::Sparse_matrix* H_;
				try
				{
					H_ = arrays[i].cast<aff3ct::tools::Sparse_matrix*>();
				}
				catch(...)
				{
					std::stringstream message;
					message << "Item " << i << " of 'arrays' tuple cannot be casted into a py_aff3ct.tools.sparse_matrix.array.";
					throw py::value_error(message.str());
				}

				int old_n_cols = n_cols;
				n_cols += H_->get_n_cols();
				if (H_->get_n_rows() !=  n_rows)
				{
					std::stringstream message;
					message << "all the input array dimensions for the concatenation axis must match exactly, but along dimension 0, the array at index " << i << " has size " << H_->get_n_rows() << " and the array at index " << i - 1 << " has size " <<  n_rows << ".";
					throw py::value_error(message.str());
				}
				H->self_resize(n_rows, n_cols, aff3ct::tools::Matrix::Origin::TOP_LEFT);
				for (size_t i = 0; i < H_->get_n_rows(); i++)
					for (size_t j = 0; j < H_->get_n_cols(); j++)
						if(H_->at(i,j) && !H->at(i,old_n_cols + j))
							H->add_connection(i,old_n_cols + j);
						else if(!H_->at(i,j) && H->at(i,old_n_cols + j))
							H->rm_connection(i,old_n_cols + j);
			}
		}
		return H;
	}, "arrays"_a, "axis"_a=0, py::return_value_policy::take_ownership);
}


void Wrapper_Sparse_matrix
::definitions()
{
	this->def(py::init<const size_t, const size_t>(),"n_rows"_a = 0, "n_cols"_a = 1, R"pbdoc()pbdoc", py::return_value_policy::take_ownership);
	this->def(py::init([](py::array_t<bool> &array){
		if (array.ndim() == 2)
		{
			size_t n_rows = array.shape(0);
			size_t n_cols = array.shape(1);
			tools::Sparse_matrix* ret_spm = new tools::Sparse_matrix(n_rows,n_cols);
			for (size_t i = 0; i < n_rows ; ++i)
				for (size_t j = 0; j < n_cols ; ++j)
					if(array.at(i,j))
						ret_spm->add_connection(i,j);
			return ret_spm;

		}
		else
		{
			throw std::runtime_error("Sparse_Matrix can only be built from a 2-dimensional boolean array.");
		}
	}), "array"_a = 1, R"pbdoc()pbdoc", py::return_value_policy::take_ownership);

	this->def("full", [](aff3ct::tools::Sparse_matrix& self) {
		auto arr = py::array_t<bool, py::array::c_style>({ self.get_n_rows(), self.get_n_cols() });
		auto f_arr = arr.mutable_unchecked<2>();
		for (size_t i = 0; i<self.get_n_rows(); i++ )
			for (size_t j = 0; j<self.get_n_cols(); j++ )
				f_arr(i,j) = self.at(i,j);
		return arr;
	},py::return_value_policy::copy);

	this->def("__getitem__", [](const aff3ct::tools::Sparse_matrix& self, const size_t& index) {

		aff3ct::tools::Sparse_matrix* ret_spm = new aff3ct::tools::Sparse_matrix(1, self.get_n_cols());

		for (size_t j = 0; j < self.get_n_cols(); ++j)
			if (self.at(index, j))
				ret_spm->add_connection(0,j);

		return ret_spm;
	}, py::return_value_policy::take_ownership);

	this->def("__getitem__", [](const aff3ct::tools::Sparse_matrix& self, const py::tuple& index) {
		if (index.size() < 1 )
		{
			std::stringstream message;
			message << "too few indices for array: array is 2-dimensional, but " << index.size() << " were indexed.";
			throw py::index_error(message.str());
		}

		if (index.size() > 2 )
		{
			std::stringstream message;
			message << "too many indices for array: array is 2-dimensional, but " << index.size() << " were indexed.";
			throw py::index_error(message.str());
		}

		py::slice si = Wrapper_Sparse_matrix::get_slice(index[0], self.get_n_rows());
		size_t starti = 0, stopi = 0, stepi = 0, slicelengthi = 0;
		if (!si.compute(self.get_n_rows(), &starti, &stopi, &stepi, &slicelengthi))
			throw py::error_already_set();

		py::slice sj;
		if (index.size() == 1)
			sj = py::slice(0, self.get_n_cols(),1);
		else
			sj = Wrapper_Sparse_matrix::get_slice(index[1], self.get_n_rows());

		size_t startj = 0, stopj = 0, stepj = 0, slicelengthj = 0;
		if (!sj.compute(self.get_n_cols(), &startj, &stopj, &stepj, &slicelengthj))
			throw py::error_already_set();

		aff3ct::tools::Sparse_matrix* ret_spm = new aff3ct::tools::Sparse_matrix(slicelengthi, slicelengthj);
		size_t read_i = starti;

		for (size_t i = 0; i < slicelengthi; ++i)
		{
			size_t read_j = startj;
			for (size_t j = 0; j < slicelengthj; ++j)
			{
				if (self.at(read_i, read_j))
					ret_spm->add_connection(i,j);
				read_j += stepj;
			}
			read_i += stepi;
		}
		return ret_spm;
	}, py::return_value_policy::take_ownership);

	this->def("__setitem__", [](aff3ct::tools::Sparse_matrix& self, const size_t& index, const aff3ct::tools::Sparse_matrix& value) {
		for (size_t j = 0; j < self.get_n_cols(); ++j)
		{
			if (value.at(0,j) && !self.at(index, j))
				self.add_connection(index, j);
			else if (!value.at(0,j) && self.at(index, j))
				self.rm_connection(index, j);
		}
	});

	this->def("__setitem__", [](aff3ct::tools::Sparse_matrix& self, const py::tuple& index, const aff3ct::tools::Sparse_matrix& value) {
		if (index.size() < 1 )
		{
			std::stringstream message;
			message << "too few indices for array: array is 2-dimensional, but " << index.size() << " were indexed.";
			throw py::index_error(message.str());
		}

		if (index.size() > 2 )
		{
			std::stringstream message;
			message << "too many indices for array: array is 2-dimensional, but " << index.size() << " were indexed.";
			throw py::index_error(message.str());
		}

		py::slice si = Wrapper_Sparse_matrix::get_slice(index[0], self.get_n_rows());
		size_t starti = 0, stopi = 0, stepi = 0, slicelengthi = 0;
		if (!si.compute(self.get_n_rows(), &starti, &stopi, &stepi, &slicelengthi))
			throw py::error_already_set();

		py::slice sj;
		if (index.size() == 1)
			sj = py::slice(0, self.get_n_cols(),1);
		else
			sj = Wrapper_Sparse_matrix::get_slice(index[1], self.get_n_rows());

		size_t startj = 0, stopj = 0, stepj = 0, slicelengthj = 0;
		if (!sj.compute(self.get_n_cols(), &startj, &stopj, &stepj, &slicelengthj))
			throw py::error_already_set();

		if (value.get_n_rows() != slicelengthi || value.get_n_cols() != slicelengthj)
		{
			std::stringstream message;
			message << " could not broadcast input array from shape (" << value.get_n_rows() << "," << value.get_n_cols() <<") into shape (" << slicelengthi << "," << slicelengthj << ").";
			throw py::index_error(message.str());
		}

		size_t write_i = starti;
		for (size_t i = 0; i < slicelengthi; ++i)
		{
			size_t write_j = startj;
			for (size_t j = 0; j < slicelengthj; ++j)
			{
				if (value.at(i,j))
				{
					if(!self.at(write_i, write_j))
						self.add_connection(write_i, write_j);
				}
				else
				{
					if(self.at(write_i, write_j))
						self.rm_connection(write_i, write_j);
				}
				write_j += stepj;
			}
			write_i += stepi;
		}
	});

	this->def("__setitem__", [](aff3ct::tools::Sparse_matrix& self, const py::tuple& index, const py::array_t<bool>& value) {
		if (index.size() < 1 )
		{
			std::stringstream message;
			message << "too few indices for array: array is 2-dimensional, but " << index.size() << " were indexed.";
			throw py::index_error(message.str());
		}

		if (index.size() > 2 )
		{
			std::stringstream message;
			message << "too many indices for array: array is 2-dimensional, but " << index.size() << " were indexed.";
			throw py::index_error(message.str());
		}

		py::slice si = Wrapper_Sparse_matrix::get_slice(index[0], self.get_n_rows());
		size_t starti = 0, stopi = 0, stepi = 0, slicelengthi = 0;
		if (!si.compute(self.get_n_rows(), &starti, &stopi, &stepi, &slicelengthi))
			throw py::error_already_set();

		py::slice sj;
		if (index.size() == 1)
			sj = py::slice(0, self.get_n_cols(),1);
		else
			sj = Wrapper_Sparse_matrix::get_slice(index[1], self.get_n_rows());

		size_t startj = 0, stopj = 0, stepj = 0, slicelengthj = 0;
		if (!sj.compute(self.get_n_cols(), &startj, &stopj, &stepj, &slicelengthj))
			throw py::error_already_set();

		if (value.shape(0) != slicelengthi || value.shape(1) != slicelengthj)
		{
			std::stringstream message;
			message << " could not broadcast input array from shape (" << value.shape(0) << "," << value.shape(1) <<") into shape (" << slicelengthi << "," << slicelengthj << ").";
			throw py::index_error(message.str());
		}

		size_t write_i = starti;
		for (size_t i = 0; i < slicelengthi; ++i)
		{
			size_t write_j = startj;
			for (size_t j = 0; j < slicelengthj; ++j)
			{
				if (value.at(i,j))
				{
					if(!self.at(write_i, write_j))
						self.add_connection(write_i, write_j);
				}
				else
				{
					if(self.at(write_i, write_j))
						self.rm_connection(write_i, write_j);
				}
				write_j += stepj;
			}
			write_i += stepi;
		}
	});

	this->def("__setitem__", [](aff3ct::tools::Sparse_matrix& self, const py::tuple& index, const bool& value) {
		if (index.size() < 1 )
		{
			std::stringstream message;
			message << "too few indices for array: array is 2-dimensional, but " << index.size() << " were indexed.";
			throw py::index_error(message.str());
		}

		if (index.size() > 2 )
		{
			std::stringstream message;
			message << "too many indices for array: array is 2-dimensional, but " << index.size() << " were indexed.";
			throw py::index_error(message.str());
		}

		py::slice si = Wrapper_Sparse_matrix::get_slice(index[0], self.get_n_rows());
		size_t starti = 0, stopi = 0, stepi = 0, slicelengthi = 0;
		if (!si.compute(self.get_n_rows(), &starti, &stopi, &stepi, &slicelengthi))
			throw py::error_already_set();

		py::slice sj;
		if (index.size() == 1)
			sj = py::slice(0, self.get_n_cols(),1);
		else
			sj = Wrapper_Sparse_matrix::get_slice(index[1], self.get_n_rows());

		size_t startj = 0, stopj = 0, stepj = 0, slicelengthj = 0;
		if (!sj.compute(self.get_n_cols(), &startj, &stopj, &stepj, &slicelengthj))
			throw py::error_already_set();

		size_t write_i = starti;

		for (size_t i = 0; i < slicelengthi; ++i)
		{
			size_t write_j = startj;
			for (size_t j = 0; j < slicelengthj; ++j)
			{
				if (value)
				{
					if(!self.at(write_i, write_j))
						self.add_connection(write_i, write_j);
				}
				else
				{
					if(self.at(write_i, write_j))
						self.rm_connection(write_i, write_j);
				}
				write_j += stepj;
			}
			write_i += stepi;
		}
	});

	this->def("__str__",[](aff3ct::tools::Sparse_matrix& self){
		std::stringstream message;
		message << "Sparse_matrix of size " << self.get_n_rows() << "x" << self.get_n_cols()<<".\nConnections:\n";

		for (size_t i = 0; i < self.get_n_rows(); ++i)
			for (size_t j = 0; j < self.get_n_cols(); ++j)
				if (self.at(i,j))
					message << "\t(" << i <<","<< j <<")\n";
		return message.str();
	});

	this->def("transpose",[](const aff3ct::tools::Sparse_matrix& self){
		return self.transpose();
	});
	this->def_property_readonly("shape", [](const aff3ct::tools::Sparse_matrix& self){
		return py::make_tuple(self.get_n_rows(),self.get_n_cols());
	});
}

py::slice Wrapper_Sparse_matrix
::get_slice(const py::object& obj, const size_t len)
{
	if (py::isinstance<py::int_>(obj))
	{
		int i_sj = obj.cast<int>();
		size_t ui_sj;
		if ( i_sj < 0 )
			ui_sj = len - (abs(i_sj) % len);
		else
			ui_sj = i_sj % len;
		py::slice sjj = py::slice(ui_sj, ui_sj+1, 1);

		return sjj;
	}
	else if(py::isinstance<py::slice>(obj))
	{
		return py::slice(obj);
	}
	else
	{
		throw std::runtime_error("Sparse_Matrix only accepts integer or slice indexing.");
	}
}
