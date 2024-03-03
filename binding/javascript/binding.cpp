#include <Gularen/Backend/Html/Transpiler.h>
#include <emscripten/bind.h>
#include <string>

std::string transpile(const std::string& content) {
	Gularen::StringSlice input(content.data(), content.size());
	Gularen::Parser parser;

	Gularen::Html::Transpiler transpiler;
	Gularen::StringSlice output = transpiler.transpile(parser.parse(input));

	return std::string(output.pointer(), output.size());
}

EMSCRIPTEN_BINDINGS(gularen) {
	emscripten::function("transpile", transpile);
}
