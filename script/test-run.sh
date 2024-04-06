for path in test/core/*.gr
do
	./build/gularen-test $path 0 > /tmp/test-0.gr
	expected=$(./build/gularen-test $path 1)
	result=$(./build/gularen to json /tmp/test-0.gr)

	if [ "$result" = "$expected" ]
	then
		echo "PASS $path"
	else
		echo "FAIL $path"
		echo
		echo 'SOURCE:'
		cat /tmp/test-0.gr
		echo
		echo 'EXPECTED:'
		echo $expected
		echo
		echo 'RESULT:'
		echo $result
		echo

		if hash jq 2>/dev/null; then
			echo $result | jq
			echo
		fi

		exit
	fi
done
