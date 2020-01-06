#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

string inputFileName;
string outputFileName;
string validComps[28] = {"0", "1", "-1", "D", "A", "!D", "!A", "-D", "-A", "D+1", "A+1", "D-1", "A-1", 
"D+A", "D-A", "A-D", "D&A", "D|A", "M", "!M", "-M", "M+1", "M-1", "D+M", "D-M", "M-D", "D&M", "D|M", };

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
	command commandType = command::NO_COMMAND;
	string dest;
	string comp;
	string jump;
	string line;
	int currentLine;
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
			line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());

			//End of line comment check here

			if (line[0] == '/' && line[1] == '/') {
				//Comment found
				advance();
			}
			else if (line == "") {
				//Empty line found
				advance();
			}
			else if (line[0] == '@') {
				//A_COMMAND found
				commandType = command::A_COMMAND;
			}
			else if (line[0] == '(' && line[line.length() - 1] == ')') {
				//L_COMMAND found
				commandType = command::L_COMMAND;
			}
			else if (line[0] == 'M' || line[0] == 'D' || line[0] == 'A' || line[0] == '0') {
				//Potential C_COMMAND found
				commandType = command::C_COMMAND;
				if (line.find_first_of('=') != string::npos) {
					string tempDest = line.substr(0, line.find_first_of('='));
					if (tempDest == "M" || tempDest == "D" || tempDest == "MD" || tempDest == "A" 
						|| tempDest == "AM" || tempDest == "AD" || tempDest == "AMD") {
						//valid dest
						dest = tempDest;
					}
					string tempComp = line.substr(line.find_first_of('=') + 1);
					//check comp
					for (int i = 0; i < 28; i++) {
						if (tempComp == validComps[i]) {
							comp = tempComp;
						}
					}

					if (comp != tempComp) {
						//invalid comp
						cerr << "Error on line " << currentLine << endl;
						exit(1);
					}

					//check potential jump
					if (line.find_first_of(';') != string::npos) {
						string tempJump = line.substr(line.find_first_of(';') + 1);
						if (tempJump == "JGT" || tempJump == "JEQ" || tempJump == "JGE" || tempJump == "JLT"
							|| tempJump == "JNE" || tempJump == "JLE" || tempJump == "JMP") {
							//valid jump
							jump = tempJump;
						}
						else {
							cerr << "Error on line " << currentLine << endl;
							exit(1);
						}
					}
					else {
						jump = "null";
					}
				}
				else {
					dest = "null";
					if (line.find_first_of(';') != string::npos) {
						string tempComp = line.substr(0, line.find_first_of(';'));
						//check comp
						for (int i = 0; i < 28; i++) {
							if (tempComp == validComps[i]) {
								comp = tempComp;
								break;
							}
						}

						if (comp != tempComp) {
							//invalid comp
							cerr << "Error on line " << currentLine << endl;
							exit(1);
						}

						//assign jump
						string tempJump = line.substr(line.find_first_of(';') + 1);
						if (tempJump == "JGT" || tempJump == "JEQ" || tempJump == "JGE" || tempJump == "JLT"
							|| tempJump == "JNE" || tempJump == "JLE" || tempJump == "JMP") {
							//valid jump
							jump = tempJump;
						}
						else {
							cerr << "Error on line " << currentLine << endl;
							exit(1);
						}
					}
					else {
						//error
						cerr << "Error on line " << currentLine << endl;
						exit(1);
					}
				}
			}
			else {
				commandType = command::NO_COMMAND;
				cerr << "Error on line " << currentLine << endl;
				exit(1);
			}
		}
		else {
			hasMoreCommands = false;
		}
		return;
	}

	string getSymbol() {
		if (commandType == command::C_COMMAND || commandType == command::NO_COMMAND) return "";

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
		return dest;
	}

	string getComp() {
		if (commandType != command::C_COMMAND) return "";
		return comp;
	}

	string getJump() {
		if (commandType != command::C_COMMAND) return "";
		return jump;
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
		cout << endl << "advance: ";
		parser.advance();
		
		if (parser.getCommandType() == command::C_COMMAND) {
			cout << "C_COMMAND" << endl;
		}
		else if (parser.getCommandType() == command::A_COMMAND) {
			cout << "A_COMMAND" << endl;
		}
		else if (parser.getCommandType() == command::L_COMMAND) {
			cout << "L_COMMAND" << endl;
		}
		else if (parser.getCommandType() == command::NO_COMMAND) {
			cout << "NO_COMMAND" << endl;
		}

		if (parser.getCommandType() == command::C_COMMAND) {
			cout << "comp: " << parser.getComp() << endl;
			cout << "dest: " << parser.getDest() << endl;
			cout << "jump: " << parser.getJump() << endl;
		}
		else if(parser.getCommandType() != command::NO_COMMAND) {
			cout << "symbol: ";
			cout << parser.getSymbol() << endl;
		}
	}

	return 0;
}