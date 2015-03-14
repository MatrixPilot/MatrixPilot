'''
Created on 31 August 2014

@author: Robert Dickenson
'''

import os, fnmatch
import uuid
import re

rootdir = ""
script_path = ""

def mkdirnotex(filename):  
	folder = os.path.dirname(filename)  
	if not os.path.exists(folder):  
		os.makedirs(folder) 

def find(masks, startdir=os.curdir):
	fullpath = []
	for (thisDir, subsHere, filesHere) in os.walk(startdir):
		path = thisDir.replace(rootdir, "..")
		for name in subsHere + filesHere:
			if fnmatch.fnmatch(name, masks):
				fullpath = fullpath + [os.path.join(path, name)]
	return fullpath

#
# MPLAB-8 section
#

file_subfolders = ""
other_files = ""
file_info = ""
file_cnt = 0


def mplab8_scan_dirs(masks, directories):
	global file_subfolders
	global other_files
	global file_info
	global file_cnt
	for directory in directories:
		path = os.path.join(rootdir, directory)
		files = find(masks, path)
		for filename in files:
			file_subfolders = file_subfolders + "file_" + str.format('{:0>3}', file_cnt) + "=" + directory + '\n'
			other_files = other_files + "file_" + str.format('{:0>3}', file_cnt) + "=no\n"
			file_info = file_info + "file_" + str.format('{:0>3}', file_cnt) + "=" + filename + '\n'
			file_cnt = file_cnt + 1

def mplab8_project(mcu_type, target_board, config_dir, includes, project_output_file):
	with open (script_path + "mplab8-template.txt", "r") as file:
		data = file.read()
		data = data.replace("%%DEVICE%%", mcu_type)
		data = data.replace("%%CONFIG%%", config_dir)
		data = data.replace("%%INCLUDES%%", includes)
		data = data.replace("%%TARGET_BOARD%%", target_board)
		data = data.replace("%%FILE_SUBFOLDERS%%", file_subfolders)
		data = data.replace("%%GENERATED_FILES%%", other_files)
		data = data.replace("%%OTHER_FILES%%", other_files)
		data = data.replace("%%FILE_INFO%%", file_info)
	mkdirnotex(project_output_file)
	with open (project_output_file, "w") as file:
		file.write(data)

#
# Visual Studio section
#

source_folders = []
header_folders = []

def vs2010_scan_dirs(masks, sources, directories):
	str = ""
	for dir in directories:
		path = os.path.join(rootdir, dir)
		for mask in masks:
			files = find(mask, path)
			for filename in files:
				if sources == 1:
					str = str + "    <ClCompile Include=\"" + filename.replace("/", "\\") + "\" />\n"
				else:
					str = str + "    <ClInclude Include=\"" + filename.replace("/", "\\") + "\" />\n"
	return str

def vs2010_scan_filter_dirs(masks, sources, directories, prefix):
	str = ""
	for dir in directories:
		path = os.path.join(rootdir, dir)
		for mask in masks:
			files = find(mask, path)
			for filename in files:
				fname = filename.replace("/", "\\")
				if sources == 1:
					source_folders.append(os.path.dirname(filename.replace("..\\", "")))
					str = str + "    <ClCompile Include=\"" + fname + "\">\n"
					str = str + "      <Filter>" + prefix + os.path.dirname(fname.replace("..\\", "")) + "</Filter>\n"
					str = str + "    </ClCompile>\n"
				else:
					header_folders.append(os.path.dirname(filename.replace("..\\", "")))
					str = str + "    <ClInclude Include=\"" + fname + "\">\n"
					str = str + "      <Filter>" + prefix + os.path.dirname(fname.replace("..\\", "")) + "</Filter>\n"
					str = str + "    </ClInclude>\n"
	return str

def vs2010_make_filter_dirs(prefix, folders):
	s = ""
	fl = list(set(folders))
	fl.sort()
	for f in fl:
		s = s + "    <Filter Include=\"" + prefix + " Files\\" + f.replace("/", "\\") + "\">\n"
		s = s + "      <UniqueIdentifier>{" + str(uuid.uuid4()) + "}</UniqueIdentifier>\n"
		s = s + "    </Filter>\n"
	return s

