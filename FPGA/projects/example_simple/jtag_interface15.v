
module jtag_interface15 #(
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
	input [WIDTH-1:0] iDATA_7,
	input [WIDTH-1:0] iDATA_8,
	input [WIDTH-1:0] iDATA_9,
	input [WIDTH-1:0] iDATA_10,
	input [WIDTH-1:0] iDATA_11,
	input [WIDTH-1:0] iDATA_12,
	input [WIDTH-1:0] iDATA_13,
	input [WIDTH-1:0] iDATA_14,
	
	output [WIDTH-1:0] oDATA_0,
	output [WIDTH-1:0] oDATA_1,
	output [WIDTH-1:0] oDATA_2,
	output [WIDTH-1:0] oDATA_3,
	output [WIDTH-1:0] oDATA_4,
	output [WIDTH-1:0] oDATA_5,
	output [WIDTH-1:0] oDATA_6,
	output [WIDTH-1:0] oDATA_7,
	output [WIDTH-1:0] oDATA_8,
	output [WIDTH-1:0] oDATA_9,
	output [WIDTH-1:0] oDATA_10,
	output [WIDTH-1:0] oDATA_11,
	output [WIDTH-1:0] oDATA_12,
	output [WIDTH-1:0] oDATA_13,
	output [WIDTH-1:0] oDATA_14
);
localparam NUMBER_OF_REGISTERS = 15;

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
assign iDATA[7]  = iDATA_7;
assign iDATA[8]  = iDATA_8;
assign iDATA[9]  = iDATA_9;
assign iDATA[10] = iDATA_10;
assign iDATA[11] = iDATA_11;
assign iDATA[12] = iDATA_12;
assign iDATA[13] = iDATA_13;
assign iDATA[14] = iDATA_14;

assign oDATA_0 = oDATA[0];
assign oDATA_1 = oDATA[1];
assign oDATA_2 = oDATA[2];
assign oDATA_3 = oDATA[3];
assign oDATA_4 = oDATA[4];
assign oDATA_5 = oDATA[5];
assign oDATA_6 = oDATA[6];
assign oDATA_7 = oDATA[7];
assign oDATA_8 = oDATA[8];
assign oDATA_9 = oDATA[9];
assign oDATA_10 = oDATA[10];
assign oDATA_11 = oDATA[11];
assign oDATA_12 = oDATA[12];
assign oDATA_13 = oDATA[13];
assign oDATA_14 = oDATA[14];

endmodule
