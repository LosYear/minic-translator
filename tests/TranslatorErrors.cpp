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
	};
}