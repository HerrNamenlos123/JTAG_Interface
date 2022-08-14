
module jtag_interface16 #(
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
	input [WIDTH-1:0] iDATA_A,
	input [WIDTH-1:0] iDATA_B,
	input [WIDTH-1:0] iDATA_C,
	input [WIDTH-1:0] iDATA_D,
	input [WIDTH-1:0] iDATA_E,
	input [WIDTH-1:0] iDATA_F,
	
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
	output [WIDTH-1:0] oDATA_A,
	output [WIDTH-1:0] oDATA_B,
	output [WIDTH-1:0] oDATA_C,
	output [WIDTH-1:0] oDATA_D,
	output [WIDTH-1:0] oDATA_E,
	output [WIDTH-1:0] oDATA_F
);

localparam NUMBER_OF_REGISTERS = 16;
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
assign iDATA[10] = iDATA_A;
assign iDATA[11] = iDATA_B;
assign iDATA[12] = iDATA_C;
assign iDATA[13] = iDATA_D;
assign iDATA[14] = iDATA_E;
assign iDATA[15] = iDATA_F;

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
assign oDATA_A = oDATA[10];
assign oDATA_B = oDATA[11];
assign oDATA_C = oDATA[12];
assign oDATA_D = oDATA[13];
assign oDATA_E = oDATA[14];
assign oDATA_F = oDATA[15];

endmodule
