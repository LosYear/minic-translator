#include "stdafx.h"
#include "CppUnitTest.h"
#include "Translator\Translator.h"
#include <string>
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(TranslatorErrorsTest)
	{
	public:

		TEST_METHOD(Translator__LexicalException_unknownSymbol)
		{

			std::istringstream stream("str + str2 +?");
			std::ostringstream errors("");
			Translator translator(stream, errors);

			translator.translate();

			Assert::AreEqual("Lexical error: Unknown symbol '?'\n After lexems: [opplus] [id, \"str2\"] [opplus]", errors.str().c_str());
		}

		TEST_METHOD(Translator__LexicalException_incompleteAnd)
		{

			std::istringstream stream("str & str2");
			std::ostringstream errors("");
			Translator translator(stream, errors);

			translator.translate();

			Assert::AreEqual("Lexical error: Incomplete AND operator\n After lexems: [id, \"str\"]", errors.str().c_str());
		}
	};
}