#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse, sys, os
import json
from pathlib import Path
import aff3ct_tools
from subprocess import call

command_path, _ = os.path.split(sys.argv[0])

# Define the program description
text = 'This is the configure program for buildign the py_aff3ct library.'
parser = argparse.ArgumentParser(description=text)

parser.add_argument(      "--verbose", help = "Set the configuration versose."          , action="store_true")
parser.add_argument(        "--clean", help = "Clean before doing consfiguration."      , action="store_true")
parser.add_argument(      "--db-path", help = "Path of the aff3ct JSON file."           , default= command_path + "/lib/aff3ct/scripts/abi/db.json")
parser.add_argument("--doxy-xml-path", help = "Path of the Doxygen XML files."          , default= command_path + "/lib/aff3ct/doc/build/doxygen/xml/")
parser.add_argument("--template-path", help = "Path of the py_aff3ct *template* folder.", default= command_path + "/template")

parser.add_argument( "--include-module", nargs="+", help = "List of aff3ct Modules to be wrapped. Example : --include-module Source Modem", default=['Source', 'Modem', 'Channel', 'Encoder', 'Decoder'])
parser.add_argument( "--exclude-module", nargs="+", help = "List of aff3ct Modules to be excluded from the wrapper. (prioritary over --include-module). Example : --exclude-module Source Modem", default=['Modem_CPM', 'Modem_OOK', 'Encoder_RSC_generic_json_sys', 'Decoder_LDPC_bit_flipping', 'Decoder_chase_pyndiah', 'Decoder_turbo_product', 'Decoder_RSC_BCJR_seq_generic_std_json', 'Reporter'])
parser.add_argument( "--include-tool", nargs="+", help = "List of aff3ct Tools to be wrapped. Example : --include-tool Constellation", default=['Constellation', 'Sparse_matrix', 'Pattern_polar_i', 'Noise', 'BCH_polynomial_generator', 'RS_polynomial_generator', 'Polar_code'])
parser.add_argument( "--exclude-tool", nargs="+", help = "List of aff3ct Tools to be excluded from the wrapper. (prioritary over --include-tool). Example : --exclude-tool Constellation", default=[])
parser.add_argument( "--include-def", nargs="+", help = "List of aff3ct defs to be wrapped. Example : --include-def get_trellis", default=['get_trellis'])

args = parser.parse_args()

db_folder, _ = os.path.split(args.db_path)

if args.clean:
	rm_command = 'rm -rf ' + args.db_path
	if args.verbose:
		print("Clean: " + rm_command)
	call(rm_command, shell=True)

if not Path(args.db_path).is_file():
	if args.verbose:
		print("Creating db.json file...")
	create_db_command = 'cd ' + db_folder + ' ; ./01_aff3ct_dump_clang_ast.sh ; ./02_aff3ct_read_clang_ast.py ; cd ' + command_path
	call(create_db_command, shell=True)

if args.clean:
	if args.verbose:
		print("Clean: delete the current src folder.")
	call('rm -rf "' + command_path + '/src"', shell=True)

cp_path = args.template_path + "/src"
aff3ct_tools.src_repair(cp_path, command_path + "/src", args.verbose)

with open(args.db_path, "r") as read_file:
	data = json.load(read_file)


if args.verbose:
	print("Wrapped tools: ", args.include_tool)
	print("Excluded tools: ", args.exclude_tool)
	print("Wrapped modules: ", args.include_module)
	print("Excluded modules: ", args.exclude_module)
	print("Wrapped defs: ", args.include_def)

include_tools  = args.include_tool
exclude_tools  = args.exclude_tool
tools_tree = aff3ct_tools.build_inheritence_tree(data, "tools", include_tools, exclude_tools, {}, True)
if args.verbose:
	aff3ct_tools.print_tree(tools_tree)

tools = aff3ct_tools.build_modules(data, tools_tree, "", {}, command_path + "/src", "Wrapper_py/Tools", {}, args.include_def)
aff3ct_tools.make_dir_tree      (tools,                                         args.verbose)
aff3ct_tools.write_hpp_wrappers (tools,                     args.template_path              )
aff3ct_tools.write_cpp_wrappers (tools,                     args.template_path              )

existing_tools = tools.copy()
existing_tools["aff3ct::tools::Gaussian_noise_generator_implem"] = {}
existing_tools["aff3ct::tools::Frozenbits_generator"] = {}
existing_tools["aff3ct::tools::Sequence"] = {}

