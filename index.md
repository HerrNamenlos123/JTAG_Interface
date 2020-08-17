# JTAG_Interface

## Content
[What is the JTAG_Interface?](#what-is-the-jtag_interface)

[How do I install it?](#how-do-i-install-it)

[How do I use it?](#how-do-i-use-it)

[How it works internally](#how-it-works-internally)

[Something's not working...](#somethings-not-working)


## Disclaimer

This piece of software is open source and you can use it for anything you like. You can download, modify and even publish projects using it, but whenever publishing a project that uses parts or at least code snippets of this repository, you need to give me credit somewhere.

Credit also goes to @riccardo_giacomazzi aka [Giako68](https://github.com/Giako68), who not only helped me in the Arduino forum, but also published a Vidor 4000 FPGA project which also uses JTAG. This project partially inspired my JTAG_Interface and parts of it are based on his TiledScreen application:
https://github.com/Giako68/TiledScreen

This project is written with the intention to be useful to as many people as possible and I do not feel responsible for any damages that might occur to your device. 


## What is the JTAG_Interface?

The JTAG_Interface is a project to help any new developer using the Arduino MKR Vidor 4000 to establish a communication between the CPU and the FPGA. When using a default library like VidorGraphics or VidorPeripherals the FPGA magically reacts to any command you give it in the Arduino IDE. But as soon as you start to create your own bitstreams using Intel Quartus, you will quickly realize that there is no obvious way to exchange data or commands between the CPU and the FPGA anymore.

Well, this has changed now! May I introduce: The JTAG_Interface!

Okay, it's actually not THAT exciting, but the heart of this project is a few Quartus modules for your custom bitstream and a few C++ files you can include in your Arduino program. This not only cares about communication via JTAG, but it also cares for you about uploading the bitstream to the FPGA, so this could be seen as a general template for custom FPGA bitstreams.

## How do I install it?

The JTAG Interface is pretty easy to install. Just download the zip file containing the code and unzip it. To use it directly, go to the folder `Arduino code/libraries/`, copy the folder `FPGA_Controller` and paste it into the libraries folder in your Sketchbook (Your sketchbook location is usually `.../Documents/Arduino`). The library is now installed! To get a first test running all you need to do is to open the file `Arduino code/FPGA_Tester/FPGA_Tester.ino` with the Arduino IDE and upload it. Now everything should be working!

If you now want to go further, you can open the Intel Quartus project with Intel Quartus (obviously :P) and compile it. When compilation was successful, you need to bit-reverse the output file (`FPGA code/projects/JTAG_PenetrationTest/output_files/MKRVIDOR4000.ttf`). For that, refer to the following page:

https://systemes-embarques.fr/wp/archives/mkr-vidor-4000-programmation-du-fpga-partie-1/  
You should only need to refer to the part which explains reversing the bitstream, as other things like uploading it are already handled by this project.
The rest of it might also be interesting tho (Google translate does the trick). 

Once you successfully reversed the bitstream, simply rename it to `FPGA_Bitstream.h` and paste it into your previously created `libraries/FPGA_Controller` directory in your sketchbook. That will overwrite the default bitstream of this project and your own custom bitstream will be uploaded next time. 

Now upload the example again and see if everything is still working. If yes, you can now finally start editing the Quartus project and your Arduino files to do something amazing!

## How do I use it?

For information on how to use it, I think it's best to just look at the examples, certainly the `Arduino code/FPGA_Tester/FPGA_Tester.ino` and the Quartus project. You can either keep modifying the project or you can copy and paste all JTAG relevant modules into your own, already existing project.

Well, there's not much more to say, if you're having trouble with anything, just [go to the trouble section](#somethings-not-working).


## How it works internally

This software gives you access to the JTAG protocol, which internally connects the CPU and
the FPGA chip on the Arduino MKR Vidor 4000. Because it is connected internally, this allows
you to communicate with the FPGA at runtime without using any external pins. Therefore you can 
make use of the full potential of the Vidor 4000.
On the FPGA side of the JTAG interface, there are 15 registers you can write to and 15 signal
bands you can read from. All of those have a bandwidth of 32 bits. Whenever transferring data,
whether it's writing to or reading from the FPGA or even both at the same time, you need to 
specify an address to write to and/or an address to read from.
The addresses for reading or writing are 4 bits wide, while addresses from 0 to 14 access a specific
register and the address 15 (0b1111) writes to nothing and returns 0 when reading from it.
When supplying an address to a function make sure it's type is uint8_t aka unsigned char and
only the 4 lowest bits are used.
Whenever possible use readWriteJTAG() to read and write to specific registers, as it reads and
writes at the same time which essentially makes communication twice as fast.


(Explanation of the internal workings)
  
The address is 9 bits long, while the leftmost bit is ignored and the lower 8 bits contain
  the 4-bit addresses for reading and writing.
  
The following example would write to register 9 while simultaneously reading from register 13:

              110011101  ->   1  1001  1101
                              ^    ^     ^ 
                             /     |      \
             Must always be 1      |       Address to read from
                                   |
                           Address to write to
                           
The 4-bit addresses can range from 0 to 14 (0b0000 to 0b1110) while 15 (0b1111) means not used,
  which writes to nothing and reads all 0s.
  
When a communication is started, the current data on the input bus chosen by the read address in
the address register is latched into a shift register on the FPGA. Then this content is shifted out
to the CPU and new data is shifted in from the CPU. When communication is done, the content of the 
shift register (new data from the cpu) is latched into the corresponding register chosen by the write
part of the address. This way you can read and write to different registers at the same time.

When only reading or only writing, the address of the other unused operation is set to 15 (0b1111),
which ignores incoming data and returns 0 when reading.

Any data written to registers will remain there until overridden by a new transfer and all registers
start with the value 0 on startup.

The values written to and read from the FPGA are taken as int32_t aka long, but they can simply be 
casted to unsigned int or any other type if needed. Just make sure the bitwise representation when
stored in a uint32_t is correct, as the FPGA doesn't care about data types anyways. You just need
to interpret the bits correctly in your design.

Example of using the interface:

	#include "FPGA_Controller.h" 
	
	void setup() {
	
		uploadBitstream();  // Upload the bitstream in FPGA_Bitstream.h to the FPGA
		initJTAG();         // Initialize the interface, call this once on startup
		
		write(4, 187);      // Write the value 187 to the register with id 4 
		
		int32_t result = readJTAG(7);       // Read the value on input bus 7
		
		
		                                    // Write the value 467 to register 11 while reading
		result = readWriteJTAG(2, 11, 467); //  the value from input bus 2
		
		
		<optional>
		closeJTAG();    // Communication can be stopped when needed, but in most cases it's best to 
		                // just let it run for the lifetime of the program
	}

## Something's not working...

In case something isn't working as expected or you're having trouble getting JTAG to work or you need help modifying the project to fit your needs, don't hesitate to contact me at:  
<HerrNamenlos123@gmail.com>

Also, in case you find a bug or want to request a feature or anything similar, please go to the [issues tab](https://github.com/HerrNamenlos123/JTAG_Interface/issues) of the repository and create an issue.

Now as that's out of the way, let's get coding and create something amazing!
