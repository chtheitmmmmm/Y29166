cd test
make
if [ -n $1 ]; then
	./main $1
else
	./main
fi
