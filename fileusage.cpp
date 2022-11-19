#include <iostream>
#include<filesystem>
#include <vector>
#include <regex>
#include <map>
#include <utility>   
#include <iomanip>
#include "TextTable.h" // Source : https://github.com/haarcuba/cpp-text-table
#include <locale>
#include <sstream>

using namespace std;
using namespace::filesystem;


// class for extension object
class extension {


public:
// data memebers
	string name;
	unsigned totalFiles;
	unsigned long long totalSize;
	string extFamily;

	// constructors

	extension() :
		name(""), totalFiles(0), totalSize(0) {}


	extension(string name, unsigned totalFiles , unsigned long long totalSize ) :
	 name(name), totalFiles(totalFiles), totalSize(totalSize) {}

	bool operator > (const extension& str) const
	{
		return (name > str.name);
	}

};

// operator overriding for sorting purposes
 bool operator< (const extension& lhs, const extension& rhs)
{
	return lhs.name < rhs.name;
}
 bool sortBySize(const extension& lhs, const extension& rhs) {
	 return lhs.totalSize < rhs.totalSize;
}

// main scan + report + print
vector<int> isExtExcict(vector<vector<extension>>& extF, string& extensionTarget) {
	vector<int> found(2); 
	int index = 0;
	int innerIndex;
	
	for (; index < extF.size(); ++index) {
		for(innerIndex = 0;innerIndex < extF[index].size(); ++innerIndex)
			if (extF[index][innerIndex].name == extensionTarget) {
			found[0] = index; found[1] = innerIndex;
			return found;
		}
		
}
found[0] = -1;  found[1] = -1;
return found;
}
vector<vector<extension>> report(vector<path>& paths) {
vector<extension> extV;
vector<vector<extension>> extFamily = vector<vector<extension>>(5);
extension newExt;
vector<int> index;
for (filesystem::path p : paths) {
index = isExtExcict(extFamily, p.extension().string());
	if (index[0] >= 0 ) {
		extFamily[index[0]][index[1]].totalFiles++;
		extFamily[index[0]][index[1]].totalSize += file_size(p);
		}

	else {
		newExt = extension(p.extension().string(), 1, file_size(p));

		if (regex_match(newExt.name, regex(".c|.h"))) {
			newExt.extFamily = "C Files";
			extFamily[0].push_back(newExt);
		}

		else if (regex_match(newExt.name, regex(".cc|.cp|.cpp|.cxx|.c\\+\\+|.hpp|.hxx"))) {
			newExt.extFamily = "C++ Files";
			extFamily[1].push_back(newExt);
		}

		else if (regex_match(newExt.name, regex(".class|.j|.jad|.jar|.java|.jsp|.ser"))) {
			newExt.extFamily = "Java Files";
			extFamily[2].push_back(newExt);
		}
		else if (regex_match(newExt.name, regex(".cs|.vb|.jsl"))) {
			newExt.extFamily = "C# Files";
			extFamily[3].push_back(newExt);
		}
		else if (regex_match(newExt.name, regex(".htm|.html|.html5|.js|.jse|.jsc"))) {
			newExt.extFamily = "Web Files";
			extFamily[4].push_back(newExt);
		}

		else {
			newExt.extFamily = newExt.name.substr(1) + " Files" ;
			vector<extension> newFamily; newFamily.push_back(newExt);
			extFamily.push_back(newFamily);
		}
	}
	
} // end for loop


return extFamily;
}
void regexScan(path const& f, vector<path>& allFiles, regex reg, bool rec) {

	directory_iterator d(f);	// first entry of folder 'f'
	directory_iterator e;		// virtual match to the 'end' of any folder

	if (rec)
		while (d != e) {

			if (regex_match(d->path().extension().string(), reg))
				allFiles.push_back(d->path());

			else if (is_directory(d->path()))
				regexScan(d->path(), allFiles, reg, rec);
			++d;
		}

	else
		while (d != e) {

			if (regex_match(d->path().extension().string(), reg))
				allFiles.push_back(d->path());

			++d;
		}

}
void printOut(vector<vector<extension>> test, bool sortSize, bool reversed, bool sum, path targetFolder)
{
	unsigned totalFiles;
	unsigned long long totalSize;
	unsigned totalExt;

	unsigned long long allTotalSize = 0;
	unsigned allTotal = 0;
	unsigned allTotalExt = 0;
	stringstream stream;
	stream.imbue(locale(""));

	// for summary if switch is specified
	TextTable s('-', ':', '+');
	s.add("Ext");
	s.add("#");
	s.add("Total");
	s.endOfRow();


	for (unsigned i = 0; i < test.size(); ++i) {
		if (test[i].size() != 0) {
			// sorting 
			if (sortSize) {
				if (reversed)
					sort(test[i].rbegin(), test[i].rend(), sortBySize);

				else
					sort(test[i].begin(), test[i].end(), sortBySize);
			} // end size Sorting

			else {
				if (reversed)
					sort(test[i].rbegin(), test[i].rend());

				else
					sort(test[i].begin(), test[i].end());
			} // end name Sorting




			totalExt = test[i].size();

			totalFiles = 0;
			totalSize = 0;
			allTotalExt++;
			TextTable t('-', ':', '+');
			// printing content

			cout << test[i][0].extFamily << " : " << canonical( targetFolder) << endl;

			t.add("Ext");
			t.add("#");
			t.add("Total");
			t.endOfRow();
			for (extension ext : test[i]) {
				totalSize += ext.totalSize;
				totalFiles += ext.totalFiles;


				t.add(ext.name);
				stream << ext.totalFiles;
				t.add(stream.str()); stream.str("");
				stream << ext.totalSize;
				t.add(stream.str()); stream.str("");
				t.endOfRow();

			}

			stream << totalExt;
			t.add(stream.str()); stream.str("");
			stream << totalFiles;
			t.add(stream.str()); stream.str("");
			stream << totalSize;
			t.add(stream.str()); stream.str("");
			t.endOfRow();


			// for summary
			if (sum == true) {

				allTotal += totalFiles;
				allTotalSize += totalSize;
				s.add(test[i][0].extFamily);
				stream << totalFiles; s.add(stream.str());  stream.str("");
				stream << totalSize;  s.add(stream.str()); stream.str("");
				s.endOfRow();
			}

			cout << t << endl;
		}  // end main if



	} // end for


	if (sum == true) {

		stream << allTotalExt;   s.add(stream.str());  stream.str("");
		stream << allTotal;      s.add(stream.str());  stream.str("");
		stream << allTotalSize;  s.add(stream.str());  stream.str("");
		s.endOfRow();

		cout << "All searches: " << canonical(targetFolder) << endl;
		cout << s << endl;
	}

}


