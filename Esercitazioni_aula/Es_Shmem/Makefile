all: start

start: read_shmem_str.o main_es_str.o
	gcc -o main_es_str_ex main_es_str.o
	gcc -o read_shmem_str_ex read_shmem_str.o

main_es_str.o: main_es_str.c read_shmem_str.c
	gcc -c main_es_str.c
	gcc -c read_shmem_str.c

clean:
	rm -rf *.o
	rm -rf main_es_str_ex
	rm -rf read_shmem_str_ex
