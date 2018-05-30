#include "stdafx.h"
#include "CppUnitTest.h"
#include "Translator\Translator.h"
#include "Translator\Exception.h"
#include <typeinfo>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(TranslatorArraysSupport)
	{
	public:

		TEST_METHOD(Array__DeclarationGlobal)
		{
			std::istringstream stream("int a, b[10], c = 3; int k[15]; int main(){}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printSymbolTable(result);

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          a          var        integer    -1         0          -1         0          \n" +
				"1          b          array      integer    10         0          -1         0          \n" +
				"2          c          var        integer    -1         3          -1         0          \n" +
				"3          k          array      integer    15         0          -1         0          \n" +
				"4          main       func       integer    0          0          -1         2          \n";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Array__DeclarationLocal)
		{
			std::istringstream stream("int main(){int a, b[10], c = 3; int k[15];}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printSymbolTable(result);

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          main       func       integer    0          0          -1         10         \n" +
				"1          a          var        integer    -1         0          0          6          \n" +
				"2          b          array      integer    10         0          0          0          \n" +
				"3          c          var        integer    -1         3          0          4          \n" +
				"4          k          array      integer    15         0          0          0          \n";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Array__DeclarationAsArgument) {
			std::istringstream stream("int main(int a, int b[10]){int a, b[10], c = 3; int k[15];}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsFalse(translated);
		}
	};
}