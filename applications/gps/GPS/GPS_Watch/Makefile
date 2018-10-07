ARDUINO_CLI=arduino-cli
FQBN=arduino:avr:uno
PORT=/dev/cu.usbmodem14101

all:  compile upload

compile:
	$(ARDUINO_CLI) compile --fqbn $(FQBN) --warnings none src/

upload:
	$(ARDUINO_CLI) upload -p $(PORT) --fqbn $(FQBN) src/

clean:
	rm -f src/*.elf src/*.hex
