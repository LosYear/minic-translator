#include "Translator.h"

Translator::Translator(std::istream & stream) : _lexicalAnalyzer(LexicalScanner(stream)), _currentLexem(0), _currentLabelId(0) {}

void Translator::printAtoms(std::ostream & stream) const
{
	for (unsigned int i = 0; i < _atoms.size(); ++i) {
		stream << (_atoms[i])->toString();

		if (i != _atoms.size() - 1) {
			stream << std::endl;
		}
	}
}

void Translator::generateAtom(std::unique_ptr<Atom> atom)
{
	_atoms.push_back(std::move(atom));
}

std::shared_ptr<LabelOperand> Translator::newLabel()
{
	return std::make_shared<LabelOperand>(_currentLabelId++);
}
