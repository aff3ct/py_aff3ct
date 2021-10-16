# -*- coding: utf-8 -*-
from pathlib import Path
import os
import re
import filecmp
from subprocess import call
import copy

class bcolors:
    HEADER    = '\033[95m'
    OKBLUE    = '\033[94m'
    OKCYAN    = '\033[96m'
    OKGREEN   = '\033[92m'
    WARNING   = '\033[93m'
    FAIL      = '\033[91m'
    ENDC      = '\033[0m'
    BOLD      = '\033[1m'
    UNDERLINE = '\033[4m'

def make_dir_tree(modules, verbose = False):
	for _,module in modules.items():
		folder_path = module['mk_dir_path']
		if not Path(folder_path).is_dir():
			Path(folder_path).mkdir(parents=True, exist_ok=True)
			if verbose:
				print("Creating folder : " + folder_path)

def write_hpp_wrappers(modules, template_path, verbose = False):
	for _,module in modules.items():
		if (("Decoder_LDPC_BP_horizontal_layered" in module['short_name']
		or   "Decoder_LDPC_BP_vertical_layered"   in module['short_name']
		or   "Decoder_LDPC_BP_flooding"           in module['short_name'])
		and ("ONMS" not in module['short_name'] and "Gallager" not in module['short_name'])):
			write_LDPC_BP_HL_hpp_wrapper(template_path, module['mk_dir_path'], module['short_name'], verbose)
			continue

		wrapper_hpp = ""
		with open(template_path + "/Wrapper_template.hpp","r") as f:
			wrapper_hpp = f.read()
			if 'parent' in module.keys() and module['parent']:
				wrapper_hpp = wrapper_hpp.replace("{parent}", ", " + module['parent'])
			else:
				wrapper_hpp = wrapper_hpp.replace("{parent}", "")
			wrapper_hpp = wrapper_hpp.replace("{SHORT_NAME}"      , module['short_name'     ].upper())
			wrapper_hpp = wrapper_hpp.replace("{short_name}"      , module['short_name'     ]        )
			wrapper_hpp = wrapper_hpp.replace("{name}"            , module['name'           ]        )
			wrapper_hpp = wrapper_hpp.replace("{full_template}"   , module['template']['full'   ]    )
			wrapper_hpp = wrapper_hpp.replace("{medium_template}" , module['template']['medium' ]    )
			wrapper_hpp = wrapper_hpp.replace("{short_template}"  , module['template']['short'  ]    )
			wrapper_hpp = wrapper_hpp.replace("{default_template}", module['template']['default']    )
			wrapper_hpp = wrapper_hpp.replace("{dtor_trick}"      , module['dtor_trick'     ]        )


		file_path = module['mk_dir_path'] + "/" + module['short_name'] + ".hpp"
		write_if_different(file_path, wrapper_hpp, verbose)

def write_LDPC_BP_HL_hpp_wrapper(template_path, mk_dir_path, short_name, verbose):
	types              = [ "MS", "OMS",       "NMS",            "SPA",           "LSPA"]

	for i in range(len(types)):
		wrapper_hpp = ""
		with open(template_path + "/special/Wrapper_Decoder_LDPC_BP_horizontal_layered_template.hpp","r") as f:
			wrapper_hpp = f.read()
			if "inter" in short_name:
				wrapper_hpp = wrapper_hpp.replace("{simd}", "_simd")
			else:
				wrapper_hpp = wrapper_hpp.replace("{simd}", "")

			wrapper_hpp = wrapper_hpp.replace("{type}", types[i])
			wrapper_hpp = wrapper_hpp.replace("{TYPE}", types[i].upper())
			wrapper_hpp = wrapper_hpp.replace("{short_name}", short_name)
			wrapper_hpp = wrapper_hpp.replace("{SHORT_NAME}", short_name.upper())
			file_path = mk_dir_path + "/" + short_name + "_" + types[i] + ".hpp"
			write_if_different(file_path, wrapper_hpp, verbose)

