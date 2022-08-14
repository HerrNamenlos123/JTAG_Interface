
//
// Parameters: 
//  REGISTER_SIZE		 -> bit width of each register
// 	NUMBER_OF_REGISTERS  -> number of usable registers (+1 for the ID)
//
// For a transaction, JTAG must write the address into the JTAG virtual instruction register and then
//   write the data into the virtual data register. When either the write or read index is -1 (all ones),
//   nothing is written and 0 is shifted out. When both write and read indices are -1, the configuration
//   identifier with a fixed with of 16 bits is shifted out, containing the register width (high byte) 
//   and the number of usable registers (low byte). It is used in the Arduino program at startup to check, 
//   if the right instance matches on the MCU and FPGA.
//
// The address in the instruction register contains both the write and read index. Its width depends on the
//   configuration. The total number of registers is used -> usable registers + 1.
//
//   addressWidth = ceil(log2(NUMBER_OF_REGISTERS + 1))
// 
// Example used for demonstration below: 
//   NUMBER_OF_REGISTERS = 16 -> addressWidth = 5
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
//   efficient and fastest.
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
