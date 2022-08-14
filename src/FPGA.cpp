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

#include "FPGA.h"
#include "upload.h"
#include <SPI.h>

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

_FPGA FPGA;

extern void enableFpgaClock(void);

_FPGA::_FPGA() {
	memset(errorMessage, 0, sizeof(errorMessage));
}

bool _FPGA::begin(int registerWidth, int numOfRegisters) {

	this->registerWidth = registerWidth;
	this->numOfRegisters = numOfRegisters;
	totalRegisters = numOfRegisters + 1;
	addressWidth = ceil(log2(totalRegisters));
	for (int i = 0; i < addressWidth; i++) {
		addressBitmask <<= 1;
		addressBitmask |= 1;
	}

	if (registerWidth < 8 || registerWidth > 64) {
		strncpy(errorMessage, "Register width is out of bounds. Values between 8 and 64 are allowed.", 
			sizeof(errorMessage));
		error = true;
		return false;
	}

	if (numOfRegisters < 2 || numOfRegisters > 254) {
		strncpy(errorMessage, "Number of registers is out of bounds. Values between 2 and 254 are allowed.", 
			sizeof(errorMessage));
		error = true;
		return false;
	}

	// Upload the bitstream to the FPGA
    enableFpgaClock();
    __uploadBitstream();

	// Initialize virtual JTAG client
    setup();
    
	// Check the JTAG configuration
	struct _ModuleInfo info = FPGA.getIdentifier();

	if (info.registerSize == 0 && info.numberOfRegisters == 0) {
		strncpy(errorMessage, "Looks like the JTAG module did not respond properly. "
			"Make sure the right FPGA bitstream is being loaded.", sizeof(errorMessage));
		error = true;
		return false;
	}

	if (info.registerSize != registerWidth) {
		strncpy(errorMessage, "The register size does not match the JTAG module on the FPGA. "
			"Make sure the right FPGA bitstream is being loaded.", sizeof(errorMessage));
		error = true;
		return false;
	}

	if (info.numberOfRegisters != numOfRegisters) {
		strncpy(errorMessage, "The number of usable registers does not match the JTAG module on the FPGA. "
			"Make sure the right FPGA bitstream is being loaded.", sizeof(errorMessage));
		error = true;
		return false;
	}
	
	return true;
}

void _FPGA::end() {
    shutdown();
	error = false;
}











#define JTAG_WRITE_INSTRUCTION(data, bits) writeRaw(14, data, bits);
#define JTAG_READ_DATA(data, bits) readRaw(12, data, bits);
#define JTAG_WRITE_DATA(data, bits) writeRaw(12, data, bits);
#define JTAG_TRANSFER_DATA(send, recv, bits) transferRaw(12, send, recv, bits);

uint32_t _FPGA::makeAddress(uint8_t writeAddr, uint8_t readAddr) {
	uint32_t address = 1 << (addressWidth * 2);
	address |= (writeAddr & addressBitmask) << addressWidth;
	address |= (readAddr & addressBitmask);
	return address;
}
    

struct _ModuleInfo _FPGA::getIdentifier() {
	_ModuleInfo info;

    uint8_t id[2] = { 0, 0 };
	transferBytes(nullptr, -1, &id, -1, IDRegSize);

	info.numberOfRegisters = id[0];
	info.registerSize = id[1];
	return info;
}

void _FPGA::incrementStateMachine(uint8_t numticks, uint16_t path) {
    for(int i = 0; i < numticks; i++, path >>= 1) pulseTCK(path & 0x0001);
}

int64_t _FPGA::read(uint8_t index) {
	if (error) return 0;

    if (index >= numOfRegisters) {
        return 0;
    }

    int64_t recv = 0;
	transferBytes(nullptr, -1, &recv, index, registerWidth);
    return recv;
}

void _FPGA::write(uint8_t index, int64_t value) {
	if (error) return;

    if (index >= numOfRegisters) {
        return;
    }

	transferBytes(&value, index, nullptr, -1, registerWidth);
}

int64_t _FPGA::transfer(uint8_t readIndex, uint8_t writeIndex, int64_t value) {
	if (error) return 0;

    if (readIndex >= numOfRegisters || writeIndex >= numOfRegisters) {
        return 0;
    }

    int64_t recv = 0;
	transferBytes(&value, writeIndex, &recv, readIndex, registerWidth);
    return recv;
}

