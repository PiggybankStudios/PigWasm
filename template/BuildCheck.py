
import sys
import re
import os

buildCallTemplate = "clang \"{inputFile}\"";
buildCallTemplate += " -Wno-enum-compare-switch";
buildCallTemplate += " -DWASM_COMPILATION -DDEBUG_BUILD=1 -DDEVELOPER_BUILD=1 -DASSERTIONS_ENABLED=1";
buildCallTemplate += " --target=wasm32 -mbulk-memory -c -O0 --no-standard-includes";
sourceDirectory = "..\\source"
libDirectory = "..\\lib\\include"
inputFilePath = sourceDirectory + "\\compile_me.cpp";
includeFolders = [
	sourceDirectory,
	libDirectory,
	libDirectory + "\\pig_wasm",
	libDirectory + "\\pig_wasm\\std",
];
for includeFolder in includeFolders:
#
	buildCallTemplate += " -I\"%s\"" % (includeFolder);
#
print("Doing build check using \"%s\"..." % (inputFilePath));

searchRegex = "[ \\t]*\\#include[ \\t]*\\\"([^\\\"\\n]*)\\\"";

numFilesFound = 0;
with open(inputFilePath) as inputFile:
#
	inputFileContents = inputFile.read();
	searchResults = re.findall(searchRegex, inputFileContents);
	for match in searchResults:
	#
		numFilesFound += 1;
		
		# print("Found match \"%s\"" % (match));
		fullPath = None;
		for includeFolder in includeFolders:
		#
			potentialFullPath = includeFolder + "\\" + match;
			if (os.path.exists(potentialFullPath)):
			#
				fullPath = potentialFullPath;
				break;
			#
		#
		
		if (fullPath != None):
		#
			# print("Found file at \"%s\"" % (fullPath));
			buildCall = buildCallTemplate.replace("{inputFile}", fullPath);
			# print("Running %s" % (buildCall));
			os.system(buildCall);
		#
		else:
		#
			print("Couldn't find \"%s\" in any of the include directories" % (match));
		#
	#
#

print("Build checked %d files!" % (numFilesFound));
