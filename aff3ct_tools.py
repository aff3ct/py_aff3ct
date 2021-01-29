# -*- coding: utf-8 -*-
from pathlib import Path
import os
import re
import filecmp
from subprocess import call

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

def build_inheritence_tree(data, template, include, exclude, tree, use_flt):
	for key, value in data.items():
		res = [s for s in include if s == value["class_short_name"]]
		if not res and use_flt:
			continue

		res = [s for s in exclude if s in value["class_short_name"]]
		if res:
			continue

		if("tool" in template):
			tree[key] = {}
			tree[key] = build_inheritence_tree(data, value["class_short_name"], include, exclude, tree[key], False)
		elif "class_inheritence" in value:
			for s in value["class_inheritence"]:
				short_class_inh = s.replace("public ", "")
				short_class_inh = short_class_inh.split("::")[-1]
				short_class_inh = short_class_inh.split('<')[0]
				if (template == short_class_inh):
					tree[key] = {}
					tree[key] = build_inheritence_tree(data, value["class_short_name"], include, exclude, tree[key], False)
					break

	return tree

def compute_tree_str(tree, str, prefix):
	for key,value in tree.items():
		str += prefix + "|- " + key + "\n"
		str +=  compute_tree_str(value, "", "\t" + prefix )
	if tree:
		return str
	else:
		return ""

def print_tree(tree):
	print(compute_tree_str(tree, "", ""))

def build_modules(data, tree, base, modules, install_path, path, existing_tools, defs):
	for key, value in tree.items():
		class_info   = data[key]
		short_name   = class_info["class_short_name"]
		name		 = class_info["class_name"]
		if "class_templates" in class_info.keys():
			has_template = class_info["class_nb_templates"] > 0
		else:
			has_template = False
		full_template   = ""
		medium_template = ""
		short_template  = ""
		default_template  = ""

		module_info = {}
		module_info['name'] = name
		module_info['short_name' ] = short_name
		module_info['is_abstract'] = class_info["class_is_abstract"]

		if "class_inheritence" in class_info.keys():

			matching = [s for s in class_info['class_inheritence'] if base in s]
			if matching and base:
				matching = matching[0]
				matching = matching.replace("public ", "")
				module_info['parent'] = matching

			module_info['definitions'] = []
			for s in class_info['class_inheritence']:
				if "Interface" in s:
					s_ = s.replace("public ", "")
					s_ = s_.split("::")[-1]
					s_ = s_.split("<")[0]
					s_ = 'aff3ct::tools::' + s_
					for _,method in data[s_]["class_methods"].items():
						method_info = {}
						method_info['short_name'] = method['method_short_name']
						module_info['definitions'].append(method_info)
						message = bcolors.OKGREEN + "(II) Definition " + method_info['short_name'] + " added to module " + module_info['name'] + '.'+ bcolors.ENDC
						print(message)
			if 'class_methods' in class_info.keys():
				for _,method in class_info['class_methods'].items():
					if method['method_access'] == 'public' and method['method_short_name'] in defs:
						method_info = {}
						method_info['short_name'] = method['method_short_name']
						module_info['definitions'].append(method_info)
						message = bcolors.OKGREEN + "(II) Definition " + method_info['short_name'] + " added to module " + module_info['name'] + '.'+ bcolors.ENDC
						print(message)

		module_info['has_template'] = has_template
		module_info['include_path']  = path + '/' + short_name
		module_info['mk_dir_path']   = install_path + '/' + path + '/' + short_name
		if has_template:
			try:
				full_template, medium_template, short_template, default_template = get_templates(class_info)
			except RuntimeError as err:
				print(err)
				continue
			medium_template  = 'template <' + medium_template  + '>'
			short_template   = '<' + short_template   + '>'
			default_template = '<' + default_template + '>'

		module_info['template'   ] = {}
		module_info['template'   ]['full'   ] = full_template
		module_info['template'   ]['medium' ] = medium_template
		module_info['template'   ]['short'  ] = short_template
		module_info['template'   ]['default'] = default_template
		module_info['constructors'] = []

		public_destructor = False
		if "class_destructors" in class_info.keys():
			for _, destructor in class_info["class_destructors"].items():
				if destructor["method_access"] == "public":
					public_destructor = True
		if len(class_info["class_destructors"].items()) == 0:
			message = bcolors.OKBLUE + "(II) No destructor for class " + module_info['name'] + '.'+ bcolors.ENDC
			print(message)

		dtor_trick = ""
		if not public_destructor and len(class_info["class_destructors"].items()) > 0:
			message = bcolors.OKBLUE + "(II) Protected destructor for class " + module_info['name'] + '.'+ bcolors.ENDC
			print(message)
			dtor_trick = ", std::unique_ptr<" + module_info['name'] + module_info['template']['short'] + ", py::nodelete>"
		module_info['dtor_trick'] = dtor_trick

		if module_info['is_abstract']:
			message = bcolors.OKBLUE + "(II) Abstract class " + module_info['name'] + '.'+ bcolors.ENDC
			print(message)
		else:
			if "class_constructors" in class_info.keys():
				class_constructors = class_info["class_constructors"]
				for _, constructor in class_constructors.items():
					if constructor["method_access"] == "public":
						arg_nbr = constructor["method_nb_arguments"]
						args_description = []
						every_arg_OK = True
						for a_idx in range(arg_nbr):
							arg_info = constructor["method_arguments"][a_idx]
							reduced_arg_type = arg_info["arg_type"]
							reduced_arg_type = reduced_arg_type.split("::")[-1]
							reduced_arg_type = reduced_arg_type.split("<")[0]
							reduced_arg_type = reduced_arg_type.split(" ")[0]
							if "tool" in arg_info["arg_type"] and not any([x for x in existing_tools.keys() if reduced_arg_type in x]):
								message = bcolors.WARNING + "(WW) Constructor not be wrapped for class " + class_info['class_name'] + "\t-> argument " + arg_info["arg_name"] + " has unknown type '" + arg_info["arg_type"] + "'." + bcolors.ENDC
								print(message)
								every_arg_OK = False
								break

							arg_type = ""
							if arg_info["arg_is_const"]:
								arg_type += "const "
							arg_type += arg_info["arg_type"]

							arg_default = ""
							if "=" in arg_info["arg_signature"]:
								arg_default = " = " + arg_info["arg_signature"].split(" = ")[1]

							arg_description = {}
							arg_description['name'   ] = arg_info["arg_name"]
							arg_description['type'   ] = arg_type
							arg_description['default'] = arg_default
							args_description.append(arg_description)
						if every_arg_OK:
							module_info['constructors'].append({})
							module_info['constructors'][-1]['args'] = args_description
			else:
				message = bcolors.WARNING + "(WW) Concrete class without constructor: " + module_info['name']+ '.'+ bcolors.ENDC
				print(message)

		if value:
			module_info['leaf'] = False
		else:
			module_info['leaf'] = True
		modules[name] = module_info
		if value:
			modules = build_modules(data, value, short_name, modules, install_path, path + '/' + short_name, existing_tools, defs)

	return modules