def vs2010_project(mcu_type, target_board, config_dir, includes, header_files, source_files, project_output_file):
	with open (script_path + "template.vcxproj", "r") as file:
		data = file.read()
		data = data.replace("%%CONFIG%%", config_dir)
		data = data.replace("%%INCLUDES%%", includes)
		data = data.replace("%%SOURCE_FILES%%", source_files)
		data = data.replace("%%HEADER_FILES%%", header_files)
	mkdirnotex(project_output_file)
	with open (project_output_file, "w") as file:
		file.write(data)

def vs2010_filters(mcu_type, target_board, config_dir, filters, header_files, source_files, project_output_file):
	with open (script_path + "template.vcxproj.filters", "r") as file:
		data = file.read()
		data = data.replace("%%FILTERS%%", filters)
		data = data.replace("%%SOURCE_FILES%%", source_files)
		data = data.replace("%%HEADER_FILES%%", header_files)
	mkdirnotex(project_output_file)
	with open (project_output_file, "w") as file:
		file.write(data)

#
# MPLAB-X section
#

mplabX_proj_path = "../.."

def mplabX_count_files(masks, dir):
	count = 0
	for entry in os.listdir(dir):
		if os.path.isfile(os.path.join(dir, entry)):
			for mask in masks:
				if fnmatch.fnmatch(entry, mask):
					count = count + 1
	return count

def mplabX_find_files(masks, dir):
	str = ""
	for entry in os.listdir(dir):
		if os.path.isdir(os.path.join(dir, entry)):
			title = entry
			count = mplabX_count_files(masks, os.path.join(dir, title))
			if count != 0:
				str = str + "      <logicalFolder name=\"" + title + "\" displayName=\"" + title + "\" projectFiles=\"true\">\n"
				str = str + mplabX_find_files(masks, os.path.join(dir, entry))
				str = str + "      </logicalFolder>\n"
		else:
			for mask in masks:
				if fnmatch.fnmatch(entry, mask):
					itemPath = mplabX_proj_path + dir.replace(rootdir, "/")
					itemPath = itemPath.replace("\\", "/")
					itemPath = itemPath.replace("//", "/")
					str = str + "        <itemPath>" + itemPath + "/" + entry + "</itemPath>\n"
	return str

def mplabX_scan_dirs(masks, directories):
	str = ""
	for dir in directories:
		path = os.path.join(rootdir, dir)
		str = str + "      <logicalFolder name=\"" + dir + "\" displayName=\"" + dir + "\" projectFiles=\"true\">\n"
		str = str + mplabX_find_files(masks, path)
		str = str + "      </logicalFolder>\n"
	return str

def mplabX_project(mcu_type, name, target_board, config_dir, includes, header_files, source_files, project_path):
	mkdirnotex(os.path.join(project_path, "Makefile"))
	with open (script_path + "Makefile", "r") as file:
		data = file.read()
	with open (os.path.join(project_path, "Makefile"), "w") as file:
		file.write(data)
	project_path = os.path.join(project_path, "nbproject")
	mkdirnotex(os.path.join(project_path, "nbproject"))
	with open (script_path + "configurations.xml", "r") as file:
		data = file.read()
		data = data.replace("%%NAME%%", name)
		data = data.replace("%%DEVICE%%", mcu_type)
		data = data.replace("%%CONFIG%%", config_dir)
		data = data.replace("%%INCLUDES%%", includes)
		data = data.replace("%%TARGET_BOARD%%", target_board)
		data = data.replace("%%HEADER_FILES%%", header_files)
		data = data.replace("%%SOURCE_FILES%%", source_files)
	with open (os.path.join(project_path, "configurations.xml"), "w") as file:
		file.write(data)
	with open (script_path + "project.xml", "r") as file:
		data = file.read()
		data = data.replace("%%NAME%%", name)
		data = data.replace("%%TARGET_BOARD%%", target_board)
	with open (os.path.join(project_path, "project.xml"), "w") as file:
		file.write(data)

#
# main application
#

