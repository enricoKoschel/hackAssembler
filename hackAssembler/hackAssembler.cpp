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
	NO_COMMAND,
};

class parserModule {
private:
	ifstream inputFile;
	string symbol;
	bool hasMoreCommands = true;
	command commandType;
	string dest;
	string comp;
	string jump;
	string line;
	int currentLine;

	void checkSpaces(int i) {
		if (i != 0) {
			for (int j = i - 1; j >= 0; j--) {
				if (line[j] != ' ') {
					cerr << "Error on line " << currentLine << " at " << line[i] << endl;
					exit(1);
				}
			}
		}
	}
public:	
	parserModule(string inputFileName) {
		inputFile = ifstream(inputFileName);
		if (!inputFile.is_open()) {
			cerr << inputFileName << " could not be opened!";
			exit(1);
		}
	}

	void advance() {
		if (!hasMoreCommands) return;
		if (getline(inputFile, line)) {
			currentLine++;
			for (int i = 0; i < line.length(); i++) {
				switch (line[i]) {
				case '/':
					if (line[i + 1] == '/') {
						advance();
					}
					else {
						cerr << "Error on line " << currentLine << " at " << line[i] << endl;
					}
					return;
				case ' ':
					break;
				case '@':
					checkSpaces(i);
					if (!isalnum(line[i + 1])) {
						cerr << "Error on line " << currentLine << " at " << line[i] << endl;
						exit(1);
					}
					//A_COMMAND found
					commandType = command::A_COMMAND;
					return;
				case '(':
					checkSpaces(i);
					//L_COMMAND found
					commandType = command::L_COMMAND;
					return;
				case ';':
					jump = line.substr(i);
					if (jump.length() != 3) {
						cerr << "Error on line " << currentLine << " at " << line[i] << endl;
						exit(1);
					}
					cout << jump << endl;
					return;
				case '\n':
					commandType = command::NO_COMMAND;
					return;
				default:
					break;
				}
			}
			if (line == "") {
				advance();
			}
		}
		else {
			hasMoreCommands = false;
		}
		//debug line
		cout << "valid command: " << line << endl;
		return;
	}

	string getSymbol() {
		if (commandType == command::C_COMMAND) return "";

		if (commandType == command::A_COMMAND) {
			return line.substr(line.find('@') + 1);
		}
		else if (commandType == command::L_COMMAND) {
			return line.substr(line.find('(') + 1, line.find(')') - 1);
		}
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
};

class codeModule {
private:
	ofstream outputFile;
public:

	codeModule(string outputFileName) {
		outputFile = ofstream(outputFileName, ios::trunc);
		if (!outputFile.is_open()) {
			cerr << outputFileName << " could not be created/opened!";
			exit(1);
		}
	}

	string assembleDest(string mnemonic) {

	}

	string assembleComp(string mnemonic) {

	}

	string assembleJump(string mnemonic) {

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
	symbolTable symbols = symbolTable();
	
	//------------------------
	while(parser.getHasMoreCommands()) {
		parser.advance();
		cout << parser.getSymbol() << endl;
	}

	return 0;
}