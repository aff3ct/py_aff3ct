#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse, sys, os
import json
from pathlib import Path
import aff3ct_tools
from subprocess import call
import re
from os import listdir
from os.path import isfile, isdir, join
import xmltodict

command_path, _ = os.path.split(sys.argv[0])

# Define the program description
text = 'This is the configure program for buildign the py_aff3ct library.'
parser = argparse.ArgumentParser(description=text)

parser.add_argument(      "--verbose", help = "Set the configuration verbose."          , action="store_true")
parser.add_argument(        "--clean", help = "Clean before doing configuration."       , action="store_true")
parser.add_argument("--doxy-xml-path", help = "Path of the Doxygen XML files."          , default= command_path + "/lib/aff3ct/doc/build/doxygen/xml/")
parser.add_argument("--template-path", help = "Path of the py_aff3ct *template* folder.", default= command_path + "/template")

parser.add_argument( "--include-module", nargs="+", help = "List of aff3ct Modules to be wrapped. Example : --include-module Source Modem", default=['CRC', 'Channel', 'Modem', 'Source', 'Encoder', 'Decoder', 'Iterator', 'Sink'])
parser.add_argument( "--exclude-module", nargs="+", help = "List of aff3ct Modules to be excluded from the wrapper. (prioritary over --include-module). Example : --exclude-module Source Modem", default=['Modem_CPM', 'Modem_OOK', 'Encoder_RSC_generic_json_sys', 'Decoder_LDPC_bit_flipping', 'Decoder_chase_pyndiah', 'Decoder_turbo_product', 'Decoder_RSC_BCJR_seq_generic_std_json', 'Reporter', 'Decoder_LDPC_BP_flooding_inter', 'Decoder_LDPC_BP_flooding_SPA', 'Decoder_LDPC_bit_flipping_hard', 'Decoder_polar_MK_SC_naive', 'Encoder_polar_MK', 'Decoder_polar_MK_SCL_naive', 'Decoder_polar_MK_SCL_naive_CA', 'Decoder_polar_MK_ASCL_naive_CA', 'Decoder_polar_MK_SCL_naive_CA_sys', 'Decoder_polar_MK_ASCL_naive_CA_sys', 'Decoder_polar_MK_SCL_naive_sys'])
parser.add_argument( "--include-tool",   nargs="+", help = "List of aff3ct Tools to be wrapped. Example : --include-tool Constellation", default=['Constellation', 'Pattern_polar_i', 'Noise', 'BCH_polynomial_generator', 'RS_polynomial_generator', 'Polar_code','Interleaver_core','dvbs2_values'])
parser.add_argument( "--exclude-tool",   nargs="+", help = "List of aff3ct Tools to be excluded from the wrapper. (prioritary over --include-tool). Example : --exclude-tool Constellation", default=[])

args = parser.parse_args()

if args.clean:
	if args.verbose:
		print("Clean: delete the current src folder.")
	call('rm -rf "' + command_path + '/src"', shell=True)

cp_path = args.template_path + "/src"
aff3ct_tools.src_repair(cp_path, command_path + "/src", args.verbose)

if args.verbose:
	print("Wrapped tools: ",    args.include_tool)
	print("Excluded tools: ",   args.exclude_tool)
	print("Wrapped modules: ",  args.include_module)
	print("Excluded modules: ", args.exclude_module)
	# print("Wrapped defs: ",     args.include_def)

if not isdir(args.doxy_xml_path):
	print("The '" + args.doxy_xml_path + "' path is not a directory, please provide a valid path.")
	exit(-1)

# convert the XML files into a Python dictionary
doxyFiles = [f for f in listdir(args.doxy_xml_path) if isfile(join(args.doxy_xml_path, f))]
if len(doxyFiles) == 0:
	print("The '" + args.doxy_xml_path + "' dir. is empty, no documentation files have been generated.")
	exit(-1)
