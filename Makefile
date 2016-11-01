all:
	gcc `pkg-config --cflags gtk+-3.0` -o c6502 *.c `pkg-config --libs gtk+-3.0`
clean:
	rm c6502
