#include <iostream>
#include <stdlib.h> 
#include <cstring>
#include <time.h>
#include <chrono>
#include "Platform.cpp"
#include <bitset>
using namespace std;

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

const unsigned char fontset[0x10*5] = //manually designed sprite for 0-F characters
	{//all sprites have a 4*5 0-padding on their right
		0b01100000, 
        0b10010000, 
        0b10010000, 
        0b10010000, 
        0b01100000, // 0

		0b01000000,
        0b11000000,
        0b01000000,
        0b01000000,
        0b11100000, // 1

        0b01100000,
        0b10010000,
        0b00100000,
        0b01000000,
        0b11110000, // 2

        0b11100000,
        0b00010000,
        0b01100000,
        0b00010000,
        0b11100000, // 3

        0b01100000,
        0b10100000,
        0b10100000,
        0b11110000,
        0b00100000, // 4

        0b11110000,
        0b10000000,
        0b11110000,
        0b00010000,
        0b11110000, // 5

        0b01110000,
        0b10000000,
        0b11100000,
        0b10010000,
        0b01100000, // 6

        0b11100000,
        0b00100000,
        0b00100000,
        0b01000000,
        0b01000000, // 7

        0b01100000,
        0b10010000,
        0b01100000,
        0b10010000,
        0b01100000, // 8

        0b01100000,
        0b10010000,
        0b01110000,
        0b00010000,
        0b01100000, // 9

        0b01100000,
        0b10010000,
        0b11110000,
        0b10010000,
        0b10010000, // A

        0b11100000,
        0b10010000,
        0b11100000,
        0b10010000,
        0b11100000, // B

        0b01100000,
        0b10010000,
        0b10000000,
        0b10010000,
        0b01100000, //C

        0b11100000,
        0b10010000,
        0b10010000,
        0b10010000,
        0b11100000, // D

        0b01100000,
        0b10000000,
        0b11100000,
        0b10000000,
        0b01100000, // E

        0b11100000,
        0b10000000,
        0b11000000,
        0b10000000,
        0b10000000, // F        
	};



struct CPUandRAM{
    unsigned char V[16];
    unsigned short I;
    unsigned short SP;
    unsigned short PC;
    unsigned char delay;
    unsigned char sound;
    unsigned char *ram;
    unsigned char *screen; // points ram(0xF00)
    unsigned char key_state[16];
    unsigned char save_key_state[16];
    bool waiting;
    unsigned char halt;
};

CPUandRAM* InitState(){
    CPUandRAM* s = new CPUandRAM;

    void* tmp = calloc(1024*4, 1); //this will be redone by the file reading
    s->ram = (unsigned char*)tmp;
    s->screen = &s->ram[0xF00];
    s->SP = 0xEA0;
    s->PC = 0x200;
    return s;
}


/**
 * param: *state, actual state of our system
 * Syntax is homemade based on AT&T syntax
 */