def write_LDPC_BP_HL_cpp_wrapper(template_path, mk_dir_path, include_path, short_name, verbose):
	types              = [ "MS", "OMS",       "NMS",            "SPA",           "LSPA"]
	update_rule_args   = [ "",   "(R)offset", "(R)norm_factor", "max_CN_degree", "max_CN_degree"]
	max_CN_degree_decl = "const auto max_CN_degree = (unsigned int)H.get_cols_max_degree();\n"
	offset_decl        = "const float offset, "
	norm_factor_decl   = "const float norm_factor, "

	for i in range(len(types)):
		wrapper_cpp = ""
		with open(template_path + "/special/Wrapper_Decoder_LDPC_BP_horizontal_layered_template.cpp","r") as f:
			wrapper_cpp = f.read()
			wrapper_cpp = wrapper_cpp.replace("{short_name}", short_name)
			wrapper_cpp = wrapper_cpp.replace("{path}", include_path)
			wrapper_cpp = wrapper_cpp.replace("{type}", types           [i])
			wrapper_cpp = wrapper_cpp.replace("{args}", update_rule_args[i])
			if "inter" in short_name:
				wrapper_cpp = wrapper_cpp.replace("{simd}", "_simd")
			else:
				wrapper_cpp = wrapper_cpp.replace("{simd}", "")

			if update_rule_args[i] == "(R)offset":
				wrapper_cpp = wrapper_cpp.replace("{offset_decl}", offset_decl)
				wrapper_cpp = wrapper_cpp.replace("{offset_arg}", '"offset"_a=(R)0.0f, ')
			else:
				wrapper_cpp = wrapper_cpp.replace("{offset_decl}", "")
				wrapper_cpp = wrapper_cpp.replace("{offset_arg}", '')

			if update_rule_args[i] == "(R)norm_factor":
				wrapper_cpp = wrapper_cpp.replace("{norm_factor_decl}", norm_factor_decl)
				wrapper_cpp = wrapper_cpp.replace("{norm_factor_arg}", '"norm_factor"_a=(R)1.0f, ')
			else:
				wrapper_cpp = wrapper_cpp.replace("{norm_factor_decl}", "")
				wrapper_cpp = wrapper_cpp.replace("{norm_factor_arg}", '')

			if update_rule_args[i] == "max_CN_degree":
				wrapper_cpp = wrapper_cpp.replace("{max_CN_degree_decl}", max_CN_degree_decl)
			else:
				wrapper_cpp = wrapper_cpp.replace("{max_CN_degree_decl}", "")

			file_path = mk_dir_path + "/" + short_name + "_" + types[i] + ".cpp"
			write_if_different(file_path, wrapper_cpp, verbose)

