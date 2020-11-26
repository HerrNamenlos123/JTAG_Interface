# JTAG_Interface

## Content
[What is the JTAG_Interface?](#what-is-the-jtag_interface)

[How do I install it?](#how-do-i-install-it)

[How do I use it?](#how-do-i-use-it)

[How it works internally](#how-it-works-internally)

[Something's not working...](#somethings-not-working)

[Webpage for JTAG_Interface](https://herrnamenlos123.github.io/JTAG_Interface/)


## Disclaimer

This piece of software is open source and you can use it for anything you like. You can download, modify and even publish projects using it, but whenever publishing a project that uses parts or at least code snippets of this repository, you need to give me credit somewhere.

Credit also goes to @riccardo_giacomazzi aka [Giako68](https://github.com/Giako68), who not only helped me in the Arduino forum, but also published a Vidor 4000 FPGA project which also uses JTAG. This project partially inspired my JTAG_Interface and parts of it are based on his TiledScreen application:
https://github.com/Giako68/TiledScreen

This project is written with the intention to be useful to as many people as possible and I do not feel responsible for any damages that might occur to your device. 


## What is the JTAG_Interface?

The JTAG_Interface is a project to help any new developer using the Arduino MKR Vidor 4000 to establish a communication between the CPU and the FPGA. When using a default library like VidorGraphics or VidorPeripherals the FPGA magically reacts to any command you give it in the Arduino IDE. But as soon as you start to create your own bitstreams using Intel Quartus, you will quickly realize that there is no obvious way to exchange data or commands between the CPU and the FPGA anymore.

Well, this has changed now! May I introduce: The JTAG_Interface!

Okay, it's actually not THAT exciting, but the heart of this project is a few Quartus modules for your custom bitstream and a few C++ files you can include in your Arduino program. This not only cares about communication via JTAG, but it also cares for you about uploading the bitstream to the FPGA, so this could be seen as a general template for custom FPGA bitstreams.

Here you can see the main module for Intel Quartus:

![JTAG_Interface module](https://github.com/HerrNamenlos123/JTAG_Interface/blob/master/images/interface.png)

This module simply needs a clock, I would recommend the 120 MHz clock `wCLK120`. Besides that, only the software on the MCU must be running correctly, then everything should work. Whenever your program calls a writeJTAG or readJTAG or readWriteJTAG function, the value currently on the left side of the module from the corresponding index is read and the given value is written into memory and will stay on the right side of the module until it is overwritten.

## How do I install it?

The JTAG Interface is pretty easy to install. Just download the zip file containing the code and unzip it. To use it directly, go to the folder `Arduino code/libraries/`, copy the folder `FPGA_Controller` and paste it into the libraries folder in your Sketchbook (Your sketchbook location is by default `C:/Users/<USERNAME>/Documents/Arduino`). The library is now installed! To get a first test running all you need to do is to open the file `Arduino code/FPGA_Tester/FPGA_Tester.ino` from the repository with the Arduino IDE and upload it. Now everything should be working!

If you now want to go further, you can open the Intel Quartus project `FPGA code/projects/JTAG_Interface/MKRVIDOR4000.qpf` with Intel Quartus (obviously :P) and compile it. When compilation was successful, you need to bit-reverse the output file (`FPGA code/projects/JTAG_Interface/output_files/MKRVIDOR4000.ttf`). For that, refer to my reply i posted to this topic:

https://forum.arduino.cc/index.php?topic=700223.msg4710460#msg4710460

Once you successfully reversed the bitstream, simply rename it to `FPGA_Bitstream.h` if the output of the script was something different and paste it into your previously pasted `libraries/FPGA_Controller` directory in your sketchbook. That will overwrite the default bitstream of this project and your own custom bitstream will be uploaded next time.

Make sure to use the script i posted in the linked forum post above, which will make it very easy for you to regenerate the bitstream automatically. 

Now upload the example again and see if everything is still working. If yes, you can now finally start editing the Quartus project and your Arduino files to do something amazing!

## How do I use it?

For information on how to use it, I think it's best to just look at the examples, certainly the `Arduino code/FPGA_Tester/FPGA_Tester.ino` and the Quartus project. You can either keep modifying the project or you can copy and paste all JTAG relevant modules into your own, already existing project.

Here you can see the example file, which is `FPGA code/projects/JTAG_Interface/MyDesign.bdf`.

![JTAG_Interface example usage](https://github.com/HerrNamenlos123/JTAG_Interface/blob/master/images/example.png)

If you want to start creating your own designs, open `MKRVIDOR4000_top.v`. This is where everything is based on and from here you can route any signals to your own designs, like it is done with `MyDesign.bdf`.

Well, there's not much more to say, try playing around and if you're having trouble with anything just [go to the trouble section](#somethings-not-working).


## How it works internally

I won't write the explanation here, but if you still want to know more precisely how it works, take a look at the file `Arduino code/libraries/FPGA_Controller/FPGA_Controller.h` where it is explained in quite some detail.

## Something's not working...

In case something isn't working as expected or you're having trouble getting JTAG to work or you need help modifying the project to fit your needs, don't hesitate to contact me at:  
<HerrNamenlos123@gmail.com>

Don't be shy, just write something like `Hey, i can't get my project to work...` and I would be glad to be helpful.

Also, in case you find a bug or want to request a feature or anything similar, please go to the [issues tab](https://github.com/HerrNamenlos123/JTAG_Interface/issues) of the repository and create an issue.

Now as that's out of the way, let's get coding and create something amazing!