void execute(CPUandRAM *state){
    unsigned char *operation = &state->ram[state->PC];
    unsigned char up = operation[0];
    unsigned char low = operation[1];
    unsigned char nib0 = (up >> 4);
    unsigned char nib1 = up & 0xf;
    unsigned char nib2 = (low >> 4);
    unsigned char nib3 = low & 0xf;
     // debug printing. prints framebuffer and registers
    /**
    printf("%04x %02x %02x\n", state->PC, operation[0], operation[1]);
    printf("V: ");
    for (unsigned char i = 0;i<16;i++){
        printf("%02x ", state->V[i]);
    }
    printf("\n");
    /*
    for (unsigned char y = 0;y<32;y++){
        for (unsigned char x = 0;x<8;x++){
            //printf("%02x", state->screen[x+y*64]);
            bitset<8> b(state->screen[x+y*8]);
            cout << b;
        }
        printf("\n");
    }
    */
    switch(nib0){
        
        case 0x00: //we need to check for second 4 bits
            {
                if (up == 0x00){
                    if (low == 0xE0){//CLRS
                        memset(state->screen, 0, 64*32/8);
                    }
                    else if (low == 0xEE){//return
                        state->SP-=2;
                        unsigned short target = state->ram[state->SP] + (state->ram[state->SP+1] << 8);
                        if (target == state->PC){state->halt = 1;}
                        state->PC = target;
                        break;
                    }
                }
                else{ printf("not implemented CMCODE *%01x%02x", nib1, low);}   //this instruction is almost never used
                state->PC+=2;
                break;
            }
        
        case 0x01:
            {   //JMP $NNN
                unsigned short target = (nib1<<8) | low;
                if (target == state->PC){state->halt = 1;}
                state->PC = target;
                break;
            }
        
        case 0x02:
            { // call $NNN
                state->ram[state->SP] = state->PC+2;
                state->ram[state->SP+1] = (state->PC+2 >> 8);
                //printf("%02x%02x address to return\n", state->ram[state->SP+1], state->ram[state->SP]);
                state->SP+=2;
                unsigned short target = (nib1<<8) | low;
                if (target == state->PC){state->halt = 1;}
                state->PC = target;
                break;
            }

        case 0x03:
            { //SKPEQ VX $NN
                if (state->V[nib1] == low){
                    state->PC+=2;
                }
                state->PC+=2;
                break;
            }

        case 0x04:
            { //SKPNEQ VX $NN
                if (state->V[nib1] != low){
                    state->PC+=2;
                }
                state->PC+=2;
                break;
            }

        case 0x05:
            { //SKPEQ VX VY
                if (state->V[nib1] == state->V[nib2]){
                    state->PC+=2;
                }
                state->PC+=2;
                break;
            }

        case 0x06:
            {//MOV $NN VX
                state->V[nib1] = low;
                state->PC+=2; 
                break;
            }

        case 0x07:
            {//ADD $NN VX
                state->V[nib1] += low;
                state->PC+=2; 
                break;
            }

        case 0x08: //check last 4 bits a dot duffix indicates that VF is set with carry or other results
            switch(nib3){
                case 0x0:
                    {//MOV VY VX
                        state->V[nib1] = state->V[nib2];
                        state->PC+=2; 
                        break;
                    }
                case 0x1:
                    {//OR VY VX
                        state->V[nib1] |= state->V[nib2];
                        state->PC+=2; 
                        break;
                    }
                case 0x2:
                    {//AND VY VX
                        state->V[nib1] &= state->V[nib2];
                        state->PC+=2; 
                        break;
                    }
                case 0x3:
                    {//XOR VY VX
                        state->V[nib1] ^= state->V[nib2];
                        state->PC+=2; 
                        break;
                    }
                case 0x4:
                    {//ADD. VY VX, check overflow
                        unsigned short sum = state->V[nib1] + state->V[nib2];
                        if (sum >= 0x100){state->V[0xF] = 1;}
                        else {state->V[0xF] = 0;}
                        state->V[nib1] += state->V[nib2];
                        state->PC+=2; 
                        break;
                    }
                case 0x5:
                    {//SUB. VY VX
                        short sub = state->V[nib1] - state->V[nib2];
                        if (sub < 0){state->V[0xF] = 1;}
                        else {state->V[0xF] = 0;}
                        state->V[nib1] -= state->V[nib2];
                        state->PC+=2; 
                        break;
                    }

                case 0x6:
                    {//SHR. VX
                        state->V[0xF] = (state->V[nib1] & 0x1);
                        state->V[nib1] >>= 1;
                        state->PC+=2; 
                        break;
                    }

                case 0x7: // backward subs, instead of VX = VX - VY we have VX = VY - VX
                    {//SUB. VY VX
                        short sub = state->V[nib2] - state->V[nib1];
                        if (sub < 0){state->V[0xF] = 1;}
                        else {state->V[0xF] = 0;}
                        state->V[nib1] = state->V[nib2] - state->V[nib1];
                        state->PC+=2; 
                        break;
                    }

                case 0xE:
                    {//SHL. VX
                        state->V[0xF] = (state->V[nib1] & 0x80);
                        state->V[nib1] <<= 1;
                        state->PC+=2; 
                        break;
                    }
                default:
                    {
                        printf("Wrong instruction format %02x %02x", up, low);
                        state->PC+=2;
                        break;
                    }
            }
            break;
        case 0x09:
            { //SKPNEQ VX VY
                if (state->V[nib1] != state->V[nib2]){
                    state->PC+=2;
                }
                state->PC+=2;
                break;
            }

        case 0x0A:
            { //SETI $NNN
                state->I = (nib1<<8) | low;
                state->PC+=2;
                break;
            }

        case 0x0B:
            {   //JMP $NNN(V0)
                unsigned short target = ((nib1<<8) | low) + state->V[0];
                state->PC = target;
                break;
            }

        case 0x0C:
            {//RND VX $NN
                unsigned char rnd = rand() % 256;
                state->V[nib1] = rnd & low; // VX = $NN & random(0,255)
                state->PC +=2;
                break;
            }

        case 0x0D:
            {
                
                //Draw sprite
                int lines = nib3;
                int x = state->V[nib1];
                int y = state->V[nib2];	
                int i,j;
                
                state->V[0xf] = 0;
                for (i=0; i<lines; i++){
                    unsigned char *sprite = &state->ram[state->I+i];
                    int spritebit=7;
                    for (j=x; j<(x+8) && j<64; j++)
                    {
                        int jover8 = j / 8;     //picks the byte in the row
                        int jmod8 = j % 8;      //picks the bit in the byte
                        unsigned char srcbit = (*sprite >> spritebit) & 0x1;
                        
                        if (srcbit){
                            unsigned char *destbyte_p = &state->screen[ (i+y) * (8) + jover8];
                            unsigned char destbyte = *destbyte_p;
                            unsigned char destmask = (0x80 >> jmod8);
                            unsigned char destbit = destbyte & destmask;

                            srcbit = srcbit << (7-jmod8);
                            
                            if (srcbit & destbit)
                                state->V[0xf] = 1;
                            
                            destbit ^= srcbit;
                            
                            destbyte = (destbyte & ~destmask) | destbit;

                            *destbyte_p = destbyte;
                        }
                        spritebit--;
                    }
                }
                
                
                state->PC+=2;
                break;
            }


        case 0x0E:
            {
                switch(low){
                    case 0x9E:
                        {//SKIPKEY VX
                            if (state->key_state[state->V[nib1]] != 0){
                                state->PC+=2;
                            }
                            state->PC+=2;
                            break;
                        }
                    case 0xA1:
                        {//SKIPNOKEY VX
                            if (state->key_state[state->V[nib1]] == 0){
                                state->PC+=2;
                            }
                            state->PC+=2;
                            break;
                        }
                    default:
                        {
                            printf("Wrong instruction format %02x %02x\n", up, low);
                            break;
                        }
                }
                break;
            }
        
        case 0x0F:
            {
                switch(low){
                    case 0x07:
                        {//MOV DELAY VX
                            state->V[nib1] = state->delay;
                            state->PC+=2;
                            break; 
                        }
                    case 0x0A:
                        {//WAITKEY VX
                            if (!state->waiting){
                                state->waiting = true;
                                memcpy(&state->save_key_state, state->key_state, 16);
                                break;
                            }
                            else{
                                unsigned char i;
                                for (i = 0;i<16;i++){
                                    if (state->save_key_state[i] == 0 && state->key_state[1]){
                                        state->waiting = false;
                                        state->V[nib1] = i;
                                        state->PC+=2;
                                        break;
                                    }
                                    state->save_key_state[i] = state->key_state[i];
                                }
                                break;
                            }
                        }
                    case 0x15:
                        {//MOV VX DELAY
                            state->delay = state->V[nib1];
                            state->PC+=2;
                            break;
                        }
                    case 0x18:
                        {//MOV VX SOUND
                            state->sound = state->V[nib1];
                            state->PC+=2;
                            break;
                        }
                    case 0x1E:
                        {//ADD VX I
                            state->I += state->V[nib1];
                            state->PC+=2;
                            break;
                        }
                    case 0x29:
                        {//SETSPRITE VX
                            state->I = state->V[nib1]*5;
                            state->PC+=2;
                            break;
                        }
                    case 0x33:
                        {//MOVBCD VX I
                            int reg = operation[0]&0xf;    
                            unsigned char ones, tens, hundreds;    
                            unsigned char value=state->V[reg];    
                            ones = value % 10;    
                            value = value / 10;    
                            tens = value % 10;    
                            hundreds = value / 10;    
                            state->ram[state->I] = hundreds;    
                            state->ram[state->I+1] = tens;    
                            state->ram[state->I+2] = ones; 
                            state->PC+=2;
                            break;
                        }
                    case 0x55:  
                        {//DUMP VX (I)
                            //dump V0 to VX included in memory from I to I+X(I unchanged)
                            for (unsigned char i = 0;i<=nib1;i++){
                                state->ram[state->I+i] = state->V[i];
                            }
                            state->PC+=2;
                            break;
                        }
                    case 0x65:
                        {//DUMP (I) VX
                            for (unsigned char i = 0;i<=nib1;i++){
                                state->V[i] = state->ram[state->I+i];
                            }
                            state->PC+=2;
                            break;
                        }
                    default:
                        {
                            printf("Wrong instruction format");
                            break;
                        }
                }
                break;
            }
        default:
            printf("%01x not implemented", nib0);
    }

}


