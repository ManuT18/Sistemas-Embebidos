module temporizador (reloj, salida);

	input reloj;
	output [31:0] salida;
	
	reg [31:0] salida;
	
	always @(posedge reloj)
		begin
			salida <= salida + 1;
		end
		
endmodule
