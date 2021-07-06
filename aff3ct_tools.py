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

def write_cpp_wrappers(modules, template_path, verbose = False):
	for _,module in modules.items():
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

def gen_py_aff3ct_cpp(modules, tree, mod, root_mod, cpp_content):
	for key, value in tree.items():
		if key in modules:
			module_info = modules[key]
			the_mod = mod
			if not module_info["leaf"] and mod == root_mod:
				cpp_content += '\tpy::module_ mod_' + module_info['short_name'].lower() + " = " + mod + '.def_submodule("' +  module_info['short_name'].lower() + '");\n'
				the_mod = 'mod_' + module_info['short_name'].lower()
				if mod == 'm0':
					cpp_content += '\t' + the_mod + '.doc() = ' + 'R"pbdoc(AFF3CT Tools inheritating from ' + module_info['short_name'] + '.)pbdoc";\n'
					cpp_content += '\t' + 'doc_m0 += R"pbdoc(           ' + module_info['short_name'].lower() + '\n' + ')pbdoc";\n'
				else:
					cpp_content += '\t' + the_mod + '.doc() = ' + 'R"pbdoc(AFF3CT Modules inheritating from ' + module_info['short_name'] + '.)pbdoc";\n'
					cpp_content += '\t' + 'doc_m1 += R"pbdoc(           ' + module_info['short_name'].lower() + '\n' + ')pbdoc";\n'

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

def extract_type(val):
	type_val = ""
	is_const = False
	is_ref   = False
	is_ptr   = False
	template = ""
	has_template = False
	container = ""
	has_container = False
	initial_val = val
	if isinstance(val, dict):
		if "#text" in val.keys():

			val_text = val["#text"]
			initial_val = val_text
			if val["#text"].startswith("const"):
				is_const = True
				val_text = val_text[5:]

			if val["#text"].endswith("&"):
				is_ref = True
				val_text = val_text[:-1:]

			if val["#text"].endswith("*"):
				is_ptr = True
				val_text = val_text[:-1:]

			val_text = val_text.replace(" ", "")
			val_text = val_text.replace("\n", "")
			val_text = val_text.replace("\t", "")
			val_text = val_text.replace("\r", "")

			if val_text:
				if val_text.startswith("<") and val_text.startswith("<"):
					has_template = True
					template = val_text
				else:
					has_container = True
					container = val_text.split("<")[0]
					val_text = val_text[len(container)+1:-1] #Also remove <>

			if is_const:
				type_val += "const "

			if has_container:
				type_val += container + "<"
				new_val = {"#text": val_text,
							"ref" : val["ref"]}
				# type_val += extract_type(new_val) + ">"
				type_val += new_val + ">"
			else:
				type_val += val["ref"]["#text"]

			if has_template:
				type_val += template

			if is_ptr:
				type_val += "*"

			if is_ref:
				type_val += "&"
	else:
		type_val = val
	print("Initial : ", initial_val, " | new val : ", type_val)
	return type_val

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
			#t = extract_type(tp["defval"])
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
				#full += tp["type"] + " " + tp["defname"] + " = " + extract_type(tp["defval"]) + ","
				full += tp["type"] + " " + tp["defname"] + " = " + tp["defval"] + ","
			else:
				#full += tp["type"] + " = " + extract_type(tp["defval"]) + ","
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

		if val_notp in classes_list:
			if (has_template(dictio[val_notp])):
				tp = gen_template(dictio[val_notp])
				return val_notp + tp["short"]
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
							#type_val = extract_type(a["type"])
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
									message = bcolors.WARNING + "(WW) Constructor not be wrapped for class '" + get_name(entry) + "' -> argument '" + name_val + "' has unknown type '" + type_val + "'." + bcolors.ENDC
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
