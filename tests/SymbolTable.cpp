#include "stdafx.h"
#include "CppUnitTest.h"
#include "SymbolTable\SymbolTable.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(SymbolTableTest)
	{
	public:

		TEST_METHOD(SymbolTable__InsertNew)
		{
			SymbolTable table;
			const int index0 = table.insert("First string");
			const int index1 = table.insert("Second string");
			const int index2 = table.insert("Third string");

			Assert::AreEqual(0, index0);
			Assert::AreEqual(1, index1);
			Assert::AreEqual(2, index2);
		}

		TEST_METHOD(SymbolTable__InsertExisting)
		{
			SymbolTable table;
			std::string str = "repeating string";
			table.insert("First string");
			const int index = table.insert(str);
			table.insert("Third string");

			const int rep_index = table.insert(str);

			Assert::AreEqual(index, rep_index);
		}

		TEST_METHOD(SymbolTable__OperatorAt)
		{
			SymbolTable table;
			std::string str = "test string";
			table.insert("a");
			const int index = table.insert(str);
			table.insert("b");

			Assert::AreEqual(str.c_str(), table[index].name.c_str());
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
	};
}