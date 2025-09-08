all:
	gcc -DWDC65c02 `pkg-config --cflags gtk+-3.0` -o c6502 src/*.c src/instr/*.c `pkg-config --libs gtk+-3.0`
	gcc -Wall ppa.c -o ppa

clean:
	rm -f c6502 ppa a.out
