
module Synchronizer(iCLK, iSIGNAL, oSYNCHRONIZED);

	input  iCLK, iSIGNAL;
	output oSYNCHRONIZED;
	
	reg sync1 = 0;
	reg sync2 = 0;
	
	assign oSYNCHRONIZED = sync2;

	always @(posedge iCLK) begin
	
		sync1 <= iSIGNAL;
		sync2 <= sync1;
		
	end
	
endmodule
