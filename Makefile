all: client.c aws.c serverA.c serverB.c serverC.c
	gcc -o serverA serverA.c
	gcc -o serverB serverB.c
	gcc -o serverC serverC.c
	gcc -o aws aws.c
	gcc -o client client.c

serverA:serverA.o
	./serverA
serverB:serverB.o
	./serverB
serverC:serverC.o
	./serverC
aws:aws.o
	./aws
