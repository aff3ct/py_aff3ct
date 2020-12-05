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
parser.add_argument(      "--db-path", help = "Path of the aff3ct JSON file."           , default= command_path + "/lib/aff3ct/abi/db.json")
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