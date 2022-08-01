
#include "FPGA.h"

//
// This is an example on how to use the JTAG_Interface: The file FPGA_Bitstream.h in the libraries 
// folder is generated from Intel Quartus and supposed to be replaced by your own. Make sure to
// take a look at the attached Quartus project, where you can extend this project or copy
// the Verilog modules related to the JTAG interface and paste them into your own design.
//
// Those JTAG related modules and the corresponding C++ functions allow you
// to easily and reliably send and read data from and to the FPGA. The point of this
// project is to make it as easy as possible for any new user to establish a communication
// between the CPU and the FPGA, as most commonly the first problem you encounter when creating
// your own bitstream is that you lose the ability to talk to the FPGA.
//
// When replacing this bitstream with your own, make sure to compile it correctly with Intel Quartus
//  (try the attached Quartus project which the default bitstream is generated from).
//
// When successfully compiled, take the output file ( output_files/MKRVIDOR4000.ttf ) and
// bit-reverse it using ReverseByte. For info on that, refer to:
//
// 		https://systemes-embarques.fr/wp/archives/mkr-vidor-4000-programmation-du-fpga-partie-1/
// 
// When the bitstream is finally reversed, simply rename it to "FPGA_Bitstream.h" and paste it
// into the FPGA_Controller folder of the library, which will overwrite the default bitstream.
//


#include <SPI.h>
#include "jtag.h"

// Declaration of private functions, only used in this file
void walkJTAG(uint8_t numticks, uint16_t path);
void writeDirectJTAG(uint16_t IR, uint8_t* data, uint32_t numbits);
void readDirectJTAG(uint16_t IR, uint8_t* data, uint32_t numbits);
void readWriteDirectJTAG(uint16_t IR, uint8_t* recv, uint8_t* send, uint32_t numbits);

extern void enableFpgaClock(void);


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

void setup() {

	Serial.begin(115200);

	// Upload the bitstream to the FPGA
	uploadBitstream();

    // Wait for serial monitor to connect
	while(!Serial);

	Serial.println("Welcome!");

	// Warning: In this example the digital pins 6, 7 and 8 are configured as outputs
	// on the FPGA, so shortening them or configuring them as outputs
	// on the CPU as well may kill your Arduino permanently!

	initJTAG();

	/*if (!initJTAG(32, 16)) {
        Serial.println("Error: Invalid JTAG interface configuration!");     // JTAG interface instance was not recognized:
        while (true);                                                       // Either the FPGA is not loaded correctly or
	} */                                                                      // the regsize and number of regs is incorrect
}

/*uint8_t makeAddress(uint32_t* address, uint8_t readAddress, uint8_t writeAddress, uint8_t numberOfRegisters) {
	uint8_t addressWidth = ceil(log2(numberOfRegisters + 1));
	*address = 1 << addressWidth * 2 | (writeAddress & makeM1(addressWidth)) << addressWidth | (readAddress & makeM1(addressWidth));
	return addressWidth;
}*/

/*void penetrationTest() {
	const uint8_t numberOfRegisters = 16;
	//uint32_t values[numberOfRegisters] = {};
	//uint32_t fpga[numberOfRegisters] = {};

	while (true) {

		//uint32_t addr1;
		//uint8_t bits1 = makeAddress(&address, readAddress, writeAddress, numberOfRegisters);
  		//writeDirectJTAG(14, (uint8_t*)&addr1, bits1);

		uint32_t value = rand() % 100;
  		writeDirectJTAG(12, (uint8_t*)&value, 32);

		for (int i = 0; i < 16; i++) {
			uint32_t read;
			uint8_t bits1 = makeAddress(&address, readAddress, writeAddress, numberOfRegisters);
  			writeDirectJTAG(14, (uint8_t*)&addr, 10);
			readDirectJTAG(12, (uint8_t*)&read, 32);
			Serial.print("[");
			Serial.print(i);
			Serial.print("] = ");
			Serial.println(read);
		}

		//Serial.print("")

		delay(1000);
	}
}*/

