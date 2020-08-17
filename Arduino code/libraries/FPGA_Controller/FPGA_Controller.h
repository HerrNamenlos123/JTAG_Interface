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

#ifndef FPGA_CONTROLLER_H
#define FPGA_CONTROLLER_H

#include "Arduino.h"

// Upload the bitstream in the file FPGA_Bitstream.h to the FPGA
void uploadBitstream();



//
// This software gives you access to the JTAG protocol, which internally connects the CPU and
// the FPGA chip on the Arduino MKR Vidor 4000. Because it is connected internally, this allows
// you to communicate with the FPGA at runtime without using any external pins. Therefore you can 
// make use of the full potential of the Vidor 4000.
// 
// On the FPGA side of the JTAG interface, there are 15 registers you can write to and 15 signal
// bands you can read from. All of those have a bandwidth of 32 bits. Whenever transferring data,
// whether it's writing to or reading from the FPGA or even both at the same time, you need to 
// specify an address to write to and/or an address to read from.
//
// The addresses for reading or writing are 4 bits wide, while addresses from 0 to 14 access a specific
// register and the address 15 (0b1111) writes to nothing and returns 0 when reading from it.
//
// When supplying an address to a function make sure it's type is uint8_t aka unsigned char and
// only the 4 lowest bits are used.
//
// Whenever possible use readWriteJTAG() to read and write to specific registers, as it reads and
// writes at the same time which essentially makes communication twice as fast.
//
//
//
// (Explanation of the internal workings)
//   
// The address is 9 bits long, while the leftmost bit is ignored and the lower 8 bits contain
//   the 4-bit addresses for reading and writing.
//
// The following example would write to register 9 while simultaneously reading from register 13:
//
//               110011101  ->   1  1001  1101
//                               ^    ^     ^ 
//                              /     |      \
//              Must always be 1      |       Address to read from
//                                    |
//                            Address to write to
//
// The 4-bit addresses can range from 0 to 14 (0b0000 to 0b1110) while 15 (0b1111) means not used,
//   which writes to nothing and reads all 0s.
//
//
// When a communication is started, the current data on the input bus chosen by the read address in
// the address register is latched into a shift register on the FPGA. Then this content is shifted out
// to the CPU and new data is shifted in from the CPU. When communication is done, the content of the 
// shift register (new data from the cpu) is latched into the corresponding register chosen by the write
// part of the address. This way you can read and write to different registers at the same time.
//
// When only reading or only writing, the address of the other unused operation is set to 15 (0b1111),
// which ignores incoming data and returns 0 when reading.
//
// Any data written to registers will remain there until overridden by a new transfer and all registers
// start with the value 0 on startup.
//
// The values written to and read from the FPGA are taken as int32_t aka long, but they can simply be 
// casted to unsigned int or any other type if needed. Just make sure the bitwise representation when
// stored in a uint32_t is correct, as the FPGA doesn't care about data types anyways. You just need
// to interpret the bits correctly in your design.
//
//
// Example of using the interface:
//
//	#include "FPGA_Controller.h" 
//
//	void setup() {
//
//		uploadBitstream();  // Upload the bitstream in FPGA_Bitstream.h to the FPGA
//
//		initJTAG();         // Initialize the interface, call this once on startup
//
//		write(4, 187);      // Write the value 187 to the register with id 4 
//
//		int32_t result = readJTAG(7);       // Read the value on input bus 7
//
//		                                    // Write the value 467 to register 11 while reading
//		result = readWriteJTAG(2, 11, 467); //  the value from input bus 2
//
//		<optional>
//		closeJTAG();    // Communication can be stopped when needed, but in most cases it's best to 
//		                // just let it run for the lifetime of the program
//	}
//

void    initJTAG();
void	closeJTAG();
int32_t readJTAG(uint8_t readAddress);
void    writeJTAG(uint8_t writeAddress, int32_t value);
int32_t readWriteJTAG(uint8_t readAddress, uint8_t writeAddress, int32_t value);

#endif // FPGA_CONTROLLER_H
