#! /bin/bash
rosrun tue_config test_tue_config > /tmp/test-output.txt
diff=$(diff /tmp/test-output.txt `rospack find tue_config`/test/test-output.txt)

if [ -z "$diff" ]
then
	echo "OK"
else
	echo "DIFFERENCE!"
	echo ""
	echo "$diff" | colordiff
fi
