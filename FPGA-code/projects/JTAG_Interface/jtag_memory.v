
//
// Parameters: 
// 	n = NUMBER_OF_REGISTERS  -> Number of usable registers (there's always 1 register more)
//    w = clog2(n + 1)	       -> bit width of the address
//
// Example: n = 15 
// 			-> w = 4
//
// The most significant bit is ignored and the lower 8 bits contain the 4-bit addresses for reading and writing
//
// The following example would write to register 5 while simultaneously reading from register 9:
//
// 		        110011101  ->   1  0101  1001
//                               ^    ^     ^ 
// 									  /     |      \
//              Must always be 1      |      Address to read from
//												  |
//                            Address to write to
//
// The addresses can range from 0 to 14 (0b0000 to 0b1110), 15 (0b1111, -1) means not used,
// 	which writes to nothing and reads 0
//
// When the total number of registers is not a power of 2 there are free register addresses: Writing to and 
//    reading from them is undefined behaviour! Use the unique id system to check the size in software!
// 
// When both read and write addresses are 15, a 16-bit unique id is sent back, used to identify the type:
//    -> The high byte represents the register width, the low byte the total number of registers (usable + 1)
//

module jtag_memory #(
	parameter NUMBER_OF_REGISTERS = 16,
	parameter REGISTER_SIZE = 32
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


reg [REGISTER_SIZE-1:0] workReg = REGISTER_SIZE'('b0);
reg [NUMBER_OF_REGISTERS-1:0][REGISTER_SIZE-1:0] memory;
reg [15:0] idReg = 15'b0;

wire [ADDRESS_WIDTH-1:0] writeAddress;
wire [ADDRESS_WIDTH-1:0] readAddress;
wire bUniqueID = readAddress != -1 && writeAddress != -1 && REGISTER_SIZE >= 8;

assign oDATA = memory;
assign readAddress = iADDRESS[ADDRESS_WIDTH-1:0];
assign writeAddress = iADDRESS[ADDRESS_WIDTH*2-1:ADDRESS_WIDTH];

// Reset the memory content at startup
integer i;
initial begin
  for (i=0;i < NUMBER_OF_REGISTERS;i=i+1)
    memory[i] = REGISTER_SIZE'('b0);
end

// Assign output bit
always @* begin
	if (bUniqueID) begin
		oTDO = idReg[0];
	end else begin 
		oTDO = workReg[0];
	end
end

// Main procedure
always @(posedge iTCK) begin

	if(iSTATE_CDR) begin  // Capture data register: Latch data from input bus 
		
		if (readAddress != -1) begin
		
			workReg <= iDATA[readAddress];
			
		end else if (bUniqueID) begin
		
			idReg <= { 8'(REGISTER_SIZE), 8'(NUMBER_OF_REGISTERS) };
		
		end else begin
		
			workReg <= REGISTER_SIZE'('b0);
		
		end
	
	end else if(iSTATE_SDR) begin		// Shift data register: Main workload
		
		// Shift data in
		workReg <= {iTDI, workReg[REGISTER_SIZE-1:1]};
		idReg <= {1'b0, idReg[15:1]};
		
	end else if(iSTATE_UDR) begin		// Update data register: Latch received data to the output bus
		
		// Transfer done, now write received data to corresponding register
		
		if (writeAddress != -1) begin
		
			memory <= workReg;
		
		end
	
	end
	
end
	
endmodule
