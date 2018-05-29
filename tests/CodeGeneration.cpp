#include "stdafx.h"
#include "CppUnitTest.h"
#include "Translator\Translator.h"
#include <string>
#include <iostream>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(CodeGenerationTest)
	{
	public:

		TEST_METHOD(Code__OR)
		{

			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			auto res = table.insertVar("c", -1, SymbolTable::TableRecord::RecordType::integer);

			SimpleBinaryOpAtom atom("OR", left, right, res);

			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (OR, 0, 1, 2)\nLDA VAR1\nMOV B, A\nLDA VAR0\nORA B\nSTA VAR2\n", stream.str().c_str());
		}

		TEST_METHOD(Code__AND)
		{

			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			auto res = table.insertVar("c", -1, SymbolTable::TableRecord::RecordType::integer);

			SimpleBinaryOpAtom atom("AND", left, right, res);

			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (AND, 0, 1, 2)\nLDA VAR1\nMOV B, A\nLDA VAR0\nANA B\nSTA VAR2\n", stream.str().c_str());
		}

		TEST_METHOD(Code__MOV)
		{

			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			auto res = table.insertVar("c", -1, SymbolTable::TableRecord::RecordType::integer);

			UnaryOpAtom atom("MOV", left, res);

			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (MOV, 0, , 2)\nLDA VAR0\nSTA VAR2\n", stream.str().c_str());
		}

		TEST_METHOD(Code__NOT) {
			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			auto res = table.insertVar("c", -1, SymbolTable::TableRecord::RecordType::integer);

			UnaryOpAtom atom("NOT", left, res);

			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (NOT, 0, , 2)\nLDA VAR0\nCMA\nSTA VAR2\n", stream.str().c_str());
		}

		TEST_METHOD(Code__OUT_str) {
			StringTable table;
			std::shared_ptr<StringOperand> record = table.insert("TEST");

			OutAtom atom(record);

			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (OUT, , , str`0`)\nLXI A, str0\nCALL @PRINT\n", stream.str().c_str());
		}

		TEST_METHOD(Code__OUT_value) {
			StringTable table;
			std::shared_ptr<NumberOperand> a = std::make_shared<NumberOperand>(5);

			OutAtom atom(a);

			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (OUT, , , '5')\nMVI A, 5\nOUT 1\n", stream.str().c_str());
		}


		TEST_METHOD(Code__ADD)
		{
			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			auto res = table.insertVar("c", -1, SymbolTable::TableRecord::RecordType::integer);

			SimpleBinaryOpAtom atom("ADD", left, right, res);

			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (ADD, 0, 1, 2)\nLDA VAR1\nMOV B, A\nLDA VAR0\nADD B\nSTA VAR2\n", stream.str().c_str());
		}

		TEST_METHOD(Code__SUB)
		{

			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			auto res = table.insertVar("c", -1, SymbolTable::TableRecord::RecordType::integer);

			SimpleBinaryOpAtom atom("SUB", left, right, res);

			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (SUB, 0, 1, 2)\nLDA VAR1\nMOV B, A\nLDA VAR0\nSUB B\nSTA VAR2\n", stream.str().c_str());
		}

		TEST_METHOD(Code__MUL) {
			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			auto res = table.insertVar("c", -1, SymbolTable::TableRecord::RecordType::integer);

			FnBinaryOpAtom atom("MUL", left, right, res);

			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (MUL, 0, 1, 2)\nLDA VAR1\nMOV B, A\nLDA VAR0\nMOV C, A\nMOV D, B\nCALL @MUL\nMOV A, C\nSTA VAR2\n", stream.str().c_str());
		}

		TEST_METHOD(Code__EQ) {
			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			LabelOperand label(0);

			SimpleConditionalJumpAtom atom("EQ", left, right, std::make_shared<LabelOperand>(label));
			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (EQ, 0, 1, lbl`0`)\nLDA VAR1\nMOV B, A\nLDA VAR0\nCMP B\nJZ LBL0\n", stream.str().c_str());
		}

		TEST_METHOD(Code__NE) {
			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			LabelOperand label(0);

			SimpleConditionalJumpAtom atom("NE", left, right, std::make_shared<LabelOperand>(label));
			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (NE, 0, 1, lbl`0`)\nLDA VAR1\nMOV B, A\nLDA VAR0\nCMP B\nJNZ LBL0\n", stream.str().c_str());
		}

		TEST_METHOD(Code__GT) {
			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			LabelOperand label(0);

			SimpleConditionalJumpAtom atom("GT", left, right, std::make_shared<LabelOperand>(label));
			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (GT, 0, 1, lbl`0`)\nLDA VAR1\nMOV B, A\nLDA VAR0\nCMP B\nJP LBL0\n", stream.str().c_str());
		}

		TEST_METHOD(Code__LT) {
			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			LabelOperand label(0);

			SimpleConditionalJumpAtom atom("LT", left, right, std::make_shared<LabelOperand>(label));
			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (LT, 0, 1, lbl`0`)\nLDA VAR1\nMOV B, A\nLDA VAR0\nCMP B\nJM LBL0\n", stream.str().c_str());
		}

		TEST_METHOD(Code__LE) {
			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", -1, SymbolTable::TableRecord::RecordType::integer);
			LabelOperand label(0);

			ComplexConditinalJumpAtom atom("LE", left, right, std::make_shared<LabelOperand>(label));
			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (LE, 0, 1, lbl`0`)\nLDA VAR1\nMOV B, A\nLDA VAR0\nCMP B\nJZ LBL0\nJM LBL0\n", stream.str().c_str());
		}

		TEST_METHOD(Code__LBL) {
			LabelOperand label(0);

			LabelAtom atom(std::make_shared<LabelOperand>(label));
			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("LBL0: ", stream.str().c_str());
		}

		TEST_METHOD(Code__JMP) {
			LabelOperand label(0);

			JumpAtom atom(std::make_shared<LabelOperand>(label));
			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (JMP, , , lbl`0`)\nJMP LBL0\n", stream.str().c_str());
		}

		TEST_METHOD(CODE__IN) {
			SymbolTable table;
			auto left = table.insertVar("a", -1, SymbolTable::TableRecord::RecordType::integer);

			InAtom atom(left);
			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (IN, , , 0)\nIN 0\nSTA VAR0\n", stream.str().c_str());
		}

		TEST_METHOD(Code__RET) {
			SymbolTable table;
			auto func = table.insertFunc("f", SymbolTable::TableRecord::RecordType::integer, 0);
			auto left = table.insertVar("a", 0, SymbolTable::TableRecord::RecordType::integer);
			auto right = table.insertVar("b", 0, SymbolTable::TableRecord::RecordType::integer);

			table.calculateOffset();
			RetAtom atom(std::make_shared<NumberOperand>(5), 0, table);
			std::ostringstream stream;
			atom.generate(stream);

			Assert::AreEqual("; (RET, , , '5')\nMVI A, 5\nLXI H, 6\nDAD SP\nMOV M, A\nPOP B\nPOP B\nRET\n", stream.str().c_str());
		}

		TEST_METHOD(Code__CALL) {
			SymbolTable table;
			std::shared_ptr<MemoryOperand> func = table.insertFunc("func", SymbolTable::TableRecord::RecordType::integer, 1);
			std::shared_ptr<MemoryOperand> n = table.insertVar("n", 0, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> tmp1 = table.insertVar("[tmp1]", 0, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> tmp2 = table.insertVar("[tmp2]", 0, SymbolTable::TableRecord::RecordType::integer);
			std::shared_ptr<MemoryOperand> res = table.insertVar("res", -1, SymbolTable::TableRecord::RecordType::integer);
			table.calculateOffset();

			std::ostringstream stream;

			std::deque<std::shared_ptr<RValue>> paramsList;
			ParamAtom param(std::make_shared<NumberOperand>(5), paramsList);
			param.generate(stream);

			CallAtom callAtom(func, res, table, paramsList);
			callAtom.generate(stream);

			Assert::AreEqual("; (CALL, 0, , 4)\nLXI B, 0\nPUSH B\nMVI A, 5\nMOV C, A\nPUSH B\nCALL func\nPOP B\nPOP B\nMOV A, C\nSTA VAR4\n", stream.str().c_str());
		}
	};
}