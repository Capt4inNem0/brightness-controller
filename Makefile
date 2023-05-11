CC=g++

make:
	$(CC) -o brightnessadjustment main.cpp -std=c++11

clean:
	rm brightnessadjustment

install:
	make clean && make && sudo cp brightnessadjustment /usr/bin/brightnessadjustment

uninstall:
	sudo rm /usr/bin/brightnessadjustment