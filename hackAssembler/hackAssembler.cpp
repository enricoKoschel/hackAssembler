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

string compBin[18] = {"101010", "111111", "111010", "001100", "110000", "001101", "110001", "001111", 
"110011", "011111", "110111", "001110", "110010", "000010", "010011", "000111", "000000", "010101"};

enum class command {
	A_COMMAND,
	C_COMMAND,
	L_COMMAND,
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
	int currentInstruction = -1;
	int currentVariable = 16;
public:	
	parserModule(string inputFileName) {
		inputFile = ifstream(inputFileName);
		if (!inputFile.is_open()) {
			cerr << inputFileName << " could not be opened!";
			exit(1);
		}
	}

	string getLine() {
		return line;
	}

	int getCurrentInstruction() {
		return currentInstruction;
	}

	int getCurrentVariableAndInc() {
		return currentVariable++;
	}

	void resetFile() {
		inputFile.clear();
		inputFile.seekg(0, ios::beg);

		hasMoreCommands = true;
	}

	void advance() {
		if (!hasMoreCommands) return;
		if (getline(inputFile, line)) {
			currentLine++;
			line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());

			//End of line comment check
			line = line.substr(0, line.find("//"));

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
				currentInstruction++;
			}
			else if (line[0] == '(' && line[line.length() - 1] == ')') {
				//L_COMMAND found
				commandType = command::L_COMMAND;
			}
			else if (line[0] == 'M' || line[0] == 'D' || line[0] == 'A' || line[0] == '0') {
				//Potential C_COMMAND found
				commandType = command::C_COMMAND;
				currentInstruction++;

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

	void closeFile() {
		inputFile.close();
	}
};

class codeModule {
private:
	ofstream outputFile;

	string assembleDest(string dest) {
		if (dest == "null") {
			return "000";
		}
		else if (dest == "M") {
			return "001";
		}
		else if (dest == "D") {
			return "010";
		}
		else if (dest == "MD") {
			return "011";
		}
		else if (dest == "A") {
			return "100";
		}
		else if (dest == "AM") {
			return "101";
		}
		else if (dest == "AD") {
			return "110";
		}
		else if (dest == "AMD") {
			return "111";
		}
		else {
			return "-1";
		}
	}

	string assembleComp(string comp) {
		string temp;
		
		temp += (comp.find('M') != string::npos) ? "1" : "0"; //Sets 'a' bit

		//Replace 'M' with 'D' to allow easier access to array of binary values
		if (comp.find('M') != string::npos) {
			comp.replace(comp.find('M'), 1, "A");
		}

		for (int i = 0; i < 28; i++) {
			if (comp == validComps[i]) {
				return temp + compBin[i];
			}
		}
	}

	string assembleJump(string jump) {
		if (jump == "null") {
			return "000";
		}
		else if (jump == "JGT") {
			return "001";
		}
		else if (jump == "JEQ") {
			return "010";
		}
		else if (jump == "JGE") {
			return "011";
		}
		else if (jump == "JLT") {
			return "100";
		}
		else if (jump == "JNE") {
			return "101";
		}
		else if (jump == "JLE") {
			return "110";
		}
		else if (jump == "JMP") {
			return "111";
		}
		else {
			return "-1";
		}
	}

	string strToBin(string constant) {
		int number = stoi(constant);
		int binNumber[15] = {};
		int i = 0;
		string output;

		do {
			binNumber[i] = number % 2;
			number /= 2;
			i++;
		} while (number > 0);

		for (i = 14; i >= 0; i--) {
			output += to_string(binNumber[i]);
		}
		return output;
	}

	string intToBin(int constant) {
		int number = constant;
		int binNumber[15] = {};
		int i = 0;
		string output;

		do {
			binNumber[i] = number % 2;
			number /= 2;
			i++;
		} while (number > 0);

		for (i = 14; i >= 0; i--) {
			output += to_string(binNumber[i]);
		}
		return output;
	}
public:

	codeModule(string outputFileName) {
		outputFile = ofstream(outputFileName, ios::trunc);
		if (!outputFile.is_open()) {
			cerr << outputFileName << " could not be created/opened!";
			exit(1);
		}
	}

	void assemble(string dest, string comp, string jump) {
		string output = "111"; //3 constant bits for c instruction
		output += assembleComp(comp); //returns 7 bits
		output += assembleDest(dest); //returns 3 bits
		output += assembleJump(jump); //returns 3 bits
		outputFile << output << endl; //output now 16 bits
	}

	void writeConstant(string constant) {
		outputFile << "0" + strToBin(constant) << endl;
	}

	void writeConstant(int constant) {
		outputFile << "0" + intToBin(constant) << endl;
	}

	void closeFile() {
		outputFile.close();
	}
};

class symbolTable {
private:
	unordered_map<string, int> table;

	bool contains(string symbol) {
		return table.find(symbol) != table.end();
	}
public:
	symbolTable() {
		for (int i = 0; i <= 15; i++) {
			table.insert({ "R" + to_string(i), i });
		}
		
		table.insert({ "SP", 0 });
		table.insert({ "LCL", 1 });
		table.insert({ "ARG", 2 });
		table.insert({ "THIS", 3 });
		table.insert({ "THAT", 4 });
		table.insert({ "SCREEN", 16384 });
		table.insert({ "KBD", 24576 });
	}

	void addEntry(string symbol, int address) {
		table.insert({ symbol, address });
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
	//First pass
	while (parser.getHasMoreCommands()) {
		parser.advance();
		if (!parser.getHasMoreCommands()) break;

		if (parser.getCommandType() == command::L_COMMAND) {
			//add label to symbolTable
			string symbol = parser.getSymbol();

			while (true) {
				parser.advance();
				if (parser.getCommandType() == command::L_COMMAND) {
					symbol += "\n";
					symbol += parser.getSymbol();
				}
				else {
					break;
				}
			}

			while (symbol.find("\n") != string::npos) {
				symbols.addEntry(symbol.substr(0, symbol.find("\n")), parser.getCurrentInstruction());
				symbol = symbol.substr(symbol.find("\n") + 1);
			}
			symbols.addEntry(symbol, parser.getCurrentInstruction());
		}
	}

	parser.resetFile();

	//Second pass
	while(parser.getHasMoreCommands()) {
		//Parse and assemble input file
		parser.advance();
		if (!parser.getHasMoreCommands()) break;

		if (parser.getCommandType() == command::C_COMMAND) {
			//assembles parsed command into outputFIle
			code.assemble(parser.getDest(), parser.getComp(), parser.getJump());
		}
		else if (parser.getCommandType() == command::A_COMMAND) {
			//extract label from symbolTable
			//write constant/address to outputFile
			string symbol = parser.getSymbol();
			bool label = false;

			for (int i = 0; i < symbol.length(); i++) {
				if (!isdigit(symbol[i])) {
					if (symbols.getAddress(symbol) == -1) {
						//Variable
						symbols.addEntry(symbol, parser.getCurrentVariableAndInc());

					}
					code.writeConstant(symbols.getAddress(symbol));

					label = true;
					break;
				}
			}
			if (!label) code.writeConstant(symbol);
		}
		else if (parser.getCommandType() == command::L_COMMAND) {
			//ignore on second pass
		}
	}

	parser.closeFile();
	code.closeFile();

	return 0;
}