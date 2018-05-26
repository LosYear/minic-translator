#include <iostream>
#include "LexicalAnalyzer\Scanner.h"
#include "Translator\Translator.h"

int main() {
	std::istringstream stream("int main(){int i; for(i = 0; i < 10; ++i){i = i + 0;}}");
	Translator translator(stream);

	bool translated = translator.translate();

	std::ostringstream result;
	translator.printAtoms(result, 0);

	while (true) {};
	return 0;
}