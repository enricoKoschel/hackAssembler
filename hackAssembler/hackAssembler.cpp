#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string inputFileName;
string outputFileName;

int handleArguments(int argc, char* argv[]) {
	if (argc == 3) {
		if (strcmp(argv[1], "-i") == 0) {
			inputFileName = argv[2];
			int pos = inputFileName.find(".asm", 1);
			if (pos != string::npos) {
				outputFileName = inputFileName.substr(0, pos) + ".hack";
			}
			else {
				return 1;
			}
		}
		else {
			return 1;
		}
	}
	else {
		return 1;
	}
	return 0;
}

int main(int argc, char* argv[]) {
	if (handleArguments(argc, argv) == 1) {
		cerr << "Usage: hackAssembler.exe -i [inputFileName.asm]\noutput filename is inputFileName.hack";
		return 1;
	}

	ifstream inputFile(inputFileName);
	ofstream outputFile(outputFileName, ios::trunc);

	if (!inputFile.is_open()) {
		cerr << inputFileName << " could not be opened!";
		return 1;
	}
	if (!outputFile.is_open()) {
		cerr << outputFileName << " could not be created/opened!";
		return 1;
	}

	return 0;
}