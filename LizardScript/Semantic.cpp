#include "ByteCodeGenerator.h"
#include "LizardScriptLibrary.h"

using namespace LizardScript;

SyntaxCore* IOperator::core;

#define CAST/*(predicate, ...)*/ if (
#define THEN ) { 
#define ENDCAST if (from.full_eq(to)) return true; else return cast(reg, to); }

#define KEYWORD(text) if (_tcscmp(kwtoken->value, text) == 0)//�������� ���, ����� ���������� ��-���� ������� ���������
#define WHEN { int csize = code.data.size(); if (
#define OPCODE ) { code.push(
#define RETURNS(T) ); r1.type = T; reg.push(r1); return; } else code.data.resize(csize); }
#define VOID ); } else code.data.resize(csize); }

bool ByteCodeGenerator::cast(typed_reg reg, TypeInfo to)
{
	TypeInfo& from = reg.type;

	if (from.full_eq(to))
		return true;

	CAST
		from.size() <= 8
		&& to.size() <= 8
		&& from.ptr > to.ptr
		THEN open_reg(reg, to.ptr); 
	ENDCAST;

	CAST 
		from.ptr > to.ptr
		&& to.ptr >= 1
		THEN open_reg(reg, to.ptr);
	ENDCAST;

	CAST
		from == TYPEINFO(int)
		&& to == TYPEINFO(float)
		&& from.ptr == 0 && to.ptr == 0
		THEN code << opcode::int_to_float << reg; from.t = to.t;
	ENDCAST;

	CAST
		from == TYPEINFO(float)
		&& to == TYPEINFO(int)
		&& from.ptr == 0 && to.ptr == 0
		THEN code << opcode::float_to_int << reg; from.t = to.t;
	ENDCAST;

	return false;
}

void ByteCodeGenerator::addKeywordUnary(Keyword* kwtoken, typed_reg r1)
{
		KEYWORD("++") WHEN
		cast(r1, TYPEINFO(int, 1))
		OPCODE opcode::inc
		RETURNS(TYPEINFO(int, 1))

		KEYWORD("--") WHEN
		cast(r1, TYPEINFO(int, 1))
		OPCODE opcode::dec
		RETURNS(TYPEINFO(int, 1))
}

void ByteCodeGenerator::addKeywordBinary(Keyword* kwtoken, typed_reg r1, typed_reg r2)
{
	//int arithmetics

		KEYWORD("+") WHEN
		cast(r1, TYPEINFO(int, 0)) &&
		cast(r2, TYPEINFO(int, 0))
		OPCODE opcode::add_int_int
		RETURNS(TYPEINFO(int))

		KEYWORD("-") WHEN
		cast(r1, TYPEINFO(int, 0)) &&
		cast(r2, TYPEINFO(int, 0))
		OPCODE opcode::sub_int_int
		RETURNS(TYPEINFO(int))

		KEYWORD("*") WHEN
		cast(r1, TYPEINFO(int, 0)) &&
		cast(r2, TYPEINFO(int, 0))
		OPCODE opcode::mul_int_int
		RETURNS(TYPEINFO(int))

		KEYWORD("/") WHEN
		cast(r1, TYPEINFO(int, 0)) &&
		cast(r2, TYPEINFO(int, 0))
		OPCODE opcode::div_int_int
		RETURNS(TYPEINFO(int))

	//float arithmetics

		KEYWORD("+") WHEN
		cast(r1, TYPEINFO(float, 0)) &&
		cast(r2, TYPEINFO(float, 0))
		OPCODE opcode::add_float_float
		RETURNS(TYPEINFO(float))

		KEYWORD("-") WHEN
		cast(r1, TYPEINFO(float, 0)) &&
		cast(r2, TYPEINFO(float, 0))
		OPCODE opcode::sub_float_float
		RETURNS(TYPEINFO(float))

		KEYWORD("*") WHEN
		cast(r1, TYPEINFO(float, 0)) &&
		cast(r2, TYPEINFO(float, 0))
		OPCODE opcode::mul_float_float
		RETURNS(TYPEINFO(float))

		KEYWORD("/") WHEN
		cast(r1, TYPEINFO(float, 0)) &&
		cast(r2, TYPEINFO(float, 0))
		OPCODE opcode::div_float_float
		RETURNS(TYPEINFO(float))

	//int comparators

		KEYWORD(">") WHEN
		cast(r1, TYPEINFO(int, 0)) &&
		cast(r2, TYPEINFO(int, 0))
		OPCODE opcode::more_int_int
		RETURNS(TYPEINFO(bool))

		KEYWORD("<") WHEN
		cast(r1, TYPEINFO(int, 0)) &&
		cast(r2, TYPEINFO(int, 0))
		OPCODE opcode::less_int_int
		RETURNS(TYPEINFO(bool))

	//=

		KEYWORD("=") WHEN
		cast(r1, TYPEINFO(int, 1)) &&
		cast(r2, TYPEINFO(int, 0))
		OPCODE opcode::set_32
		RETURNS(TYPEINFO(int, 1))

		KEYWORD("=") WHEN
		cast(r1, TYPEINFO(float, 1)) &&
		cast(r2, TYPEINFO(float, 0))
		OPCODE opcode::set_32
		RETURNS(TYPEINFO(float, 1))

		//����� ���������� � ��� ���������, ��������� ������ �� �������?

	throw Exception(std::string("Unknown operator \"") + kwtoken->value + "\" for types " + r1.type.text() + " and " + r2.type.text() + ".");
}

