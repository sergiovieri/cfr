
for i in {1..10} 
do
    ssh xcna$i "cd ~/cfr/ && tmux new -d -s sess 'bash generate.sh 10 100000 $i'"
done