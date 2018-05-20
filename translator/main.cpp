#include <iostream>
#include "LexicalAnalyzer\Scanner.h"
#include "Translator\Translator.h"

int main() {
	std::istringstream stream("str + str2 +?");
	Translator translator(stream);

	translator.translate();

	while (true) {};
	return 0;
}