def write_cpp_wrappers(modules, template_path, verbose = False):
	for _,module in modules.items():
		if (("Decoder_LDPC_BP_horizontal_layered" in module['short_name']
		or   "Decoder_LDPC_BP_vertical_layered"   in module['short_name']
		or   "Decoder_LDPC_BP_flooding"           in module['short_name'])
		and ("ONMS" not in module['short_name'] and "Gallager" not in module['short_name'])):
			write_LDPC_BP_HL_cpp_wrapper(template_path, module['mk_dir_path'], module['include_path'], module['short_name'], verbose)
			continue
		init_lines = ""
		wrapper_cpp = ""

		class_constructors = module['constructors']
		if not module['is_abstract']:
			for constructor in class_constructors:
				arg_types = ""
				arg_init  = ""
				new_line  = '\n\tthis->def(py::init<{types}>(){init}, R"pbdoc(' + constructor['doc']+ ')pbdoc", py::return_value_policy::take_ownership);'
				arg_nbr = len(constructor['args'])
				for a_idx in range(arg_nbr):
					arg_types += constructor['args'][a_idx]['type'] + ", "
					arg_init  += '"' + constructor['args'][a_idx]['name'] +'"_a' + constructor['args'][a_idx]['default'] + ', '
				arg_types = arg_types[:-2]
				arg_init  = arg_init [:-2]
				new_line  = new_line.replace("{types}", arg_types)
				if arg_init:
					arg_init = "," + arg_init

				new_line  = new_line.replace("{init}", arg_init)
				init_lines += new_line
		if init_lines:
			init_lines += '\n'

		def_lines = ""
		if module['short_name'] == 'Encoder':
			def_lines += '\n\tthis->def("get_info_bits_pos", &Encoder<B>::get_info_bits_pos);\n'
		if 'definitions' in module.keys():
			for def_ in module['definitions']:
				if module['short_name'] == 'Decoder' and def_['short_name'] == 'reset':
					def_lines += 'this->def("reset", [](aff3ct::module::Decoder& self){self.reset();});'
				else:
					def_lines += '\n\tthis->def("' + def_['short_name'] + '"'
					def_lines += ', &' + module['short_name'] + module['template']['short'] + '::' + def_['short_name'] + ');'

		if 'tasks_doc' in module.keys():
			if module["tasks_doc"]:
				def_lines += '\n\tthis->def_property_readonly("tasks", [](' + module['short_name'] + module['template']['short'] + '& self)-> std::vector<std::shared_ptr<Task>> { return self.tasks; },R"pbdoc(List of tasks:\n\n'+ module["tasks_doc"]+ ')pbdoc");'

		if 'class_doc' in module.keys():
			if module["class_doc"]:
				def_lines += '\n\tthis->doc() = R"pbdoc('+ module["class_doc"]+ ')pbdoc";'


		if def_lines:
			def_lines += '\n'

		wrapper_cpp = ""
		with open(template_path + "/Wrapper_template.cpp","r") as f:
			wrapper_cpp = f.read()
			if 'parent' in module.keys() and module['parent']:
				wrapper_cpp = wrapper_cpp.replace("{parent}", "," + module['parent'])
			else:
				wrapper_cpp = wrapper_cpp.replace("{parent}", "")

			if module['has_template']:
				footer = '#include "Tools/types.h"\ntemplate class aff3ct::wrapper::Wrapper_'+ module['short_name'] + module['template']['default']+';'
			else:
				footer = ''
			wrapper_cpp = wrapper_cpp.replace("{short_name}",       module['short_name'         ])
			wrapper_cpp = wrapper_cpp.replace("{name}",             module['name'               ])
			wrapper_cpp = wrapper_cpp.replace("{short_template}",   module['template']['short'  ])
			wrapper_cpp = wrapper_cpp.replace("{medium_template}" , module['template']['medium' ])
			wrapper_cpp = wrapper_cpp.replace("{default_template}", module['template']['default'])
			wrapper_cpp = wrapper_cpp.replace("{footer}",           footer                       )
			wrapper_cpp = wrapper_cpp.replace("{path}",             module['include_path'       ])
			wrapper_cpp = wrapper_cpp.replace("{init_lines}",       init_lines                   )
			wrapper_cpp = wrapper_cpp.replace("{def_lines}",        def_lines                    )
			wrapper_cpp = wrapper_cpp.replace("{dtor_trick}"      , module['dtor_trick'         ])

		file_path = module['mk_dir_path'] + "/" + module['short_name'] + ".cpp"
		write_if_different(file_path, wrapper_cpp, verbose)

def gen_wrapper_cpp_lines_LDPC_BP_HL(module_info, the_mod):
	types = [ "MS", "OMS", "NMS", "SPA", "LSPA"]
	line = ""
	for ty in types:
		new_module_info = copy.deepcopy(module_info)
		new_module_info["short_name"] += "_" + ty
		line += gen_wrapper_cpp_line(new_module_info, the_mod)
	return line


def gen_wrapper_cpp_line(module_info, the_mod):
	line = '\tstd::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_'
	if module_info['has_template']:
		line +=  module_info["short_name"].lower() + '(new aff3ct::wrapper::Wrapper_' + module_info["short_name"] + '<>(' + the_mod + '));\n'
	else:
		line +=  module_info["short_name"].lower() + '(new aff3ct::wrapper::Wrapper_' + module_info["short_name"] + '(' + the_mod + '));\n'

	line += '\twrappers.push_back(wrapper_' + module_info["short_name"].lower() + '.get()); \n\n'
	return line

