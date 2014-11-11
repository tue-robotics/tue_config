#! /bin/bash
rosrun tue_config2 test_tue_config2 > /tmp/test-output.txt
diff=$(diff /tmp/test-output.txt `rospack find tue_config2`/test/test-output.txt)

if [ -z "$diff" ]
then
	echo "OK"
else
	echo "DIFFERENCE!"
	echo ""
	echo "$diff" | colordiff
fi
