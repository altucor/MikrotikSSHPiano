#include <iostream>
#include <string>

#include <conio.h>

#include "Mikrotik.hpp"
#include "KeyToNote.hpp"

void splashScreen(int octave) {

	std::cout <<
		"  __  __ _ _              _   _ _    _____ _                   \n"
		" |  \\/  (_) |            | | (_) |  |  __ (_)                  \n"
		" | \\  / |_| | ___ __ ___ | |_ _| | _| |__) |  __ _ _ __   ___  \n"
		" | |\\/| | | |/ / '__/ _ \\| __| | |/ /  ___/ |/ _` | '_ \\ / _ \\ \n"
		" | |  | | |   <| | | (_) | |_| |   <| |   | | (_| | | | | (_) |\n"
		" |_|  |_|_|_|\\_\\_|  \\___/ \\__|_|_|\\_\\_|   |_|\\__,_|_| |_|\\___/ \n\n"
		"+--------------------------------------------------------------------------+\n"
		"|  Octave N+1                                                              |\n"
		"|       +---+       +---+             +---+       +---+       +---+        |\n"
		"| +---+ | 2 | +---+ | 3 | +---+ +---+ | 5 | +---+ | 6 | +---+ | 7 | +---+  |\n"
		"| ||Q|| +---+ ||W|| +---+ ||E|| ||R|| +---+ ||T|| +---+ ||Y|| +---+ ||U||  |\n"
		"| +---+       +---+       +---+ +---+       +---+       +---+       +---+  |\n"
		"|                                                                          |\n"
		"+--------------------------------------------------------------------------+\n"
		"|  Octave N                                                                |\n"
		"|       +---+       +---+             +---+       +---+       +---+        |\n"
		"| +---+ | S | +---+ | D | +---+ +---+ | G | +---+ | H | +---+ | J | +---+  |\n"
		"| ||Z|| +---+ ||X|| +---+ ||C|| ||V|| +---+ ||B|| +---+ ||N|| +---+ ||M||  |\n"
		"| +---+       +---+       +---+ +---+       +---+       +---+       +---+  |\n"
		"+--------------------------------------------------------------------------+\n"
		"|                                                                          |\n"
		"| +----------------------------------+      +-+-+ Current +-+-+            |\n"
		"| |               Exit               |      | + |    " << octave <<"    | - |            |\n"
		"| +----------------------------------+      +-+-+         +-+-+            |\n"
		"|                                                                          |\n"
		"|                                         Octave Up    Octave Down         |\n"
		"|                                                                          |\n"
		"+--------------------------------------------------------------------------+\n";
}

void help()
{
	std::cout << "1) argument *router ip*\n";
	std::cout << "2) argument *port number*\n";
	std::cout << "3) argument *username*\n";
	std::cout << "4) argument *password*\n";

	std::cout << "Example: 192.168.0.1 22 admin password\n";
}

uint32_t getSymbol()
{
	return _getch();
}

int main(int argc, char * argv[])
{
	if (argc < 4) {
		help();
		exit(-1);
	}

	
	std::string ip = argv[1];
	unsigned int port = std::stoi(std::string(argv[2]));
	std::string user = argv[3];
	std::string password = argv[4];

	Mikrotik mikrotik(ip, port, user, password);
	for (auto i = 0; i < 14; i++) {
		mikrotik.playNote(i*100.0, 0.1);
		Sleep(50);
	}

	KeyToNote keyToNote;

	splashScreen(keyToNote.getCurOctave());

	int key = 0;
	while (true) {
		key = getSymbol();
		if (key == 32) exit(-1); // space btn
		
		float freq = keyToNote.keyToFreq(key);
		if (freq > 20.0 && freq < 20000.0) {
			splashScreen(keyToNote.getCurOctave());
			mikrotik.playNote(freq, 0.5);
		}
		
		if (key == 61 || key == 43) { // Plus buttons
			keyToNote.octaveUp();
			splashScreen(keyToNote.getCurOctave());
			mikrotik.playNote(freq, 0.5);
		}

		if (key == 45) { // Minus buttons
			keyToNote.octaveDown(); 
			splashScreen(keyToNote.getCurOctave());
			mikrotik.playNote(freq, 0.5);
		}
	}

	return 0;
}

