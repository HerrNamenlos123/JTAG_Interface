
#include "FPGA_Controller.h"

void setup() {

	Serial.begin(115200); // Serial.begin() should be called before uploadBitstream

	// Upload the bitstream to the FPGA
	uploadBitstream();

	while(!Serial);

	Serial.println("Welcome!");

	Serial.println("Uploading bitstream...");
	Serial.println("Done Uploading!");

	// Warning: The digital pins 6, 7 and 8 are configured as outputs
	// on the FPGA, so shortening them out or configuring them as outputs
	// on the CPU as well may kill your Arduino permanently

	initJTAG();
}

void loop() {

	// Make some space
	Serial.println("\n\n\n\n");

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
	int32_t oldValue = readWriteJTAG(13, 14, newValue);

	Serial.print("The register #14 just contained the value ");
	Serial.print(oldValue);
	Serial.print(", which was replaced by the value ");
	Serial.println(newValue);

	delay(500);

}
