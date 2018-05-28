#include "stdafx.h"
#include "CppUnitTest.h"
#include "StringTable\StringTable.h"
#include "Operand\Operand.h"
#include <string>
#include <sstream>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{		
	TEST_CLASS(StringTableTest)
	{
	public:
		
		TEST_METHOD(StringTable__InsertNew)
		{
			StringTable table;
			std::shared_ptr<StringOperand> op0 = table.insert("First string");
			std::shared_ptr<StringOperand> op1 = table.insert("Second string");
			std::shared_ptr<StringOperand> op2 = table.insert("Third string");

			Assert::IsTrue(StringOperand(0, &table) == *op0);
			Assert::IsTrue(StringOperand(1, &table) == *op1);
			Assert::IsTrue(StringOperand(2, &table) == *op2);
		}

		TEST_METHOD(StringTable__InsertExisting)
		{
			StringTable table;
			std::string str = "repeating string";
			table.insert("First string");
			std::shared_ptr<StringOperand> op = table.insert(str);
			table.insert("Third string");

			std::shared_ptr<StringOperand> rep_op = table.insert(str);

			Assert::IsTrue(*op == *rep_op);
		}

		TEST_METHOD(StringTable__OperatorAt)
		{
			StringTable table;
			std::string str = "test string";
			table.insert("a");
			table.insert(str);
			table.insert("b");

			Assert::AreEqual(str.c_str(), table[1].c_str());
		}

		TEST_METHOD(StringTable__OperatorOut)
		{
			std::ostringstream stream;
			StringTable table;

			table.insert("First");
			table.insert("Second");
			table.insert("Third");

			stream << table;

			std::string excepted = "STRING TABLE:\n0 First\n1 Second\n2 Third";

			Assert::AreEqual(excepted.c_str(), stream.str().c_str());
		}

		TEST_METHOD(StringTable__generateGlobals) {
			std::ostringstream stream;
			StringTable table;

			table.insert("First");
			table.insert("Second");
			table.insert("Third");
			table.generateGlobalsSection(stream);

			Assert::AreEqual("str0: DB 'First', 0\nstr1: DB 'Second', 0\nstr2: DB 'Third', 0\n", stream.str().c_str());
		}
	};
}