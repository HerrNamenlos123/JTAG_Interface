
module jtag_interface7 #(
	parameter WIDTH = 32
) (
	input iMAIN_CLK,
	input [WIDTH-1:0] iDATA_0,
	input [WIDTH-1:0] iDATA_1,
	input [WIDTH-1:0] iDATA_2,
	input [WIDTH-1:0] iDATA_3,
	input [WIDTH-1:0] iDATA_4,
	input [WIDTH-1:0] iDATA_5,
	input [WIDTH-1:0] iDATA_6,
	
	output [WIDTH-1:0] oDATA_0,
	output [WIDTH-1:0] oDATA_1,
	output [WIDTH-1:0] oDATA_2,
	output [WIDTH-1:0] oDATA_3,
	output [WIDTH-1:0] oDATA_4,
	output [WIDTH-1:0] oDATA_5,
	output [WIDTH-1:0] oDATA_6
);
localparam NUMBER_OF_REGISTERS = 7;

wire [NUMBER_OF_REGISTERS-1:0][WIDTH-1:0] iDATA;
wire [NUMBER_OF_REGISTERS-1:0][WIDTH-1:0] oDATA;

jtag_interface #(

	.REGISTER_SIZE(WIDTH),
	.NUMBER_OF_REGISTERS(NUMBER_OF_REGISTERS)
	
) jtag_inst (

	.iMAIN_CLK(iMAIN_CLK),
	.iDATA(iDATA),
	.oDATA(oDATA)

);

assign iDATA[0]  = iDATA_0;
assign iDATA[1]  = iDATA_1;
assign iDATA[2]  = iDATA_2;
assign iDATA[3]  = iDATA_3;
assign iDATA[4]  = iDATA_4;
assign iDATA[5]  = iDATA_5;
assign iDATA[6]  = iDATA_6;

assign oDATA_0 = oDATA[0];
assign oDATA_1 = oDATA[1];
assign oDATA_2 = oDATA[2];
assign oDATA_3 = oDATA[3];
assign oDATA_4 = oDATA[4];
assign oDATA_5 = oDATA[5];
assign oDATA_6 = oDATA[6];

endmodule