void _FPGA::transferBytes(const void* txBuffer, uint8_t txIndex, void* rxBuffer, uint8_t rxIndex, uint8_t bits) {
	if (error) return;

	int64_t writeDummy = 0, readDummy = 0;
	const void* _txBuffer = (txBuffer != nullptr) ? txBuffer : &writeDummy;
	void* _rxBuffer = (rxBuffer != nullptr) ? rxBuffer : &readDummy;

	uint32_t address = makeAddress(txIndex, rxIndex);
    JTAG_WRITE_INSTRUCTION(&address, addressWidth * 2 + 1);
    JTAG_TRANSFER_DATA(_txBuffer, _rxBuffer, bits);
}

const char* _FPGA::getErrorMessage() {
	return errorMessage;
}









#define JTAG_ANY_TO_SIR() incrementStateMachine(10, 0b0011011111);	// from any state to: shift IR	(instruction register)
#define JTAG_SIR_TO_SDR() incrementStateMachine(5, 0b00111);		// from shift IR to shift DR	(data register)
#define JTAG_RESET() incrementStateMachine(5, 0b11111);				// from any state to reset

void _FPGA::readRaw(uint16_t IR, void* data, uint32_t numbits) {
	uint8_t* _data = (uint8_t*)data;

    JTAG_ANY_TO_SIR();
    pulseTDIO_instruction(10, (unsigned int)IR);
    JTAG_SIR_TO_SDR();

    if (numbits / 8 > 0) pulseTDO(_data, (size_t)(numbits >> 3));
    if ((numbits & 0b111) != 0) _data[numbits >> 3] = pulseTDIO((int)(numbits & 0b111), 0);

    JTAG_RESET();
}

void _FPGA::writeRaw(uint16_t IR, const void* data, uint32_t numbits) { 
	uint8_t* _data = (uint8_t*)data;

    JTAG_ANY_TO_SIR();
    pulseTDIO_instruction(10, (unsigned int)IR);
    JTAG_SIR_TO_SDR();

    int NumBytes = numbits >> 3;
    int NumBits = numbits & 0x07;
    if (NumBits == 0) {
        NumBits = 8;
        NumBytes = NumBytes - 1;
    }

    if (NumBytes > 0) pulseTDI(_data, (size_t)(NumBytes));
    pulseTDIO(NumBits, (unsigned int)_data[NumBytes]);
    JTAG_RESET();
}

void _FPGA::transferRaw(uint16_t IR, const void* send, void* recv, uint32_t numbits) {
	uint8_t* _send = (uint8_t*)send;
	uint8_t* _recv = (uint8_t*)recv;

    JTAG_ANY_TO_SIR();
    pulseTDIO_instruction(10, (unsigned int)IR);
    JTAG_SIR_TO_SDR();

    int NumBytes = numbits >> 3;
    int NumBits = numbits & 0b111;
    if (NumBits == 0) {
        NumBits = 8;
        NumBytes = NumBytes - 1;
    }
    
    if (NumBytes > 0) pulseTDIO_SPI(_send, _recv, (size_t)(NumBytes));
    _recv[NumBytes] = pulseTDIO(NumBits, (unsigned int)_send[NumBytes]);
    JTAG_RESET();
}














void _FPGA::setup() {

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

void _FPGA::shutdown() {

	SPI_JTAG.endTransaction();
	SPI_JTAG.end();

	TMS_LOW();
	TCK_LOW();
	TDI_LOW();

	pinMode(TMS, INPUT);
	pinMode(TCK, INPUT);
	pinMode(TDO, INPUT);
	pinMode(TDI, INPUT);
}

void _FPGA::pulseTCK(bool tms) {
	TMS_WRITE(tms);
	TCK_LOW();
	TCK_HIGH();
}

void _FPGA::pulseTDI(const void* data, size_t size) {

	TCK_LOW();
	TCK_PMUX();
	TDI_PMUX();

	for (size_t i = 0; i < size; i++) {
		SPI_JTAG.transfer(*(const uint8_t*)data++);
	}

	TCK_UNPMUX();
	TDI_UNPMUX();
}

void _FPGA::pulseTDO(void* _data, size_t size) {
    uint8_t* data = (uint8_t*)_data;

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

unsigned int _FPGA::pulseTDIO(int bits, unsigned int out) {

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

unsigned int _FPGA::pulseTDIO_instruction(int bits, unsigned int out) {

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

void _FPGA::pulseTDIO_SPI(const void* send, void* recv, size_t size) {
	const uint8_t* _send = (const uint8_t*)send;
	uint8_t* _recv = (uint8_t*)recv;

	TCK_LOW();
	TCK_PMUX();
	TDI_PMUX();
	TDO_PMUX();

	for (size_t i = 0; i < size; i++) {
		*_recv = SPI_JTAG.transfer(*_send++);
		_recv++;
	}

	TCK_UNPMUX();
	TDI_UNPMUX();
	TDO_UNPMUX();
}
