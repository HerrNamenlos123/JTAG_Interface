
module jtag_interface8 #(
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
	
	output [WIDTH-1:0] oDATA_0,
	output [WIDTH-1:0] oDATA_1,
	output [WIDTH-1:0] oDATA_2,
	output [WIDTH-1:0] oDATA_3,
	output [WIDTH-1:0] oDATA_4,
	output [WIDTH-1:0] oDATA_5,
	output [WIDTH-1:0] oDATA_6,
	output [WIDTH-1:0] oDATA_7
);

wire [7:0][WIDTH-1:0] iDATA;
wire [7:0][WIDTH-1:0] oDATA;

jtag_interface #(

	.REGISTER_SIZE(WIDTH),
	.NUMBER_OF_REGISTERS(8)
	
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

assign oDATA_0 = oDATA[0];
assign oDATA_1 = oDATA[1];
assign oDATA_2 = oDATA[2];
assign oDATA_3 = oDATA[3];
assign oDATA_4 = oDATA[4];
assign oDATA_5 = oDATA[5];
assign oDATA_6 = oDATA[6];
assign oDATA_7 = oDATA[7];

endmodule
