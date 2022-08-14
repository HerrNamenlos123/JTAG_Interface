
module jtag_interface15 #(
	parameter WIDTH = 32
) (
	input iMAIN_CLK,
	input [WIDTH-1:0] iDATA_00,
	input [WIDTH-1:0] iDATA_01,
	input [WIDTH-1:0] iDATA_02,
	input [WIDTH-1:0] iDATA_03,
	input [WIDTH-1:0] iDATA_04,
	input [WIDTH-1:0] iDATA_05,
	input [WIDTH-1:0] iDATA_06,
	input [WIDTH-1:0] iDATA_07,
	input [WIDTH-1:0] iDATA_08,
	input [WIDTH-1:0] iDATA_09,
	input [WIDTH-1:0] iDATA_10,
	input [WIDTH-1:0] iDATA_11,
	input [WIDTH-1:0] iDATA_12,
	input [WIDTH-1:0] iDATA_13,
	input [WIDTH-1:0] iDATA_14,
	
	output [WIDTH-1:0] oDATA_00,
	output [WIDTH-1:0] oDATA_01,
	output [WIDTH-1:0] oDATA_02,
	output [WIDTH-1:0] oDATA_03,
	output [WIDTH-1:0] oDATA_04,
	output [WIDTH-1:0] oDATA_05,
	output [WIDTH-1:0] oDATA_06,
	output [WIDTH-1:0] oDATA_07,
	output [WIDTH-1:0] oDATA_08,
	output [WIDTH-1:0] oDATA_09,
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

assign iDATA[0]  = iDATA_00;
assign iDATA[1]  = iDATA_01;
assign iDATA[2]  = iDATA_02;
assign iDATA[3]  = iDATA_03;
assign iDATA[4]  = iDATA_04;
assign iDATA[5]  = iDATA_05;
assign iDATA[6]  = iDATA_06;
assign iDATA[7]  = iDATA_07;
assign iDATA[8]  = iDATA_08;
assign iDATA[9]  = iDATA_09;
assign iDATA[10] = iDATA_10;
assign iDATA[11] = iDATA_11;
assign iDATA[12] = iDATA_12;
assign iDATA[13] = iDATA_13;
assign iDATA[14] = iDATA_14;

assign oDATA_00 = oDATA[0];
assign oDATA_01 = oDATA[1];
assign oDATA_02 = oDATA[2];
assign oDATA_03 = oDATA[3];
assign oDATA_04 = oDATA[4];
assign oDATA_05 = oDATA[5];
assign oDATA_06 = oDATA[6];
assign oDATA_07 = oDATA[7];
assign oDATA_08 = oDATA[8];
assign oDATA_09 = oDATA[9];
assign oDATA_10 = oDATA[10];
assign oDATA_11 = oDATA[11];
assign oDATA_12 = oDATA[12];
assign oDATA_13 = oDATA[13];
assign oDATA_14 = oDATA[14];

endmodule
