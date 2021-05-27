hepsi: derle calistir

derle:
	gcc -I ./include/ -o ./lib/banka.o -c ./src/banka.c
	gcc -I ./include/ -o ./lib/hisse.o -c ./src/hisse.c
	gcc -I ./include/ -o ./lib/portfoy.o -c ./src/portfoy.c
	gcc -I ./include/ -o ./lib/emir.o -c ./src/emir.c
	gcc -I ./include/ -o ./bin/Test ./lib/banka.o ./lib/hisse.o ./lib/portfoy.o ./lib/emir.o ./src/test.c

calistir:
	./bin/Test.exe
