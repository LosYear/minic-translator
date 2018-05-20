#include "Atom.h"

BinaryOpAtom::BinaryOpAtom(const std::string& name, const std::shared_ptr<RValue> left, const std::shared_ptr<RValue> right, const std::shared_ptr<MemoryOperand> result) :
	_name(name), _left(left), _right(right), _result(result)
{
}

std::string BinaryOpAtom::toString() const
{
	return "(" + _name + ", " + ((_left != nullptr) ? _left->toString() : "") + ", " + ((_right != nullptr) ? _right->toString() : "") + ", " + _result->toString() + ")";
}

UnaryOpAtom::UnaryOpAtom(const std::string& name, const std::shared_ptr<RValue> operand, const std::shared_ptr<MemoryOperand> result) :
	_name(name), _operand(operand), _result(result)
{
}

std::string UnaryOpAtom::toString() const
{
	return "(" + _name + ", " + _operand->toString() + ", , " + _result->toString() + ")";
}

ConditionalJumpAtom::ConditionalJumpAtom(const std::string& cond, const std::shared_ptr<RValue> left, const std::shared_ptr<RValue> right, const std::shared_ptr<LabelOperand> label) :
	_condition(cond), _left(left), _right(right), _label(label)
{
}

std::string ConditionalJumpAtom::toString() const
{
	return "(" + _condition + ", " + _left->toString() + ", " + _right->toString() + ", " + _label->toString() + ")";
}

OutAtom::OutAtom(const std::shared_ptr<Operand> value) : _value(value)
{
}

std::string OutAtom::toString() const
{
	return "(OUT, , , " + _value->toString() + ")";
}

InAtom::InAtom(const std::shared_ptr<MemoryOperand> result) : _result(result)
{
}

std::string InAtom::toString() const
{
	return "(IN, , , " + _result->toString() + ")";
}

LabelAtom::LabelAtom(const std::shared_ptr<LabelOperand> label) : _label(label)
{
}

std::string LabelAtom::toString() const
{
	return "(LBL, , , " + _label->toString() + ")";
}

JumpAtom::JumpAtom(const std::shared_ptr<LabelOperand> label) : _label(label)
{
}

std::string JumpAtom::toString() const
{
	return "(JMP, , , " + _label->toString() + ")";
}

CallAtom::CallAtom(const std::shared_ptr<MemoryOperand> function, const std::shared_ptr<MemoryOperand> result) 
	: _function(function), _result(result)
{
}

std::string CallAtom::toString() const
{
	return "(CALL, " + _function->toString() + ", , " + _result->toString() + ")";
}

RetAtom::RetAtom(const std::shared_ptr<RValue> value) : _value(value)
{
}

std::string RetAtom::toString() const
{
	return "(RET, , , " + _value->toString() + ")";
}

ParamAtom::ParamAtom(const std::shared_ptr<RValue> value) : _value(value)
{
}

std::string ParamAtom::toString() const
{
	return "(PARAM, , , " + _value->toString() + ")";
}
