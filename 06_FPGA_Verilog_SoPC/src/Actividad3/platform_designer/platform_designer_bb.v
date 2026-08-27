
module platform_designer (
	clk_clk,
	pio_leds_external_connection_export,
	pio_switches_external_connection_export,
	reset_reset_n);	

	input		clk_clk;
	output	[7:0]	pio_leds_external_connection_export;
	input	[7:0]	pio_switches_external_connection_export;
	input		reset_reset_n;
endmodule
