#include "stdafx.h"
#include <exception>
#include <sstream>
#include <vector>
#include "CppUnitTest.h"
#include "LexicalAnalyzer\Token.h"
#include "LexicalAnalyzer\Scanner.h"
#include <memory>

#define AssertAreTypesEqual(a,b) (Assert::AreEqual(static_cast<int>(a), static_cast<int>(b)))

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(TestLexicalAnalyzer)
	{
	public:

		/*
		* Token tests
		*/

		TEST_METHOD(LexicalToken__TypeConstructor)
		{
			LexicalToken token(LexemType::kwcase);
			Assert::AreEqual((int)LexemType::kwcase, (int)token.type());

			LexicalToken token_1(LexemType::kwout);
			Assert::AreEqual((int)LexemType::kwout, (int)token_1.type());
		}

		TEST_METHOD(LexicalToken__ConstructorExceptions)
		{
			auto createStr = [this] {LexicalToken token(LexemType::str); };
			Assert::ExpectException<std::invalid_argument>(createStr);

			auto createNum = [this] {LexicalToken token(LexemType::num); };
			Assert::ExpectException<std::invalid_argument>(createNum);

			auto createChr = [this] {LexicalToken token(LexemType::chr); };
			Assert::ExpectException<std::invalid_argument>(createChr);

			auto createId = [this] {LexicalToken token(LexemType::id); };
			Assert::ExpectException<std::invalid_argument>(createId);

			auto createError = [this] {LexicalToken token(LexemType::error); };
			Assert::ExpectException<std::invalid_argument>(createError);
		}

		TEST_METHOD(LexicalToken__NumConstructor)
		{
			LexicalToken num(12);

			Assert::AreEqual((int)LexemType::num, (int)num.type());
			Assert::AreEqual(num.value(), 12);

			LexicalToken num_2(-2213);

			Assert::AreEqual((int)LexemType::num, (int)num_2.type());
			Assert::AreEqual(num_2.value(), -2213);
		}

		TEST_METHOD(LexicalToken__StringConstructor)
		{
			LexicalToken error_token(LexemType::error, "error");

			Assert::AreEqual((int)LexemType::error, (int)error_token.type());
			Assert::AreEqual(error_token.str().c_str(), "error");

			LexicalToken str_token(LexemType::str, "ABC");

			Assert::AreEqual((int)LexemType::str, (int)str_token.type());
			Assert::AreEqual(str_token.str().c_str(), "ABC");

			LexicalToken id_token(LexemType::id, "name");

			Assert::AreEqual((int)LexemType::id, (int)id_token.type());
			Assert::AreEqual(id_token.str().c_str(), "name");
		}

		TEST_METHOD(LexicalToken__CharConstructor)
		{
			LexicalToken char_token('Z');

			Assert::AreEqual((int)LexemType::chr, (int)char_token.type());
			Assert::AreEqual((char)char_token.value(), 'Z');
		}

		TEST_METHOD(LexicalToken__ToString)
		{
			LexicalToken num(123);
			Assert::AreEqual("[num, 123]", num.toString().c_str());

			LexicalToken str(LexemType::str, "Hello, it's me");
			Assert::AreEqual("[str, \"Hello, it's me\"]", str.toString().c_str());

			LexicalToken error(LexemType::error, "Error message");
			Assert::AreEqual("[error, \"Error message\"]", error.toString().c_str());

			LexicalToken id(LexemType::id, "var");
			Assert::AreEqual("[id, \"var\"]", id.toString().c_str());

			LexicalToken chr('a');
			Assert::AreEqual("[chr, 'a']", chr.toString().c_str());

			LexicalToken comma(LexemType::comma);
			Assert::AreEqual("[comma]", comma.toString().c_str());
		}

		TEST_METHOD(LexicalToken__Print)
		{
			std::ostringstream output_stream;
			LexicalToken kwout(LexemType::kwout);

			kwout.print(output_stream);

			Assert::AreEqual(output_stream.str().c_str(), "[kwout]");
		}

		
		/*
		* Scanner tests
		*/
		TEST_METHOD(LexicalScanner__Empty) {
			std::istringstream input("");
			LexicalScanner scanner(input);
			AssertAreTypesEqual(scanner.getNextToken().type(), LexemType::eof);
		}

		TEST_METHOD(LexicalScanner__Num) {
			std::istringstream input("12");
			LexicalScanner scanner(input);
			LexicalToken num = scanner.getNextToken();
			AssertAreTypesEqual(num.type(), LexemType::num);
			Assert::AreEqual(num.value(), 12);

			std::istringstream input_1("-122");
			LexicalScanner scanner_1(input_1);
			LexicalToken num_1 = scanner_1.getNextToken();
			AssertAreTypesEqual(num_1.type(), LexemType::num);
			Assert::AreEqual(num_1.value(), -122);

			std::istringstream input_2("2 34");
			LexicalScanner scanner_2(input_2);
			LexicalToken num_2 = scanner_2.getNextToken();
			AssertAreTypesEqual(num_2.type(), LexemType::num);
			Assert::AreEqual(num_2.value(), 2);
		}

		TEST_METHOD(LexicalScanner__Char) {
			std::istringstream input("'a'");
			LexicalScanner scanner(input);
			LexicalToken chr = scanner.getNextToken();
			AssertAreTypesEqual(chr.type(), LexemType::chr);
			Assert::AreEqual((char)chr.value(), 'a');
		}

		TEST_METHOD(LexicalScanner__String) {
			std::istringstream input("\"ABCdef\"");
			LexicalScanner scanner(input);
			LexicalToken str = scanner.getNextToken();
			AssertAreTypesEqual(str.type(), LexemType::str);
			Assert::AreEqual(str.str().c_str(), "ABCdef");
		}

		TEST_METHOD(LexicalScanner__Id) {
			std::istringstream input("main");
			LexicalScanner scanner(input);
			LexicalToken id = scanner.getNextToken();
			AssertAreTypesEqual(id.type(), LexemType::id);
			Assert::AreEqual(id.str().c_str(), "main");
		}

		TEST_METHOD(LexicalScanner__Punctuation) {
			std::istringstream input("(){}[];,:");
			LexicalScanner scanner(input);
			std::vector<LexemType> excepted = { LexemType::lpar, LexemType::rpar,
				LexemType::lbrace, LexemType::rbrace, LexemType::lbracket, LexemType::rbracket,
				LexemType::semicolon, LexemType::comma, LexemType::colon };
			LexemType* current = &excepted[0];
			std::unique_ptr<LexicalToken> token;

			while (token = std::make_unique<LexicalToken>(scanner.getNextToken()), token->type() != LexemType::eof) {
				AssertAreTypesEqual(token->type(), *current);
				current++;
			}
		}

		TEST_METHOD(LexicalScanner__Ops) {
			std::istringstream input("= + - * ++ == != < > <= ! || &&");
			LexicalScanner scanner(input);
			std::vector<LexemType> excepted = { LexemType::opassign, LexemType::opplus, LexemType::opminus,
				LexemType::opmult, LexemType::opinc, LexemType::opeq, LexemType::opne, LexemType::oplt,
				LexemType::opgt, LexemType::ople, LexemType::opnot, LexemType::opor, LexemType::opand
			};
			LexemType* current = &excepted[0];
			std::unique_ptr<LexicalToken> token;

			while (token = std::make_unique<LexicalToken>(scanner.getNextToken()), token->type() != LexemType::eof) {
				AssertAreTypesEqual(token->type(), *current);
				current++;
			}
		}

		TEST_METHOD(LexicalScanner__IncompleteOps) {
			std::istringstream input("|");
			LexicalScanner scanner(input);
			AssertAreTypesEqual(scanner.getNextToken().type(), LexemType::error);

			std::istringstream input_2("&");
			LexicalScanner scanner_2(input_2);
			AssertAreTypesEqual(scanner_2.getNextToken().type(), LexemType::error);
		}

		TEST_METHOD(LexicalScanner__Keywords) {
			std::istringstream input("int char if else switch case while for return in out default");
			LexicalScanner scanner(input);
			std::vector<LexemType> excepted = { LexemType::kwint, LexemType::kwchar, LexemType::kwif,
				LexemType::kwelse, LexemType::kwswitch, LexemType::kwcase, LexemType::kwwhile,
				LexemType::kwfor, LexemType::kwreturn, LexemType::kwin, LexemType::kwout, LexemType::kwdefault
			};
			LexemType* current = &excepted[0];
			std::unique_ptr<LexicalToken> token;

			while (token = std::make_unique<LexicalToken>(scanner.getNextToken()),  token->type() != LexemType::eof) {
				AssertAreTypesEqual(token->type(), *current);
				current++;
			}
		}

		TEST_METHOD(LexicalScanner__Equations) {
			std::istringstream input("(a||b)&&c!=b++>a-!a");
			LexicalScanner scanner(input);

			std::vector<LexemType> excepted = { LexemType::lpar, LexemType::id, LexemType::opor, LexemType::id,
				LexemType::rpar, LexemType::opand, LexemType::id, LexemType::opne, LexemType::id,
				LexemType::opinc, LexemType::opgt, LexemType::id, LexemType::opminus, LexemType::opnot, LexemType::id
			};
			LexemType* current = &excepted[0];
			std::unique_ptr<LexicalToken> token;

			while (token = std::make_unique<LexicalToken>(scanner.getNextToken()), token->type() != LexemType::eof) {
				AssertAreTypesEqual(token->type(), *current);
				current++;
			}
		}

		TEST_METHOD(LexicalScanner__FibProg) {
			std::istringstream input("int fib(int n){ if (n == 0 || n == 1) { return 1; } \
			return fib(n - 1) + fib(n - 2); } int main() { int a = 0; in(a); out(fib(a)); return 0;}");
			LexicalScanner scanner(input);

			std::vector<LexemType> excepted = { LexemType::kwint, LexemType::id, LexemType::lpar, LexemType::kwint, LexemType::id,
				LexemType::rpar, LexemType::lbrace, LexemType::kwif, LexemType::lpar, LexemType::id, LexemType::opeq, LexemType::num, LexemType::opor,
				LexemType::id, LexemType::opeq, LexemType::num, LexemType::rpar, LexemType::lbrace, LexemType::kwreturn, LexemType::num, LexemType::semicolon,
				LexemType::rbrace, LexemType::kwreturn, LexemType::id, LexemType::lpar, LexemType::id, LexemType::opminus, LexemType::num, LexemType::rpar, LexemType::opplus,
				LexemType::id, LexemType::lpar, LexemType::id, LexemType::opminus, LexemType::num, LexemType::rpar, LexemType::semicolon, LexemType::rbrace, LexemType::kwint, LexemType::id,
				LexemType::lpar, LexemType::rpar, LexemType::lbrace, LexemType::kwint, LexemType::id, LexemType::opassign, LexemType::num, LexemType::semicolon, LexemType::kwin,
				LexemType::lpar, LexemType::id, LexemType::rpar, LexemType::semicolon, LexemType::kwout, LexemType::lpar, LexemType::id, LexemType::lpar, LexemType::id,
				LexemType::rpar, LexemType::rpar, LexemType::semicolon, LexemType::kwreturn, LexemType::num, LexemType::semicolon, LexemType::rbrace
			};
			LexemType* current = &excepted[0];
			std::unique_ptr<LexicalToken> token;

			while (token = std::make_unique<LexicalToken>(scanner.getNextToken()), token->type() != LexemType::eof) {
				AssertAreTypesEqual(token->type(), *current);
				current++;
			}

		}

		TEST_METHOD(LexicalScanner__HelloWorldProg) {
			std::istringstream input("int main(){char[] message = \"Hello world!\"; out(message); return 0; }");
			LexicalScanner scanner(input);

			std::vector<LexemType> excepted = { LexemType::kwint, LexemType::id, LexemType::lpar,
				LexemType::rpar, LexemType::lbrace, LexemType::kwchar, LexemType::lbracket, LexemType::rbracket, LexemType::id, LexemType::opassign, LexemType::str,
				LexemType::semicolon, LexemType::kwout, LexemType::lpar, LexemType::id, LexemType::rpar, LexemType::semicolon, LexemType::kwreturn, LexemType::num, LexemType::semicolon,
				LexemType::rbrace
			};
			LexemType* current = &excepted[0];
			std::unique_ptr<LexicalToken> token;

			while (token = std::make_unique<LexicalToken>(scanner.getNextToken()), token->type() != LexemType::eof) {
				AssertAreTypesEqual(token->type(), *current);
				current++;
			}

		}

		TEST_METHOD(LexicalScanner__MulticharChar) {
			std::istringstream input("'12345'");
			LexicalScanner scanner(input);

			LexicalToken token = scanner.getNextToken();

			AssertAreTypesEqual(token.type(), LexemType::error);
		}

		TEST_METHOD(LexicalScanner__UnclosedQuotes) {
			std::istringstream input("'1");
			LexicalScanner scanner(input);

			LexicalToken token = scanner.getNextToken();

			AssertAreTypesEqual(token.type(), LexemType::error);

			std::istringstream input_1("\"12356");
			LexicalScanner scanner_1(input_1);

			LexicalToken token_1 = scanner_1.getNextToken();

			AssertAreTypesEqual(token_1.type(), LexemType::error);
		}
	};
}