all: compile build_so build_arduino_a

compile: setup
	gcc -Wall -std=c11 -Dx86 sources/s2hell.c sources/s2hell_tokenizer.c sources/s2hell_terminal.c sources/s2hell_std.c sources/s2hell_commands.c sources/main.x86.c sources/s2hell_command_list.x86.c -o output/x86/s2hell

test: build_so
	gcc -E sources/s2hell.h > output/s2hell.h.preprocessed
	python3 -m unittest tests/test_tokenizer.py
	python3 -m unittest tests/test_std.py
	python3 -m unittest tests/test_commands.py
	python3 -m unittest tests/test_terminal.py

build_so: setup
	gcc -Wall -std=c11 -Dx86 -shared -o output/x86/s2hell.so -fPIC sources/s2hell.c sources/s2hell_tokenizer.c sources/s2hell_terminal.c sources/s2hell_std.c sources/s2hell_commands.c sources/s2hell_command_list.x86.c

build_arduino_a:
	avr-gcc -Os -std=c11 -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/arduino/main.arduino.o sources/main.arduino.c
	avr-gcc -Os -std=c11 -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/arduino/s2hell.o sources/s2hell.c
	avr-gcc -Os -std=c11 -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/arduino/s2hell_tokenizer.o sources/s2hell_tokenizer.c
	avr-gcc -Os -std=c11 -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/arduino/s2hell_terminal.o sources/s2hell_terminal.c
	avr-gcc -Os -std=c11 -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/arduino/s2hell_std.o sources/s2hell_std.c
	avr-gcc -Os -std=c11 -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/arduino/s2hell_commands.o sources/s2hell_commands.c
	avr-gcc -Os -std=c11 -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/arduino/s2hell_command_list.arduino.o sources/s2hell_command_list.arduino.c
	avr-ar -r "output/arduino/libs2hell.a" output/arduino/s2hell*.o

build_arduino: build_arduino_a
	# need apt install gcc-avr binutils-avr avr-libc avrdude
	avr-gcc -Os -std=c11 -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/arduino/main.o sources/main.arduino.c
	avr-gcc -mmcu=atmega328p -L"$(shell pwd)/output/arduino" -o output/arduino/s2hell.bin output/arduino/main.o -ls2hell
	avr-objcopy -O ihex -R .eeprom output/arduino/s2hell.bin output/arduino/s2hell.hex
	@echo -n "program size : " ; du -h output/arduino/s2hell.hex

upload:
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:output/arduino/s2hell.hex

setup:
	mkdir -p output/
	mkdir -p output/x86
	mkdir -p output/arduino

clean:
	rm output/arduino/*
	rm output/x86/*

example_uart: build_arduino_a
	avr-gcc -Os -std=c11 -DF_CPU=16000000UL -mmcu=atmega328p -I"$(shell pwd)/sources" -c -o output/arduino/uart.o examples/uart.arduino.c
	avr-gcc -mmcu=atmega328p -L"$(shell pwd)/output/arduino" -o output/arduino/uart.bin output/arduino/uart.o -ls2hell
	avr-objcopy -O ihex -R .eeprom output/arduino/uart.bin output/arduino/uart.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:output/arduino/uart.hex
