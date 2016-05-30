outfile=${1%mis}out
echo test case: $1
time ./main $1 > $outfile
tail -1 $outfile
echo
