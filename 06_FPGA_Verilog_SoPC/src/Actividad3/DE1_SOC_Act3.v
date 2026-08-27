//=======================================================
// Modulo: DE1_SOC_Act3.v
// Descripcion: Este modulo instancia el sistema SoPC (Nios V)
//              y lo conecta a los pines de la placa.
//=======================================================

module DE1_SOC_Act3 (
    input         CLOCK_50,

    input  [3:0]  KEY,
    
    input  [9:0]  SW,
    
    output [9:0]  LEDR,
    
    output [6:0]  HEX0,
    output [6:0]  HEX1,
    output [6:0]  HEX2,
    output [6:0]  HEX3,
    output [6:0]  HEX4,
    output [6:0]  HEX5
);

// ===============================================
// PUNTO 3: INSTANCIACION DEL SISTEMA SoPC (Qsys)
// ===============================================

    platform_designer u0 (
        // --- Conexiones Globales ---
        
		  // Conecta el reloj del sistema al CLOCK_50 de la placa
        .clk_clk      											(CLOCK_50),
        
        // Conecta el reset del sistema a KEY[0]
        .reset_reset_n  										(KEY[0]), 

        // --- Conexiones a los Perifericos (PIOs) ---
        
        // Conecta el PIO de salida a los 8 LEDs de la derecha
        .pio_leds_external_connection_export    		(LEDR[7:0]),
        
        // Conecta el PIO de entrada a los 8 switches de la derecha
        .pio_switches_external_connection_export  		(SW[7:0])
    );

// ===============================================
// LIMPIEZA DE PUERTOS NO UTILIZADOS
// ===============================================

    // Apagamos los 2 LEDs de la izquierda (no usados por el Nios)
    assign LEDR[9:8] = 2'b00;

    // Apagamos todos los displays 7-segmentos
    assign HEX0 = 7'b1111111;
    assign HEX1 = 7'b1111111;
    assign HEX2 = 7'b1111111;
    assign HEX3 = 7'b1111111;
    assign HEX4 = 7'b1111111;
    assign HEX5 = 7'b1111111;

endmodule