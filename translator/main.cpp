#include <iostream>
#include "Translator\Translator.h"
#include <string>
#include <fstream>

int main() {
	std::cout << "Enter input filename (in input folder): ";
	std::string filename;
	std::cin >> filename;

	std::cout << std::endl << "Results of translation will appear in output/" << filename << " folder" << std::endl;

	// Open file
	std::ifstream input(filename + ".minic");

	if (!input) {
		std::cout << "ERROR: can't read file" << std::endl;
		while (true) {}
	}

	// Translation
	std::ofstream status(filename + ".status.log");
	Translator translator(input, status);

	if (translator.translate()) {
		status << "Translated OK" << std::endl;

		// Print info
		translator.printSymbolTable(status);
		translator.printStringTable(status);

		// Print atoms
		std::ofstream atoms(filename + ".atoms.txt");
		translator.printAtoms(atoms);

		// Print code
		std::ofstream asmCode(filename + ".asm.txt");
		translator.generateCode(asmCode);

		status.close();
		input.close();
		atoms.close();

	}
	else {
		status << std::endl << "Error occured during translation";
		status.close();
		input.close();
	}

	while (true) {};
	return 0;
}