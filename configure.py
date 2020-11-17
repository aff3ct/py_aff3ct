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
parser.add_argument( "--include-list", nargs="+", help = "List of aff3ct Modules to be wrapped. Example : --include-list Source Modem", default=[])
parser.add_argument( "--exclude-list", nargs="+", help = "List of aff3ct Modules to be excluded from the wrapper. (prioritary over --include-list). Example : --exclude-list Source Modem", default=[])
args = parser.parse_args()

db_folder, _ = os.path.split(args.db_path)

if args.clean:
	if args.verbose:
		print("Clean: delete " + args.db_path)
	call('rm -f ' + args.db_path, shell=True)

if not  Path(args.db_path).is_file():
	if args.verbose:
		print("Creating db.json file...")
	create_db_command = 'cd ' + db_folder + ' ; ./01_aff3ct_dump_clang_ast.sh ; ./02_aff3ct_read_clang_ast.py -f aff3ct_fixes.json -m aff3ct_modules.json ; cd ' + command_path
	call(create_db_command, shell=True)

with open(args.db_path, "r") as read_file:
	data = json.load(read_file)

include_modules  = args.include_list
exclude_modules  = args.exclude_list
tree = aff3ct_tools.build_inheritence_tree(data, "Module", include_modules, exclude_modules, {})
if args.verbose:
	aff3ct_tools.print_tree(tree)

if args.clean:
	if args.verbose:
		print("Clean: delete the current src folder.")
	call('rm -rf "src"', shell=True)

cp_path = args.template_path + "/src"
if args.verbose:
	print("Copy " + cp_path + " to py_aff3ct.")
call('cp -R ' + cp_path + ' ' + command_path, shell=True)

existing_tools = []
modules = aff3ct_tools.build_modules(data, tree, "aff3ct::module::Module", {}, command_path + "/src", "Wrapper_py/Module", existing_tools)

aff3ct_tools.make_dir_tree      (modules,                                         args.verbose)
aff3ct_tools.write_hpp_wrappers (modules,                     args.template_path              )
aff3ct_tools.write_cpp_wrappers (modules,                     args.template_path              )
aff3ct_tools.write_py_aff3ct_hpp(modules,       command_path, args.template_path, args.verbose)
aff3ct_tools.write_py_aff3ct_cpp(modules, tree, command_path, args.template_path, args.verbose)