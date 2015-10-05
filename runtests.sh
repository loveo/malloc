#!/bin/bash
#clean
echo "Cleaning up..."
rm -rf "tests"
mkdir "tests"

#compile and copy outfiles
echo "Compiling files..."
gcc stats.c -o tests/stats.out -lm
gcc tester.c brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=1 -O3
cp "a.out" "tests/t1.out"
gcc tester.c brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=2 -O3
cp "a.out" "tests/t2.out"
gcc tester.c brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=3 -O3
cp "a.out" "tests/t3.out"
gcc tester.c brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=4 -O3
cp "a.out" "tests/t4.out"
gcc real_malloc_tester.c -O3
cp "a.out" "tests/t5.out"

#run tests
cd "tests"
echo "Running best case with first fit"
./t1.out 1 1 | ./stats.out > "best_case_first_fit.txt"
echo "Running worst case with first fit"
./t1.out 2 1 | ./stats.out > "worst_case_first_fit.txt"
echo "Running reasonable large case with first fit"
./t1.out 3 1 | ./stats.out > "reasonable_large_case_first_fit.txt"
echo "Running reasonable small case with first fit"
./t1.out 4 1 | ./stats.out > "reasonable_small_case_first_fit.txt"



echo "Running best case with best fit"
./t2.out 1 2 | ./stats.out > "best_case_best_fit.txt"
echo "Running worst case with best fit"
./t2.out 2 2 | ./stats.out > "worst_case_best_fit.txt"
echo "Running reasonable large case with best fit"
./t2.out 3 2 | ./stats.out > "reasonable_large_case_best_fit.txt"
echo "Running reasonable small case with best fit"
./t2.out 4 2 | ./stats.out > "reasonable_small_case_best_fit.txt"



echo "Running best case with worst fit"
./t3.out 1 3 | ./stats.out > "best_case_worst_fit.txt"
echo "Running worst case with worst fit"
./t3.out 2 3 | ./stats.out > "worst_case_worst_fit.txt"
echo "Running reasonable large case with worst fit"
./t3.out 3 3 | ./stats.out > "reasonable_large_case_worst_fit.txt"
echo "Running reasonable small case with worst fit"
./t3.out 4 3 | ./stats.out > "reasonable_small_case_worst_fit.txt"



echo "Running best case with quick fit"
./t4.out 1 4 | ./stats.out > "best_case_quick_fit.txt"
echo "Running worst case with quick fit"
./t4.out 2 4 | ./stats.out > "worst_case_quick_fit.txt"
echo "Running reasonable large case with quick fit"
./t4.out 3 4 | ./stats.out > "reasonable_large_case_quick_fit.txt"
echo "Running reasonable small case with quick fit"
./t4.out 4 4 | ./stats.out > "reasonable_small_case_quick_fit.txt"



echo "Running all test cases with real malloc..."

echo "Running best case with first fit"
./t5.out 1 1 | ./stats.out > "real_malloc_best_case_first_fit.txt"
echo "Running worst case with first fit"
./t5.out 2 1 | ./stats.out > "real_malloc_worst_case_first_fit.txt"
echo "Running reasonable large case with first fit"
./t5.out 3 1 | ./stats.out > "real_malloc_reasonable_large_case_first_fit.txt"
echo "Running reasonable small case with first fit"
./t5.out 4 1 | ./stats.out > "real_malloc_reasonable_small_case_first_fit.txt"



echo "Running best case with best fit"
./t5.out 1 2 | ./stats.out > "real_malloc_best_case_best_fit.txt"
echo "Running worst case with best fit"
./t5.out 2 2 | ./stats.out > "real_malloc_worst_case_best_fit.txt"
echo "Running reasonable large case with best fit"
./t5.out 3 2 | ./stats.out > "real_malloc_reasonable_large_case_best_fit.txt"
echo "Running reasonable small case with best fit"
./t5.out 4 2 | ./stats.out > "real_malloc_reasonable_small_case_best_fit.txt"



echo "Running best case with worst fit"
./t5.out 1 3 | ./stats.out > "real_malloc_best_case_worst_fit.txt"
echo "Running worst case with worst fit"
./t5.out 2 3 | ./stats.out > "real_malloc_worst_case_worst_fit.txt"
echo "Running reasonable large case with worst fit"
./t5.out 3 3 | ./stats.out > "real_malloc_reasonable_large_case_worst_fit.txt"
echo "Running reasonable small case with worst fit"
./t5.out 4 3 | ./stats.out > "real_malloc_reasonable_small_case_worst_fit.txt"



echo "Running best case with quick fit"
./t5.out 1 4 | ./stats.out > "real_malloc_best_case_quick_fit.txt"
echo "Running worst case with quick fit"
./t5.out 2 4 | ./stats.out > "real_malloc_worst_case_quick_fit.txt"
echo "Running reasonable large case with quick fit"
./t5.out 3 4 | ./stats.out > "real_malloc_reasonable_large_case_quick_fit.txt"
echo "Running reasonable small case with quick fit"
./t5.out 4 4 | ./stats.out > "real_malloc_reasonable_small_case_quick_fit.txt"

#remove binary files
rm *.out
echo "Testing completed!"



