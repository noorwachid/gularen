#include "ConsoleInterface.hpp"

int main(int argc, char** argv) {
	ConsoleInterface consoleInterface;
	consoleInterface.onEntry(argc, argv);

	return 0;
}
