#pragma once
#include <string>
#include <memory>
#include "..\Operand\Operand.h"

// Base class for all atoms
class Atom {
public:
	virtual std::string toString() const = 0;
};


// Atom for all binary operations
class BinaryOpAtom : public Atom {
public:
	BinaryOpAtom(const std::string& name, const std::shared_ptr<RValue> left,
		const std::shared_ptr<RValue> right, const std::shared_ptr<MemoryOperand> result);
	std::string toString() const;

private:
	// Operation name, e.g. ADD
	const std::string _name;

	const std::shared_ptr<RValue> _left;
	const std::shared_ptr<RValue> _right;

	const std::shared_ptr<MemoryOperand> _result;
};


// Atom for all unary operations
class UnaryOpAtom : public Atom {
public:
	UnaryOpAtom(const std::string& name, const std::shared_ptr<RValue> operand,
		const std::shared_ptr<MemoryOperand> result);
	std::string toString() const;

private:
	// Operation name, e.g. NEG
	const std::string _name;

	const std::shared_ptr<RValue> _operand;
	const std::shared_ptr<MemoryOperand> _result;
};


// Atom for conditional jumps, e.g. EQ, GE, ...
class ConditionalJumpAtom : public Atom {
public:
	ConditionalJumpAtom(const std::string& cond, const std::shared_ptr<RValue> left,
		const std::shared_ptr<RValue> right, const std::shared_ptr<LabelOperand> label);
	std::string toString() const;

private:
	// e.g. EQ
	const std::string _condition;

	const std::shared_ptr<RValue> _left;
	const std::shared_ptr<RValue> _right;

	const std::shared_ptr<LabelOperand> _label;
};


class OutAtom : public Atom {
public:
	OutAtom(const std::shared_ptr<Operand> value);
	std::string toString() const;

private:
	const std::shared_ptr<Operand> _value;
};


class InAtom : public Atom {
public:
	InAtom(const std::shared_ptr<MemoryOperand> result);
	std::string toString() const;

private:
	const std::shared_ptr<MemoryOperand> _result;
};


// Atom for labeling
class LabelAtom : public Atom {
public:
	LabelAtom(const std::shared_ptr<LabelOperand> label);
	std::string toString() const;

private:
	const std::shared_ptr<LabelOperand> _label;
};


// Unconditional jump atom
class JumpAtom : public Atom {
public:
	JumpAtom(const std::shared_ptr<LabelOperand> label);
	std::string toString() const;

private:
	const std::shared_ptr<LabelOperand> _label;
};