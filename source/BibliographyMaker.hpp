#pragma once

#include "Parser.hpp"

namespace BibliographyMaker {

	Array<Node*> mention(FuncNode* func, ResourceNode* res);

	Array<Node*> make(FuncNode* func);
}
