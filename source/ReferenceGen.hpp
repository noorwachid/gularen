#pragma once

#include "Parser.hpp"

Array<Node*> mention(ResourceNode* resource, FuncNode* func);

Array<Node*> genReference(FuncNode* func);
