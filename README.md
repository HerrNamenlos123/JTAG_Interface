# JTAG_Interface

## Content
What is the JTAG_Interface?

[Go to Real Cool Heading section](#What-is-the-JTAG_Interface?)

How do i install it?

How do i use it?

How does it work internally?


# What is the JTAG_Interface?

The JTAG_Interface is a project to help any new developer using the Arduino MKR Vidor 4000 to establish a communication between the CPU and the FPGA. When using a default library like VidorGraphics or VidorPeripherals the FPGA magically reacts to any command you give it in the Arduino IDE. But as soon as you start to create your own bitstreams using Intel Quartus, you will quickly realize that there is no obvious way to exchange data or commands between the CPU and the FPGA anymore.

Well, this has changed now! May I introduce: The JTAG_Interface!

Okay, it's actually not THAT exciting, but the heart of this project is a few Quartus modules for your custom bitstream and a few C++ files you can include in your Arduino program. This not only cares about communication via JTAG, but it also cares for you about uploading the bitstream to the FPGA, so this could be seen as a general template for custom FPGA bitstreams.