int main (int argc, char**argv)    {  
    srand (time(NULL));  
    FILE *f= fopen(argv[1], "rb");    
    if (f==NULL)    
    {    
        printf("error: Couldn't open %s\n", argv[1]);    
        exit(1);    
    }    
    int clocktime = 0;
    if (argc > 2) clocktime = atoi(argv[2]); // in milliseconds
    if (clocktime <= 0){clocktime = 16;}

    //Get the file size    
    fseek(f, 0L, SEEK_END);    
    int fsize = ftell(f);    
    fseek(f, 0L, SEEK_SET);    

    //CHIP-8 convention puts programs in memory at 0x200    
    // They will all have hardcoded addresses expecting that    
    //    
    //Read the file into memory at 0x200 and close it.

    CPUandRAM* system = InitState();

    Platform platform("CIPPOTTO", VIDEO_WIDTH*16, VIDEO_HEIGHT*16, VIDEO_WIDTH, VIDEO_HEIGHT); 
    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    void *temp=calloc(4096, 1);    
    unsigned char *buffer = (unsigned char*)temp;
    fread(buffer+0x200, fsize, 1, f);    
    fclose(f);    
    system->ram = buffer;

    for (unsigned char i = 0;i<80;i++) {
        system->ram[i] = fontset[i]; //load fonts in address 0x00-0x50
    }
    bool quit = false;
    while (!quit && system->PC < (fsize+0x200))    
    {    
        quit = platform.ProcessInput(system->key_state);
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        if (dt > clocktime || clocktime == 420){
            unsigned char old_pc = system->PC;
            execute(system);   
            lastCycleTime = currentTime;
            if (system->delay > 0) system->delay--;
            if (system->sound > 0) system->sound--;
            //if (system->halt == 1){quit = true;}
            platform.Update(system->screen, VIDEO_WIDTH);
            /* sound is broken
            if (system->sound > 0){
                platform.beep();
                }
            */
            if (system->PC == old_pc){printf("Loop detected");}
        }
    }    
    printf("%04x", system->PC);
    return 0;    
} 