def gen_py_aff3ct_cpp(modules, tree, mod, root_mod, cpp_content):
	for key, value in tree.items():
		if key in modules:
			module_info = modules[key]
			the_mod = mod
			if not module_info["leaf"] and mod == root_mod and mod == 'm0':
				cpp_content += '\tpy::module_ mod_' + module_info['short_name'].lower() + " = " + mod + '.def_submodule("' +  module_info['short_name'].lower() + '");\n'
				the_mod = 'mod_' + module_info['short_name'].lower()
				cpp_content += '\t' + the_mod + '.doc() = ' + 'R"pbdoc(AFF3CT Tools inheritating from ' + module_info['short_name'] + '.)pbdoc";\n'
				cpp_content += '\t' + 'doc_m0 += R"pbdoc(           ' + module_info['short_name'].lower() + '\n' + ')pbdoc";\n'
			elif mod == root_mod and mod == 'm1':
				cpp_content += '\tpy::module_ mod_' + module_info['short_name'].lower() + " = " + mod + '.def_submodule("' +  module_info['short_name'].lower() + '");\n'
				the_mod = 'mod_' + module_info['short_name'].lower()
				cpp_content += '\t' + the_mod + '.doc() = ' + 'R"pbdoc(AFF3CT Modules inheritating from ' + module_info['short_name'] + '.)pbdoc";\n'
				cpp_content += '\t' + 'doc_m1 += R"pbdoc(           ' + module_info['short_name'].lower() + '\n' + ')pbdoc";\n'

			if (("Decoder_LDPC_BP_horizontal_layered" in module_info['short_name']
			or   "Decoder_LDPC_BP_vertical_layered"   in module_info['short_name']
			or   "Decoder_LDPC_BP_flooding"           in module_info['short_name'])
			and ("ONMS" not in module_info['short_name'] and "Gallager" not in module_info['short_name'])):
				cpp_content += gen_wrapper_cpp_lines_LDPC_BP_HL(module_info, the_mod)
			else:
				cpp_content += gen_wrapper_cpp_line(module_info, the_mod)

			if not module_info["leaf"]:
				cpp_content = gen_py_aff3ct_cpp(modules, value, the_mod, root_mod, cpp_content)

	return cpp_content

def write_py_aff3ct_cpp(tools, tools_tree, modules, module_tree, command_path, template_path, verbose = False):
	other_tool_wrappers   = gen_py_aff3ct_cpp(tools, tools_tree, 'm0', 'm0', '')
	other_module_wrappers = gen_py_aff3ct_cpp(modules, module_tree, 'm1', 'm1', '')
	py_aff3ct_cpp = ""
	with open(template_path + "/py_aff3ct_template.cpp","r") as f:
		py_aff3ct_cpp = f.read()
		py_aff3ct_cpp = py_aff3ct_cpp.replace("{other_tool_wrappers}", other_tool_wrappers[0:-2])
		py_aff3ct_cpp = py_aff3ct_cpp.replace("{other_module_wrappers}", other_module_wrappers[0:-2])

	file_path = command_path + "/src/py_aff3ct.cpp"
	write_if_different(file_path, py_aff3ct_cpp, verbose)

def write_py_aff3ct_hpp(modules, command_path, template_path, verbose = False):
	other_includes = gen_py_aff3ct_hpp(modules)
	with open(template_path + "/py_aff3ct_template.hpp","r") as f:
			py_aff3ct_hpp = f.read()
			py_aff3ct_hpp = py_aff3ct_hpp.replace("{other_includes}", other_includes)
	file_path = command_path + "/src/py_aff3ct.hpp"
	write_if_different(file_path, py_aff3ct_hpp, verbose)