// these four functions are only for default (NO SWitches specified)
int isExtExcictDefault(vector<extension>& ext, string& extensionTarget) {
	int index = 0;

	for (; index < ext.size(); ++index) {
			if (ext[index].name == extensionTarget) {
				return index;
			}
	}
	index = -1;
	return index;
}
vector<extension> defaultReporting(vector<path>& paths) {
	vector<extension> extV;
	extension newExt;
	int index;
	for (path p : paths) {
		index = isExtExcictDefault(extV, p.extension().string());
		if (index >= 0) {
			extV[index].totalFiles++;
			extV[index].totalSize += file_size(p);
		}

		else {
			newExt = extension(p.extension().string(), 1, file_size(p));
				extV.push_back(newExt);
			} // end else
 
	} // end for loop


	return extV;
}
vector<path> defaultScan(path const& f, vector<path>& allFiles) {
	
	directory_iterator d(f);	// first entry of folder 'f'
	directory_iterator e;		// virtual match to the 'end' of any folder
	while (d != e) {

			if (is_regular_file(d->path()) && d->path().has_extension())
			     allFiles.push_back(d->path());

			else if(is_directory(d->path()))
				defaultScan(d->path(),allFiles);
		++d;
	}

	

	return allFiles;
	}
void defaultPrintOut(path pathFolder) {
	vector<path> p;
	defaultScan(pathFolder, p);
	vector<extension> test;
	test = defaultReporting(p);

	unsigned totalExts = test.size();
	unsigned totalFiles = 0;
	stringstream ss;
	ss.imbue(locale(""));

	unsigned long long totalSizes = 0;
	cout << "All files: " << canonical( pathFolder) << endl;
	TextTable t;
	t.add("Ext");
	t.add("#");
	t.add("Total");
	t.endOfRow();

	sort(test.begin(), test.end());

	for (extension ext : test) {
		t.add(ext.name);

		ss << fixed << ext.totalFiles;    t.add(ss.str());    ss.str("");
		ss << fixed << ext.totalSize;     t.add(ss.str());    ss.str("");

		t.endOfRow();


		totalFiles += ext.totalFiles;
		totalSizes += ext.totalSize;
	}
	ss << fixed << totalExts;     t.add(ss.str());    ss.str("");
	ss << fixed << totalFiles;     t.add(ss.str());    ss.str("");
	ss << fixed << totalSizes;     t.add(ss.str());    ss.str("");
	t.endOfRow();
	cout << t << endl;

}

