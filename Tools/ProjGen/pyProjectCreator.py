'''
Created on 31 August 2014

@author: Robert Dickenson
'''

import os, fnmatch
import uuid
import re

rootdir = ""
script_path = ""

def remove_duplicates(input):
  output = []
  for x in input:
    if x not in output:
      output.append(x)
  return output

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

def redef(defines, sep):
	redefs = ''
	for d in defines:
		if not d == '':
			redefs = redefs + sep + d
	print "redefs: ", redefs
	return redefs

#
# configuration from makefile scripts
#

def parse_options_file(filename, option):
	str = ''
	with open (filename, "r") as file:
		data = file.read()
		match = re.search(r"(^" + option + " .= )(.*$)", data, re.MULTILINE)
		if match:
			str = match.group(2)
	return str

def parse_mk_file(filename, option, list):
	with open (filename, "r") as file:
		data = file.read()
		match = re.search(r"(^" + option + " .= )(.*$)", data, re.MULTILINE)
		if match:
			list.extend(match.group(2).split())
	return list

#
# EXAMPLE MAKEFILE/target-* FILE:
#
# modules := MatrixPilot MAVLink libDCM
# incpath := MAVLink/include
# cfgpath := Config

#
# EXAMPLE MAKEFILE/device-* FILE:
#
# TOOLCHAIN ?= XC16
# TARGET_TYPE := hex
# CPU := 33FJ256GP710A
# modules := libUDB libDCM MatrixPilot MAVLink
# incpath := Config Microchip Microchip/Include libVectorMatrix
#


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

def mplab8_project(mcu_type, target_board, prjname, root_sep, config_dir, includes, project, defines):
	config = ''
	for e in config_dir:
		config = config + root_sep + e + ';'
	fixdeps = root_sep + config_dir[0]
	defs = ""
	for d in defines:
		if d:
			defs = defs + "-D" + d + " "
	with open (script_path + "mplab8-template.txt", "r") as file:
		data = file.read()
		data = data.replace("%%PROJECT%%", prjname)
		data = data.replace("%%DEVICE%%", mcu_type)
		data = data.replace("%%CONFIG%%", config)
		data = data.replace("%%INCLUDES%%", includes)
		data = data.replace("%%TARGET_BOARD%%", target_board)
		data = data.replace("%%FILE_SUBFOLDERS%%", file_subfolders)
		data = data.replace("%%GENERATED_FILES%%", other_files)
		data = data.replace("%%OTHER_FILES%%", other_files)
		data = data.replace("%%FILE_INFO%%", file_info)
		data = data.replace("%%EXTRA_DEFS%%", defs)
		data = data.replace("%%FIXDEPS%%", fixdeps)
	mkdirnotex(project + ".mcp")
	with open (project + ".mcp", "w") as file:
		print "writing: " + project + ".mcp"
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

def vs2010_project(mcu_type, target_board, root_sep, config_dir, includes, header_files, source_files, project, defines, prjname):

	config = ''
	for e in config_dir:
		config = config + root_sep + e + ';'
	defs = ""
	for d in defines:
		if d:
			defs = defs + d + " "
	with open (script_path + "template.vcxproj", "r") as file:
		data = file.read()
		data = data.replace("%%CONFIG%%", config)
		data = data.replace("%%PROJECT%%", prjname)
		data = data.replace("%%INCLUDES%%", includes)
		data = data.replace("%%SOURCE_FILES%%", source_files)
		data = data.replace("%%HEADER_FILES%%", header_files)
		data = data.replace("%%EXTRA_DEFS%%", defs)
	mkdirnotex(project + ".vcxproj")
	with open (project + ".vcxproj", "w") as file:
		print "writing: " + project + ".vcxproj"
		file.write(data)
	with open (script_path + "template.sln", "r") as file:
		data = file.read()
		data = data.replace("%%PROJECT%%", prjname)
	mkdirnotex(project + ".sln")
	with open (project + ".sln", "w") as file:
		print "writing: " + project + ".sln"
		file.write(data)

def vs2010_filters(mcu_type, target_board, root_sep, config_dir, filters, header_files, source_files, project):
	with open (script_path + "template.vcxproj.filters", "r") as file:
		data = file.read()
		data = data.replace("%%FILTERS%%", filters)
		data = data.replace("%%SOURCE_FILES%%", source_files)
		data = data.replace("%%HEADER_FILES%%", header_files)
	mkdirnotex(project + ".vcxproj.filters")
	with open (project + ".vcxproj.filters", "w") as file:
		print "writing: " + project + ".vcxproj.filters"
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

