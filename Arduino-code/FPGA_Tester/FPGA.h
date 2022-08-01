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

bool    initJTAG(uint8_t registerSize, uint8_t numberOfRegisters);
//void	closeJTAG();
//int32_t readJTAG(uint8_t readAddress);
//void    writeJTAG(uint8_t writeAddress, int32_t value);
//int32_t readWriteJTAG(uint8_t readAddress, uint8_t writeAddress, int32_t value);

void jtag_host_setup();
void jtag_host_shutdown();
void jtag_host_pulse_tms(bool tms);
void jtag_host_rw_bytes(uint8_t* recv, uint8_t* send, size_t size);
void jtag_host_rw_bits(uint8_t* recv, uint8_t* send, uint8_t write);
void jtag_host_rw(uint8_t* recv, uint8_t* send, size_t bits);



void jtag_host_pulse_tck(int tms);
void jtag_host_pulse_tdi(const uint8_t* data, size_t size);
void jtag_host_pulse_tdo(uint8_t* data, size_t size);
unsigned int jtag_host_pulse_tdio(int bits, unsigned int out);
unsigned int jtag_host_pulse_tdio_instruction(int bits, unsigned int out);
void jtag_host_pulse_tdio_spi(const uint8_t* send, uint8_t* recv, size_t size);

void test();

#endif // FPGA_CONTROLLER_H
