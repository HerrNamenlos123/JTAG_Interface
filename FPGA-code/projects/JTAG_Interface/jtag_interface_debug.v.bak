
module jtag_interface #(
	parameter NUMBER_OF_REGISTERS = 16,
	parameter REGISTER_SIZE = 32
) (
	input iMAIN_CLK,
	input [NUMBER_OF_REGISTERS-1:0][REGISTER_SIZE-1:0] iDATA,
	output [NUMBER_OF_REGISTERS-1:0][REGISTER_SIZE-1:0] oDATA
);

localparam ADDRESS_WIDTH = $clog2(NUMBER_OF_REGISTERS + 1);

wire tdi;
wire tdo;
wire tck;
wire cdr;
wire sdr;
wire udr;

wire tdiSync;
wire tdoSync;
wire tckSync;
wire cdrSync;
wire sdrSync;
wire udrSync;

wire [ADDRESS_WIDTH*2:0] address;

sld_virtual_jtag #(

	.sld_ir_width(ADDRESS_WIDTH*2 + 1)
	
) jtag (

	.tdo(tdo),
	.tdi(tdi),
	.tck(tck),
	.ir_in(address),
	.virtual_state_cdr(cdr),
	.virtual_state_sdr(sdr),
	.virtual_state_udr(udr)
	
);

jtag_synchronizer jtag_sync(

	.iMAIN_CLK(iMAIN_CLK),
	
	.iTDI(tdi),
	.iTCK(tck),
	.iSTATE_CDR(cdr),
	.iSTATE_SDR(sdr),
	.iSTATE_UDR(udr),
	.oTDO(tdo),
	
	.oTDI(tdiSync),
	.oTCK(tckSync),
	.oSTATE_CDR(cdrSync),
	.oSTATE_SDR(sdrSync),
	.oSTATE_UDR(udrSync),
	.iTDO(tdoSync)
	
);

jtag_memory #(

	.NUMBER_OF_REGISTERS(NUMBER_OF_REGISTERS),
	.REGISTER_SIZE(REGISTER_SIZE)

) memory (
	
	.iADDRESS(address),
	.iTCK(tckSync),
	.iTDI(tdiSync),
	.iSTATE_CDR(cdrSync),
	.iSTATE_SDR(sdrSync),
	.iSTATE_UDR(udrSync),
	.oTDO(tdoSync),
	
	.iDATA(iDATA),
	.oDATA(oDATA)
	
);

endmodule
