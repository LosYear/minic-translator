#include "stdafx.h"
#include "CppUnitTest.h"
#include "Translator\Translator.h"
#include "Translator\Exception.h"
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(TranslatorTest)
	{
	public:

		TEST_METHOD(Translator__Print)
		{
			std::istringstream stream;
			Translator translator(stream);

			translator.generateAtom(std::make_unique<JumpAtom>(std::make_shared<LabelOperand>(10)));
			translator.generateAtom(std::make_unique<LabelAtom>(std::make_shared<LabelOperand>(10)));

			std::ostringstream out;
			translator.printAtoms(out);

			Assert::AreEqual("(JMP, , , lbl`10`)\n(LBL, , , lbl`10`)", out.str().c_str());
		}

		TEST_METHOD(Translator__NewLabel)
		{
			std::istringstream stream;
			Translator translator(stream);

			auto lbl0 = translator.newLabel();
			auto lbl1 = translator.newLabel();

			Assert::AreEqual("lbl`0`", lbl0->toString().c_str());
			Assert::AreEqual("lbl`1`", lbl1->toString().c_str());
		}

		TEST_METHOD(Translator__SyntaxError)
		{
			std::istringstream stream;
			Translator translator(stream);

			auto createException = [&translator] {translator.throwSyntaxError("Invalid syntax"); };
			Assert::ExpectException<SyntaxError>(createException);
		}

		TEST_METHOD(Translator__LexicalError)
		{
			std::istringstream stream;
			Translator translator(stream);

			auto createException = [&translator] {translator.throwLexicalError("Invalid lexem"); };
			Assert::ExpectException<LexicalError>(createException);
		}
	};
}