def gen_include_line_LDPC_BP_HL_hpp(module):
	types = [ "MS", "OMS", "NMS", "SPA", "LSPA"]
	line = ""
	for t in types:
		line += '#include "' + module['include_path'] + '/' + module['short_name'] + '_' + t + '.hpp"\n'
	return line

def gen_include_line_hpp(module):
	return '#include "' + module['include_path'] + '/' + module['short_name'] + '.hpp"\n'

def gen_py_aff3ct_hpp(modules):
	hpp_content = ""
	for _,module in modules.items():
		if (("Decoder_LDPC_BP_horizontal_layered" in module['short_name']
		or   "Decoder_LDPC_BP_vertical_layered"   in module['short_name']
		or   "Decoder_LDPC_BP_flooding"           in module['short_name'])
		and ("ONMS" not in module['short_name'] and "Gallager" not in module['short_name'])):
			hpp_content += gen_include_line_LDPC_BP_HL_hpp(module)
		else:
			hpp_content += gen_include_line_hpp(module)
	return hpp_content

def write_if_different(file_path, new_content, verbose):
	old_content = ""
	if Path(file_path).is_file():
		with open(file_path,"r") as f:
			old_content = f.read()

	if not old_content == new_content:
		with open(file_path,"w") as f:
			if verbose and not old_content:
				print("Creating file : " + file_path)
			else:
				print("Updating file : " + file_path)
			f.write(new_content)

def src_repair(template_src_path, src_path, verbose):
	if Path(src_path).is_dir():
		for tplt_currentpath, folders, files in os.walk(template_src_path):
			#
			src_currentpath = tplt_currentpath.replace(template_src_path, src_path)
			if not Path(src_currentpath).is_dir():
				if verbose:
					print("Folder not found ", src_currentpath, ", replaced by ", tplt_currentpath)
				call('cp -R ' + tplt_currentpath + ' ' + src_currentpath, shell=True)
			else:
				for file in files:
					template_file_path = os.path.join(tplt_currentpath, file)
					file_path = template_file_path.replace(template_src_path, src_path)
					if Path(file_path).is_file():
						if not filecmp.cmp(template_file_path, file_path):
							if verbose:
								print("Detected change in ", file_path, ", it will be replaced by ", template_file_path)
							call('cp -f ' + template_file_path + ' ' + file_path, shell=True)
					else:
						if verbose:
							print("File not found ", file_path, ", it will be replaced by ", template_file_path)

						call('cp ' + template_file_path + ' ' + file_path, shell=True)
	else:
		if Path(template_src_path).is_dir():
			if verbose:
				print("Copy folder ", template_src_path, ", into ", src_path)
			call('cp -R ' + template_src_path + ' ' + src_path, shell=True)
		else:
			print("Template folder not found.")

def doxyname_to_stdname(doxyname):
	stdname = re.sub("^class",  "",   doxyname)
	stdname = re.sub("^struct", "",   stdname)
	stdname = re.sub("_1_1",    "::", stdname)
	stdname = re.sub("_3_01",   "< ", stdname)
	stdname = re.sub("_01_4",   " >", stdname)
	stdname = re.sub("__",      "_",  stdname)
	stdname = re.sub(".xml",    "",   stdname)
	return stdname

def recursive_build_classes_list(data, include_list, exclude_list, prefix, tree = {}):
	classes_list = []
	for i in include_list:
		class_name = prefix + i
		if class_name in data.keys() and i not in exclude_list:
			classes_list.append(class_name)
			tree[class_name] = {}
			if "derivedcompoundref" in data[class_name]["compounddef"]:
				dc_list = data[class_name]["compounddef"]["derivedcompoundref"]
				if type(dc_list) is not list:
					dc_list = [dc_list]
				sub_include_list = []
				for dc in dc_list:
					sub_include_list.append(dc["#text"].split("::")[2].split("<")[0])
				# recursive call on the derived classes
				classes_list = classes_list + recursive_build_classes_list(data, sub_include_list, exclude_list, prefix, tree[class_name])
	return classes_list

def get_name(entry):
	val = entry["compounddef"]["compoundname"]
	return val

