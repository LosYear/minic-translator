#include "stdafx.h"
#include "CppUnitTest.h"
#include "Operand\Operand.h"
#include "SymbolTable\SymbolTable.h"
#include "StringTable\StringTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(OperandsTest)
	{
	public:

		TEST_METHOD(MemoryOperand__Init)
		{
			SymbolTable symbolTable;
			symbolTable.insertVar("a", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			symbolTable.insertVar("b", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			symbolTable.insertVar("c", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);

			MemoryOperand memOp(1, &symbolTable);
			Assert::AreEqual("[MemOp, 1, b]", memOp.toString(true).c_str());
		}

		TEST_METHOD(NumberOperand__Init)
		{
			NumberOperand numOp(-14);
			Assert::AreEqual("[NumOp, -14]", numOp.toString(true).c_str());
		}

		TEST_METHOD(StringOperand__Init)
		{
			StringTable stringTable;
			stringTable.insert("str1");
			stringTable.insert("str2");

			StringOperand strOp(1, &stringTable);
			Assert::AreEqual("[StrOp, 1, 'str2']", strOp.toString(true).c_str());
		}

		TEST_METHOD(LabelOperand__Init)
		{
			LabelOperand lblOp(14);
			Assert::AreEqual("[LblOp, 14]", lblOp.toString(true).c_str());
		}
	};
}