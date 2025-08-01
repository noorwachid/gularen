#pragma once

#include "String.hpp"

String readFile(String const& path);

void writeFile(String const& path, String const& content);
