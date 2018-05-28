#include "Atom.h"

BinaryOpAtom::BinaryOpAtom(const std::string& name, const std::shared_ptr<RValue> left, const std::shared_ptr<RValue> right, const std::shared_ptr<MemoryOperand> result) :
	_name(name), _left(left), _right(right), _result(result)
{
}

std::string BinaryOpAtom::toString() const
{
	return "(" + _name + ", " + ((_left != nullptr) ? _left->toString() : "") + ", " + ((_right != nullptr) ? _right->toString() : "") + ", " + _result->toString() + ")";
}

void BinaryOpAtom::generate(std::ostream & stream) const
{
	stream << "; " << toString() << std::endl;

	_right->load(stream);
	stream << "MOV B, A" << std::endl;
	_left->load(stream);

	_generateOperation(stream);

	_result->save(stream);

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

void ConditionalJumpAtom::generate(std::ostream & stream) const
{
	stream << "; " << toString() << std::endl;

	_right->load(stream);
	stream << "MOV B, A" << std::endl;
	_left->load(stream);

	stream << "CMP B" << std::endl;

	_generateOperation(stream);
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

void LabelAtom::generate(std::ostream & stream) const
{
	stream << "LBL" << _label->id() << ": ";
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

void SimpleBinaryOpAtom::_generateOperation(std::ostream & stream) const
{
	std::string name;

	if (_name == "OR") {
		name = "ORA";
	}
	else if (_name == "AND") {
		name = "ANA";
	}
	else if (_name == "SUB" || _name == "ADD") {
		name = _name;
	}
	else {
		name = "ERROR: UNKNOWN";
	}

	stream << name << " B" << std::endl;

}

void FnBinaryOpAtom::_generateOperation(std::ostream & stream) const
{
	if (_name == "MUL") {
		stream << "MOV C, A" << std::endl;
		stream << "MOV D, B" << std::endl;
		stream << "CALL @MUL" << std::endl;
		stream << "MOV A, C" << std::endl;
	}
	else {
		stream << "ERROR: UNKNOWN " << _name << std::endl;
	}
}

void SimpleConditionalJumpAtom::_generateOperation(std::ostream & stream) const
{
	if (_condition == "EQ") {
		stream << "JZ LBL" << _label->id() << std::endl;
	}
	else if (_condition == "NE") {
		stream << "JNZ LBL" << _label->id() << std::endl;
	}
	else if (_condition == "GT") {
		stream << "JP LBL" << _label->id() << std::endl;
	}
	else if (_condition == "LT") {
		stream << "JM LBL" << _label->id() << std::endl;
	}
	else {
		stream << "ERROR: UNKNOWN " << _condition;
	}
}

void ComplexConditinalJumpAtom::_generateOperation(std::ostream & stream) const
{
	if (_condition == "LE") {
		stream << "JZ LBL" << _label->id() << std::endl;
		stream << "JM LBL" << _label->id() << std::endl;
	}
	else {
		stream << "ERROR: UNKOWN " << _condition;
	}
}
