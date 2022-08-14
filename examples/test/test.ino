//
// This is an example on how to use the JTAG_Interface: The file FPGA_Bitstream.h is
// generated from Intel Quartus and supposed to be replaced by your own. Make sure to
// take a look at the attached Quartus project, where you can extend this project or copy
// the Verilog modules related to the JTAG interface and paste them into your own design.
//
// Those files allow you to easily and reliably send and read data from and to the FPGA. 
// The goal of this project is to make it as easy as possible for any new user to establish 
// a communication between the CPU and the FPGA in a custom bitstream, as most commonly 
// the first problem you encounter when creating your own bitstream is that you lose the 
// ability to talk to the FPGA.
//
// When replacing this bitstream with your own, make sure to compile it correctly with Intel Quartus
//  (try the attached Quartus project which the default bitstream is generated from).
//
// When successfully compiled, take the output file ( output_files/MKRVIDOR4000.ttf ) and
// bit-reverse it using the ByteReverser:
//
//      https://github.com/HerrNamenlos123/bytereverse
// 

#include "FPGA.h"

void setup() {

	Serial.begin(115200);	// Wait for serial monitor to open
	while(!Serial);

	// Next, upload the FPGA bitstream and initialize the JTAG_Interface (takes a sec)
	// You must specify the register bit width and the number of registers.

	if (!FPGA.begin(32, 16)) {							// You should always check this
		Serial.println("JTAG FPGA mismatch. Error:");
		Serial.println(FPGA.getErrorMessage());
		while (true);
	}

	Serial.println("JTAG initialized");

}

void loop() {

	// Bits 0, 1 & 2 of output register 0 are connected to 
	// outputs D6, D7 and D8 on the FPGA (see MKRVIDOR4000_top.v)
	FPGA.write(0, 0b111);

	// WARNING: DO NOT SET D6, D7 OR D8 AS AN OUTPUT HERE!
	// When pins are configured as outputs on the CPU and the
	// FPGA at the same time, your board will be damaged!

	// Bits 0, 1 & 2 of input register 0 are connected to digital
	// inputs A0, A1 & A2 on the FPGA (see MKRVIDOR4000_top.v)
	uint8_t pins = FPGA.read(0);
	Serial.print("A0 = "); Serial.println(pins & 0b001 > 0);
	Serial.print("A1 = "); Serial.println(pins & 0b010 > 0);
	Serial.print("A2 = "); Serial.println(pins & 0b100 > 0);
	Serial.println();

	// On the FPGA, a counter is connected to input 9
	uint32_t ticks = FPGA.read(9);
	Serial.print("The FPGA program has been running for ");
	Serial.print(ticks);
	Serial.print(" ticks, that is ");
	Serial.print(ticks / 120000000.0);
	Serial.println(" seconds.");

	// When the data type is not an int, you can use FPGA.toByte<>() and FPGA.fromByte<>()
	// It converts your non-int data to an int64_t and transmits it. Your data can only be 64 bit max.
	FPGA.write(5, FPGA.toBytes<float>(18.7f));		// Writing a 32-bit float
	
	// This function reads the bytes and then converts it to your 64-bit double.
	// WARNING: To do this, you must set the register size to 64 bits in your FPGA and in FPGA.begin()

	//double value = FPGA.fromBytes<double>(FPGA.read(3));


    // This functions writes the variable to register 7 while simultaneously
    // reading from register 3. This function is the most efficient for data throughput,
    // so make sure to use it when you need to read and write at the same time. It takes
    // just as long as the other functions.
    uint32_t write = 5;
    uint32_t read = FPGA.transfer(3, 7, write);

	delay(50);

}
