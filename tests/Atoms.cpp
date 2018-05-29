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
			SimpleBinaryOpAtom atom("ADD", left, right, table.insertVar("test", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer));

			Assert::AreEqual("(ADD, '2', '4', 0)", atom.toString().c_str());
		}

		TEST_METHOD(UnaryOpAtom__Init)
		{
			SymbolTable table;
			std::shared_ptr<NumberOperand> op = std::make_shared<NumberOperand>(2);
			UnaryOpAtom atom("MOV", op, table.insertVar("test", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer));

			Assert::AreEqual("(MOV, '2', , 0)", atom.toString().c_str());
		}

		TEST_METHOD(ConditionalJumpAtom__Init)
		{
			SymbolTable table;
			auto op1 = table.insertVar("a", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			auto op2 = table.insertVar("b", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer);
			SimpleConditionalJumpAtom atom("EQ", op1, op2, std::make_shared<LabelOperand>(3));

			Assert::AreEqual("(EQ, 0, 1, lbl`3`)", atom.toString().c_str());
		}

		TEST_METHOD(OutAtom__Init)
		{
			SymbolTable table;
			OutAtom atom(table.insertVar("test", SymbolTable::GLOBAL_SCOPE, SymbolTable::TableRecord::RecordType::integer));

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
			std::deque<std::shared_ptr<RValue>> list;
			CallAtom atom(std::make_shared<MemoryOperand>(1, &table), std::make_shared<MemoryOperand>(2, &table), table, list);

			Assert::AreEqual("(CALL, 1, , 2)", atom.toString().c_str());
		}

		TEST_METHOD(ParamAtom__Init)
		{
			std::deque<std::shared_ptr<RValue>> list;
			ParamAtom atom(std::make_shared<NumberOperand>(1), list);

			Assert::AreEqual("(PARAM, , , '1')", atom.toString().c_str());
		}

		TEST_METHOD(RetAtom__Init)
		{
			SymbolTable table;
			RetAtom atom(std::make_shared<NumberOperand>(1), -1, table);

			Assert::AreEqual("(RET, , , '1')", atom.toString().c_str());
		}
	};
}