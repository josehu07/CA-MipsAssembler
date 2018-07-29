make
if [ ! -d "log/my" ];then
	mkdir log/my
fi
if [ ! -d "out/my" ];then
	mkdir out/my
fi
echo
echo "+-> Assembling simple..."
./assembler input/simple.s out/my/simple.int out/my/simple.out
echo
echo "+-> Assembling imm..."
./assembler input/imm.s out/my/imm.int out/my/imm.out
echo
echo "+-> Assembling comments..."
./assembler input/comments.s out/my/comments.int out/my/comments.out
echo
echo "+-> Assembling labels..."
./assembler input/labels.s out/my/labels.int out/my/labels.out
echo
echo "+-> Assembling pseudo..."
./assembler input/pseudo.s out/my/pseudo.int out/my/pseudo.out
echo
echo "+-> Assembling combined..."
./assembler input/combined.s out/my/combined.int out/my/combined.out
echo
echo "+-> Assembling p1_errors..."
./assembler -p1 input/p1_errors.s out/my/p1_errors.int -log log/my/p1_errors.txt
echo
echo "+-> Assembling p2_errors..."
./assembler input/p2_errors.s out/my/p2_errors.int out/my/p2_errors.out -log log/my/p2_errors.txt
rm out/my/p1_errors.int out/my/p2_errors.int out/my/p2_errors.out
echo
echo ">-< Diff .int and .out files ^-^"
diff out/my out/ref
echo
echo ">-< Diff log files ^-^"
diff log/my log/ref
echo