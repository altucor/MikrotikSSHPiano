# Mikrotik SSH Piano

That program allows you play on mikrotik beeper like on piano or one voice synth. All is works over SSH session.

## Requirements
That project uses libssh for windows. So you need dll's from libssh. Precompiled dll's are available in Debug folder, also u can download Debug folder and enjoy using it!

## Arguments 
* First argument - username
* Second argument - router ip
* Third - ssh port on router
* Example: **MikrotikSSHPiano.exe admin 192.168.0.1 22** 

## Keyboard markdown
Keyboard markdown same as in FL Studio. In FL you can use it, when you turn on feature "Typing keyboard to piano keyboard".

		+--------------------------------------------------------------------------+
		|  Octave N+1                                                              |
		|       +---+       +---+             +---+       +---+       +---+        |
		| +---+ | 2 | +---+ | 3 | +---+ +---+ | 5 | +---+ | 6 | +---+ | 7 | +---+  |
		| ||Q|| +---+ ||W|| +---+ ||E|| ||R|| +---+ ||T|| +---+ ||Y|| +---+ ||U||  |
		| +---+       +---+       +---+ +---+       +---+       +---+       +---+  |
		|                                                                          |
		+--------------------------------------------------------------------------+
		|  Octave N                                                                |
		|       +---+       +---+             +---+       +---+       +---+        |
		| +---+ | S | +---+ | D | +---+ +---+ | G | +---+ | H | +---+ | J | +---+  |
		| ||Z|| +---+ ||X|| +---+ ||C|| ||V|| +---+ ||B|| +---+ ||N|| +---+ ||M||  |
		| +---+       +---+       +---+ +---+       +---+       +---+       +---+  |
		+--------------------------------------------------------------------------+