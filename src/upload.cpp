/*
	MIT License
	
	Copyright (c) 2020 HerrNamenlos123
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include <SPI.h>
#include "jtag.h"


// Here comes all logic for uploading the FPGA_Bitstream.h file to the FPGA
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//

// For High level functions such as pinMode or digitalWrite, you have to use FPGA_xxx
// Low level functions (in jtag.c file) use other kind of #define (TDI,TDO,TCK,TMS) with different values
#define FPGA_TDI                            (26u)
#define FPGA_TDO                            (29u)
#define FPGA_TCK                            (27u)
#define FPGA_TMS                            (28u)

// Clock send by SAMD21 to the FPGA
#define FPGA_CLOCK                        (30u)

// SAMD21 to FPGA control signal (interrupt ?)
#define FPGA_MB_INT                       (31u)

// FPGA to SAMD21 control signal (interrupt ?)
#define FPGA_INT                          (33u) //B2 N2

// For MKR pinout assignments see : https://systemes-embarques.fr/wp/brochage-connecteur-mkr-vidor-4000/

#define no_data     0xFF, 0xFF, 0xFF, 0xFF, \
                    0xFF, 0xFF, 0xFF, 0xFF, \
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
                    0xFF, 0xFF, 0xFF, 0xFF, \
                    0x00, 0x00, 0x00, 0x00  \

#define NO_BOOTLOADER   no_data
#define NO_APP        no_data
#define NO_USER_DATA    no_data

__attribute__ ((used, section(".fpga_bitstream_signature")))
const unsigned char signatures[4096] = {
    //#include "signature.ttf"
    NO_BOOTLOADER,

    0x00, 0x00, 0x08, 0x00,
    0xA9, 0x6F, 0x1F, 0x00,   // Don't care.
    0x20, 0x77, 0x77, 0x77, 0x2e, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x65, 0x73, 0x2d, 0x65, 0x6d, 0x62, 0x61, 0x72, 0x71, 0x75, 0x65, 0x73, 0x2e, 0x66, 0x72, 0x20, 0x00, 0x00, 0xff, 0xf0, 0x0f,
    0x01, 0x00, 0x00, 0x00,   
    0x01, 0x00, 0x00, 0x00,   // Force

    NO_USER_DATA,
};
__attribute__ ((used, section(".fpga_bitstream")))
const unsigned char bitstream[] = {
    #include "FPGA_Bitstream.h"
};

void __uploadBitstream() {

    int ret;
    uint32_t ptr[1];

    //Init Jtag Port
    ret = jtagInit();
    mbPinSet();

    // Load FPGA user configuration
    ptr[0] = 0 | 3;
    mbEveSend(ptr, 1);

    // Give it delay (no feedback if it's done)
    delay(1000);

    // Configure onboard LED Pin as output
    pinMode(LED_BUILTIN, OUTPUT);

    // Disable all JTAG Pins (usefull for USB BLASTER connection)
    pinMode(FPGA_TDO, INPUT);
    pinMode(FPGA_TMS, INPUT);
    pinMode(FPGA_TDI, INPUT);
    pinMode(FPGA_TCK, INPUT);

    // Configure other share pins as input too
    pinMode(FPGA_INT, INPUT);
}
