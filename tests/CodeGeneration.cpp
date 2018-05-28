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

			Assert::IsFalse(true);
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
	};
}