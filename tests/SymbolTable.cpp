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

		TEST_METHOD(SymbolTable__InsertNew)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op0 = table.insert("First string");
			std::shared_ptr<MemoryOperand> op1 = table.insert("Second string");
			std::shared_ptr<MemoryOperand> op2 = table.insert("Third string");

			Assert::IsTrue(MemoryOperand(0, &table) == *op0);
			Assert::IsTrue(MemoryOperand(1, &table) == *op1);
			Assert::IsTrue(MemoryOperand(2, &table) == *op2);
		}

		TEST_METHOD(SymbolTable__InsertExisting)
		{
			SymbolTable table;
			std::string str = "repeating string";
			table.insert("First string");
			std::shared_ptr<MemoryOperand> op = table.insert(str);
			table.insert("Third string");

			std::shared_ptr<MemoryOperand> rep_op = table.insert(str);

			Assert::IsTrue(*op == *rep_op);
		}

		TEST_METHOD(SymbolTable__OperatorAt)
		{
			SymbolTable table;
			std::string str = "test string";
			table.insert("a");
			table.insert(str);
			table.insert("b");

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
			auto record = symbolTable.alloc();
			auto record2 = symbolTable.alloc();

			Assert::IsTrue(MemoryOperand(0, &symbolTable) == *record);
			Assert::IsTrue(MemoryOperand(1, &symbolTable) == *record2);
		}

		TEST_METHOD(SymbolTable__Print)
		{
			SymbolTable symbolTable;
			auto record = symbolTable.insert("var", SymbolTable::TableRecord::RecordKind::var, SymbolTable::TableRecord::RecordType::integer, -1, 10, 1, 0);
			auto record2 = symbolTable.insert("func", SymbolTable::TableRecord::RecordKind::func, SymbolTable::TableRecord::RecordType::integer, 0, 0, 5, 0);

			std::ostringstream stream;
			stream << symbolTable;

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          var        var        integer    -1         10         1          0          \n" +
				"1          func       func       integer    0          0          5          0          \n";

			Assert::AreEqual(excepted.c_str(), stream.str().c_str());
		}
	};
}