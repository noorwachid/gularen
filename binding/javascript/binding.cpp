#include <emscripten/bind.h>
#include <gularen/transpiler/html.h>


EMSCRIPTEN_BINDINGS(my_module)  {
	emscripten::function("transpile", &Gularen::Transpiler::HTML::transpile);
	emscripten::function("transpileSL", &Gularen::Transpiler::HTML::transpileSL);
}
