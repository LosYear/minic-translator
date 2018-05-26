#include "stdafx.h"
#include "CppUnitTest.h"
#include "Translator\Translator.h"
#include "Translator\Exception.h"
#include <typeinfo>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// @TODO: check bool of translation

namespace tests
{
	TEST_CLASS(TranslatorRulesTest)
	{
	public:

		TEST_METHOD(Translator__Print)
		{
			std::istringstream stream;
			Translator translator(stream);

			translator.generateAtom(std::make_unique<JumpAtom>(std::make_shared<LabelOperand>(10)), SymbolTable::GLOBAL_SCOPE);
			translator.generateAtom(std::make_unique<LabelAtom>(std::make_shared<LabelOperand>(10)), SymbolTable::GLOBAL_SCOPE);

			std::ostringstream out;
			translator.printAtoms(out, 2);

			Assert::AreEqual("-1 (JMP, , , lbl`10`)\n-1 (LBL, , , lbl`10`)", out.str().c_str());
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

		TEST_METHOD(Translator__E1_lpar)
		{
			std::istringstream stream("((var1 || var2) && var3) == (10 + 5)");
			Translator translator(stream);
			translator.insertSymbolTableVar("var1", -1, SymbolTable::TableRecord::RecordType::integer);
			translator.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);
			translator.insertSymbolTableVar("var3", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 6, [tmp6]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (OR, 0, 1, 3)\n-1 (AND, 3, 2, 4)\n-1 (ADD, '10', '5', 5)\n-1 (MOV, '1', , 6)\n-1 (EQ, 4, 5, lbl`0`)\n-1 (MOV, '0', , 6)\n-1 (LBL, , , lbl`0`)", atoms.str().c_str());
		}

		TEST_METHOD(Translator__E1_num)
		{
			std::istringstream stream("123");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(NumberOperand) == typeid(*result));
			Assert::AreEqual("'123'", result->toString().c_str());
		}

		TEST_METHOD(Translator__E1_chr)
		{
			std::istringstream stream("'b'");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(NumberOperand) == typeid(*result));
			Assert::AreEqual("'98'", result->toString().c_str());
		}

