#include <iostream>
#include "LexicalAnalyzer\Scanner.h"
#include "Translator\Translator.h"

int main() {
	std::istringstream stream("var");
	Translator translator(stream);
	return 0;
}