def mplabX_find_files(masks, dir, recursions):
	str1 = ""
	ws = "  " * recursions

	if os.path.isdir(dir):
		itemList = []
		dirList = []
		for entry in os.listdir(dir):
			if os.path.isdir(os.path.join(dir, entry)):
				dirList.append(entry)
			else:
				for mask in masks:
					if fnmatch.fnmatch(entry, mask):
						if entry:
							itemList.append(entry)
		dirList.sort(key=str.lower)
		for entry in dirList:
			title = entry
			count = mplabX_count_files(masks, os.path.join(dir, title))
			if count != 0:
				if len(title) < 12:
					str1 = str1 + ws + "      <logicalFolder name=\"" + title + "\" displayName=\"" + title + "\" projectFiles=\"true\">\n"
				else:
					str1 = str1 + ws + "      <logicalFolder name=\"" + title + "\"\n"
					str1 = str1 + ws + "                     displayName=\"" + title + "\"\n"
					str1 = str1 + ws + "                     projectFiles=\"true\">\n"
				str1 = str1 + mplabX_find_files(masks, os.path.join(dir, entry), recursions + 1)
				str1 = str1 + ws + "      </logicalFolder>\n"
		itemList.sort(key=str.lower)
		for entry in itemList:
			itemPath = mplabX_proj_path + dir.replace(rootdir, "/")
			itemPath = itemPath.replace("\\", "/")
			itemPath = itemPath.replace("//", "/")
			str1 = str1 + ws + "      <itemPath>" + itemPath + "/" + entry + "</itemPath>\n"
	return str1

def mplabX_scan_dirs(masks, directories):
	str1 = ""
	dirList = []
	for dir in directories:
		dirList.append(dir)
	dirList.sort(key=str.lower)
	for dir in dirList:
		path = os.path.join(rootdir, dir)
		if len(dir) < 12:
			str1 = str1 + "      <logicalFolder name=\"" + dir + "\" displayName=\"" + dir + "\" projectFiles=\"true\">\n"
		else:
			str1 = str1 + "      <logicalFolder name=\"" + dir + "\"\n"
			str1 = str1 + "                     displayName=\"" + dir + "\"\n"
			str1 = str1 + "                     projectFiles=\"true\">\n"
		str1 = str1 + mplabX_find_files(masks, path, 1)
		str1 = str1 + "      </logicalFolder>\n"
	return str1

def mplabX_project(mcu_type, name, target_board, root_sep, config_dir, includes, header_files, source_files, project_path, defines, offpath):
	print "writing: " + project_path
	mkdirnotex(os.path.join(project_path, "Makefile"))
	with open (script_path + "Makefile", "r") as file:
		data = file.read()
	with open (os.path.join(project_path, "Makefile"), "w") as file:
		file.write(data)
	project_path = os.path.join(project_path, "nbproject")
	mkdirnotex(os.path.join(project_path, "nbproject"))

	config = ''
	for e in config_dir:
		config = config + root_sep + e + ';'
	defs = ""
	for d in defines:
		if d:
			defs = defs + d + " "
	with open (script_path + "configurations.xml", "r") as file:
		data = file.read()
		data = data.replace("%%NAME%%", name)
		data = data.replace("%%DEVICE%%", mcu_type)
		data = data.replace("%%CONFIG%%", config)
		data = data.replace("%%INCLUDES%%", includes)
		data = data.replace("%%TARGET_BOARD%%", target_board)
		data = data.replace("%%HEADER_FILES%%", header_files)
		data = data.replace("%%SOURCE_FILES%%", source_files)
		data = data.replace("%%EXTRA_DEFS%%", defs)
	with open (os.path.join(project_path, "configurations.xml"), "w") as file:
		file.write(data)
	with open (script_path + "project.xml", "r") as file:
		data = file.read()
		data = data.replace("%%NAME%%", name)
		data = data.replace("%%TARGET_BOARD%%", target_board)
	with open (os.path.join(project_path, "project.xml"), "w") as file:
		file.write(data)

#
# Em::Blocks section
#

