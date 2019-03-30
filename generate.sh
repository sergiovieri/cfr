thread=$1
ngen=$2
nodeid=$3
echo "$thread $ngen $nodeid" | ./generate

cat out_*_$nodeid.txt >> ~/hs-precom/out_$nodeid.txt
rm out_*_$nodeid.txt
