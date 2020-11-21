#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from pathlib import Path

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def build_inheritence_tree(data, template, include, exclude, tree):
	for key, value in data.items():
		res = [s for s in include if s in value["class_short_name"]]
		if not res:
			continue

		res = [s for s in exclude if s in value["class_short_name"]]
		if res:
			continue

		if "class_inheritence" in value:
			for s in value["class_inheritence"]:
				short_class_inh = s.replace("public ", "")
				short_class_inh = short_class_inh.split("::")[-1]
				short_class_inh = short_class_inh.split('<')[0]
				if (template == short_class_inh):
					tree[key] = {}
					tree[key] = build_inheritence_tree(data, value["class_short_name"], include, exclude, tree[key])
				break
		elif("tool" in template):
			tree[key] = {}
			tree[key] = build_inheritence_tree(data, value["class_short_name"], include, exclude, tree[key])

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

def build_modules(data, tree, base, modules, install_path, path, existing_tools):
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

		module_info = {}
		module_info['name']		 = name
		module_info['short_name' ] = short_name
		module_info['is_abstract'] = class_info["class_is_abstract"]
		if "class_inheritence" in class_info.keys():
			matching = [s for s in class_info['class_inheritence'] if base in s]
			if matching:
				matching = matching[0]
				matching = matching.replace("public ", "")
			module_info['parent']	   = matching

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
		if module_info['is_abstract']:
			message = bcolors.OKBLUE + "(II) Abstract class : " + module_info['name'] + '.'+ bcolors.ENDC
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
				message = bcolors.WARNING + "(WW) Concrete class without constructor : " + module_info['name']+ '.'+ bcolors.ENDC
				print(message)

		if value:
			module_info['leaf'] = False
		else:
			module_info['leaf'] = True
		modules[name] = module_info
		if value:
			modules = build_modules(data, value, short_name, modules, install_path, path + '/' + short_name, existing_tools)

	return modules

def make_dir_tree(modules, verbose = False):
	for _,module in modules.items():
		if verbose:
			print("Creating folder : " + module['mk_dir_path'])
		Path(module['mk_dir_path']).mkdir(parents=True, exist_ok=True)

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

		with open(module['mk_dir_path'] + "/" + module['short_name'] + ".hpp","w") as f:
			if verbose:
				print("Creating file : " + module['mk_dir_path'] + "/" + module['short_name'] + ".hpp")
			f.write(wrapper_hpp)


def write_cpp_wrappers(modules, template_path, verbose = False):
	for _,module in modules.items():
		init_lines = ""
		wrapper_cpp = ""

		class_constructors = module['constructors']
		for constructor in class_constructors:
			arg_types = ""
			arg_init  = ""
			new_line  = "\tthis->def(py::init<{types}>(), {init});"
			arg_nbr = len(constructor['args'])
			for a_idx in range(arg_nbr):
				arg_types += constructor['args'][a_idx]['type'] + ", "
				arg_init  += '"' + constructor['args'][a_idx]['name'] +'"_a' + constructor['args'][a_idx]['default'] + ', '
			arg_types = arg_types[:-2]
			arg_init  = arg_init [:-2]
			new_line  = new_line.replace("{types}", arg_types)
			new_line  = new_line.replace("{init}", arg_init)
			init_lines += new_line + "\n"

		init_lines = init_lines[:-1]
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

		with open(module['mk_dir_path'] + "/" + module['short_name'] + ".cpp","w") as f:
			if verbose:
				print("Creating file : " + module['mk_dir_path'] + "/" + module['short_name'] + ".cpp")
			f.write(wrapper_cpp)


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

	if verbose:
		print("Creating file: " + command_path + "/src/py_aff3ct.cpp")
	with open(command_path + "/src/py_aff3ct.cpp","w") as f:
		f.write(py_aff3ct_cpp)

def write_py_aff3ct_hpp (modules, command_path, template_path, verbose = False):
	other_includes = gen_py_aff3ct_hpp(modules)
	with open(template_path + "/py_aff3ct_template.hpp","r") as f:
			py_aff3ct_hpp = f.read()
			py_aff3ct_hpp = py_aff3ct_hpp.replace("{other_includes}", other_includes)
	if verbose:
		print("Creating file: " + command_path + "/src/py_aff3ct.hpp")
	with open(command_path + "/src/py_aff3ct.hpp","w") as f:
		f.write(py_aff3ct_hpp)

def gen_py_aff3ct_hpp(modules):
	hpp_content = ""
	for _,module in modules.items():
		hpp_content += '#include "' + module['include_path'] + '/' + module['short_name'] + '.hpp"\n'
	return hpp_content