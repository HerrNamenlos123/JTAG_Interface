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

#include "FPGA_Controller.h"

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

extern void enableFpgaClock(void);

#define no_data    0xFF, 0xFF, 0xFF, 0xFF, \
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

void uploadBitstream() {

  int ret;
  uint32_t ptr[1];

  enableFpgaClock();

  //Init Jtag Port
  ret = jtagInit();
  mbPinSet();

  // Load FPGA user configuration
  ptr[0] = 0 | 3;
  mbEveSend(ptr, 1);

  // Give it delay
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










// Now all logic for the communication interface between the CPU and the FPGA chip
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv


// Declaration of private functions, only used in this file
void walkJTAG(uint8_t numticks, uint16_t path);
void writeDirectJTAG(uint16_t IR, uint8_t* data, uint32_t numbits);
void readDirectJTAG(uint16_t IR, uint8_t* data, uint32_t numbits);
void readWriteDirectJTAG(uint16_t IR, uint8_t* recv, uint8_t* send, uint32_t numbits);

void jtag_host_setup();
void jtag_host_shutdown();
void jtag_host_pulse_tck(int tms);
void jtag_host_pulse_tdi(const uint8_t* data, size_t size);
void jtag_host_pulse_tdo(uint8_t* data, size_t size);
unsigned int jtag_host_pulse_tdio(int bits, unsigned int out);
unsigned int jtag_host_pulse_tdio_instruction(int bits, unsigned int out);
void jtag_host_pulse_tdio_spi(const uint8_t* send, uint8_t* recv, size_t size);



// Function definitions of the private functions

void walkJTAG(uint8_t numticks, uint16_t path) {
  for(int i=0; i<numticks; i++, path >>= 1)
    jtag_host_pulse_tck(path & 0x0001);
}

void writeDirectJTAG(uint16_t IR, uint8_t* data, uint32_t numbits) { 
  walkJTAG(10, 0b0011011111);
  jtag_host_pulse_tdio_instruction(10, (unsigned int)IR);
  walkJTAG(5, 0b00111);

  int NumBytes = numbits >> 3;
  int NumBits = numbits & 0x07;
  if (NumBits == 0) {
    NumBits = 8;
    NumBytes = NumBytes - 1;
  }

  if (NumBytes > 0) jtag_host_pulse_tdi(data, (size_t)(NumBytes));
  jtag_host_pulse_tdio(NumBits, (unsigned int)data[NumBytes]);
  walkJTAG(5, 0b11111);
}

void readDirectJTAG(uint16_t IR, uint8_t* data, uint32_t numbits) {
  walkJTAG(10, 0x00DF);
  jtag_host_pulse_tdio_instruction(10, (unsigned int)IR);
  walkJTAG(5, 0x0007);

  if ((numbits >> 3) > 0)
    jtag_host_pulse_tdo(data, (size_t)(numbits >> 3));

  if ((numbits & 0b111) != 0)
    data[numbits >> 3] = jtag_host_pulse_tdio((int)(numbits & 0b111), 0);

  walkJTAG(5, 0x001F);
}

void readWriteDirectJTAG(uint16_t IR, uint8_t* recv, uint8_t* send, uint32_t numbits) {
  walkJTAG(10, 0x00DF);
  jtag_host_pulse_tdio_instruction(10, (unsigned int)IR);
  walkJTAG(5, 0b00111);

  int NumBytes = numbits >> 3;
  int NumBits = numbits & 0b111;
  if (NumBits == 0) {
    NumBits = 8;
    NumBytes = NumBytes - 1;
  }
  
  if (NumBytes > 0) jtag_host_pulse_tdio_spi(send, recv, (size_t)(NumBytes));
  recv[NumBytes] = jtag_host_pulse_tdio(NumBits, (unsigned int)send[NumBytes]);

  walkJTAG(5, 0x001F);
}




// Now the publicly accessible functions for communicating with the FPGA

void initJTAG() {
  enableFpgaClock();
  jtag_host_setup();
}

void closeJTAG() {
  jtag_host_shutdown();
}

int32_t readJTAG(uint8_t readAddress) {

  if(readAddress > 14) {
    Serial.print("Warning: Can't read from JTAG Interface: Invalid address ");
    Serial.println(readAddress);
    return 0;
  }

  uint32_t recv;
  // Set write address to 15 to ignore it
  uint16_t addr = 0b111110000 | (readAddress & 0b1111); // Lower 4 bits are the read address
  writeDirectJTAG(14, (uint8_t*)&addr, 9);
  readDirectJTAG(12, (uint8_t*)&recv, 32);

  return (int32_t)recv;
}

void writeJTAG(uint8_t writeAddress, int32_t value) {

  if(writeAddress > 14) {
    Serial.print("Warning: Can't write to JTAG Interface: Invalid address ");
    Serial.println(writeAddress);
    return;
  }

  // Set read address to 15 to ignore it
  uint16_t addr = 0b100001111 | ((writeAddress & 0b1111) << 4); // Higher 4 bits are the write address
  writeDirectJTAG(14, (uint8_t*)&addr, 9);
  writeDirectJTAG(12, (uint8_t*)&value, 32);
}

int32_t readWriteJTAG(uint8_t readAddress, uint8_t writeAddress, int32_t value) {

  if(readAddress > 14) {
    Serial.print("Warning: Can't read from JTAG Interface: Invalid address ");
    Serial.println(readAddress);
    return 0;
  }

  if(writeAddress > 14) {
    Serial.print("Warning: Can't write to JTAG Interface: Invalid address ");
    Serial.println(writeAddress);
    return 0;
  }

  uint32_t recv;

  // Higher 4 bits are the write address and lower 4 bits are the read address
  uint16_t addr = 0b100000000 | ((writeAddress & 0b1111) << 4) | (readAddress & 0b1111); 
  writeDirectJTAG(14, (uint8_t*)&addr, 9);
  readWriteDirectJTAG(12, (uint8_t*)&recv, (uint8_t*)&value, 32);

  return (int32_t)recv;
}






// Now all necessary, slightly modified parts from the jtag_host.cpp file
// taken from the official VidorPeripherals library

#define TMS     28 // PA14             | SERCOM2/ PAD[2]
#define TCK     27 // PA13 -> SPI CLK  | SERCOM2/ PAD[1]
#define TDO     29 // PA15 -> MISO     | SERCOM2/ PAD[3]
#define TDI     26 // PA12 -> MOSI     | SERCOM2/ PAD[0]

#define TMS_HIGH() (digitalPinToPort(TMS)->OUTSET.reg = digitalPinToBitMask(TMS))
#define TMS_LOW()  (digitalPinToPort(TMS)->OUTCLR.reg = digitalPinToBitMask(TMS))

#define TCK_HIGH() (digitalPinToPort(TCK)->OUTSET.reg = digitalPinToBitMask(TCK))
#define TCK_LOW()  (digitalPinToPort(TCK)->OUTCLR.reg = digitalPinToBitMask(TCK))
#define TCK_PMUX() (digitalPinToPort(TCK)->PINCFG[g_APinDescription[TCK].ulPin].bit.PMUXEN = 1)
#define TCK_UNPMUX() (digitalPinToPort(TCK)->PINCFG[g_APinDescription[TCK].ulPin].bit.PMUXEN = 0)

#define TDI_HIGH() (digitalPinToPort(TDI)->OUTSET.reg = digitalPinToBitMask(TDI))
#define TDI_LOW()  (digitalPinToPort(TDI)->OUTCLR.reg = digitalPinToBitMask(TDI))
#define TDI_PMUX() (digitalPinToPort(TDI)->PINCFG[g_APinDescription[TDI].ulPin].bit.PMUXEN = 1)
#define TDI_UNPMUX() (digitalPinToPort(TDI)->PINCFG[g_APinDescription[TDI].ulPin].bit.PMUXEN = 0)

#define TDO_PMUX() (digitalPinToPort(TDO)->PINCFG[g_APinDescription[TDO].ulPin].bit.PMUXEN = 1)
#define TDO_UNPMUX() (digitalPinToPort(TDO)->PINCFG[g_APinDescription[TDO].ulPin].bit.PMUXEN = 0)

#define TMS_WRITE(val) {if (val) { TMS_HIGH(); } else { TMS_LOW(); }}
#define TDI_WRITE(val) {if (val) { TDI_HIGH(); } else { TDI_LOW(); }}
#define TDO_READ() ((digitalPinToPort(TDO)->IN.reg & digitalPinToBitMask(TDO)) != 0)

#define SPI_JTAG SPI1
SPISettings JTAG_SPISettings(12000000, LSBFIRST, SPI_MODE0);


void jtag_host_setup()
{
	pinMode(TMS, OUTPUT);
	pinMode(TCK, OUTPUT);
	pinMode(TDO, INPUT);
	pinMode(TDI, OUTPUT);

	TMS_LOW();
	TCK_LOW();
	TDI_LOW();

	SPI_JTAG.begin();
	SPI_JTAG.beginTransaction(JTAG_SPISettings);

	TCK_UNPMUX();
	TDI_UNPMUX();
}

void jtag_host_shutdown()
{
	TMS_LOW();
	TCK_LOW();
	TDI_LOW();

	pinMode(TMS, INPUT);
	pinMode(TCK, INPUT);
	pinMode(TDO, INPUT);
	pinMode(TDI, INPUT);
}

void jtag_host_pulse_tck(int tms)
{
	TMS_WRITE(tms);

	TCK_LOW();
	TCK_HIGH();
}

void jtag_host_pulse_tdi(const uint8_t* data, size_t size)
{
	TCK_LOW();

	TCK_PMUX();
	TDI_PMUX();

	for (size_t i = 0; i < size; i++) {
		SPI_JTAG.transfer(*data++);
	}

	TCK_UNPMUX();
	TDI_UNPMUX();
}

void jtag_host_pulse_tdo(uint8_t* data, size_t size)
{
	TCK_LOW();

	TCK_PMUX();
	TDO_PMUX();

	for (size_t i = 0; i < size; i++) {
		*data = SPI_JTAG.transfer(0x00);
		data++;
	}

	TCK_UNPMUX();
	TDO_UNPMUX();
}

unsigned int jtag_host_pulse_tdio(int bits, unsigned int out)
{
	unsigned int in = 0;

	for (int i = 0; i < bits; i++) {
		TDI_WRITE(out & 1);

		TCK_LOW();
		TCK_HIGH();

		in |= (TDO_READ() << i);

		out >>= 1;
	}

	return in;
}

unsigned int jtag_host_pulse_tdio_instruction(int bits, unsigned int out)
{
	unsigned int in = 0;

	for (int i = 0; i < bits; i++) {
		TDI_WRITE(out & 1);

		if (i != (bits - 1)) {
			TCK_LOW();
			TCK_HIGH();
		
			in |= (TDO_READ() << i);

			out >>= 1;
		}
	}

	return in;
}

void jtag_host_pulse_tdio_spi(const uint8_t* send, uint8_t* recv, size_t size)
{
	TCK_LOW();

	TCK_PMUX();
	TDI_PMUX();
	TDO_PMUX();

	for (size_t i = 0; i < size; i++) {
		*recv = SPI_JTAG.transfer(*send++);
		recv++;
	}

	TCK_UNPMUX();
	TDI_UNPMUX();
	TDO_UNPMUX();
}