def make_dir_tree(modules, verbose = False):
	for _,module in modules.items():
		folder_path = module['mk_dir_path']
		if not Path(folder_path).is_dir():
			Path(folder_path).mkdir(parents=True, exist_ok=True)
			if verbose:
				print("Creating folder : " + folder_path)

def write_hpp_wrappers(modules, template_path, verbose = False):
	for _,module in modules.items():
		wrapper_hpp = ""
		with open(template_path + "/Wrapper_template.hpp","r") as f:
			wrapper_hpp = f.read()
			if 'parent' in module.keys():
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


def write_cpp_wrappers(modules, template_path, verbose = False):
	for _,module in modules.items():
		init_lines = ""
		wrapper_cpp = ""

		class_constructors = module['constructors']
		for constructor in class_constructors:
			arg_types = ""
			arg_init  = ""
			new_line  = "\n\tthis->def(py::init<{types}>(){init}, py::return_value_policy::reference);"
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
		if 'definitions' in module.keys():
			for def_ in module['definitions']:
				if module['short_name'] == 'Decoder' and def_['short_name'] == 'reset':
					def_lines += 'this->def("reset", [](aff3ct::module::Decoder& self){self.reset();});'
				else:
					def_lines += '\n\tthis->def("' + def_['short_name'] + '"'
					def_lines += ', &' + module['short_name'] + module['template']['short'] + '::' + def_['short_name'] + ');'
		if def_lines:
			def_lines += '\n'

		wrapper_cpp = ""
		with open(template_path + "/Wrapper_template.cpp","r") as f:
			wrapper_cpp = f.read()
			if 'parent' in module.keys():
				wrapper_cpp = wrapper_cpp.replace("{parent}", "," + module['parent'])
			else:
				wrapper_cpp = wrapper_cpp.replace("{parent}", "")

			if module['has_template']:
				footer = '#include "Tools/types.h"\ntemplate class aff3ct::wrapper::Wrapper_'+ module['short_name'         ] + module['template']['default']+';'
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
			wrapper_cpp = wrapper_cpp.replace("{def_lines}",        def_lines                   )
			wrapper_cpp = wrapper_cpp.replace("{dtor_trick}"      , module['dtor_trick'     ]        )

		file_path = module['mk_dir_path'] + "/" + module['short_name'] + ".cpp"
		write_if_different(file_path, wrapper_cpp, verbose)