include_modules  = args.include_module
exclude_modules  = args.exclude_module
tree = aff3ct_tools.build_inheritence_tree(data, "Module", include_modules, exclude_modules, {}, True)
if args.verbose:
	aff3ct_tools.print_tree(tree)

modules = aff3ct_tools.build_modules(data, tree, "aff3ct::module::Module", {}, command_path + "/src", "Wrapper_py/Module", existing_tools, args.include_def)

aff3ct_tools.make_dir_tree      (modules,                                         args.verbose)

aff3ct_tools.write_hpp_wrappers (modules,                     args.template_path ,args.verbose)
aff3ct_tools.write_cpp_wrappers (modules,                     args.template_path ,args.verbose)

full_dict = {}
full_dict.update(tools)
full_dict.update(modules)

aff3ct_tools.write_py_aff3ct_hpp(full_dict,                           command_path, args.template_path, args.verbose)
aff3ct_tools.write_py_aff3ct_cpp(tools, tools_tree, modules, tree, command_path, args.template_path, args.verbose)

# NEW !!!!
import re
from os import listdir
from os.path import isfile, isdir, join
import xmltodict

if not isdir(args.doxy_xml_path):
	print("The '" + args.doxy_xml_path + "' path is not a directory, please provide a valid path.")
	exit(-1)

# build a map of Doxygen XML classes
doxyFiles = [f for f in listdir(args.doxy_xml_path) if isfile(join(args.doxy_xml_path, f))]

if len(doxyFiles) == 0:
	print("The '" + args.doxy_xml_path + "' dir. is empty, no documentation files have been generated.")
	exit(-1)

doxygen = {}
for f in doxyFiles:
	pattern = re.compile("^class|^struct")
	if pattern.match(f):
		realName = re.sub( "^class",   "",        f)
		realName = re.sub("^struct",   "", realName)
		realName = re.sub(   "_1_1", "::", realName)
		realName = re.sub(  "_3_01", "< ", realName)
		realName = re.sub(  "_01_4", " >", realName)
		realName = re.sub(     "__",  "_", realName)
		realName = re.sub(   ".xml",   "", realName)

		pattern = re.compile("^aff3ct::module::|^aff3ct::tools::")
		if pattern.match(realName):
			data = open(join(args.doxy_xml_path, f), 'r').read()
			dict = xmltodict.parse(data)
			doxygen[realName] = dict["doxygen"]

			if "doxygen" in dict:
				if "compounddef" in dict["doxygen"]:
					if "compoundname" in dict["doxygen"]["compounddef"]:
						if dict["doxygen"]["compounddef"]["compoundname"] != realName:
							print("There is a problem, salut 1!")
							exit(-1)
					else:
						print("There is a problem, salut 2!")
						exit(-1)
				else:
					print("There is a problem, salut 3!")
					exit(-1)
			else:
				print("There is a problem, salut 4!")
				exit(-1)

jsonStr = json.dumps(doxygen, sort_keys=True)
f = open("doxygen.json", "w")
f.write(jsonStr)
f.close()

with open("doxygen.json", "r") as read_file:
	data = json.load(read_file)

includes = []
for i in args.include_tool:
	pattern = re.compile("^aff3ct::tools::"+i)
	for key, value in data.items():
		if pattern.match(key):
			includes.append(key)

for i in args.exclude_tool:
	pattern = re.compile("^aff3ct::tools::"+i)
	for j in includes:
		if pattern.match(j):
			includes.remove(j)

tools2 = aff3ct_tools.build_modules2(data, command_path + "/src", "Wrapper_py", includes)

jsonStr = json.dumps(tools2, sort_keys=True)
f = open("dbg_tools.json", "w")
f.write(jsonStr)
f.close()

includes = []
for i in args.include_module:
	pattern = re.compile("^aff3ct::module::"+i)
	for key, value in data.items():
		if pattern.match(key):
			includes.append(key)

for i in args.exclude_module:
	pattern = re.compile("^aff3ct::module::"+i)
	for j in includes:
		if pattern.match(j):
			includes.remove(j)

module2 = aff3ct_tools.build_modules2(data, command_path + "/src", "Wrapper_py", includes)

jsonStr = json.dumps(module2, sort_keys=True)
f = open("dbg_module.json", "w")
f.write(jsonStr)
f.close()