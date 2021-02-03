# chip-8-emulator
A very simple Chip-8 emulator and disassembler

Instructions

# Disassembler
```shell
g++ main.cpp -o disas
./disas ROMFILE
```
# Emulator
```shell
g++ -lSDL2 main.cpp -o emu
./emu ROMFILE clocktime(ms)
```
If clock time isnìt specified it defaults to 16, with clocktime==420 there is no delay between operations

Sound is broken, can't test it on my machine as SDl doesn't find the audio cards