def get_templates(class_info):
	full_template  = class_info["class_templates"][0]["template_signature"]
	template_args  = class_info["class_templates"][0]["template_args"     ]
	short_template = ""
	default_template = ""

	for arg in template_args:
		new_short_template = arg["template_arg_name"].split(" ")[-1]
		short_template += new_short_template  + ", "
		if not arg["template_arg_default"]:
			message = bcolors.FAIL + "(EE) Class and children classes not wrapped. Class " + class_info['class_name'] + " has a template without default value." + bcolors.ENDC
			raise RuntimeError(message)
		else:
			default_template += arg["template_arg_default"] + ", "

	short_template = short_template[:-2]
	default_template = default_template[:-2]
	medium_template = ""

	for arg in template_args:
		medium_template = medium_template + arg["template_arg_name"] + ", "
	medium_template = medium_template[:-2]
	return full_template, medium_template, short_template, default_template

def gen_py_aff3ct_cpp(modules, tree, mod, root_mod, cpp_content):
	for key, value in tree.items():
		if key in modules:
			module_info = modules[key]
			the_mod = mod
			if not module_info["leaf"] and mod == root_mod:
				cpp_content += '\tpy::module_ mod_' + module_info['short_name'].lower() + " = " + mod + '.def_submodule("' +  module_info['short_name'].lower() + '");\n'
				the_mod = 'mod_' + module_info['short_name'].lower()

			line = '\tstd::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_'
			if module_info['has_template']:
				line +=  module_info["short_name"].lower() + '(new aff3ct::wrapper::Wrapper_' + module_info["short_name"] + '<>(' + the_mod + '));\n'
			else:
				line +=  module_info["short_name"].lower() + '(new aff3ct::wrapper::Wrapper_' + module_info["short_name"] + '(' + the_mod + '));\n'

			line += '\twrappers.push_back(wrapper_' + module_info["short_name"].lower() + '.get()); \n\n'
			cpp_content += line

			if not module_info["leaf"]:
				cpp_content = gen_py_aff3ct_cpp(modules, value, the_mod, root_mod, cpp_content)
	return cpp_content

def write_py_aff3ct_cpp (tools, tools_tree, modules, module_tree, command_path, template_path, verbose = False):
	other_tool_wrappers   = gen_py_aff3ct_cpp(tools, tools_tree, 'm0', 'm0', '')
	other_module_wrappers = gen_py_aff3ct_cpp(modules, module_tree, 'm1', 'm1', '')
	py_aff3ct_cpp = ""
	with open(template_path + "/py_aff3ct_template.cpp","r") as f:
		py_aff3ct_cpp = f.read()
		py_aff3ct_cpp = py_aff3ct_cpp.replace("{other_tool_wrappers}", other_tool_wrappers[0:-2])
		py_aff3ct_cpp = py_aff3ct_cpp.replace("{other_module_wrappers}", other_module_wrappers[0:-2])

	file_path = command_path + "/src/py_aff3ct.cpp"
	write_if_different(file_path, py_aff3ct_cpp, verbose)

def write_py_aff3ct_hpp (modules, command_path, template_path, verbose = False):
	other_includes = gen_py_aff3ct_hpp(modules)
	with open(template_path + "/py_aff3ct_template.hpp","r") as f:
			py_aff3ct_hpp = f.read()
			py_aff3ct_hpp = py_aff3ct_hpp.replace("{other_includes}", other_includes)
	file_path = command_path + "/src/py_aff3ct.hpp"
	write_if_different(file_path, py_aff3ct_hpp, verbose)

def gen_py_aff3ct_hpp(modules):
	hpp_content = ""
	for _,module in modules.items():
		hpp_content += '#include "' + module['include_path'] + '/' + module['short_name'] + '.hpp"\n'
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

# NEW !!!!

def get_name(dict):
	val = dict["compounddef"]["compoundname"]
	return val

def get_short_name(dict):
	name = get_name(dict)
	short_name = name.split("::")[2]
	return short_name

def is_abstract(dict):
	return "@abstract" in dict["compounddef"].keys()

def has_template(dict):
	return "templateparamlist" in dict["compounddef"].keys()

