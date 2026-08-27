	component platform_designer is
		port (
			clk_clk                                 : in  std_logic                    := 'X';             -- clk
			pio_leds_external_connection_export     : out std_logic_vector(7 downto 0);                    -- export
			pio_switches_external_connection_export : in  std_logic_vector(7 downto 0) := (others => 'X'); -- export
			reset_reset_n                           : in  std_logic                    := 'X'              -- reset_n
		);
	end component platform_designer;

	u0 : component platform_designer
		port map (
			clk_clk                                 => CONNECTED_TO_clk_clk,                                 --                              clk.clk
			pio_leds_external_connection_export     => CONNECTED_TO_pio_leds_external_connection_export,     --     pio_leds_external_connection.export
			pio_switches_external_connection_export => CONNECTED_TO_pio_switches_external_connection_export, -- pio_switches_external_connection.export
			reset_reset_n                           => CONNECTED_TO_reset_reset_n                            --                            reset.reset_n
		);

