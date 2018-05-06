#include "stdafx.h"
#include "CppUnitTest.h"
#include "Translator\Translator.h"
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
	};
}