def emBlocks_scan_dirs(masks, sources, directories):
	str = ""
	for dir in directories:
		path = os.path.join(rootdir, dir)
		for mask in masks:
			files = find(mask, path)
			for filename in files:
				if sources == 1:
					if mask == "*.s":
						str = str + "\t\t<Unit filename=\"" + filename.replace("/", "\\") + "\">\n\t\t\t<Option compilerVar=\"ASM\" />\n\t\t</Unit>\n"
					else:
						str = str + "\t\t<Unit filename=\"" + filename.replace("/", "\\") + "\">\n\t\t\t<Option compilerVar=\"CC\" />\n\t\t</Unit>\n"
				else:
					str = str + "\t\t<Unit filename=\"" + filename.replace("/", "\\") + "\" />\n"
	return str

def emBlocks_project(mcu_type, name, target_board, config_dir, defines, includes, headers, sources, project):
	with open (script_path + "template.ebp", "r") as file:
		data = file.read()
		data = data.replace("%%PROJECT%%", name + "-" + target_board)
		data = data.replace("%%DEFINES%%", defines)
		data = data.replace("%%INCLUDES%%", includes)
		data = data.replace("%%TARGET_BOARD%%", target_board)
		data = data.replace("%%SOURCE_FILES%%", sources)
		data = data.replace("%%HEADER_FILES%%", headers)
	mkdirnotex(project + ".ebp")
	with open (project + ".ebp", "w") as file:
		print "writing: " + project + ".ebp"
		file.write(data)


#
# main application
#

if __name__ == '__main__':

	from optparse import OptionParser
	parser = OptionParser("pyProjectCreator.py [options]")
	parser.add_option("-r", "--root",   dest="root",     help="project root path",                       default=".")
	parser.add_option("-n", "--name",   dest="name",     help="specify the project name", type="string", default="MatrixPilot", metavar="MatrixPilot")
	parser.add_option("-t", "--target", dest="target",   help="specify the target board", type="string", default="UDB5", metavar="UDB5")
	parser.add_option("-m", "--mod",    dest="modules",  help="search path for module.mk file",          default=[], action='append')
	parser.add_option("-d", "--def",    dest="defines",  help="additional preprocessor defines",         default=[], action='append')
	parser.add_option("-i", "--inc",    dest="includes", help="additional include files directory",      default=[], action='append')
	parser.add_option("-c", "--cfg",    dest="config",   help="specify configuration files directory",   default=[], action='append')
	parser.add_option("-o", "--out",    dest="out",      help="project files output path",               default="_build")
	parser.add_option("-f", "--file",   dest="file",     help="configuration file",                      default="")
	parser.add_option("-k", "--make",   dest="mkdir",    help="path to makefile includes",               default="/Tools/makefiles")
	parser.add_option("-s", "--short",  dest="shortfn",  help="only generate short project file names",  default="")
	parser.add_option("-p", "--path",   dest="offpath",  help="prepend offset path to source files",     default=[], action='append')
	(opts, args) = parser.parse_args()

	rootdir = opts.root
	opts.out = opts.root + "/" + opts.out + "/" + opts.name

	script_path = os.path.dirname(os.path.realpath(__file__)) + "/"
	work = os.getcwd()

	if opts.target == "UDB4":
		arch = "dsPIC33FJ256GP710A"
	elif opts.target == "UDB5":
		arch = "dsPIC33FJ256GP710A"
	elif opts.target == "AUAV3":
		arch = "dsPIC33EP512MU810"
	elif opts.target == "nucleo":
		arch = "STM32F401xE"
	elif opts.target == "PX4":
		arch = "STM32F427xx"
	else:
		arch = ""

#
# Parse options from the 'target-*.mk' specific makefile
	target_mk_path = opts.root + opts.mkdir + "/target-" + opts.name + ".mk"
	opts.modules  = parse_mk_file(target_mk_path, "modules", opts.modules)
	opts.defines  = parse_mk_file(target_mk_path, "defines", opts.defines)
	opts.includes = parse_mk_file(target_mk_path, "incpath", opts.includes)
	opts.config   = parse_mk_file(target_mk_path, "cfgpath", opts.config)
	opts.offpath  = parse_mk_file(target_mk_path, "offpath", opts.offpath)

