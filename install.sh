if [ ! -d 'build' ]; then
	mkdir build
fi

OS="`uname`"
case $OS in
	'Linux')
		g++ -o build/gularen -std=c++17 -I source cli/main.cpp -O2
		sudo cp build/gularen /usr/local/bin/gularen
		;;

	'Darwin') 
		clang++ -o build/gularen -std=c++17 -I source cli/main.cpp -O2
		sudo cp build/gularen /usr/local/bin/gularen
		;;

	*) 
		echo 'unsupported operating system'
		;;
esac

