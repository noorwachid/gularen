OS="`uname`"
case $OS in
	'Linux')
		g++ -o binary/gularen -std=c++11 -I source cli/main.cpp -O2
		sudo cp binary/gularen /usr/local/bin/gularen
		;;

	'Darwin') 
		clang++ -o binary/gularen -std=c++11 -I source cli/main.cpp -O2
		sudo cp binary/gularen /usr/local/bin/gularen
		;;

	*) 
		echo 'unsupported operating system'
		;;
esac

