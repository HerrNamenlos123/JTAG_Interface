//
// jtag_interface.v is the main module if you want to go barebones. jtag_interfaceXX.v is to make it easier for you.
//   All other jtag_... files are internal and not to be used manually. If you want to copy this into your own project,
//   simply copy/paste all files starting with jtag_... . That's it.
//
// This module is completely configurable with parameters (register bit width & number of registers) and has input/output
//   registers as 2-dimensional arrays. This can be used in Verilog, but is not supported in block design files. This is
//   why jtag_interface7.v, jtag_interface15.v, jtag_interface31.v, ... exist. They basically just wrap the original
//   main module (jtag_interface.v) and map the 2-dimensional arrays to a fixed number of registers, so that they can
//   be used graphically in block design files.
//
// Parameters: 
//  REGISTER_SIZE		 -> bit width of each register
// 	NUMBER_OF_REGISTERS  -> number of usable registers (+1 for the ID)
//
// This entire protocol works like a shift register. The Altera Virtual JTAG instance provides us with an 
//   address register, a data out pin and three control signals. These are synchronized with the main clock to prevent
//   undefined behaviour and are then fed into this memory block. 
//
// When transaction starts, first the address is shifted bit-by-bit into the Altera Virtual address register 
//   (not handled by us). This has a fixed bit-width, defined by the parameters provided to this FPGA module.
//   Now the data from the reading index (which in contained in the address like described below) is latched so that
//   it cannot change during transmission. Now the data is shifted in and out at the same time. Out from the latched input
//   and IN to the temporary register. When it's done the temporary register is latched to the corresponding output specified
//   by the address. This means you don't need to care about signals changing during transmission. Basically an 
//   instantaneous snapshot is taken before transmitting.
//
// When both write and read indices are -1 at the same time, 
//   a 16-bit identifier value is shifted out, containing the register width (high byte) 
//   and the number of usable registers (low byte). It is used in the Arduino program at startup to check
//   for bit width mismatches, preventing configuration mistakes.
//
// The address in the instruction register contains both the write and read index. Its width depends on the
//   configuration. The total number of registers is used -> usable registers + 1.
//
//   addressWidth = ceil(log2(TOTAL_NUMBER_OF_REGISTERS + 1))
// 
// Example used for demonstration below: 
//   NUMBER_OF_USABLE_REGISTERS = 16       ->      addressWidth = 5
//
//   In this example the address would be 11 bits: addressWidth * 2 + 1(reserved).
//
//                                  1  00000  00000
//                                  ^    ^      ^ 
//                                 /     |       \
//                 Must always be 1      |       Index to read from
//                                       |
//                              Index to write to
//
// When the total number of registers is not a power of 2 there are free register addresses: For this reason
//   the number of registers available by default seem so off (3, 7, 15, 31, ...). These are the most memory
//   and speed efficient. More registers mean a wider address, which is more data to transmit, which is slower.
//   These numbers each offer the highest number of registers without making the data transaction slower.
//
// The parameters are by default limited to 255 registers with 64 bits each. However, this restriction is only
//   on the Arduino side. The FPGA side can take much more than that, but you would have to adapt the 
//   Arduino library.
//
// For now, only one JTAG_Interface can be instanced in one FPGA program. (Only the first one is chosen 
//   for transaction). In the future, it might be possible to use multiple instances and reference them
//   using their ID, but it would be more complicated. Let me know if you would be interested in this feature!
//

module jtag_memory #(
	parameter REGISTER_SIZE,
	parameter NUMBER_OF_REGISTERS
) (
	input iTCK,
	input iTDI,
	input iSTATE_SDR,
	input iSTATE_CDR,
	input iSTATE_UDR,
	output oTDO,
	
	input [ADDRESS_WIDTH*2:0] iADDRESS,
	input [NUMBER_OF_REGISTERS-1:0][REGISTER_SIZE-1:0] iDATA,
	output [NUMBER_OF_REGISTERS-1:0][REGISTER_SIZE-1:0] oDATA
);

localparam NUMBER_OF_ALL_REGISTERS = NUMBER_OF_REGISTERS + 1;
localparam ADDRESS_WIDTH = $clog2(NUMBER_OF_ALL_REGISTERS);
localparam IDREG_SIZE = 16;

wire [ADDRESS_WIDTH-1:0] NEG_ONE;
assign NEG_ONE = $unsigned(-1);		// Constant -1


reg [REGISTER_SIZE-1:0] workReg = 'b0;
reg [NUMBER_OF_REGISTERS-1:0][REGISTER_SIZE-1:0] memory;
reg [IDREG_SIZE-1:0] idReg = 'b0;				// Identifier reg

wire [ADDRESS_WIDTH-1:0] writeAddress;
wire [ADDRESS_WIDTH-1:0] readAddress;
wire bIdRequested;

assign oDATA = memory;
assign readAddress = iADDRESS[ADDRESS_WIDTH-1:0];
assign writeAddress = iADDRESS[ADDRESS_WIDTH*2-1:ADDRESS_WIDTH];
assign bIdRequested = (readAddress == NEG_ONE) && (writeAddress == NEG_ONE);

// Reset the memory content at startup
integer i;
initial begin
  for (i=0;i < NUMBER_OF_REGISTERS;i=i+1)
    memory[i] = 'b0;
end

// Assign output bit
assign oTDO = bIdRequested ? idReg[0] : workReg[0];

// Main procedure
always @(posedge iTCK) begin

	if (iSTATE_CDR) begin  // Capture data register: Latch data from input bus 
		
		if (bIdRequested) begin
		
			idReg <= { 8'(REGISTER_SIZE), 8'(NUMBER_OF_REGISTERS) };	// Fill identifier register
		
		end else if (readAddress < NUMBER_OF_ALL_REGISTERS) begin
		
			workReg <= iDATA[readAddress];								// Capture input
			
		end else begin
		
			workReg <= 'b0;		// Dummy data
		
		end
	
	end else if (iSTATE_SDR) begin		// Shift data register: Main workload
		
		// Shift data in
		workReg <= {iTDI, workReg[REGISTER_SIZE-1:1]};
		idReg <= {1'b0, idReg[IDREG_SIZE-1:1]};
		
	end else if (iSTATE_UDR) begin		// Update data register: Latch received data to the output bus
		
		// Transfer done, now write received data to corresponding register
		
		if (writeAddress < NUMBER_OF_ALL_REGISTERS) begin
		
			memory[writeAddress] <= workReg;
		
		end
	
	end
	
end
	
endmodule
