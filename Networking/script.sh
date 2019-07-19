#mv log* data/400

for ((i = 0; i<=5; i++))
do
	a=$(( 5*i + 5))
	timeout 10s ./client $IP 80 "$a"
	mkdir -p data/"$a"
	mv log* data/"$a"
done