void printPins() {

    Serial.print(digitalRead(0));
    Serial.print(" ");
    Serial.print(digitalRead(1));
    Serial.print(" ");
    Serial.print(digitalRead(2));
    Serial.print(" ");
    Serial.print(digitalRead(3));
    Serial.print(" ");
    Serial.println(digitalRead(4));

    //Serial.print("CDR=");
    //Serial.println(digitalRead(0));
    //Serial.print("SDR=");
    //Serial.println(digitalRead(1));
    //Serial.print("UDR=");
    //Serial.println(digitalRead(2));
}
/*
void test() {

	while (true) {*/

void jtag_host_pulse_tms(bool tms);
#define CHANGE_JTAG_STATE(numticks, p) { \
    uint16_t path = p; \
    for (int i = 0; i < numticks; i++, path >>= 1) { \
      jtag_host_pulse_tms(path & 1); \
    } \
}

void loop() {

       /* CHANGE_JTAG_STATE(5, 0b11111); printPins();
        delay(1000);

        CHANGE_JTAG_STATE(1, 0); printPins();
        CHANGE_JTAG_STATE(1, 1); printPins();
        CHANGE_JTAG_STATE(1, 1); printPins();
        CHANGE_JTAG_STATE(1, 0); printPins();
        CHANGE_JTAG_STATE(1, 0); printPins();
        delay(1000);

        CHANGE_JTAG_STATE(1, 0); printPins();
        CHANGE_JTAG_STATE(1, 1); printPins();
        CHANGE_JTAG_STATE(1, 1); printPins();
        CHANGE_JTAG_STATE(1, 0); printPins();
        CHANGE_JTAG_STATE(1, 0); printPins();
        delay(1000);

        CHANGE_JTAG_STATE(1, 1); printPins();
        CHANGE_JTAG_STATE(1, 1); printPins();
        CHANGE_JTAG_STATE(1, 1); printPins();
        CHANGE_JTAG_STATE(1, 1); printPins();
        CHANGE_JTAG_STATE(1, 1); printPins();
        delay(1000);*/

	Serial.println(readJTAG(1));
	delay(10);

	//Serial.println();

	//test();

	// Make some space
	/*Serial.println("\n\n\n\n");

	// In the FPGA code we connected the analog ports 0-2 to the lowest 3 bits of input bus 0,
	// so we're gonna read this bus and extract the first 3 bits to read them
	// It's only a digital signal in our case

	// Read the input band with id 0
	uint32_t inputs = readJTAG(0);
	Serial.print("A0: "); Serial.println((inputs & 0b001) > 0);
	Serial.print("A1: "); Serial.println((inputs & 0b010) > 0);
	Serial.print("A2: "); Serial.println((inputs & 0b100) > 0);



	// Write to register #0 as we connected its lowest 3 bits to digital outputs 6, 7 and 8
	// Let's set the pins to random values:
	bool d6 = rand() % 2 == 0;
	bool d7 = rand() % 2 == 0;
	bool d8 = rand() % 2 == 0;

	uint32_t output = (d8 << 2) | (d7 << 1) | d6;	// Write the value to register #0, which the
	writeJTAG(0, output);							// digital pins are constantly reading from

	// Warning: In this example the digital pins 6, 7 and 8 are configured as outputs
	// on the FPGA, so shortening them or configuring them as outputs
	// here as well may kill your Arduino permanently!

	Serial.print("\nSet D6 to "); Serial.println(d6);
	Serial.print("Set D7 to "); Serial.println(d7);
	Serial.print("Set D8 to "); Serial.println(d8);



	// We also connected the output of a counter to input bus #1, so we're gonna read this now
	// However, as the full clock range is connected to the 32 bits it's going to overflow
	// back to 0 after about every 35 seconds
	uint32_t counter = readJTAG(1);
	Serial.print("\nThe FPGA program has been running for ");
	Serial.print(counter);
	Serial.print(" clock cycles, which is ");
	Serial.print(counter / 120000000.0);			// Divide by 120 million because the clock we 
	Serial.println(" seconds\n");					// connected it to runs on 120MHz


	// As we connected the output #14 back to input #13 we can read input #13 to see what output #14
	// currently is.
	// The readWriteJTAG() function is a lot more efficient as it reads and writes at the same time
	int32_t newValue = random(-100, 100);
	int32_t oldValue = readWriteJTAG(13, 14, newValue);	// Read input #13 and write newValue to output #14

	Serial.print("The register #14 just contained the value ");
	Serial.print(oldValue);
	Serial.print(", which was replaced by the value ");
	Serial.println(newValue);

	delay(500);*/

}