if __name__ == '__main__':

	from optparse import OptionParser
	parser = OptionParser("pyProjectCreator.py [options]")
	parser.add_option("-n", "--name", dest="name", help="specify the project name", type="string", default="MatrixPilot", metavar="MatrixPilot")
	parser.add_option("-t", "--target", dest="target", help="specify the target board", type="string", default="UDB5", metavar="UDB5")
	parser.add_option("-m", "--mod",    dest="modules",  help="search path for module.mk file",          default=[], action='append')
	parser.add_option("-d", "--def",    dest="defines",  help="additional preprocessor defines",         default=[], action='append')
	parser.add_option("-i", "--inc",    dest="includes",    help="additional include files directory", default=[], action='append')
	parser.add_option("-c", "--cfg",    dest="config",   help="specify configuration files directory",   default="")
	parser.add_option("-o", "--out", dest="out", help="project files output path", default="output")
	parser.add_option("-r", "--root",   dest="root",        help="project root path",                     default=".")
	parser.add_option("-f", "--file",   dest="file",     help="configuration file",                      default="")
	(opts, args) = parser.parse_args()

	rootdir = opts.root

	script_path = os.path.dirname(os.path.realpath(__file__)) + "/"
	work = os.getcwd()

	if opts.target == "UDB4":
		arch = "dsPIC33FJ256GP710A"
	elif opts.target == "UDB5":
		arch = "dsPIC33FJ256GP710A"
	elif opts.target == "AUAV3":
		arch = "dsPIC33EP512MU810"
	else:
		arch = ""

	target_mk_path = opts.root + "/target-" + opts.name + ".mk"
	opts.modules  = opts.modules  + parse_options_file(target_mk_path, "modules").split(' ')
	opts.defines  = opts.includes + parse_options_file(target_mk_path, "defines").split(' ')
	opts.includes = opts.includes + parse_options_file(target_mk_path, "incpath").split(' ')
	opts.config   = opts.config   + parse_options_file(target_mk_path, "cfgpath")

	opts.out = opts.root + "/build"
	opts.file = opts.root + "/device-" + opts.target + ".mk"

	if opts.file != "":
		opts.modules = opts.modules + parse_options_file(opts.file, "modules").split(' ')
		opts.includes = opts.includes + parse_options_file(opts.file, "incpath").split(' ')
		opts.defines = opts.defines + parse_options_file(opts.file, "defines").split(' ')
		arch = "dsPIC" + parse_options_file(opts.file, "CPU")

# TODO: prehaps we want to check that the modules list (etc) is not empty..

	rootsep = "../"
	inc_list = [rootsep + str(x) for x in opts.includes]
	includes = ';'.join(inc_list)

	filters = ""
	project = os.path.join(opts.out, opts.name + "-" + opts.target)
	if opts.target == "SIL":
		sources = vs2010_scan_dirs(["*.c"], 1, opts.modules)
		headers = vs2010_scan_dirs(["*.h"], 0, [opts.config] + opts.modules + ["libUDB"])
		project_path = project + ".vcxproj"
		print "writing: " + project_path
		vs2010_project(arch, opts.target, rootsep + opts.config, includes, headers, sources, project_path)
		sources = vs2010_scan_filter_dirs(["*.c"], 1, opts.modules, "Source Files\\")
		headers = vs2010_scan_filter_dirs(["*.h"], 0, [opts.config] + opts.modules + ["libUDB"], "Header Files\\")
		filters = filters + vs2010_make_filter_dirs("Source", source_folders)
		filters = filters + vs2010_make_filter_dirs("Header", header_folders)
		project_path = project + ".vcxproj.filters"
		vs2010_filters(arch, opts.target, rootsep + opts.config, filters, headers, sources, project_path)
	else:
		mplab8_scan_dirs("*.c", opts.modules)
		mplab8_scan_dirs("*.s", opts.modules)
		mplab8_scan_dirs("*.h", [opts.config] + opts.modules)
		project_path = project + ".mcp"
		print "writing: " + project_path
		mplab8_project(arch, opts.target, rootsep + opts.config, includes, project_path)
		headers = mplabX_scan_dirs(["*.h", "*.inc"], [opts.config] + opts.modules)
		sources = mplabX_scan_dirs(["*.c", "*.s"], opts.modules)
		project_path = project + ".X"
		print "writing: " + project_path
		includes = ';'.join(["../" + str(x) for x in inc_list])
		mplabX_project(arch, opts.name, opts.target, "../" + rootsep + opts.config, includes, headers, sources, project_path)
