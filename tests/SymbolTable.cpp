#include "stdafx.h"
#include "CppUnitTest.h"
#include "SymbolTable\SymbolTable.h"
#include "Operand\Operand.h"
#include <string>
#include <sstream>
#include <memory>
#include "Translator\Translator.h"

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

		TEST_METHOD(SymbolTable__InsertArray)
		{
			SymbolTable table;
			std::shared_ptr<MemoryOperand> op0 = table.insertArray("First string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 2);
			std::shared_ptr<MemoryOperand> op1 = table.insertArray("Second string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 15);
			std::shared_ptr<MemoryOperand> op2 = table.insertArray("Third string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 3);

			Assert::IsTrue(MemoryOperand(0, &table) == *op0);
			Assert::IsTrue(MemoryOperand(1, &table) == *op1);
			Assert::IsTrue(MemoryOperand(2, &table) == *op2);
		}

		TEST_METHOD(SymbolTable__InserArray_Existing)
		{
			SymbolTable table;
			std::string str = "repeating string";
			table.insertArray("First string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 10);
			std::shared_ptr<MemoryOperand> op = table.insertArray(str, 5, SymbolTable::TableRecord::RecordType::integer, 10);
			table.insertArray("Third string", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 10);

			std::shared_ptr<MemoryOperand> rep_op = table.insertVar(str, 5, SymbolTable::TableRecord::RecordType::integer, 10);
			std::shared_ptr<MemoryOperand> rep_op_otherScope = table.insertVar(str, 105, SymbolTable::TableRecord::RecordType::integer, 10);

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
			std::shared_ptr<MemoryOperand> global = table.insertVar("var1", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
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
			auto record3 = symbolTable.insertArray("array", 1, SymbolTable::TableRecord::RecordType::integer, 10);

			std::ostringstream stream;
			stream << symbolTable;

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          var        var        integer    -1         10         1          0          \n" +
				"1          func       func       integer    0          0          -1         0          \n" +
				"2          array      array      integer    10         0          1          0          \n";

			Assert::AreEqual(excepted.c_str(), stream.str().c_str());
		}

		TEST_METHOD(SymbolTable__generateGlobals) {
			std::ostringstream stream;
			SymbolTable table;

			table.insertVar("a", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer, 10);
			table.insertVar("c", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			table.insertVar("b", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			table.insertVar("b", 4, SymbolTable::TableRecord::RecordType::integer);
			table.generateGlobalsSection(stream);

			Assert::AreEqual("var0: DB 10\nvar1: DB 0\nvar2: DB 0\n", stream.str().c_str());
		}

		TEST_METHOD(SymbolTable__offsetWithArrays)
		{
			std::istringstream stream("int main(){int a, b[10], c = 3; int k[15];} int f1(int p1, int p2){int a, b[10], c; int k[13], d, e;}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printSymbolTable(result);

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          main       func       integer    0          0          -1         56         \n" +
				"1          a          var        integer    -1         0          0          2          \n" +
				"2          b          array      integer    10         0          0          4          \n" +
				"3          c          var        integer    -1         3          0          0          \n" +
				"4          k          array      integer    15         0          0          24         \n" +
				"5          f1         func       integer    2          0          -1         60         \n" +
				"6          p1         var        integer    -1         0          5          58         \n" +
				"7          p2         var        integer    -1         0          5          56         \n" +
				"8          a          var        integer    -1         0          5          6          \n" +
				"9          b          array      integer    10         0          5          8          \n" +
				"10         c          var        integer    -1         0          5          4          \n" +
				"11         k          array      integer    13         0          5          28         \n" +
				"12         d          var        integer    -1         0          5          2          \n" +
				"13         e          var        integer    -1         0          5          0          \n";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}
	};
}