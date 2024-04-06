sh script/build.sh

OS="`uname`"
case $OS in
	'Linux')
		g++ -o build/gularen-test -std=c++17 -I source test/main.cpp
		;;

	'Darwin') 
		clang++ -o build/gularen-test -std=c++17 -I source test/main.cpp
		;;

	*) 
		echo 'unsupported operating system'
		;;
esac


