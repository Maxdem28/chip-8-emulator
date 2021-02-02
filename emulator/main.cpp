#include <iostream>
#include <stdlib.h> 
#include <time.h> 

using namespace std;


struct CPUandRAM{
    unsigned char V[16];
    unsigned short I;
    unsigned short SP;
    unsigned short PC;
    unsigned char delay;
    unsigned char sound;
    unsigned char *ram;
    unsigned char *screen; // points ram(0xF00)
};

CPUandRAM* InitState(){
    CPUandRAM* s = new CPUandRAM;

    void* tmp = calloc(1024*4, 1); //this will be redone by the file reading
    s->ram = (unsigned char*)tmp;
    s->screen = &s->ram[0xF00];
    s->SP = 0xFA0;
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
    //printf("%04x %02x %02x\n", state->PC, operation[0], operation[1]);

    switch(nib0){
        
        case 0x00: //we need to check for second 4 bits
            //printf("%02x%02x", up, low);
            {
                if (up == 0x00){
                    if (low == 0xE0){
                        printf("CLRS\n"); //clear screen  
                    }
                    else if (low == 0xEE){
                        printf("RET\n");  //return
                    }
                }
                else{ printf("not implemented CMCODE *%01x%02x", nib1, low);}   //this instruction is almost never used
                state->PC+=2;
                break;
            }
        
        case 0x01:
            {   //JMP $NNN
                unsigned short target = (nib1<<8) + low;
                state->PC = target;
                break;
            }
        
        case 0x02:
            { // call $NNN
                printf("CALL *%01x%02x", nib1, low);
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
                state->I = (nib1<<8) + low;
                state->PC+=2;
                break;
            }

        case 0x0B:
            {   //JMP $NNN(V0)
                unsigned short target = (nib1<<8) + low + state->V[0];
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
            {//DRAW VX VY $NN
                printf("DRAW %%V%01x, %%V%01x, $%01x", nib1, nib2, nib3);
                state->PC+=2;
                break;
            }

        case 0x0E:
            {
                switch(low){
                    case 0x9E:
                        {//SKIPKEY VX
                            printf("SKIPKEY %%V%01x", nib1);
                            break;
                        }
                    case 0xA1:
                        {//SKIPNOKEY VX
                            printf("SKIPNOKEY %%V%01x", nib1);
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
                            printf("WAITKEY %%VV01x", nib1);
                            break;
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
                            printf("SETSPRITE %%V%01x", nib1); //I = location of 4x5 sprite representing character 0-F in VX
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

    //Get the file size    
    fseek(f, 0L, SEEK_END);    
    int fsize = ftell(f);    
    fseek(f, 0L, SEEK_SET);    

    //CHIP-8 convention puts programs in memory at 0x200    
    // They will all have hardcoded addresses expecting that    
    //    
    //Read the file into memory at 0x200 and close it.

    CPUandRAM* system = InitState();

    void *temp=calloc(4096, 1);    
    unsigned char *buffer = (unsigned char*)temp;
    fread(buffer+0x200, fsize, 1, f);    
    fclose(f);    
    system->ram = buffer;
    while (system->PC < (fsize+0x200))    
    {    
        execute(system);   
    }    
    printf("%04x", system->PC);
    return 0;    
} 