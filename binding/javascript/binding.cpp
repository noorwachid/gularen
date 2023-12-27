#include <emscripten/bind.h>
#include <Gularen/transpile.h>

EMSCRIPTEN_BINDINGS(gularen) {
	emscripten::function("transpile", &Gularen::transpile);
	emscripten::function("transpileSyncLine", &Gularen::transpileSyncLine);
}
