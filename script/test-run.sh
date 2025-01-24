for path in test/core/*.gr
do
	./build/gularen-test $path 0 > /tmp/gularen-source.gr
	./build/gularen-test $path 1 > /tmp/gularen-json.gr
	./build/gularen to json /tmp/gularen-source.gr > /tmp/gularen-json-compiled.gr
	expected=$(cat /tmp/gularen-json.gr)
	result=$(cat /tmp/gularen-json-compiled.gr)

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
