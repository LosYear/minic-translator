#include "stdafx.h"
#include "CppUnitTest.h"
#include "Atom\Atom.h"
#include "SymbolTable\SymbolTable.h"
#include <string>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(AtomsTest)
	{
	public:

		TEST_METHOD(BinaryOpAtom__Init)
		{
			SymbolTable table;
			std::shared_ptr<NumberOperand> left = std::make_shared<NumberOperand>(2);
			std::shared_ptr<NumberOperand> right = std::make_shared<NumberOperand>(4);
			BinaryOpAtom atom("ADD", left, right, table.insert("test"));

			Assert::AreEqual("(ADD, '2', '4', 0)", atom.toString().c_str());
		}

		TEST_METHOD(UnaryOpAtom__Init)
		{
			SymbolTable table;
			std::shared_ptr<NumberOperand> op = std::make_shared<NumberOperand>(2);
			UnaryOpAtom atom("MOV", op, table.insert("test"));

			Assert::AreEqual("(MOV, '2', , 0)", atom.toString().c_str());
		}

		TEST_METHOD(ConditionalJumpAtom__Init)
		{
			SymbolTable table;
			auto op1 = table.insert("a");
			auto op2 = table.insert("b");
			ConditionalJumpAtom atom("EQ", op1, op2, std::make_shared<LabelOperand>(3));

			Assert::AreEqual("(EQ, 0, 1, lbl`3`)", atom.toString().c_str());
		}

		TEST_METHOD(OutAtom__Init)
		{
			SymbolTable table;
			OutAtom atom(table.insert("test"));

			Assert::AreEqual("(OUT, , , 0)", atom.toString().c_str());
		}

		TEST_METHOD(JumpAtom__Init)
		{
			SymbolTable table;
			JumpAtom atom(std::make_shared<LabelOperand>(1));

			Assert::AreEqual("(JMP, , , lbl`1`)", atom.toString().c_str());
		}

		TEST_METHOD(LabelAtom__Init)
		{
			LabelAtom atom(std::make_shared<LabelOperand>(1));

			Assert::AreEqual("(LBL, , , lbl`1`)", atom.toString().c_str());
		}

		TEST_METHOD(CallAtom__Init)
		{
			SymbolTable table;
			CallAtom atom(std::make_shared<MemoryOperand>(1, &table), std::make_shared<MemoryOperand>(2, &table));

			Assert::AreEqual("(CALL, 1, , 2)", atom.toString().c_str());
		}

		TEST_METHOD(ParamAtom__Init)
		{
			ParamAtom atom(std::make_shared<NumberOperand>(1));

			Assert::AreEqual("(PARAM, , , '1')", atom.toString().c_str());
		}

		TEST_METHOD(RetAtom__Init)
		{
			RetAtom atom(std::make_shared<NumberOperand>(1));

			Assert::AreEqual("(RET, , , '1')", atom.toString().c_str());
		}
	};
}