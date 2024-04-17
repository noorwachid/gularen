if [ ! -d 'build' ]; then
	mkdir build
fi

OS="`uname`"
case $OS in
	'Linux')
		gcc++ -o build/gularen -std=c++17 -I source cli/main.cpp
		;;

	'Darwin') 
		clang++ -o build/gularen -Wall -std=c++17 -I source cli/main.cpp
		;;

	*) 
		echo 'unsupported operating system'
		;;
esac


