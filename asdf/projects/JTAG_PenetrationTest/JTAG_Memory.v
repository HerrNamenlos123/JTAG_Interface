
module JTAG_Memory(iADDRESS, iTCK, iTDI, iSTATE_SDR, iSTATE_CDR, iSTATE_UDR,
					    iREAD_0, iREAD_1, iREAD_2, iREAD_3, iREAD_4, iREAD_5, iREAD_6, iREAD_7, iREAD_8, iREAD_9, iREAD_A, iREAD_B, iREAD_C, iREAD_D, iREAD_E,
					    oWRITE_0, oWRITE_1, oWRITE_2, oWRITE_3, oWRITE_4, oWRITE_5, oWRITE_6, oWRITE_7, oWRITE_8, oWRITE_9, oWRITE_A, oWRITE_B, oWRITE_C, oWRITE_D, oWRITE_E,
						 oTDO);

	// I/O:
	input  [8:0] iADDRESS;
	input  iTCK, iTDI;
	input  iSTATE_SDR, iSTATE_CDR, iSTATE_UDR;
	
	// Input for reading
	input [31:0] iREAD_0;
	input [31:0] iREAD_1;
	input [31:0] iREAD_2;
	input [31:0] iREAD_3;
	input [31:0] iREAD_4;
	input [31:0] iREAD_5;
	input [31:0] iREAD_6;
	input [31:0] iREAD_7;
	input [31:0] iREAD_8;
	input [31:0] iREAD_9;
	input [31:0] iREAD_A;
	input [31:0] iREAD_B;
	input [31:0] iREAD_C;
	input [31:0] iREAD_D;
	input [31:0] iREAD_E;
	
	// Output
	output [31:0] oWRITE_0;
	output [31:0] oWRITE_1;
	output [31:0] oWRITE_2;
	output [31:0] oWRITE_3;
	output [31:0] oWRITE_4;
	output [31:0] oWRITE_5;
	output [31:0] oWRITE_6;
	output [31:0] oWRITE_7;
	output [31:0] oWRITE_8;
	output [31:0] oWRITE_9;
	output [31:0] oWRITE_A;
	output [31:0] oWRITE_B;
	output [31:0] oWRITE_C;
	output [31:0] oWRITE_D;
	output [31:0] oWRITE_E;
	
	output oTDO;
	
	
	//
	// The address is 9 bits long, while the leftmost bit is ignored and the lower 8 bits contain
	// 	the 4-bit addresses for reading and writing
	//
	// The following example would write to register 9 while simultaneously reading from register 13:
	//
	// 		        110011101  ->   1  1001  1101
	//                               ^    ^     ^ 
	// 									  /     |      \
	//              Must always be 1      |      Address to read from
	//												  |
	//                            Address to write to
	//
	// The 4-bit addresses can range from 0 to 14 (0b0000 to 0b1110), 15 (0b1111) means not used,
	// 	which writes to nothing and reads all 0s
	//
	
	
	reg [31:0] workReg = 32'b0;
	
	reg [31:0] oWRITE_0 = 32'b0;
	reg [31:0] oWRITE_1 = 32'b0;
	reg [31:0] oWRITE_2 = 32'b0;
	reg [31:0] oWRITE_3 = 32'b0;
	reg [31:0] oWRITE_4 = 32'b0;
	reg [31:0] oWRITE_5 = 32'b0;
	reg [31:0] oWRITE_6 = 32'b0;
	reg [31:0] oWRITE_7 = 32'b0;
	reg [31:0] oWRITE_8 = 32'b0;
	reg [31:0] oWRITE_9 = 32'b0;
	reg [31:0] oWRITE_A = 32'b0;
	reg [31:0] oWRITE_B = 32'b0;
	reg [31:0] oWRITE_C = 32'b0;
	reg [31:0] oWRITE_D = 32'b0;
	reg [31:0] oWRITE_E = 32'b0;
	
	assign oTDO = workReg[0];
	
	

	always @(posedge iTCK) begin
	
		if(iSTATE_CDR) begin
		
			// Capture correct data to be read before the transfer
			
			case(iADDRESS[3:0])		// Use lower 4 bits as read address
			
				4'b0000:	workReg <= iREAD_0;
				4'b0001:	workReg <= iREAD_1;
				4'b0010:	workReg <= iREAD_2;
				4'b0011:	workReg <= iREAD_3;
				4'b0100:	workReg <= iREAD_4;
				4'b0101:	workReg <= iREAD_5;
				4'b0110:	workReg <= iREAD_6;
				4'b0111:	workReg <= iREAD_7;
				4'b1000:	workReg <= iREAD_8;
				4'b1001:	workReg <= iREAD_9;
				4'b1010:	workReg <= iREAD_A;
				4'b1011:	workReg <= iREAD_B;
				4'b1100:	workReg <= iREAD_C;
				4'b1101:	workReg <= iREAD_D;
				4'b1110:	workReg <= iREAD_E;
				default:	workReg <= 32'b0;
			
			endcase
		
		end else if(iSTATE_SDR) begin
			
			// Shift data in
			workReg <= {iTDI, workReg[31:1]};
			
		end else if(iSTATE_UDR) begin
			
			// Transfer done, now write received data to corresponding register
			
			case(iADDRESS[7:4])		// Use higher 4 bits as write address
			
				4'b0000:	oWRITE_0 <= workReg;
				4'b0001:	oWRITE_1 <= workReg;
				4'b0010:	oWRITE_2 <= workReg;
				4'b0011:	oWRITE_3 <= workReg;
				4'b0100:	oWRITE_4 <= workReg;
				4'b0101:	oWRITE_5 <= workReg;
				4'b0110:	oWRITE_6 <= workReg;
				4'b0111:	oWRITE_7 <= workReg;
				4'b1000:	oWRITE_8 <= workReg;
				4'b1001:	oWRITE_9 <= workReg;
				4'b1010:	oWRITE_A <= workReg;
				4'b1011:	oWRITE_B <= workReg;
				4'b1100:	oWRITE_C <= workReg;
				4'b1101:	oWRITE_D <= workReg;
				4'b1110:	oWRITE_E <= workReg;
				default:	;	// Do nothing
			
			endcase
		
		end
		
	end
	
endmodule
