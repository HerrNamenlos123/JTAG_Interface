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

//
// This library gives you access to the JTAG protocol, which internally connects the CPU and
// the FPGA chip on the Arduino MKR Vidor 4000. Because it is connected internally, this allows
// you to communicate with the FPGA at runtime without using any external pins. Therefore you can 
// make use of the full potential of the Vidor 4000.
// 
// If you want to know more, please visit https://github.com/HerrNamenlos123/JTAG_Interface
// or open the file FPGA/projects/example_simple/jtag_memory.v
//

#ifndef FPGA_H
#define FPGA_H

#ifndef ARDUINO_SAMD_MKRVIDOR4000
  	#error “This library is exclusively for the Arduino MKR Vidor 4000.”
#endif

#include "Arduino.h"

struct _ModuleInfo {
	int registerSize = 0;
	int numberOfRegisters = 0;
};

class _FPGA {
public:
	_FPGA();

	///
	/// @brief Upload the FPGA_Bitstream.h to the FPGA.
	/// @return bool - false if the register size/num configuration
	/// does not match with the FPGA module. 
	///
	bool begin(int registerWidth, int numOfRegisters);

	///
	/// @brief Stop the JTAG communication, in case you need the pins for something else.
	/// You wouldn't usually use this function. In rare cases, it could be used to clear the
	/// error and dynamically reload in the future.
	///
	void end();

	///
	/// @brief Reads bytes and returns them. Internally 64-bit integers are used, but the bits
	/// can be interpreted as any data type you want. Use the template functions 
	/// FPGA.toBytes<>() and FPGA.fromBytes<>() to convert to your data type.
	///
	int64_t read(uint8_t index);

	///
	/// @brief Writes bytes. Internally 64-bit integers are used, but the bits can be interpreted as any data
	/// type you want. Use the template functions FPGA.toBytes<>() and FPGA.fromBytes<>() to convert
	/// to your data type.
	///
	void write(uint8_t index, int64_t value);

	///
	/// @brief Transfers bytes in both directions at the same time, this is the most efficient function 
	/// if data needs to be read and written. Use the template functions FPGA.toBytes<>() and FPGA.fromBytes<>()
	/// to convert to your data type. Internally 64-bit integers are used, but the bits can be interpreted 
	/// as any data type you want.
	///
	int64_t transfer(uint8_t readIndex, uint8_t writeIndex, int64_t value);

	///
	/// @brief Transfers bytes. This is the underlying function, only use it if you know what you're doing
	///
	void transferBytes(const void* txBuffer, uint8_t txIndex, void* rxBuffer, uint8_t rxIndex, uint8_t bits);

	///
	/// @brief Returns the pointer to the error message. If there was no error, the message is empty.
	///
	const char* getErrorMessage();

	///
	/// @brief These functions help you to convert your data type. 
	/// Refer to the example to see how to use it.
	///
	template<typename T>
	int64_t toBytes(T value) {
		int64_t bytes = 0;
		memcpy(&bytes, &value, min(sizeof(bytes), sizeof(value)));
		return bytes;
	}

	///
	/// @brief These functions help you to convert your data type. 
	/// Refer to the example to see how to use it.
	///
	template<typename T>
	T fromBytes(int64_t bytes) {
		T value = 0;
		memcpy(&value, &bytes, min(sizeof(bytes), sizeof(value)));
		return value;
	}

private:
	uint32_t makeAddress(uint8_t writeAddr, uint8_t readAddr);
	struct _ModuleInfo getIdentifier();

	void incrementStateMachine(uint8_t numticks, uint16_t path);
	void readRaw(uint16_t IR, void* data, uint32_t numbits);
	void writeRaw(uint16_t IR, const void* data, uint32_t numbits);
	void transferRaw(uint16_t IR, const void* send, void* recv, uint32_t numbits);

	void setup();
	void shutdown();
	void pulseTCK(bool tms);
	void pulseTDI(const void* data, size_t size);
	void pulseTDO(void* data, size_t size);
	unsigned int pulseTDIO(int bits, unsigned int out);
	unsigned int pulseTDIO_instruction(int bits, unsigned int out);
	void pulseTDIO_SPI(const void* send, void* recv, size_t size);

	char errorMessage[128];
	bool error = false;

	int registerWidth = 0;
	int numOfRegisters = 0;
	int totalRegisters = 0;
	int addressWidth = 0;
	uint32_t addressBitmask = 0;

	const int IDRegSize = 16;	// This value is fixed 
};

extern _FPGA FPGA;

#endif // FPGA_H
