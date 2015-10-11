#!/bin/bash
#clean
echo "Cleaning up..."
rm -rf "sanity_tests/results"
mkdir "sanity_tests/results"

#compile and copy outfiles
echo "Compiling files..."

cp "sanity_tests/tst.h" "tst.h"
cp "sanity_tests/tstalgorithms.c" "tstalgorithms.c"
cp "sanity_tests/tstextreme.c" "tstextreme.c"
cp "sanity_tests/tstmalloc.c" "tstmalloc.c"
cp "sanity_tests/tstmemory.c" "tstmemory.c"
cp "sanity_tests/tstmerge.c" "tstmerge.c"
cp "sanity_tests/tstrealloc.c" "tstrealloc.c"



gcc brk.h tst.h malloc.c malloc.h node.c node.h tstalgorithms.c -DSTRATEGY=1 -O3
cp "a.out" "sanity_tests/results/algo_1.out"
gcc tst.h brk.h malloc.c malloc.h node.c node.h tstalgorithms.c -DSTRATEGY=2 -O3
cp "a.out" "sanity_tests/results/algo_2.out"
gcc tst.h brk.h malloc.c malloc.h node.c node.h tstalgorithms.c -DSTRATEGY=3 -O3
cp "a.out" "sanity_tests/results/algo_3.out"
gcc tst.h brk.h malloc.c malloc.h node.c node.h tstalgorithms.c -DSTRATEGY=4 -O3
cp "a.out" "sanity_tests/results/algo_4.out"

gcc tst.h brk.h malloc.c malloc.h node.c node.h tstextreme.c -DSTRATEGY=1 -O3
cp "a.out" "sanity_tests/results/extreme_1.out"
gcc tst.h brk.h malloc.c malloc.h node.c node.h tstextreme.c -DSTRATEGY=2 -O3
cp "a.out" "sanity_tests/results/extreme_2.out"
gcc tst.h brk.h malloc.c malloc.h node.c node.h tstextreme.c -DSTRATEGY=3 -O3
cp "a.out" "sanity_tests/results/extreme_3.out"
gcc tst.h brk.h malloc.c malloc.h node.c node.h tstextreme.c -DSTRATEGY=4 -O3
cp "a.out" "sanity_tests/results/extreme_4.out"

gcc tst.h brk.h malloc.c malloc.h node.c node.h tstmalloc.c -DSTRATEGY=1 -O3
cp "a.out" "sanity_tests/results/malloc_1.out"
gcc tst.h brk.h malloc.c malloc.h node.c node.h tstmalloc.c -DSTRATEGY=2 -O3
cp "a.out" "sanity_tests/results/malloc_2.out"
gcc tst.h brk.h malloc.c malloc.h node.c node.h tstmalloc.c -DSTRATEGY=3 -O3
cp "a.out" "sanity_tests/results/malloc_3.out"
gcc tst.h brk.h malloc.c malloc.h node.c node.h tstmalloc.c -DSTRATEGY=4 -O3
cp "a.out" "sanity_tests/results/malloc_4.out"

gcc tstmemory.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=1 -O3
cp "a.out" "sanity_tests/results/memory_1.out"
gcc tstmemory.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=2 -O3
cp "a.out" "sanity_tests/results/memory_2.out"
gcc tstmemory.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=3 -O3
cp "a.out" "sanity_tests/results/memory_3.out"
gcc tstmemory.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=4 -O3
cp "a.out" "sanity_tests/results/memory_4.out"

gcc tstmerge.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=1 -O3
cp "a.out" "sanity_tests/results/merge_1.out"
gcc tstmerge.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=2 -O3
cp "a.out" "sanity_tests/results/merge_2.out"
gcc tstmerge.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=3 -O3
cp "a.out" "sanity_tests/results/merge_3.out"
gcc tstmerge.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=4 -O3
cp "a.out" "sanity_tests/results/merge_4.out"

gcc tstrealloc.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=1 -O3
cp "a.out" "sanity_tests/results/realloc_1.out"
gcc tstrealloc.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=2 -O3
cp "a.out" "sanity_tests/results/realloc_2.out"
gcc tstrealloc.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=3 -O3
cp "a.out" "sanity_tests/results/realloc_3.out"
gcc tstrealloc.c tst.h brk.h malloc.c malloc.h node.c node.h -DSTRATEGY=4 -O3
cp "a.out" "sanity_tests/results/realloc_4.out"


rm "tst.h"
rm "tstalgorithms.c"
rm "tstextreme.c"
rm "tstmalloc.c"
rm "tstmemory.c"
rm "tstmerge.c"
rm "tstrealloc.c"

#run tests
cd "sanity_tests/results"

./algo_1.out &> "algo_1.test"
./algo_2.out &> "algo_2.test"
./algo_3.out &> "algo_3.test"
./algo_4.out &> "algo_4.test"

./extreme_1.out &> "extreme_1.test"
./extreme_2.out &> "extreme_2.test"
./extreme_3.out &> "extreme_3.test"
./extreme_4.out &> "extreme_4.test"

./malloc_1.out &> "malloc_1.test"
./malloc_2.out &> "malloc_2.test"
./malloc_3.out &> "malloc_3.test"
./malloc_4.out &> "malloc_4.test"

./memory_1.out &> "memory_1.test"
./memory_2.out &> "memory_2.test"
./memory_3.out &> "memory_3.test"
./memory_4.out &> "memory_4.test"

./merge_1.out &> "merge_1.test"
./merge_2.out &> "merge_2.test"
./merge_3.out &> "merge_3.test"
./merge_4.out &> "merge_4.test"

./realloc_1.out &> "realloc_1.test"
./realloc_2.out &> "realloc_2.test"
./realloc_3.out &> "realloc_3.test"
./realloc_4.out &> "realloc_4.test"

cat *.test >> test_results.txt

#remove binary files
rm *.out
echo "Testing completed!"
