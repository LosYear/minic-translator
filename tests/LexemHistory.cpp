#include "stdafx.h"
#include "CppUnitTest.h"
#include "Translator\LexemHistory.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(LexemHistoryTest)
	{
	public:

		TEST_METHOD(LexemHistory__push)
		{
			LexemHistory history;

			auto state1 = history.getAll();
			Assert::AreEqual(0, static_cast<int>(state1.size()));

			LexicalToken token1(1);
			history.push(token1);
			auto state2 = history.getAll();
			Assert::AreEqual(1, static_cast<int>(state2.size()));

			LexicalToken token2('c');
			history.push(token2);
			auto state3 = history.getAll();
			Assert::AreEqual(2, static_cast<int>(state3.size()));

			LexicalToken token3(3);
			history.push(token3);
			auto state4 = history.getAll();
			Assert::AreEqual(3, static_cast<int>(state4.size()));


			LexicalToken token4('a');
			history.push(token4);
			auto state5 = history.getAll();
			Assert::AreEqual(3, static_cast<int>(state5.size()));

			// Test stored tokens
			Assert::AreEqual("[chr, 'c']", state5[0].toString().c_str());
			Assert::AreEqual("[num, 3]", state5[1].toString().c_str());
			Assert::AreEqual("[chr, 'a']", state5[2].toString().c_str());
		}
	};
}