#include <Gularen/Transpiler/HTML.h>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
	if (argc <= 1) {
		std::cout << "please specify the input file\n";
		return 0;
	}

	if (std::filesystem::is_directory(argv[1])) {
		std::cout << "please specify the input file, make sure its not a directory\n";
		return 0;
	}

	std::ifstream file(argv[1]);
	std::string content;
	file.seekg(0, std::ios::end);   
	content.reserve(file.tellg());
	file.seekg(0, std::ios::beg);
	content.assign(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);

	std::string out = Gularen::Transpiler::HTML::transpile(content);
	std::cout << out << '\n';

	return 0;
}