def extract_type(val):
	type_val = ""
	if type(val) is dict:
		if "#text" in val.keys():
			if "const" in val["#text"]:
				type_val += "const " + val["ref"]["#text"]
			else:
				type_val += val["ref"]["#text"]
			if "&" in val["#text"]:
				type_val += "&"
			elif "*" in val["#text"]:
				type_val += "*"
	else:
		type_val = val
	return type_val

def gen_template(dict):
	full    = ""
	medium  = ""
	short   = ""
	default = ""

	if has_template(dict):

		templates_list = dict["compounddef"]["templateparamlist"]["param"]
		if type(templates_list) is not list:
			templates_list = [templates_list]

		default = "<"
		for tp in templates_list:
			if not "defval" in tp.keys():
				message = bcolors.FAIL + "(EE) Class and children classes not wrapped. Class " + get_short_name(dict) + " has a template without default value." + bcolors.ENDC
				raise RuntimeError(message)
			default += extract_type(tp["defval"]) + ","
		default = default[:-1] + ">"

		short = "<"
		for tp in templates_list:
			short += tp["type"].split(" ")[1] + ","
		short = short[:-1] + ">"

		medium = "template <"
		for tp in templates_list:
			medium += tp["type"] + ","
		medium = medium[:-1] + ">"

		full = "template <"
		for tp in templates_list:
			full += tp["type"] + " = " + extract_type(tp["defval"]) + ","
		full = full[:-1] + ">"

	return {"full":    full,
	        "medium":  medium,
	        "short":   short,
	        "default": default}

def get_parent(dict):
	if "basecompoundref" in dict["compounddef"].keys():
		if type(dict["compounddef"]["basecompoundref"]) is not list:
			val = dict["compounddef"]["basecompoundref"]
		else:
			val = dict["compounddef"]["basecompoundref"][0]
		return val["#text"].split("<")[0]
	return ""

def get_dtor_trick(dict):
	dtor_trick = ""

	sectiondef_list = dict["compounddef"]["sectiondef"]
	if type(sectiondef_list) is not list:
		sectiondef_list = [sectiondef_list]

	dtor_name = "~" + get_short_name(dict)
	for sd in sectiondef_list:
		if (sd["@kind"] == "private-func"):
			memberdef_list = sd["memberdef"]
			if type(memberdef_list) is not list:
				memberdef_list = [memberdef_list]
			for mb in memberdef_list:
				if mb["name"] == dtor_name:
					message = bcolors.OKBLUE + "(II) Protected destructor for class " + get_name(dict) + '.'+ bcolors.ENDC
					print(message)
					dtor_trick = ", std::unique_ptr<" + get_name(dict) + gen_template(dict)['short'] + ", py::nodelete>"
	return dtor_trick

def gen_constructors(dict):
	sectiondef_list = dict["compounddef"]["sectiondef"]
	if type(sectiondef_list) is not list:
		sectiondef_list = [sectiondef_list]

	class_name = get_short_name(dict)

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
					for a in params_list:
						name_val = ""
						if "declname" in a.keys():
							name_val = a["declname"]

						type_val = ""
						if "type" in a.keys():
							type_val = extract_type (a["type"])

						default_val = ""
						if "defval" in a.keys():
							default_val = a["defval"]

						args.append({
							"name": name_val,
							"type": type_val,
							"default": default_val
						})

					constructors.append({"args": args})
	return constructors

def gen_definitions(dict):
	defs = []
	if "basecompoundref" in dict["compounddef"]:
		refs_list = dict["compounddef"]["basecompoundref"]
		if type(refs_list) is not list:
			refs_list = [refs_list]
		for ref in refs_list:
			if 'aff3ct::tools::Interface' in ref["#text"]:
				defs.append(ref["#text"])
	return defs

def is_leaf(dict):
	return "derivedcompoundref" not in dict["compounddef"]

def get_include_path(dict, prefix):
	dbdir,_ = os.path.split(dict["compounddef"]["location"]["@file"])
	if prefix:
		return prefix + "/" + dbdir
	else:
		return dbdir

def build_modules2(data, install_path, path, includes):
	dict = {}
	for key, value in data.items():
		if key in includes:
			dict[key] = {"name":         get_name        (value),
			             "short_name":   get_short_name  (value),
			             "is_abstract":  is_abstract     (value),
			             "has_template": has_template    (value),
			             "template":     gen_template    (value),
			             "parent":       get_parent      (value),
			             "dtor_trick":   get_dtor_trick  (value),
			             "constructors": gen_constructors(value),
			             "definitions":  gen_definitions (value),
			             "leaf":         is_leaf         (value),
			             "include_path": get_include_path(value, path),
			             "mk_dir_path":  get_include_path(value, install_path + "/" + path) }
	return dict