def get_short_name(entry):
	name = get_name(entry)
	short_name = name.split("::")[2]
	return short_name

def is_abstract(entry):
	return "@abstract" in entry["compounddef"].keys()

def has_template(entry):
	return "templateparamlist" in entry["compounddef"].keys()

def gen_parent_template(entry, parent):
	short   = ""
	if has_template(parent):
		parent_templates_list = parent["compounddef"]["templateparamlist"]["param"]
		if type(parent_templates_list) is not list:
			parent_templates_list = [parent_templates_list]
	else:
		return short

	entry_templates_list = []
	if has_template(entry):
		entry_templates_list = entry["compounddef"]["templateparamlist"]["param"]
		if type(entry_templates_list) is not list:
			entry_templates_list = [entry_templates_list]

	short = "<"
	for ptp in parent_templates_list:
		if "defname" in ptp.keys():
			ptp_name = ptp["defname"]
		else:
			ptp_name = ptp["type"].split(" ")[1]

		if not "defval" in ptp.keys():
			message = bcolors.FAIL + "(EE) Class and children classes not wrapped. Class " + get_short_name(entry) + " has a template without default value." + bcolors.ENDC
			raise RuntimeError(message)

		ptp_defval = ptp["defval"]

		exist = False
		for etp in entry_templates_list:
			if "defname" in etp.keys():
				etp_name = etp["defname"]
			else:
				etp_name = etp["type"].split(" ")[1]

			if etp_name == ptp_name:
				exist = True
				break

		if exist:
			short += ptp_name + ","
		else:
			short += ptp_defval + ","

	short = short[:-1] + ">"

	return short

def gen_template(entry):
	full    = ""
	medium  = ""
	short   = ""
	default = ""

	if has_template(entry):

		templates_list = entry["compounddef"]["templateparamlist"]["param"]
		if type(templates_list) is not list:
			templates_list = [templates_list]

		default = "<"
		for tp in templates_list:
			if not "defval" in tp.keys():
				message = bcolors.FAIL + "(EE) Class and children classes not wrapped. Class " + get_short_name(entry) + " has a template without default value." + bcolors.ENDC
				raise RuntimeError(message)

			t = tp["defval"]
			default += t + ","
		default = default[:-1] + ">"

		short = "<"
		for tp in templates_list:
			if "defname" in tp.keys():
				short += tp["defname"] + ","
			else:
				short += tp["type"].split(" ")[1] + ","
		short = short[:-1] + ">"

		medium = "template <"
		for tp in templates_list:
			if "defname" in tp.keys():
				medium += tp["type"] + " " + tp["defname"]
			else:
				medium += tp["type"]
			medium += ","
		medium = medium[:-1] + ">"

		full = "template <"
		for tp in templates_list:
			if "defname" in tp.keys():
				full += tp["type"] + " " + tp["defname"] + " = " + tp["defval"] + ","
			else:
				full += tp["type"] + " = " + tp["defval"] + ","
		full = full[:-1] + ">"

	return {"full":    full,
	        "medium":  medium,
	        "short":   short,
	        "default": default}

def get_parent(entry, dictio, classes_list):
	if "basecompoundref" in entry["compounddef"].keys():
		if type(entry["compounddef"]["basecompoundref"]) is not list:
			val = entry["compounddef"]["basecompoundref"]
		else:
			val = entry["compounddef"]["basecompoundref"][0]

		val_notp = val["#text"].split("<")[0]
		if "aff3ct::module::" + val_notp in classes_list:
			val_notp = "aff3ct::module::" + val_notp
		if "aff3ct::tools::" + val_notp in classes_list:
			val_notp = "aff3ct::tools::" + val_notp
		if val_notp in classes_list:
			if (has_template(dictio[val_notp])):
				#tp = gen_template(dictio[val_notp])
				#tp_str = tp["short"]
				tp_str = gen_parent_template(entry, dictio[val_notp])
				return val_notp + tp_str
			else:
				return val_notp
		# hack for the modules...
		elif "aff3ct::module::" in get_name(entry):
			return "aff3ct::module::Module"
		else:
			return ""
	return ""

