all:
	gcc -DWDC65c02 `pkg-config --cflags gtk+-3.0` -o c6502 *.c instr/*.c `pkg-config --libs gtk+-3.0`
clean:
	rm c6502
