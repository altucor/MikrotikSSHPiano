#include <iostream>
#include <string>

#include "SshWrapper.h"
#include "KeyToNote.h"

void splahsScreen(int octave) {

	printf(
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
		"| |               Exit               |      | + |    %d    | - |            |\n"
		"| +----------------------------------+      +-+-+         +-+-+            |\n"
		"|                                                                          |\n"
		"|                                         Octave Up    Octave Down         |\n"
		"|                                                                          |\n"
		"+--------------------------------------------------------------------------+\n", octave);
}

void help()
{
	std::cout << "First argument *username*\n";
	std::cout << "Second argument *router ip*\n";
	std::cout << "Third argument *port number*\n\n";

	std::cout << "Example: admin 192.168.0.1 22\n";
}

CHAR getch() {
	DWORD mode, cc;
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

	if (h == NULL) {
		return 0; // console not found
	}

	GetConsoleMode(h, &mode);
	SetConsoleMode(h, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
	TCHAR c = 0;
	ReadConsole(h, &c, 1, &cc, NULL);
	SetConsoleMode(h, mode);
	return c;
}

int main(int argc, char * argv[])
{
	if (argc < 4) {
		help();
		exit(-1);
	}

	std::string user = argv[1];
	std::string ip = argv[2];
	unsigned int port = std::stoi(std::string(argv[3]));

	SshWrapper connection(user, ip, port);
	for (auto i = 0; i < 14; i++) {
		connection.playNote(i*100.0, 0.1);
		Sleep(50);
	}

	KeyToNote keyToNote;

	splahsScreen(keyToNote.getCurOctave());

	int key = 0;
	while (true) {
		key = getch();
		if (key == 32) exit(-1); // space btn
		
		float freq = keyToNote.keyToFreq(key);
		if (freq > 20.0 && freq < 20000.0) {
			system("cls");
			splahsScreen(keyToNote.getCurOctave());
			connection.playNote(freq, 0.5);
		}
		
		if (key == 61 || key == 43) { // Plus buttons
			keyToNote.octaveUp();
			system("cls");
			splahsScreen(keyToNote.getCurOctave());
			connection.playNote(freq, 0.5);
		}

		if (key == 45) { // Minus buttons
			keyToNote.octaveDown(); 
			system("cls");
			splahsScreen(keyToNote.getCurOctave());
			connection.playNote(freq, 0.5);
		}
	}

    return 0;
}

