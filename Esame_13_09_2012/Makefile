eseguibile: main.o procedure.o monitor.o
	gcc -o eseguibile  main.o procedure.o monitor.o

main.o: main.c header.h
	gcc -o main.o -c  main.c

procedure.o: procedure.c header.h
	gcc -o procedure.o -c   procedure.c

monitor.o: monitor_signal_continue.c monitor_signal_continue.h
	gcc -o monitor.o -c   monitor_signal_continue.c

clean:
	rm -f eseguibile
	rm -f *.o
