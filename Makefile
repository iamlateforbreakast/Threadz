all: threadz socketz serverz

threadz: threadz.c
	gcc -O0 -g threadz.c -o threadz

socketz: socketz.c
	gcc -O0 -g  $< -o $@

serverz: serverz.c
	gcc -O0 -g $< -o $@

clean:
	rm -rf threadz.o threadz

.PHONY:
	clean