doxygen = {}
for f in doxyFiles:
	pattern = re.compile("^class|^struct")
	if pattern.match(f):
		realName = aff3ct_tools.doxyname_to_stdname(f)
		pattern = re.compile("^aff3ct::module::|^aff3ct::tools::")
		if pattern.match(realName):
			data = open(join(args.doxy_xml_path, f), 'r').read()
			start_ref = data.find('<ref')
			while start_ref > -1:
				end_ref = data.find('>', start_ref)
				data = data[:start_ref] + data[end_ref+1:]
				start_ref = data.find('<ref')

			data = data.replace("</ref>", "")
			dict = xmltodict.parse(data)
			doxygen[realName] = dict["doxygen"]
			error = True
			if "doxygen" in dict:
				if "compounddef" in dict["doxygen"]:
					if "compoundname" in dict["doxygen"]["compounddef"]:
						if dict["doxygen"]["compounddef"]["compoundname"] == realName:
							error = False
			if error:
				print("There is a problem :-(")
				exit(-1)

# add the missing `derivedcompoundref` field to the Doxygen JSON
for key, value in doxygen.items():
	if "derivedcompoundref" in value["compounddef"].keys():
		value["compounddef"]["derivedcompoundref"] = []

	for key2, value2 in doxygen.items():
		if key != key2 and "basecompoundref" in value2["compounddef"].keys():
			refs_list = value2["compounddef"]["basecompoundref"]
			if type(refs_list) is not list:
				refs_list = [refs_list]
			for ref in refs_list:
				if ref["#text"].split("<")[0].replace('aff3ct::module::', '').replace('aff3ct::tools::', '') == key.replace('aff3ct::module::', '').replace('aff3ct::tools::', ''):
					elmt = {"#text": key2}
					if "derivedcompoundref" not in value["compounddef"].keys():
						value["compounddef"]["derivedcompoundref"] = [elmt]
					else:
						value["compounddef"]["derivedcompoundref"].append(elmt)

#with open('doxygen.json', 'w') as fid:
#	json.dump(doxygen, fid)
tools_tree = {}
tools_classes_list = aff3ct_tools.recursive_build_classes_list(doxygen, args.include_tool, args.exclude_tool, "aff3ct::tools::", tools_tree)
tools = aff3ct_tools.build_modules(doxygen, command_path + "/src", "Wrapper_py", tools_classes_list)

aff3ct_tools.make_dir_tree     (tools,                     args.verbose)
aff3ct_tools.write_hpp_wrappers(tools, args.template_path              )
aff3ct_tools.write_cpp_wrappers(tools, args.template_path              )

existing_tools = tools.copy()
existing_tools["aff3ct::tools::Gaussian_noise_generator_implem"] = {}
existing_tools["aff3ct::tools::Frozenbits_generator"] = {}
existing_tools["aff3ct::tools::Sequence"] = {}
existing_tools["aff3ct::tools::Pipeline"] = {}
existing_tools["aff3ct::tools::Sparse_matrix"] = {}

module_tree = {}
module_classes_list = aff3ct_tools.recursive_build_classes_list(doxygen, args.include_module, args.exclude_module, "aff3ct::module::", module_tree)

module = aff3ct_tools.build_modules(doxygen, command_path + "/src", "Wrapper_py", module_classes_list, existing_tools)

aff3ct_tools.make_dir_tree     (module,                     args.verbose)
aff3ct_tools.write_hpp_wrappers(module, args.template_path, args.verbose)
aff3ct_tools.write_cpp_wrappers(module, args.template_path, args.verbose)

full_dict = {}
full_dict.update(tools)
full_dict.update(module)

aff3ct_tools.write_py_aff3ct_hpp(full_dict, command_path, args.template_path, args.verbose)
aff3ct_tools.write_py_aff3ct_cpp(tools, tools_tree, module, module_tree, command_path, args.template_path, args.verbose)
