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
			translator.insertSymbolTableVar("str", -1, SymbolTable::TableRecord::RecordType::integer);
			translator.insertSymbolTableVar("str2", -1, SymbolTable::TableRecord::RecordType::integer);

			translator.translateExpression(0);

			Assert::AreEqual("Lexical error: Unknown symbol '?'\nAfter lexems: [opplus] [id, \"str2\"] [opplus]", errors.str().c_str());
		}

		TEST_METHOD(Translator__LexicalException_incompleteAnd)
		{

			std::istringstream stream("str & str2");
			std::ostringstream errors("");
			Translator translator(stream, errors);

			translator.translateExpression(0);

			Assert::AreEqual("Lexical error: Incomplete AND operator\nAfter lexems: [id, \"str\"]", errors.str().c_str());
		}

		TEST_METHOD(Translator__SyntaxException_excessLexems)
		{

			std::istringstream stream("str && str2 str");
			std::ostringstream errors("");
			Translator translator(stream, errors);
			translator.insertSymbolTableVar("str", -1, SymbolTable::TableRecord::RecordType::integer);
			translator.insertSymbolTableVar("str2", -1, SymbolTable::TableRecord::RecordType::integer);

			translator.translateExpression(0);

			Assert::AreEqual("Syntax error: Excess lexems are left after translation\nAfter lexems: [id, \"str\"] [opand] [id, \"str2\"]", errors.str().c_str());
		}

		TEST_METHOD(Translator__SyntaxException_unexceptedEOF)
		{

			std::istringstream stream("str &&");
			std::ostringstream errors("");
			Translator translator(stream, errors);
			translator.insertSymbolTableVar("str", -1, SymbolTable::TableRecord::RecordType::integer);
			translator.insertSymbolTableVar("str2", -1, SymbolTable::TableRecord::RecordType::integer);

			translator.translateExpression(0);

			Assert::AreEqual("Syntax error: Rule #24-28. Unxepected lexem '[eof]' in expression, expected ++, (, num, id.\nAfter lexems: [id, \"str\"] [opand]", errors.str().c_str());
		}

		TEST_METHOD(Translator__SyntaxException_otherLexemExcepted)
		{

			std::istringstream stream("(str && str2");
			std::ostringstream errors("");
			Translator translator(stream, errors);
			translator.insertSymbolTableVar("str", -1, SymbolTable::TableRecord::RecordType::integer);
			translator.insertSymbolTableVar("str2", -1, SymbolTable::TableRecord::RecordType::integer);

			translator.translateExpression(0);

			Assert::AreEqual("Syntax error: Excepted rpar, got [eof]\nAfter lexems: [id, \"str\"] [opand] [id, \"str2\"]", errors.str().c_str());
		}

		TEST_METHOD(Translator__SyntaxException_noMain)
		{

			std::istringstream stream("int even(){}");
			std::ostringstream errors("");
			Translator translator(stream, errors);

			bool translated = translator.translate();
			Assert::IsFalse(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			Assert::AreEqual("Syntax error: No entry point for given program\nAfter lexems: [rpar] [lbrace] [rbrace]", errors.str().c_str());
		}

		TEST_METHOD(Translator__SyntaxError_doubleDefault) {
			std::istringstream stream("int main(){int i; switch(i){case 0: i = 1; case 1: i = 2; default: return 1; case 2: i = 3; default: return 0;}}");
			std::ostringstream errors("");
			Translator translator(stream, errors);

			bool translated = translator.translate();
			Assert::IsFalse(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			Assert::AreEqual("Syntax error: There can't be more than ONE default section in case.\nAfter lexems: [kwreturn] [num, 0] [semicolon]", errors.str().c_str());
		}
	};
}