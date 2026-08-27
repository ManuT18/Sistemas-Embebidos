//=======================================================
// Modulo: BinToBcd.v
// Descripcion: Convierte un numero Binario de 8 bits
//              (0-255) a tres digitos BCD (12 bits).
//=======================================================

module BinToBcd (
    // --- Entrada ---
    input  [7:0] bin_in, // Numero binario (0-255)
    
    // --- Salida ---
    // 12 bits: [11:8] Centenas, [7:4] Decenas, [3:0] Unidades
    output [11:0] bcd_out
);

    // Salidas de 4 bits para cada digito BCD
    wire [3:0] centenas;
    wire [3:0] decenas;
    wire [3:0] unidades;

    // Conexión de la salida: Unimos los 3 digitos en un solo bus de 12 bits
    assign bcd_out = {centenas, decenas, unidades};

    // --- LOGICA COMBINACIONAL PARA CENTENAS (0, 1, o 2) ---
    
    assign centenas = (bin_in >= 8'd200) ? 4'd2 :
                      (bin_in >= 8'd100) ? 4'd1 :
                      4'd0; 

    // --- LOGICA COMBINACIONAL PARA DECENAS Y UNIDADES ---
    
    wire [7:0] resto_100;

    assign resto_100 = (centenas == 4'd2) ? (bin_in - 8'd200) :
                       (centenas == 4'd1) ? (bin_in - 8'd100) :
                       bin_in; 
    
    assign decenas  = resto_100 / 8'd10;
    
    assign unidades = resto_100 % 8'd10;

endmodule