def get_dtor_trick(entry):
	dtor_trick = ""

	sectiondef_list = entry["compounddef"]["sectiondef"]
	if type(sectiondef_list) is not list:
		sectiondef_list = [sectiondef_list]

	dtor_name = "~" + get_short_name(entry)
	for sd in sectiondef_list:
		if (sd["@kind"] == "private-func"):
			memberdef_list = sd["memberdef"]
			if type(memberdef_list) is not list:
				memberdef_list = [memberdef_list]
			for mb in memberdef_list:
				if mb["name"] == dtor_name:
					message = bcolors.OKBLUE + "(II) Protected destructor for class " + get_name(entry) + '.'+ bcolors.ENDC
					print(message)
					dtor_trick = ", std::unique_ptr<" + get_name(entry) + gen_template(entry)['short'] + ", py::nodelete>"
	return dtor_trick

def gen_constructors(entry, existing_tools):
	sectiondef_list = entry["compounddef"]["sectiondef"]
	if type(sectiondef_list) is not list:
		sectiondef_list = [sectiondef_list]

	class_name = get_short_name(entry)

	constructors = []
	for sd in sectiondef_list:
		if (sd["@kind"] == "public-func"):
			memberdef_list = sd["memberdef"]
			if type(memberdef_list) is not list:
				memberdef_list = [memberdef_list]

			for mb in memberdef_list:
				if mb["name"] == class_name and "param" in mb.keys():
					args = []
					params_list = mb["param"]
					if type(params_list) is not list:
						params_list = [params_list]
					tools_available = True
					for a in params_list:
						name_val = ""
						if "declname" in a.keys():
							name_val = a["declname"]

						type_val = ""
						if "type" in a.keys():
							type_val = a["type"]
							# trick to skip unsupported constructors
							if "tools::" in type_val:
								tools_available = False
								for et in existing_tools:
									ets = re.sub("^aff3ct::", "", et)
									if ets in type_val:
										tools_available = True
										break
								if not tools_available:
									message = bcolors.WARNING + "(WW) Constructor not wrapped for class '" + get_name(entry) + "' -> argument '" + name_val + "' has unknown type '" + type_val + "'." + bcolors.ENDC
									print(message)
									continue
							# end of the trick
						default_val = ""
						if "defval" in a.keys():
							default_val = " = " + a["defval"]

						args.append({
							"name": name_val,
							"type": type_val,
							"default": default_val
						})
						doc_str = extract_contructor_doc(mb)
					if not tools_available:
						continue

					constructors.append({"args": args, "doc": doc_str})
	return constructors

def extract_contructor_doc(mb):
	doc_str = ''
	if "detaileddescription" in mb.keys():
		if mb["detaileddescription"]:
			desc = mb['detaileddescription']['para']['parameterlist']['parameteritem']
			if type(desc) is not list:
				desc = [desc]
			bullet = ""
			if len(desc) > 1:
				bullet = "* "
			for item in desc:
				doc_str += bullet + item['parameternamelist']['parametername'] + ': ' + item['parameterdescription']['para'] + '\n'
	return doc_str

def is_leaf(entry):
	return "derivedcompoundref" not in entry["compounddef"]

def get_include_path(entry, prefix):
	dbdir,_ = os.path.split(entry["compounddef"]["location"]["@file"])
	if prefix:
		return prefix + "/" + dbdir
	else:
		return dbdir