#
# Parse extra options from the 'device-*.mk' specific makefile
	opts.file = opts.root + opts.mkdir + "/device-" + opts.target + ".mk"
	if opts.file != "":
		opts.modules  = parse_mk_file(opts.file, "modules", opts.modules)
		opts.includes = parse_mk_file(opts.file, "incpath", opts.includes)
		opts.defines  = parse_mk_file(opts.file, "defines", opts.defines)
		arch = ''.join(parse_mk_file(opts.file, "CPU", ["dsPIC"]))

	for mod in opts.modules:
		mod_incs = parse_options_file(opts.root + "/" + mod + "/module.mk", "local_inc").split(' ')
		if not mod_incs == ['']:
			for mi in mod_incs:
				opts.includes = opts.includes + [mod + "/" + mi]
		mod_defs = parse_options_file(opts.root + "/" + mod + "/module.mk", "defines").split(' ')
		if not mod_defs == ['']:
			for md in mod_defs:
				if not md == '':
					opts.defines = opts.defines + [md]

	rootsep = "../"
	rootsep = rootsep + "".join(opts.offpath) 
	mplabX_proj_path = "".join(opts.offpath) + mplabX_proj_path

	for c in opts.config:
		c = "".join(opts.offpath) + c

	inc_list = [rootsep + str(x) for x in opts.includes]
	includes = ';'.join(opts.offpath).join(inc_list)
	filters = ""
	prjname = opts.name + "-" + opts.target.lower()
	if opts.shortfn == "":
		prjname = prjname + "-" + opts.config[0].replace("/", " ").split(" ")[-1]
	project = os.path.join(opts.out, prjname)
	print "project: " + prjname

	if opts.target == "PX4":
		sources  = emBlocks_scan_dirs(["*.c", "*.s"], 1, opts.modules)
		headers  = emBlocks_scan_dirs(["*.h"], 0, opts.config + opts.modules + ["libUDB"])
		includes = ""
		defines = "\t\t\t<Add option=\"-D" + opts.target + "\" />\n"
		for d in opts.defines:
			if d:
				defines = defines + "\t\t\t<Add option=\"-D" + d + "\" />\n"
		for inc in inc_list:
			includes = includes + "\t\t\t<Add directory=\"" + inc + "\" />\n"
# add the config paths last
		for inc in opts.config:
			includes = includes + "\t\t\t<Add directory=\"" + rootsep + inc + "\" />\n"
		emBlocks_project(arch, opts.name, opts.target, opts.config, defines, includes, headers, sources, project)
	elif opts.target == "SIL":
		sources = vs2010_scan_dirs(["*.c"], 1, opts.modules)
		headers = vs2010_scan_dirs(["*.h"], 0, opts.config + opts.modules + ["libUDB"])
		headers = '\r'.join(remove_duplicates(headers.split('\r')))
		vs2010_project(arch, opts.target, rootsep, opts.config, includes, headers, sources, project, opts.defines, prjname)
		sources = vs2010_scan_filter_dirs(["*.c"], 1, opts.modules, "Source Files\\")
		headers = vs2010_scan_filter_dirs(["*.h"], 0, opts.config + opts.modules + ["libUDB"], "Header Files\\")
		filters = filters + vs2010_make_filter_dirs("Source", source_folders)
		filters = filters + vs2010_make_filter_dirs("Header", header_folders)
		vs2010_filters(arch, opts.target, rootsep, opts.config, filters, headers, sources, project)
	else:
		mplab8_scan_dirs("*.c", opts.modules)
		mplab8_scan_dirs("*.s", opts.modules)
		mplab8_scan_dirs("*.h", opts.config + opts.modules)
		mplab8_project(arch, opts.target, prjname, rootsep, opts.config, includes, project, opts.defines)
		headers = mplabX_scan_dirs(["*.h", "*.inc"], opts.config + opts.modules)
		sources = mplabX_scan_dirs(["*.c", "*.s", "*.S"], opts.modules)
		includes = ';'.join(["../" + str(x) for x in inc_list])
		mplabX_project(arch, opts.name, opts.target, "../" + rootsep, opts.config, includes, headers, sources, project + ".X", opts.defines, "".join(opts.offpath))

#	lst = ['Robert', 'Suzie', 'Dickenson', 'German', 'Aliye', 'Aria', 'Tols']
#	print "lst: ", lst
#	lst.sort()
#	print "lst: ", lst

