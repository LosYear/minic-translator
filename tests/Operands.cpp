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
			symbolTable.insert("a");
			const int index = symbolTable.insert("b");
			symbolTable.insert("c");

			MemoryOperand memOp(index, &symbolTable);
			Assert::AreEqual("[MemOp, 1, b]", memOp.toString().c_str());
		}

		TEST_METHOD(NumberOperand__Init)
		{
			NumberOperand numOp(-14);
			Assert::AreEqual("[NumOp, -14]", numOp.toString().c_str());
		}

		TEST_METHOD(StringOperand__Init)
		{
			StringTable stringTable;
			stringTable.insert("str1");
			const int index = stringTable.insert("str2");
			
			StringOperand strOp(index, &stringTable);
			Assert::AreEqual("[StrOp, 1, 'str2']", strOp.toString().c_str());
		}

		TEST_METHOD(LabelOperand__Init)
		{
			LabelOperand lblOp(14);
			Assert::AreEqual("[LblOp, 14]", lblOp.toString().c_str());
		}
	};
}