const std::vector<UnaryOperator>& ByteCodeGenerator::initUnary(SyntaxCore& core)
{
	UnaryOperator::core = &core;
	static std::vector<UnaryOperator> kw =
	{
		UnaryOperator("++", TYPEINFO(int, 1), opcode::inc),
		UnaryOperator("--", TYPEINFO(int, 1), opcode::dec),
	};

	return kw;
}

const std::vector<BinaryOperator>& ByteCodeGenerator::initBinary(SyntaxCore& core)
{
	BinaryOperator::core = &core;
	static std::vector<BinaryOperator> kw =
	{
		BinaryOperator("+", TYPEINFO(int), TYPEINFO(int), opcode::add_int_int),
		BinaryOperator("-", TYPEINFO(int), TYPEINFO(int), opcode::sub_int_int),
		BinaryOperator("*", TYPEINFO(int), TYPEINFO(int), opcode::mul_int_int),
		BinaryOperator("/", TYPEINFO(int), TYPEINFO(int), opcode::div_int_int),

		BinaryOperator("+", TYPEINFO(float), TYPEINFO(float), opcode::add_float_float),
		BinaryOperator("-", TYPEINFO(float), TYPEINFO(float), opcode::sub_float_float),
		BinaryOperator("*", TYPEINFO(float), TYPEINFO(float), opcode::mul_float_float),
		BinaryOperator("/", TYPEINFO(float), TYPEINFO(float), opcode::div_float_float),

		BinaryOperator(">", TYPEINFO(int), TYPEINFO(int), opcode::more_int_int, TYPEINFO(bool)),
		BinaryOperator("<", TYPEINFO(int), TYPEINFO(int), opcode::less_int_int, TYPEINFO(bool)),

		BinaryOperator("=", TYPEINFO(stringptr), TYPEINFO(stringptr), opcode::set_stringptr),


		//keyword2<int*, int, int>("=", 1, 0).setOpcode(opcode::set_32, 0, 1),
		//keyword2<stringptr, stringptr>("=", 1, 0).setOpcode(opcode::set_stringptr, 0, 1),

		//keyword2<void, void>("=", 2, 1).setOpcode((is_x64() ? opcode::set_64 : opcode::set_32), 0, 1),
		//keyword2<void, void>("=", 1, 0).setOpcode((is_x64() ? opcode::set_64 : opcode::set_32), 0, 1),

		//keyword2<float*, float, float>("=", 1, 0).setOpcode(opcode::set_32, 0, 1),

		//keyword2<float*, float, int>("=", 1, 0).setOpcode(opcode::int_to_float, 1, 1).setOpcode(opcode::set_32, 0, 1),

	};

	return kw;
}
