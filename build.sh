if [ ! -d "bin" ]; then
    mkdir bin
else
	rm bin/*
fi
g++ part2_LukeGrundy_CameronGreer.cpp -o bin/part2 -lrt
./bin/part2 rubric.txt ./exams 2 > outputs/output.txt 2>&1 &