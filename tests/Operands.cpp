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

		TEST_METHOD(NumberOperand__load) {
			std::ostringstream stream;
			NumberOperand numOp(14);
			numOp.load(stream);

			Assert::AreEqual("MVI A, 14\n", stream.str().c_str());
		}

		TEST_METHOD(MemoryOperand__loadLocal) {
			SymbolTable symbolTable;
			symbolTable.insertFunc("main", SymbolTable::TableRecord::RecordType::integer, 0);
			symbolTable.insertVar("a", 1, SymbolTable::TableRecord::RecordType::integer);

			MemoryOperand memOp(1, &symbolTable);
			std::ostringstream stream;
			memOp.load(stream);

			Assert::AreEqual("LXI H, 0\nDAD SP\nMOV A, M\n", stream.str().c_str());
		}

		TEST_METHOD(MemoryOperand__loadGlobal) {
			SymbolTable symbolTable;
			symbolTable.insertVar("a", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);

			MemoryOperand memOp(0, &symbolTable);
			std::ostringstream stream;
			memOp.load(stream);

			Assert::AreEqual("LDA VAR0\n", stream.str().c_str());
		}

		TEST_METHOD(MemoryOperand__saveLocal) {
			SymbolTable symbolTable;
			symbolTable.insertFunc("main", SymbolTable::TableRecord::RecordType::integer, 0);
			symbolTable.insertVar("a", 1, SymbolTable::TableRecord::RecordType::integer);

			MemoryOperand memOp(1, &symbolTable);
			std::ostringstream stream;
			memOp.save(stream);

			Assert::AreEqual("LXI H, 0\nDAD SP\nMOV M, A\n", stream.str().c_str());
		}

		TEST_METHOD(MemoryOperand__saveGlobal) {
			SymbolTable symbolTable;
			symbolTable.insertVar("a", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);

			MemoryOperand memOp(0, &symbolTable);
			std::ostringstream stream;
			memOp.save(stream);

			Assert::AreEqual("STA VAR0\n", stream.str().c_str());
		}

		TEST_METHOD(ArrayElementOperand__loadLocal) {
			SymbolTable symbolTable;
			symbolTable.insertFunc("f", SymbolTable::TableRecord::RecordType::integer, 0);
			symbolTable.insertArray("a", 0, SymbolTable::TableRecord::RecordType::integer, 10);
			symbolTable.calculateOffset();

			std::shared_ptr<MemoryOperand> arrayOp = std::make_shared<ArrayElementOperand>(1, std::make_shared<NumberOperand>(3), &symbolTable);
			std::ostringstream stream;
			arrayOp->load(stream);

			Assert::AreEqual("MVI A, 3\nLXI H, 0\nLXI D, 0\nMOV E, A\nADD E\nMOV E, A\nDAD SP\nDAD D\nMOV A, M\n", stream.str().c_str());
		}

		TEST_METHOD(ArrayElementOperand__loadGlobal) {
			SymbolTable symbolTable;
			symbolTable.insertArray("a", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 10);

			std::shared_ptr<MemoryOperand> arrayOp = std::make_shared<ArrayElementOperand>(0, std::make_shared<NumberOperand>(3), &symbolTable);
			std::ostringstream stream;
			arrayOp->load(stream);

			Assert::AreEqual("MVI A, 3\nLXI H, ARR0\nLXI D, 0\nMOV E, A\nADD E\nMOV E, A\nDAD D\nMOV A, M\n", stream.str().c_str());
		}

		TEST_METHOD(ArrayElementOperand__saveLocal) {
			SymbolTable symbolTable;
			symbolTable.insertFunc("f", SymbolTable::TableRecord::RecordType::integer, 0);
			symbolTable.insertArray("a", 0, SymbolTable::TableRecord::RecordType::integer, 10);
			symbolTable.calculateOffset();

			std::shared_ptr<MemoryOperand> arrayOp = std::make_shared<ArrayElementOperand>(1, std::make_shared<NumberOperand>(3), &symbolTable);
			std::ostringstream stream;
			arrayOp->save(stream);

			Assert::AreEqual("MOV C, A\nMVI A, 3\nLXI H, 0\nLXI D, 0\nMOV E, A\nADD E\nMOV E, A\nDAD SP\nDAD D\nMOV A, C\nMOV M, A\n", stream.str().c_str());
		}

		TEST_METHOD(ArrayElementOperand__saveGlobal) {
			SymbolTable symbolTable;
			symbolTable.insertArray("a", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 10);

		 	std::shared_ptr<MemoryOperand> arrayOp = std::make_shared<ArrayElementOperand>(0, std::make_shared<NumberOperand>(3), &symbolTable);
			std::ostringstream stream;
			arrayOp->save(stream);

			Assert::AreEqual("MOV C, A\nMVI A, 3\nLXI H, ARR0\nLXI D, 0\nMOV E, A\nADD E\nMOV E, A\nDAD D\nMOV A, C\nMOV M, A\n", stream.str().c_str());
		}

		TEST_METHOD(ArrayElementOperand__Init)
		{
			SymbolTable symbolTable;
			symbolTable.insertArray("a", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 10);
			symbolTable.insertArray("b", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 15);
			auto key = symbolTable.insertVar("c", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);

			ArrayElementOperand arrOp(0, key, &symbolTable);
			Assert::AreEqual("[ArrayElOp, 0[2], a]", arrOp.toString(true).c_str());
		}
	};
}