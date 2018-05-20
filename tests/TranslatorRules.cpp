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

		TEST_METHOD(Translator__E1_lpar)
		{
			std::istringstream stream("((var1 || var2) && var3) == (10 + 5)");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 6, [tmp6]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms);

			Assert::AreEqual("(OR, 0, 1, 2)\n(AND, 2, 3, 4)\n(ADD, '10', '5', 5)\n(MOV, '1', , 6)\n(EQ, 4, 5, lbl`0`)\n(MOV, '0', , 6)\n(LBL, , , lbl`0`)", atoms.str().c_str());
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

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, var]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms);

			Assert::AreEqual("(ADD, 0, '1', 0)", atoms.str().c_str());
		}

		TEST_METHOD(Translator__E1_opinc_postfix)
		{
			std::istringstream stream("var++");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms);

			Assert::AreEqual("(MOV, 0, , 1)\n(ADD, 0, '1', 0)", atoms.str().c_str());
		}

		TEST_METHOD(Translator__E1_id)
		{
			std::istringstream stream("var");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 0, var]", result->toString(true).c_str());
		}

		TEST_METHOD(Translator__E2_opnot)
		{
			std::istringstream stream("!var");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result->toString(true).c_str());


			std::ostringstream atoms;
			translator.printAtoms(atoms);

			Assert::AreEqual("(NOT, 0, , 1)", atoms.str().c_str());
		}

		TEST_METHOD(Translator__E2_downToE1)
		{
			std::istringstream stream("var");
			Translator translator(stream);

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
			translator.printAtoms(atoms);

			Assert::AreEqual("(MUL, '2', '2', 0)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var * 2");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(MUL, 0, '2', 1)", atoms2.str().c_str());
			
			// Case 3
			std::istringstream stream3("var * var2 * var");
			Translator translator3(stream3);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 3, [tmp3]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3);

			Assert::AreEqual("(MUL, 0, 1, 2)\n(MUL, 2, 0, 3)", atoms3.str().c_str());
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
			translator.printAtoms(atoms);

			Assert::AreEqual("(ADD, '2', '2', 0)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var + 2");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(ADD, 0, '2', 1)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var + var2 + var");
			Translator translator3(stream3);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 3, [tmp3]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3);

			Assert::AreEqual("(ADD, 0, 1, 2)\n(ADD, 2, 0, 3)", atoms3.str().c_str());
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
			translator.printAtoms(atoms);

			Assert::AreEqual("(SUB, '2', '2', 0)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var - 2");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(SUB, 0, '2', 1)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var - var2 - var");
			Translator translator3(stream3);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 3, [tmp3]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3);

			Assert::AreEqual("(SUB, 0, 1, 2)\n(SUB, 2, 0, 3)", atoms3.str().c_str());
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
			translator.printAtoms(atoms);

			Assert::AreEqual("(MOV, '1', , 0)\n(EQ, '2', '2', lbl`0`)\n(MOV, '0', , 0)\n(LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var == 2");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(MOV, '1', , 1)\n(EQ, 0, '2', lbl`0`)\n(MOV, '0', , 1)\n(LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var == var2");
			Translator translator3(stream3);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3);

			Assert::AreEqual("(MOV, '1', , 2)\n(EQ, 0, 1, lbl`0`)\n(MOV, '0', , 2)\n(LBL, , , lbl`0`)", atoms3.str().c_str());
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
			translator.printAtoms(atoms);

			Assert::AreEqual("(MOV, '1', , 0)\n(NE, '2', '2', lbl`0`)\n(MOV, '0', , 0)\n(LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var != 2");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(MOV, '1', , 1)\n(NE, 0, '2', lbl`0`)\n(MOV, '0', , 1)\n(LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var != var2");
			Translator translator3(stream3);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3);

			Assert::AreEqual("(MOV, '1', , 2)\n(NE, 0, 1, lbl`0`)\n(MOV, '0', , 2)\n(LBL, , , lbl`0`)", atoms3.str().c_str());
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
			translator.printAtoms(atoms);

			Assert::AreEqual("(MOV, '1', , 0)\n(GT, '2', '2', lbl`0`)\n(MOV, '0', , 0)\n(LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var > 2");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(MOV, '1', , 1)\n(GT, 0, '2', lbl`0`)\n(MOV, '0', , 1)\n(LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var > var2");
			Translator translator3(stream3);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3);

			Assert::AreEqual("(MOV, '1', , 2)\n(GT, 0, 1, lbl`0`)\n(MOV, '0', , 2)\n(LBL, , , lbl`0`)", atoms3.str().c_str());
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
			translator.printAtoms(atoms);

			Assert::AreEqual("(MOV, '1', , 0)\n(LT, '2', '2', lbl`0`)\n(MOV, '0', , 0)\n(LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var < 2");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(MOV, '1', , 1)\n(LT, 0, '2', lbl`0`)\n(MOV, '0', , 1)\n(LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var < var2");
			Translator translator3(stream3);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3);

			Assert::AreEqual("(MOV, '1', , 2)\n(LT, 0, 1, lbl`0`)\n(MOV, '0', , 2)\n(LBL, , , lbl`0`)", atoms3.str().c_str());
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
			translator.printAtoms(atoms);

			Assert::AreEqual("(MOV, '1', , 0)\n(LE, '2', '2', lbl`0`)\n(MOV, '0', , 0)\n(LBL, , , lbl`0`)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var <= 2");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 1, [tmp1]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(MOV, '1', , 1)\n(LE, 0, '2', lbl`0`)\n(MOV, '0', , 1)\n(LBL, , , lbl`0`)", atoms2.str().c_str());

			// Case 3
			std::istringstream stream3("var <= var2");
			Translator translator3(stream3);

			auto result3 = translator3.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result3->toString(true).c_str());

			std::ostringstream atoms3;
			translator3.printAtoms(atoms3);

			Assert::AreEqual("(MOV, '1', , 2)\n(LE, 0, 1, lbl`0`)\n(MOV, '0', , 2)\n(LBL, , , lbl`0`)", atoms3.str().c_str());
		}

		TEST_METHOD(Translator__E6_opand)
		{
			// Case 1
			std::istringstream stream("var1 && var2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms);

			Assert::AreEqual("(AND, 0, 1, 2)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var1 && var2 && var3");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 4, [tmp4]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(AND, 0, 1, 2)\n(AND, 2, 3, 4)", atoms2.str().c_str());
		}


		TEST_METHOD(Translator__E7_opor)
		{
			// Case 1
			std::istringstream stream("var1 || var2");
			Translator translator(stream);

			auto result = translator.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result));
			Assert::AreEqual("[MemOp, 2, [tmp2]]", result->toString(true).c_str());

			std::ostringstream atoms;
			translator.printAtoms(atoms);

			Assert::AreEqual("(OR, 0, 1, 2)", atoms.str().c_str());


			// Case 2
			std::istringstream stream2("var1 || var2 || var3");
			Translator translator2(stream2);

			auto result2 = translator2.translateExpresssion();

			Assert::IsTrue(typeid(MemoryOperand) == typeid(*result2));
			Assert::AreEqual("[MemOp, 4, [tmp4]]", result2->toString(true).c_str());


			std::ostringstream atoms2;
			translator2.printAtoms(atoms2);

			Assert::AreEqual("(OR, 0, 1, 2)\n(OR, 2, 3, 4)", atoms2.str().c_str());
		}
	};
}