// this is for verbose scanning
map<string, map<path, unsigned>> vScan(path const& f, vector<path>& allFiles, regex reg, bool rec, map<string, map<path, unsigned>>& dirMap) {

	directory_iterator d(f);	// first entry of folder 'f'
	directory_iterator e;		// virtual match to the 'end' of any folder

	map<string, map<path, unsigned>>::iterator outIter;
	map<path, unsigned >::iterator inIter;

	//map<string, vector<pair<path, unsigned>>> dirMap;
	if (rec)
		while (d != e) {

			if (regex_match(d->path().extension().string(), reg)) {
				outIter = dirMap.find(d->path().extension().string());
				if (outIter != dirMap.end()) {
					inIter = outIter->second.find(d->path().parent_path());
					if (inIter != outIter->second.end())
						inIter->second++;
					else
						outIter->second.emplace(make_pair(d->path().parent_path(), 1));
				} // end outIter if
				else {
					map<path, unsigned> newDir;newDir.emplace(make_pair(d->path().parent_path(), 1));
					dirMap.emplace(make_pair(d->path().extension().string(), newDir));
				}



			}


			else if (is_directory(d->path())) {
				vScan(d->path(), allFiles, reg, rec, dirMap);
			}

			++d;
		}
	else
		while (d != e) {

			if (regex_match(d->path().extension().string(), reg))
				allFiles.push_back(d->path());

			++d;
		}

	return dirMap;

}


int main(int argc, char* argv[]){
cout << "Abadi Basaad 2017, Fileusage v1.0\n" << endl;


if(argc == 1){  // if no switches or folder specified.
defaultPrintOut(path("."));
return EXIT_SUCCESS;
}




// switches bool variables 
bool recursive = false, reversed = false, sortSize = false, v = false, summary = false;

// string regex for all switches
string CcFilesString(".cs|.vb|.jsl|");
string webFilesString(".htm|.html|.html5|.js|.jse|.jsc|");
string cPlusFilesString(".cc|.cp|.cpp|.cxx|.c\\+\\+|.hpp|.hxx|");
string cFilesString(".c|.h|");
string javaFilesString(".class|.j|.jad|.jar|.java|.jsp|.ser|");

unsigned i = 1;
string stringReg;

if(argv[i][0] == '-')
for (char ch : (string)argv[i]){
	if (ch == 'c') {stringReg += cFilesString; }
	
	else if (ch == '+') { stringReg +=cPlusFilesString;}
	
	else if (ch == 'j'){ stringReg += javaFilesString; }
	else if (ch == '#'){stringReg += CcFilesString;  }
	else if (ch == 'w'){ stringReg += webFilesString;}
	else if (ch == 'x'){ i++;stringReg += (string)argv[i];}
	else if (ch == 's') { summary = true; }
	else if (ch == 'r') recursive = true;
	else if (ch == 'R') reversed = true;
	else if (ch == 'S') sortSize = true;
	else if (ch == 'v') v = true;
	else if (ch == 'h') { cout << "fileusage [-switches] [folder]" << endl; return 0;}
	else if(ch == '-');

	else
	cout << "'" << ch << "'" << " is not a recognized switch; it would be ignored." << endl; 

	

}

else {
	cout << "A hyphen '-' must be present before switches." << endl;
	
	EXIT_FAILURE;
}

// if no file extensions specified to look for, consider as default one
if (stringReg.empty()) {
	cout << "No extensions specified to look for, running default scan:\n" << endl;
	if(argc == i+2)
	defaultPrintOut(path((string)argv[i + 1]));
	else if (argc == i + 1)
	defaultPrintOut(path((string)argv[i ]));
	else
	defaultPrintOut(path("."));
	return EXIT_SUCCESS;
}

if(stringReg[stringReg.size() - 1] == '|')
stringReg.pop_back();
path targetFolder((string)argv[i+1]); 

regex filesReg(stringReg);
vector<path> allFiles;
regexScan(targetFolder, allFiles, filesReg, recursive);

vector<vector<extension>> test = report(allFiles);

if (v) {
	map<string, map<path, unsigned>> directory;
	vScan(targetFolder, allFiles, filesReg, recursive, directory);

	for (auto it = directory.begin(); it != directory.end(); ++it) {
		cout << it->first << " (" << it->second.size() << " folders)" << endl;
		for (auto p : it->second) {
			cout << p.second << ":   " << canonical(p.first) << endl;
		}
		cout << endl;
}
} // end verbous

cout << endl;
printOut(test,sortSize,reversed, summary, targetFolder);




}
