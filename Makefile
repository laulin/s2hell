compile: setup
	gcc -Wall -Dx86 sources/s2hell.c sources/s2hell_tokenizer.c sources/s2hell_std.c sources/s2hell_commands.c sources/main.x86.c sources/s2hell_command_list.x86.c -o output/s2hell

test: build_so
	gcc -E sources/s2hell.h > output/s2hell.h.preprocessed
	python3 -m unittest tests/test_tokenizer.py
	python3 -m unittest tests/test_std.py
	python3 -m unittest tests/test_commands.py

build_so: setup
	gcc -Wall -Dx86 -shared -o output/s2hell.so -fPIC sources/s2hell.c sources/s2hell_tokenizer.c sources/s2hell_std.c sources/s2hell_commands.c sources/s2hell_command_list.x86.c

build_arduino:
	# need apt install gcc-avr binutils-avr avr-libc avrdude
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/main.arduino.o sources/main.arduino.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/s2hell.o sources/s2hell.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/s2hell_tokenizer.o sources/s2hell_tokenizer.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/s2hell_std.o sources/s2hell_std.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/s2hell_commands.o sources/s2hell_commands.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/s2hell_command_list.arduino.o sources/s2hell_command_list.arduino.c
	avr-gcc -mmcu=atmega328p output/main.arduino.o output/s2hell.o output/s2hell_tokenizer.o output/s2hell_std.o output/s2hell_commands.o output/s2hell_command_list.arduino.o -o output/s2hell.arduino.bin 
	avr-objcopy -O ihex -R .eeprom output/s2hell.arduino.bin output/s2hell.arduino.hex
	@echo -n "program size : " ; du -h output/s2hell.arduino.hex

upload:
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:output/s2hell.arduino.hex

setup:
	mkdir -p output/

test_uart:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o output/uart.arduino.o sources/uart.arduino.c
	avr-gcc -mmcu=atmega328p output/uart.arduino.o -o output/uart.arduino.bin 
	avr-objcopy -O ihex -R .eeprom output/uart.arduino.bin output/uart.arduino.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:output/uart.arduino.hex