		TEST_METHOD(Translator__E1_opinc_prefix)
		{
			std::istringstream stream("++var");
			Translator translator(stream);
			translator.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, var]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (ADD, 0, '1', 0)", atoms.str().c_str());
		}

		TEST_METHOD(Translator__E1_opinc_postfix)
		{
			std::istringstream stream("var++");
			Translator translator(stream);
			translator.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (MOV, 0, , 1)\n-1 (ADD, 0, '1', 0)", atoms.str().c_str());
		}

		TEST_METHOD(Translator__E1_id)
		{
			std::istringstream stream("var");
			Translator translator(stream);
			translator.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, var]", result->toString(true).c_str());
		}

		TEST_METHOD(Translator__E2_opnot)
		{
			std::istringstream stream("!var");
			Translator translator(stream);
			translator.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result->toString(true).c_str());


			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (NOT, 0, , 1)", atoms.str().c_str());
		}

		TEST_METHOD(Translator__E2_downToE1)
		{
			std::istringstream stream("var");
			Translator translator(stream);
			translator.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, var]", result->toString(true).c_str());
		}

		TEST_METHOD(Translator__E3_opmult)
		{
			// Case 1
			std::istringstream stream("2 * 2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, [tmp0]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (MUL, '2', '2', 0)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var * 2");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (MUL, 0, '2', 1)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var * var2 * var");
			Translator translator3(stream3);
			translator3.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);
			translator3.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 3, [tmp3]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3, 2);

			Assert::AreEqual("-1 (MUL, 0, 1, 2)\n-1 (MUL, 2, 0, 3)", atoms3.str().c_str());
		}

		TEST_METHOD(Translator__E4_opplus)
		{
			// Case 1
			std::istringstream stream("2 + 2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, [tmp0]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (ADD, '2', '2', 0)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var + 2");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (ADD, 0, '2', 1)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var + var2 + var");
			Translator translator3(stream3);
			translator3.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);
			translator3.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 3, [tmp3]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3, 2);

			Assert::AreEqual("-1 (ADD, 0, 1, 2)\n-1 (ADD, 2, 0, 3)", atoms3.str().c_str());
		}

		TEST_METHOD(Translator__E4_opminus)
		{
			// Case 1
			std::istringstream stream("2 - 2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, [tmp0]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (SUB, '2', '2', 0)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var - 2");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (SUB, 0, '2', 1)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var - var2 - var");
			Translator translator3(stream3);
			translator3.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);
			translator3.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 3, [tmp3]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3, 2);

			Assert::AreEqual("-1 (SUB, 0, 1, 2)\n-1 (SUB, 2, 0, 3)", atoms3.str().c_str());
		}

		TEST_METHOD(Translator__E5_opeq)
		{
			// Case 1
			std::istringstream stream("2 == 2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, [tmp0]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (MOV, '1', , 0)\n-1 (EQ, '2', '2', lbl`0`)\n-1 (MOV, '0', , 0)\n-1 (LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var == 2");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (MOV, '1', , 1)\n-1 (EQ, 0, '2', lbl`0`)\n-1 (MOV, '0', , 1)\n-1 (LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var == var2");
			Translator translator3(stream3);
			translator3.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);
			translator3.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3, 2);

			Assert::AreEqual("-1 (MOV, '1', , 2)\n-1 (EQ, 0, 1, lbl`0`)\n-1 (MOV, '0', , 2)\n-1 (LBL, , , lbl`0`)", atoms3.str().c_str());
		}

		TEST_METHOD(Translator__E5_opne)
		{
			// Case 1
			std::istringstream stream("2 != 2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, [tmp0]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (MOV, '1', , 0)\n-1 (NE, '2', '2', lbl`0`)\n-1 (MOV, '0', , 0)\n-1 (LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var != 2");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (MOV, '1', , 1)\n-1 (NE, 0, '2', lbl`0`)\n-1 (MOV, '0', , 1)\n-1 (LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var != var2");
			Translator translator3(stream3);
			translator3.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);
			translator3.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3, 2);

			Assert::AreEqual("-1 (MOV, '1', , 2)\n-1 (NE, 0, 1, lbl`0`)\n-1 (MOV, '0', , 2)\n-1 (LBL, , , lbl`0`)", atoms3.str().c_str());
		}

		TEST_METHOD(Translator__E5_opgt)
		{
			// Case 1
			std::istringstream stream("2 > 2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, [tmp0]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (MOV, '1', , 0)\n-1 (GT, '2', '2', lbl`0`)\n-1 (MOV, '0', , 0)\n-1 (LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var > 2");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (MOV, '1', , 1)\n-1 (GT, 0, '2', lbl`0`)\n-1 (MOV, '0', , 1)\n-1 (LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var > var2");
			Translator translator3(stream3);
			translator3.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);
			translator3.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3, 2);

			Assert::AreEqual("-1 (MOV, '1', , 2)\n-1 (GT, 0, 1, lbl`0`)\n-1 (MOV, '0', , 2)\n-1 (LBL, , , lbl`0`)", atoms3.str().c_str());
		}

		TEST_METHOD(Translator__E5_oplt)
		{
			// Case 1
			std::istringstream stream("2 < 2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, [tmp0]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (MOV, '1', , 0)\n-1 (LT, '2', '2', lbl`0`)\n-1 (MOV, '0', , 0)\n-1 (LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var < 2");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (MOV, '1', , 1)\n-1 (LT, 0, '2', lbl`0`)\n-1 (MOV, '0', , 1)\n-1 (LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var < var2");
			Translator translator3(stream3);
			translator3.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);
			translator3.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3, 2);

			Assert::AreEqual("-1 (MOV, '1', , 2)\n-1 (LT, 0, 1, lbl`0`)\n-1 (MOV, '0', , 2)\n-1 (LBL, , , lbl`0`)", atoms3.str().c_str());
		}

		TEST_METHOD(Translator__E5_ople)
		{
			// Case 1
			std::istringstream stream("2 <= 2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, [tmp0]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (MOV, '1', , 0)\n-1 (LE, '2', '2', lbl`0`)\n-1 (MOV, '0', , 0)\n-1 (LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var <= 2");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (MOV, '1', , 1)\n-1 (LE, 0, '2', lbl`0`)\n-1 (MOV, '0', , 1)\n-1 (LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var <= var2");
			Translator translator3(stream3);
			translator3.insertSymbolTableVar("var", -1, SymbolTable::TableRecord::RecordType::integer);
			translator3.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3, 2);

			Assert::AreEqual("-1 (MOV, '1', , 2)\n-1 (LE, 0, 1, lbl`0`)\n-1 (MOV, '0', , 2)\n-1 (LBL, , , lbl`0`)", atoms3.str().c_str());
		}

		TEST_METHOD(Translator__E6_opand)
		{
			// Case 1
			std::istringstream stream("var1 && var2");
			Translator translator(stream);
			translator.insertSymbolTableVar("var1", -1, SymbolTable::TableRecord::RecordType::integer);
			translator.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (AND, 0, 1, 2)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var1 && var2 && var3");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var1", -1, SymbolTable::TableRecord::RecordType::integer);
			translator2.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);
			translator2.insertSymbolTableVar("var3", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 4, [tmp4]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (AND, 0, 1, 3)\n-1 (AND, 3, 2, 4)", atoms2.str().c_str());
		}


		TEST_METHOD(Translator__E7_opor)
		{
			// Case 1
			std::istringstream stream("var1 || var2");
			Translator translator(stream);
			translator.insertSymbolTableVar("var1", -1, SymbolTable::TableRecord::RecordType::integer);
			translator.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms, 2);

			Assert::AreEqual("-1 (OR, 0, 1, 2)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var1 || var2 || var3");
			Translator translator2(stream2);
			translator2.insertSymbolTableVar("var1", -1, SymbolTable::TableRecord::RecordType::integer);
			translator2.insertSymbolTableVar("var2", -1, SymbolTable::TableRecord::RecordType::integer);
			translator2.insertSymbolTableVar("var3", -1, SymbolTable::TableRecord::RecordType::integer);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 4, [tmp4]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2, 2);

			Assert::AreEqual("-1 (OR, 0, 1, 3)\n-1 (OR, 3, 2, 4)", atoms2.str().c_str());
		}

		TEST_METHOD(Translator__DeclareStmt_funcNoArgs) {
			std::istringstream stream("int n(){} int main(){}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printSymbolTable(result);

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          n          func       integer    0          0          -1         0          \n" +
				"1          main       func       integer    0          0          -1         0          \n";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__DeclareStmt_funcWithArgs) {
			std::istringstream stream("int n(int b, char c){} int main(){}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printSymbolTable(result);

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          n          func       integer    2          0          -1         0          \n" +
				"1          b          var        integer    -1         0          0          0          \n" +
				"2          c          var        chr        -1         0          0          0          \n" +
				"3          main       func       integer    0          0          -1         0          \n";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__DeclareStmt_varWithInit) {
			std::istringstream stream("int n = 5; int main(){}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printSymbolTable(result);

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          n          var        integer    -1         5          -1         0          \n" +
				"1          main       func       integer    0          0          -1         0          \n";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__DeclareStmt_varList) {
			std::istringstream stream("char a, b = 5, c; int k = 10; int main(){}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printSymbolTable(result);

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          a          var        chr        -1         0          -1         0          \n" +
				"1          b          var        chr        -1         5          -1         0          \n" +
				"2          c          var        chr        -1         0          -1         0          \n" +
				"3          k          var        integer    -1         10         -1         0          \n" +
				"4          main       func       integer    0          0          -1         0          \n";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__DeclareStmt_insideContext) {
			std::istringstream stream("int main(){char a, b = 5, c; int k = 10;}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printSymbolTable(result);

			std::string excepted = std::string("SYMBOL TABLE:\n") +
				"----------\n" +
				"code       name       kind       type       len        init       scope      offset     \n" +
				"0          main       func       integer    0          0          -1         0          \n" +
				"1          a          var        chr        -1         0          0          0          \n" +
				"2          b          var        chr        -1         5          0          0          \n" +
				"3          c          var        chr        -1         0          0          0          \n" +
				"4          k          var        integer    -1         10         0          0          \n";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__AssignOrCallOp_funcCall) {
			std::istringstream stream("int even(int a){} int main(){even(5);}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 1);

			std::string excepted = "0 (RET, , , '0')\n2 (PARAM, , , '5')\n2 (CALL, 0, , 3)\n2 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__AssingOrCallOp_funcCallNoParams) {
			std::istringstream stream("int even(){} int main(){even();}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = "0 (RET, , , '0')\n1 (CALL, 0, , 2)\n1 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__AssignOrCallOp_assign) {
			std::istringstream stream("int main(){int c; c = 5;}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = "0 (MOV, '5', , 1)\n0 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__WhileOp) {
			std::istringstream stream("int main(){int i = 0; while(i != 5){i = i + 1;}}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (LBL, , , lbl`0`)\n0 (MOV, '1', , 2)\n0 (NE, 1, '5', lbl`1`)\n0 (MOV, '0', , 2)\n") +
				"0 (LBL, , , lbl`1`)\n0 (EQ, 2, '0', lbl`2`)\n0 (ADD, 1, '1', 3)\n0 (MOV, 3, , 1)\n0 (JMP, , , lbl`0`)\n" +
				"0 (LBL, , , lbl`2`)\n0 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__E1_functionCallWithParams) {
			std::istringstream stream("func1(5)");
			Translator translator(stream);
			translator.insertSymbolTableFunc("func1", SymbolTable::TableRecord::RecordType::integer, 1);

			auto result = translator.translateExpresssion();
			std::ostringstream atoms;
			translator.printAtoms(atoms, 0);

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result->toString(true).c_str());
			Assert::AreEqual("-1 (PARAM, , , '5')\n-1 (CALL, 0, , 1)", atoms.str().c_str());
		}
		TEST_METHOD(Translator__E1_functionCallEmpty) {
			std::istringstream stream("func1()");
			Translator translator(stream);
			translator.insertSymbolTableFunc("func1", SymbolTable::TableRecord::RecordType::integer, 0);

			auto result = translator.translateExpresssion();
			std::ostringstream atoms;
			translator.printAtoms(atoms, 0);

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result->toString(true).c_str());
			Assert::AreEqual("-1 (CALL, 0, , 1)", atoms.str().c_str());
		}

		TEST_METHOD(Translator_ForOp_emptyInit) {
			std::istringstream stream("int main(){int i; for(; i < 10; ++i){i = i + 0;}}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (LBL, , , lbl`0`)\n")
				+ "0 (MOV, '1', , 2)\n0 (LT, 1, '10', lbl`1`)\n0 (MOV, '0', , 2)\n0 (LBL, , , lbl`1`)\n"
				+ "0 (EQ, 2, '0', lbl`4`)\n0 (JMP, , , lbl`3`)\n0 (LBL, , , lbl`2`)\n"
				+ "0 (ADD, 1, '1', 1)\n"
				+ "0 (JMP, , , lbl`0`)\n0 (LBL, , , lbl`3`)\n"
				+ "0 (ADD, 1, '0', 3)\n0 (MOV, 3, , 1)\n"
				+ "0 (JMP, , , lbl`2`)\n0 (LBL, , , lbl`4`)\n"
				+ "0 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator_ForOp_emptyCond) {
			std::istringstream stream("int main(){int i; for(i = 0; ; ++i){i = i + 0;}}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (MOV, '0', , 1)\n0 (LBL, , , lbl`0`)\n")
				+ "0 (EQ, '1', '0', lbl`3`)\n0 (JMP, , , lbl`2`)\n0 (LBL, , , lbl`1`)\n"
				+ "0 (ADD, 1, '1', 1)\n"
				+ "0 (JMP, , , lbl`0`)\n0 (LBL, , , lbl`2`)\n"
				+ "0 (ADD, 1, '0', 2)\n0 (MOV, 2, , 1)\n"
				+ "0 (JMP, , , lbl`1`)\n0 (LBL, , , lbl`3`)\n"
				+ "0 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator_ForOp_emptyExpr) {
			std::istringstream stream("int main(){int i; for(i = 0; i < 10;){i = i + 0;}}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (MOV, '0', , 1)\n0 (LBL, , , lbl`0`)\n")
				+ "0 (MOV, '1', , 2)\n0 (LT, 1, '10', lbl`1`)\n0 (MOV, '0', , 2)\n0 (LBL, , , lbl`1`)\n"
				+ "0 (EQ, 2, '0', lbl`4`)\n0 (JMP, , , lbl`3`)\n0 (LBL, , , lbl`2`)\n"
				+ "0 (JMP, , , lbl`0`)\n0 (LBL, , , lbl`3`)\n"
				+ "0 (ADD, 1, '0', 3)\n0 (MOV, 3, , 1)\n"
				+ "0 (JMP, , , lbl`2`)\n0 (LBL, , , lbl`4`)\n"
				+ "0 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator_ForOp) {
			std::istringstream stream("int main(){int i; for(i = 0; i < 10; ++i){i = i + 0;}}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (MOV, '0', , 1)\n0 (LBL, , , lbl`0`)\n")
				+ "0 (MOV, '1', , 2)\n0 (LT, 1, '10', lbl`1`)\n0 (MOV, '0', , 2)\n0 (LBL, , , lbl`1`)\n"
				+ "0 (EQ, 2, '0', lbl`4`)\n0 (JMP, , , lbl`3`)\n0 (LBL, , , lbl`2`)\n"
				+ "0 (ADD, 1, '1', 1)\n"
				+ "0 (JMP, , , lbl`0`)\n0 (LBL, , , lbl`3`)\n"
				+ "0 (ADD, 1, '0', 3)\n0 (MOV, 3, , 1)\n"
				+ "0 (JMP, , , lbl`2`)\n0 (LBL, , , lbl`4`)\n"
				+ "0 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator_IfOp) {
			std::istringstream stream("int main(){int i; if(i > 5){i = 1;} else {i = 0;}}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (MOV, '1', , 2)\n0 (GT, 1, '5', lbl`0`)\n0 (MOV, '0', , 2)\n0 (LBL, , , lbl`0`)\n")
				+ "0 (EQ, 2, '0', lbl`1`)\n0 (MOV, '1', , 1)\n0 (JMP, , , lbl`2`)\n"
				+ "0 (LBL, , , lbl`1`)\n0 (MOV, '0', , 1)\n0 (LBL, , , lbl`2`)\n"
				+ "0 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}


		TEST_METHOD(Translator_IfOp_noElse) {
			std::istringstream stream("int main(){int i; if(i > 5){i = 1;}}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (MOV, '1', , 2)\n0 (GT, 1, '5', lbl`0`)\n0 (MOV, '0', , 2)\n0 (LBL, , , lbl`0`)\n")
				+ "0 (EQ, 2, '0', lbl`1`)\n0 (MOV, '1', , 1)\n0 (JMP, , , lbl`2`)\n"
				+ "0 (LBL, , , lbl`1`)\n0 (LBL, , , lbl`2`)\n"
				+ "0 (RET, , , '0')";

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__Stmt_return) {
			std::istringstream stream("int main(){return 1;}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (RET, , , '1')\n0 (RET, , , '0')");

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__IOp) {
			std::istringstream stream("int main(){int i = 0; in i;}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (IN, , , 1)\n0 (RET, , , '0')");

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}

		TEST_METHOD(Translator__OOp_value) {
			std::istringstream stream("int main(){int i = 0; out i + 1;}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (ADD, 1, '1', 2)\n0 (OUT, , , 2)\n0 (RET, , , '0')");

			Assert::AreEqual(excepted.c_str(), result.str().c_str());

		}

		TEST_METHOD(Translator__OOp_str) {
			std::istringstream stream("int main(){int i = 0; out \"HELLO\";}");
			Translator translator(stream);

			bool translated = translator.translate();
			Assert::IsTrue(translated);

			std::ostringstream result;
			translator.printAtoms(result, 0);

			std::string excepted = std::string("0 (OUT, , , str`0`)\n0 (RET, , , '0')");

			Assert::AreEqual(excepted.c_str(), result.str().c_str());
		}
	};
}