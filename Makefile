yash: main.o job.o signal.o token.o action.o
	gcc -o yash main.o job.o signal.o token.o action.o -lreadline
main.o: main.c
	gcc -c main.c
job.0: job.c
	gcc -c job.c
signal.o: signal.c
	gcc -c signal.c
token.o: token.c
	gcc -c token.c
action.o: action.c
	gcc -c action.c
clean:
	rm -f yash *.0
