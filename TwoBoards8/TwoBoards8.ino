#include <HT1632.h>
#include <BigNumbers8.h>

const int pinCS1 = 8;
const int pinWR = 7;
const int pinDATA = 6;
const int pinCLK= 9;

unsigned long LastSecond=0;
unsigned long CurrSecond;
byte num1, num2;

char IMG[128];

byte bitswap (byte x)
{
    byte result;

    asm("mov __tmp_reg__, %[in] \n\t"
     "lsl __tmp_reg__  \n\t"   /* shift out high bit to carry */
     "ror %[out] \n\t"  /* rotate carry __tmp_reg__to low bit (eventually) */
     "lsl __tmp_reg__  \n\t"   /* 2 */
    "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 3 */
    "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 4 */
    "ror %[out] \n\t"

     "lsl __tmp_reg__  \n\t"   /* 5 */
    "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 6 */
    "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 7 */
    "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 8 */
    "ror %[out] \n\t"
: 
        [out] "=r" (result) : 
    [in] "r" (x));
    return(result);
}

void Break8(char IMG8[], byte y) {
    byte temp, temp2;
    if (y==0) {
        for (byte i=0; i<64; ++i) { 
            IMG[2*i]      = IMG8[i]  >>   4;
            IMG[2*i+1] = IMG8[i]   &   15;
        }
    }
    else {
        for (byte i=0; i<32; ++i) { 
            temp = bitswap(IMG8[2*i]);
            temp2 = bitswap(IMG8[2*i+1]);            

            IMG[127-4*i]      = temp   &   15;
            IMG[127-(4*i+1)] = temp  >>   4 ;
            IMG[127-(4*i+2)] = temp2   &   15; 
            IMG[127-(4*i+3)] = temp2 >>   4; 
        }
    }

    HT1632.drawImage(IMG, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0, 0);
}    

void drawDigit(byte num, byte y) {
    switch (num) {
    case 0:
        Break8(IMG_0,y);
        break;
    case 1:
        Break8(IMG_1,y);
        break;
    case 2:
        Break8(IMG_2,y);
        break;
    case 3:
        Break8(IMG_3,y);
        break;
    case 4:
        Break8(IMG_4,y);
        break;
    case 5:
        Break8(IMG_5,y);
        break;
    case 6:
        Break8(IMG_6,y);
        break;
    case 7:
        Break8(IMG_7,y);
        break;
    case 8:
        Break8(IMG_8,y);
        break;
    case 9:
        Break8(IMG_9,y);
        break;
    }
}

void setup () {
    HT1632.begin(pinCS1, pinWR, pinDATA, pinCLK);
}



void loop () { 
    HT1632.clear();

    CurrSecond=(millis()/1000)%60;
    if (CurrSecond!=LastSecond) { // every second
        HT1632.clear();
        LastSecond=(millis()/1000)%60;
        num1=LastSecond % 10;
        num2=(LastSecond/10) % 10;

        drawDigit(num1,0);
        drawDigit(num2,32);
        HT1632.render();
    }
}
