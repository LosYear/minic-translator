#include "stdafx.h"
#include "CppUnitTest.h"
#include "SymbolTable\SymbolTable.h"
#include "Operand\Operand.h"
#include <string>
#include <sstream>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(SymbolTableTest)
	{
	public:

		TEST_METHOD(SymbolTable__InsertVar)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op0 = table.insertVar("First string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> op1 = table.insertVar("Second string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> op2 = table.insertVar("Third string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);

			Assert::IsTrue(MemoryOperand(0, &table) == *op0);
			Assert::IsTrue(MemoryOperand(1, &table) == *op1);
			Assert::IsTrue(MemoryOperand(2, &table) == *op2);
		}

		TEST_METHOD(SymbolTable__InsertVar_Existing)
		{
			SymbolTable table;
			std::string str = "repeating string";
			table.insertVar("First string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> op = table.insertVar(str, 5, SymbolTable::TableRecord::RecordType::integer);
			table.insertVar("Third string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);

			std::shared_ptr<MemoryOperand> rep_op = table.insertVar(str, 5, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> rep_op_otherScope = table.insertVar(str, 105, SymbolTable::TableRecord::RecordType::integer);

			Assert::IsTrue(nullptr == rep_op);
			Assert::IsTrue(nullptr != rep_op_otherScope);
		}

		TEST_METHOD(SymbolTable__InsertFunc)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op0 = table.insertFunc("func", SymbolTable::TableRecord::RecordType::integer, 1);
			Assert::IsTrue(MemoryOperand(0, &table) == *op0);
		}

		TEST_METHOD(SymbolTable__InsertFunc_Existing)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op0 = table.insertFunc("func", SymbolTable::TableRecord::RecordType::integer, 1);
			std::shared_ptr<MemoryOperand> op1 = table.insertFunc("func", SymbolTable::TableRecord::RecordType::integer, 1);
			Assert::IsTrue(MemoryOperand(0, &table) == *op0);
			Assert::IsTrue(op1 == nullptr);
		}

		TEST_METHOD(SymbolTable__CheckVar_localScope)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> global = table.insertVar("var1", SymbolTable::GLOBAL_SCOPE , SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> op0 = table.insertVar("var1", 10, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> op1 = table.checkVar(10, "var1");

			Assert::IsTrue(*op1 == *op0);
		}

		TEST_METHOD(SymbolTable__CheckVar_globalScope)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op0 = table.insertVar("var1", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> op1 = table.checkVar(10, "var1");

			Assert::IsTrue(*op1 == *op0);
		}

		TEST_METHOD(SymbolTable__CheckVar_noVar)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op0 = table.insertFunc("f1", SymbolTable::TableRecord::RecordType::chr, 10);
			std::shared_ptr<MemoryOperand> op1 = table.checkVar(10, "f1");

			Assert::IsTrue(op1 == nullptr);
		}

		TEST_METHOD(SymbolTable__CheckVar_notExists)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op1 = table.checkVar(10, "f1");

			Assert::IsTrue(op1 == nullptr);
		}

		TEST_METHOD(SymbolTable__CheckFunc_notExists)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op1 = table.checkFunc("f1", 10);
			Assert::IsTrue(op1 == nullptr);
		}

		TEST_METHOD(SymbolTable__CheckVar_notFunc)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op0 = table.insertVar("f1", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> op1 = table.checkFunc("f1", 10);

			Assert::IsTrue(op1 == nullptr);
		}

		TEST_METHOD(SymbolTable__CheckVar_diffLen)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op0 = table.insertFunc("f1", SymbolTable::TableRecord::RecordType::chr, 10);
			std::shared_ptr<MemoryOperand> op1 = table.checkFunc("f1", 11);

			Assert::IsTrue(op1 == nullptr);
		}

		TEST_METHOD(SymbolTable__OperatorAt)
		{
			SymbolTable table;
			std::string str = "test string";
			table.insertVar("a", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			table.insertVar(str, SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			table.insertVar("b", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);

			Assert::AreEqual(str.c_str(), table[1].name.c_str());
		}

		TEST_METHOD(SymbolTable__OperatorEqual)
		{
			SymbolTable::TableRecord rec1("test");
			SymbolTable::TableRecord rec2("test2");
			SymbolTable::TableRecord rec3("test");

			Assert::IsTrue(rec1 == rec1);
			Assert::IsTrue(rec1 == rec3);

			Assert::IsFalse(rec2 == rec3);
		}

		TEST_METHOD(SymbolTable__Alloc)
		{
			SymbolTable symbolTable;
			auto record = symbolTable.alloc(SymbolTable::GLOBAL_SCOPE);
			auto record2 = symbolTable.alloc(SymbolTable::GLOBAL_SCOPE);

			Assert::IsTrue(MemoryOperand(0, &symbolTable) == *record);
			Assert::IsTrue(MemoryOperand(1, &symbolTable) == *record2);
			Assert::AreEqual(SymbolTable::GLOBAL_SCOPE, symbolTable[0].scope);
			Assert::AreEqual(SymbolTable::GLOBAL_SCOPE, symbolTable[1].scope);
		}

		TEST_METHOD(SymbolTable__Print)
		{
			SymbolTable symbolTable;
			auto record = symbolTable.insertVar("var", 1, SymbolTable::TableRecord::RecordType::integer, 10);
			auto record2 = symbolTable.insertFunc("func", SymbolTable::TableRecord::RecordType::integer, 0);

			std::ostringstream stream;
			stream << symbolTable;

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          var        var        integer    -1         10         1          0          \n" +
				"1          func       func       integer    0          0          -1         0          \n";

			Assert::AreEqual(excepted.c_str(), stream.str().c_str());
		}
	};
}