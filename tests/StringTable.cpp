#include "stdafx.h"
#include "CppUnitTest.h"
#include "StringTable\StringTable.h"
#include <string>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{		
	TEST_CLASS(StringTableTest)
	{
	public:
		
		TEST_METHOD(StringTable__InsertNew)
		{
			StringTable table;
			const int index0 = table.insert("First string");
			const int index1 = table.insert("Second string");
			const int index2 = table.insert("Third string");

			Assert::AreEqual(0, index0);
			Assert::AreEqual(1, index1);
			Assert::AreEqual(2, index2);
		}

		TEST_METHOD(StringTable__InsertExisting)
		{
			StringTable table;
			std::string str = "repeating string";
			table.insert("First string");
			const int index = table.insert(str);
			table.insert("Third string");

			const int rep_index = table.insert(str);

			Assert::AreEqual(index, rep_index);
		}

		TEST_METHOD(StringTable__OperatorAt)
		{
			StringTable table;
			std::string str = "test string";
			table.insert("a");
			const int index = table.insert(str);
			table.insert("b");

			Assert::AreEqual(str.c_str(), table[index].c_str());
		}

		TEST_METHOD(StringTable__OperatorOut)
		{
			std::ostringstream stream;
			StringTable table;

			table.insert("First");
			table.insert("Second");
			table.insert("Third");

			stream << table;

			std::string excepted = "0 First\n1 Second\n2 Third";

			Assert::AreEqual(excepted.c_str(), stream.str().c_str());
		}
	};
}