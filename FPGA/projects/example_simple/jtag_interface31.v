
module jtag_interface31 #(
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
	input [WIDTH-1:0] iDATA_15,
	input [WIDTH-1:0] iDATA_16,
	input [WIDTH-1:0] iDATA_17,
	input [WIDTH-1:0] iDATA_18,
	input [WIDTH-1:0] iDATA_19,
	input [WIDTH-1:0] iDATA_20,
	input [WIDTH-1:0] iDATA_21,
	input [WIDTH-1:0] iDATA_22,
	input [WIDTH-1:0] iDATA_23,
	input [WIDTH-1:0] iDATA_24,
	input [WIDTH-1:0] iDATA_25,
	input [WIDTH-1:0] iDATA_26,
	input [WIDTH-1:0] iDATA_27,
	input [WIDTH-1:0] iDATA_28,
	input [WIDTH-1:0] iDATA_29,
	input [WIDTH-1:0] iDATA_30,
	
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
	output [WIDTH-1:0] oDATA_14,
	output [WIDTH-1:0] oDATA_15,
	output [WIDTH-1:0] oDATA_16,
	output [WIDTH-1:0] oDATA_17,
	output [WIDTH-1:0] oDATA_18,
	output [WIDTH-1:0] oDATA_19,
	output [WIDTH-1:0] oDATA_20,
	output [WIDTH-1:0] oDATA_21,
	output [WIDTH-1:0] oDATA_22,
	output [WIDTH-1:0] oDATA_23,
	output [WIDTH-1:0] oDATA_24,
	output [WIDTH-1:0] oDATA_25,
	output [WIDTH-1:0] oDATA_26,
	output [WIDTH-1:0] oDATA_27,
	output [WIDTH-1:0] oDATA_28,
	output [WIDTH-1:0] oDATA_29,
	output [WIDTH-1:0] oDATA_30
);
localparam NUMBER_OF_REGISTERS = 31;

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
assign iDATA[15] = iDATA_15;
assign iDATA[16] = iDATA_16;
assign iDATA[17] = iDATA_17;
assign iDATA[18] = iDATA_18;
assign iDATA[19] = iDATA_19;
assign iDATA[20] = iDATA_20;
assign iDATA[21] = iDATA_21;
assign iDATA[22] = iDATA_22;
assign iDATA[23] = iDATA_23;
assign iDATA[24] = iDATA_24;
assign iDATA[25] = iDATA_25;
assign iDATA[26] = iDATA_26;
assign iDATA[27] = iDATA_27;
assign iDATA[28] = iDATA_28;
assign iDATA[29] = iDATA_29;
assign iDATA[30] = iDATA_30;

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
assign oDATA_15 = oDATA[15];
assign oDATA_16 = oDATA[16];
assign oDATA_17 = oDATA[17];
assign oDATA_18 = oDATA[18];
assign oDATA_19 = oDATA[19];
assign oDATA_20 = oDATA[20];
assign oDATA_21 = oDATA[21];
assign oDATA_22 = oDATA[22];
assign oDATA_23 = oDATA[23];
assign oDATA_24 = oDATA[24];
assign oDATA_25 = oDATA[25];
assign oDATA_26 = oDATA[26];
assign oDATA_27 = oDATA[27];
assign oDATA_28 = oDATA[28];
assign oDATA_29 = oDATA[29];
assign oDATA_30 = oDATA[30];

endmodule
