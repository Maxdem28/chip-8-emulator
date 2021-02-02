#include <iostream>

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

    void* tmp = calloc(1024*4, 1);
    s->ram = (unsigned char*)tmp;
    s->screen = &s->ram[0xF00];
    s->SP = 0xFA0;
    s->PC = 0x200;

    return s;
}




/**
 * param: *program, pointer to start of the code
 * param: *pc, offset from start of the code to current instruction 
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
    //printf("%04x %02x %02x ", pc, operation[0], operation[1]);

    switch(nib0){
        case 0x00: //we need to check for second 4 bits
            {
                if (up == 0x00){
                    if (low == 0xE0){
                        printf("CLRS"); //clear screen
                        break;
                    }
                    else if (low == 0xEE){
                        printf("RET");  //return
                        break;
                    }
                }
                printf("CMCODE *%01x%02x", nib1, low);   //this instruction is almost never used
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
                printf("SKPEQ %%V%01x, $%02x", nib1, low);
                break;
            }

        case 0x04:
            {//SKPEQ VX NN
                printf("SKPNEQ %%V%01x, $%02x", nib1, low);
                break;
            }

        case 0x05:
            {//SKPEQ VX VY
                printf("SKPEQ %%V%01x, %%V%01x", nib1, nib2);
                break;
            }

        case 0x06:
            {//MOV $NN VX
                printf("MOV $%02x, %%V%01x", low, nib1); 
                break;
            }

        case 0x07:
            {//ADD $NN VX
                printf("ADD $%02x, %%V%01x", low, nib1); 
                break;
            }

        case 0x08: //check last 4 bits a dot duffix indicates that VF is set with carry or other results
            switch(nib3){
                case 0x0:
                    {//MOV VY VX
                        printf("MOV %%V%01x, %%V%01x", nib2, nib1); 
                        break;
                    }
                case 0x1:
                    {//OR VY VX
                        printf("OR %%V%01x, %%V%01x", nib2, nib1); 
                        break;
                    }
                case 0x2:
                    {//AND VY VX
                        printf("AND %%V%01x, %%V%01x", nib2, nib1); 
                        break;
                    }
                case 0x3:
                    {//XOR VY VX
                        printf("XOR %%V%01x, %%V%01x", nib2, nib1); 
                        break;
                    }
                case 0x4:
                    {//ADD. VY VX
                        printf("ADD. %%V%01x, %%V%01x", nib2, nib1); 
                        break;
                    }
                case 0x5:
                    {//SUB. VY VX
                        printf("SUB. %%V%01x, %%V%01x", nib2, nib1); 
                        break;
                    }

                case 0x6:
                    {//SHR. VX
                        printf("SHR. %%V%01x", nib2, nib1); 
                        break;
                    }

                case 0x7: // backward subs, instead of VX = VX - VY we have VX = VY - VX
                    {//BSUB VY VX
                        printf("BSUB. %%V%01x, %%V%01x", nib2, nib1); 
                        break;
                    }

                case 0xE:
                    {//SHL. VX
                        printf("SHL. %%V%01x", nib2, nib1); 
                        break;
                    }
                default:
                    {
                        printf("Wrong instruction format");
                        break;
                    }
            }
            break;
        case 0x09:
            {//SKPNEQ VX VY
                printf("SKPNEQ %%V%01x, %%V%01x", nib1, nib2);
                break;
            }

        case 0x0A:
            { //SETI $NNN
                printf("SETI %01x%02x", nib1, low); //I is MAR
                break;
            }

        case 0x0B:
            {//JMP $NNN(V0)
                printf("JMP *%01x%02x(%%V1)", nib1, low);
                break;
            }

        case 0x0C:
            {//RND VX $NN
                printf("RND %%V%01x, $%02x", nib1, low); // VX = $NN & random(0,255)
                break;
            }

        case 0x0D:
            {//DRAW VX VY $NN
                printf("DRAW %%V%01x, %%V%01x, $%01x", nib1, nib2, nib3);
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
                            printf("MOV DELAY, %%V%01x", nib1);
                            break; 
                        }
                    case 0x0A:
                        {//WAITKEY VX
                            printf("WAITKEY %%VV01x", nib1);
                            break;
                        }
                    case 0x15:
                        {//MOV VX DELAY
                            printf("MOV %%V%01x, DELAY", nib1);
                            break;
                        }
                    case 0x18:
                        {//MOV VX SOUND
                            printf("MOV %%V%01x, SOUND", nib1);
                            break;
                        }
                    case 0x1E:
                        {//ADD VX I
                            printf("ADD %%V%01x, I", nib1); 
                            break;
                        }
                    case 0x29:
                        {//SETSPRITE VX
                            printf("SETSPRITE %%V%01x", nib1); //I = location of 4x5 sprite representing character 0-F in VX
                            break;
                        }
                    case 0x33:
                        {//MOVBCD VX I
                            printf("MOVBCD %%V%01x", nib1); //move 3 digits of bcd of VX in I, I+1, I+2 respectively
                            break; 
                        }
                    case 0x55:  
                        {//DUMP VX (I)
                            printf("DUMP %%V%01x, (I)", nib1); //dump V0 to VX included in memory from I to I+X(I unchanged)
                            break;
                        }
                    case 0x65:
                        {//DUMP (I) VX
                            printf("DUMP (I), %%V%01x", nib1);
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

    void *temp=malloc(fsize+0x200);    
    unsigned char *buffer = (unsigned char*)temp;
    fread(buffer+0x200, fsize, 1, f);    
    fclose(f);    

    int pc = 0x200;    
    while (pc < (fsize+0x200))    
    {    
        execute(buffer, pc);    
        pc += 2;    
        printf ("\n");    
    }    
    return 0;    
} 