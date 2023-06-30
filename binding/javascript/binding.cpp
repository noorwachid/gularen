#include <emscripten/bind.h>
#include <Gularen/Transpiler/HTML.h>


EMSCRIPTEN_BINDINGS(my_module) {
	emscripten::function("transpile", &Gularen::Transpiler::HTML::transpile);
}