def gen_definitions(entry, dictio):
	defs = []
	if "basecompoundref" in entry["compounddef"]:
		refs_list = entry["compounddef"]["basecompoundref"]
		if type(refs_list) is not list:
			refs_list = [refs_list]
		for ref in refs_list:
			if 'aff3ct::tools::Interface' in ref["#text"]:
				interface = dictio[ref["#text"].split("<")[0]]
				sd_list = interface["compounddef"]["sectiondef"]
				if type(sd_list) is not list:
					sd_list = [sd_list]
				for sd in sd_list:
					if sd["@kind"] == "public-func":
						mb_list = sd["memberdef"]
						if type(mb_list) is not list:
							mb_list = [mb_list]
						for mb in mb_list:
							defs.append({"short_name": mb["name"]})
							message = bcolors.OKGREEN + "(II) Definition '" + mb["name"] + "' added to module '" + get_name(entry) + "'." + bcolors.ENDC
							print (message)
	return defs

def gen_tasks_doc(entry):
	tasks_doc = ""
	is_module = False
	if "basecompoundref" in entry["compounddef"]:
		refs_list = entry["compounddef"]["basecompoundref"]
		if type(refs_list) is not list:
			refs_list = [refs_list]
		for ref in refs_list:
			if 'aff3ct::module::' in ref["#text"]:
				is_module = True
				break
	if is_module:
		sd_list = entry["compounddef"]["sectiondef"]
		if type(sd_list) is not list:
			sd_list = [sd_list]
		for sd in sd_list:
			if sd["@kind"] == "public-func":
				mb_list = sd["memberdef"]
				if type(mb_list) is not list:
					mb_list = [mb_list]
				for mb in mb_list:
					if "briefdescription" in mb:
						if mb["briefdescription"]:
							brief = mb["briefdescription"]["para"]
							if 'Task method' in brief:
								if tasks_doc:
									tasks_doc += '\n\n'
								tasks_doc += "* **" + mb['name'] + "**: " +brief
								if mb["detaileddescription"]:
									para = mb["detaileddescription"]["para"]
									if type(para) is not list:
										para = [para]
									for p in para:
										if type(p) is str:
											tasks_doc += ' ' + p + '\n\n'
										else:
											if tasks_doc[-1] != '\n':
												tasks_doc += '\n\n'
											desc = p["parameterlist"]["parameteritem"]
											if type(desc) is not list:
												desc = [desc]
											for item in desc:
												tasks_doc += "  * " + item['parameternamelist']['parametername'] + ': ' + item['parameterdescription']['para'] + '\n'
	return tasks_doc
def gen_class_doc(entry):
	class_doc = ""
	if "briefdescription" in entry["compounddef"]:
		if entry["compounddef"]["briefdescription"]:
			class_doc = entry["compounddef"]["briefdescription"]['para']
			if entry["compounddef"]["detaileddescription"]:
				para = entry["compounddef"]["detaileddescription"]["para"]
				if type(para) is not list:
					para = [para]
				for p in para:
					if type(p) is str:
						class_doc += p + '\n'
					else:
						desc = p["parameterlist"]["parameteritem"]
						if type(desc) is not list:
							desc = [desc]

						class_doc += '\n\nParameters:\n\n'
						for item in desc:
							class_doc += "  * " + item['parameternamelist']['parametername'] + ': ' + item['parameterdescription']['para'] + '\n'
	return class_doc

def build_modules(data, install_path, path, classes_list, existing_tools = {}):
	dictio = {}
	for class_name in classes_list:
		dictio[class_name] = {"name":         get_name        (data[class_name]),
		                      "short_name":   get_short_name  (data[class_name]),
		                      "is_abstract":  is_abstract     (data[class_name]),
		                      "has_template": has_template    (data[class_name]),
		                      "template":     gen_template    (data[class_name]),
		                      "dtor_trick":   get_dtor_trick  (data[class_name]),
		                      "leaf":         is_leaf         (data[class_name]),
		                      "parent":       get_parent      (data[class_name], data, classes_list),
		                      "definitions":  gen_definitions (data[class_name], data),
		                      "include_path": get_include_path(data[class_name], path),
		                      "constructors": gen_constructors(data[class_name], existing_tools),
		                      "mk_dir_path":  get_include_path(data[class_name], install_path + "/" + path),
		                      "tasks_doc":    gen_tasks_doc   (data[class_name]),
							  "class_doc":    gen_class_doc   (data[class_name])
							  }
	return dictio
