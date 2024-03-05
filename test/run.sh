for path in test/core/*.gr
do
	result=$(./binary/gularen-test parse $path)
	expected=$(./binary/gularen-test expect $path)

	if [ "$result" = "$expected" ]
	then
		echo "PASS $path"
	else
		echo "FAIL $path"
	fi
done
