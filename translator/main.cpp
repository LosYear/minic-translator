#include <iostream>
#include "LexicalAnalyzer\Scanner.h"
#include "Translator\Translator.h"

int main() {
	std::istringstream stream("int even(){} int main(){even(5);}");
	Translator translator(stream);

	bool translated = translator.translate();

	std::ostringstream result;
	translator.printAtoms(result, 2);

	std::string excepted = "2 (CALL, 0, , 2)";

	while (true) {};
	return 0;
}