
module jtag_interface2 #(
	parameter WIDTH = 32
) (
	input iMAIN_CLK,
	input [WIDTH-1:0] iDATA_0,
	input [WIDTH-1:0] iDATA_1,
	
	output [WIDTH-1:0] oDATA_0,
	output [WIDTH-1:0] oDATA_1
);

wire [1:0][WIDTH-1:0] iDATA;
wire [1:0][WIDTH-1:0] oDATA;

jtag_interface #(

	.REGISTER_SIZE(WIDTH),
	.NUMBER_OF_REGISTERS(2)
	
) jtag_inst (

	.iMAIN_CLK(iMAIN_CLK),
	.iDATA(iDATA),
	.oDATA(oDATA)

);

assign iDATA[0]  = iDATA_0;
assign iDATA[1]  = iDATA_1;

assign oDATA_0 = oDATA[0];
assign oDATA_1 = oDATA[1];

endmodule
