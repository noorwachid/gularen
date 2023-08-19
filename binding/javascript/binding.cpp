#include <emscripten/bind.h>
#include <gularen/transpiler/html.h>


EMSCRIPTEN_BINDINGS(my_module) 
{
	emscripten::function("transpile", &Gularen::Transpiler::HTML::Transpile);

	emscripten::function("transpileLS", &Gularen::Transpiler::HTML::TranspileLS);
}
