# chip-8-emulator
A very simple Chip-8 emulator and disassembler

# Dependencies
gcc and SDL2, install them through your distro's package manager
If you are on windows it should be fine, haven't tested it tho.

# Instructions

# Disassembler
```shell
g++ main.cpp -o disas
./disas ROMFILE
```
Disassembler syntax is an Assemlby language with AT&T syntax made by me as there is no offical Chip-8 assembly language

# Emulator
```shell
g++ main.cpp -lSDL2 -lSDL2_mixer -o emu
./emu ROMFILE clocktime(ms)
```

Clocktime indicates how many milliseconds pass between 2 cycles. In 1 cycle 8 instructions gets executed, giving us approximately 500 operations per second.
If clock time isnìt specified it defaults to 16, with clocktime==420 there is no delay between operations

