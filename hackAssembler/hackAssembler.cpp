#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

string inputFileName;
string outputFileName;

enum class command {
	A_COMMAND,
	C_COMMAND,
	L_COMMAND,
};

class parserModule {
private:
	ifstream inputFile;
	string symbol;
	bool hasMoreCommands = false;
	command commandType;
	string dest;
	string comp;
	string jump;
public:	
	parserModule(string inputFileName) {
		inputFile = ifstream(inputFileName);
	}

	void advance() {
		if (!hasMoreCommands) return;
	}

	string getSymbol() {
		if (commandType == command::C_COMMAND) return "";
	}

	command getCommandType() {
		return commandType;
	}

	bool getHasMoreCommands() {
		return hasMoreCommands;
	}

	string getDest() {
		if (commandType != command::C_COMMAND) return "";
	}

	string getComp() {
		if (commandType != command::C_COMMAND) return "";
	}

	string getJump() {
		if (commandType != command::C_COMMAND) return "";
	}

	bool isFileOpen() {
		return inputFile.is_open();
	}
};

class codeModule {
private:
	ofstream outputFile;
public:

	codeModule(string outputFileName) {
		outputFile = ofstream(outputFileName, ios::trunc);
	}

	string assembleDest(string mnemonic) {

	}

	string assembleComp(string mnemonic) {

	}

	string assembleJump(string mnemonic) {

	}

	bool isFileOpen() {
		return outputFile.is_open();
	}
};

class symbolTable {
private:
	unordered_map<string, int> table;

	bool contains(string symbol) {
		return table.find(symbol) != table.end();
	}
public:
	symbolTable() {}

	void addEntry(string symbol, int address) {
		table.insert({symbol, address});
	}

	int getAddress(string symbol) {
		if (contains(symbol)) {
			return table.find(symbol)->second;
		}
		else {
			return -1;
		}
	}
};

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
	
	parserModule parser = parserModule(inputFileName);
	codeModule code = codeModule(outputFileName);

	if (!parser.isFileOpen()) {
		cerr << inputFileName << " could not be opened!";
		return 1;
	}
	if (!code.isFileOpen()) {
		cerr << outputFileName << " could not be created/opened!";
		return 1;
	}

	return 0;
}