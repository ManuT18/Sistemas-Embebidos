//=======================================================
// Modulo: Bin8_to_3DigitHEX.v
// Descripcion: Decodificador completo de un numero binario
//              de 8 bits a 3 displays HEX de 7 segmentos.
//=======================================================

module Bin8_to_3DigitHEX (
    // --- Entrada ---
    input  [7:0] bin_in,    
    
    // --- Salidas ---
    output [6:0] HEX0,      
    output [6:0] HEX1,      
    output [6:0] HEX2       
);

    //--- Cables internos ---
    
    // Bus de 12 bits para los 3 digitos BCD (Centenas, Decenas, Unidades)
    wire [11:0] bcd_bus;
    
    // Cables separados para cada digito BCD
    wire [3:0] bcd_unidades;
    wire [3:0] bcd_decenas;
    wire [3:0] bcd_centenas;
    
    
    // ===========================================
    // 1. CONVERSION DE BINARIO A BCD
    // ===========================================
    
    // Instancia del modulo Binario-a-BCD (BinToBcd.v)
    BinToBcd inst_bin_to_bcd (
        .bin_in  (bin_in),   // Entrada binaria de 8 bits
        .bcd_out (bcd_bus)   // Salida BCD de 12 bits
    );
    
    // Separacion de los 3 digitos BCD del bus de 12 bits
    assign bcd_unidades = bcd_bus[3:0];   
    assign bcd_decenas  = bcd_bus[7:4];   
    assign bcd_centenas = bcd_bus[11:8];  


    // ===========================================
    // 2. CONVERSION DE BCD A 7-SEGMENTOS (HEX)
    // ===========================================
    
    // Instancia 1: Unidades (para el display HEX0)
    BCD2HEX inst_unidades (
        .bcd_in       (bcd_unidades),
        .segments_out (HEX0)
    );
    
    // Instancia 2: Decenas (para el display HEX1)
    BCD2HEX inst_decenas (
        .bcd_in       (bcd_decenas),
        .segments_out (HEX1)
    );
    
    // Instancia 3: Centenas (para el display HEX2)
    BCD2HEX inst_centenas (
        .bcd_in       (bcd_centenas),
        .segments_out (HEX2)
    );

endmodule