//=======================================================
// Modulo: BCD2HEX.v
// Descripcion: Conversor de un digito BCD (4 bits) a un
//              display de 7 segmentos de Anodo Comun
//              (Logica Activo-Bajo).
//=======================================================

module BCD2HEX (
    // --- Entrada ---
    input  [3:0] bcd_in, // Digito BCD (0-9)
    
    // --- Salida ---
    // 7 segmentos, (g,f,e,d,c,b,a)
    output reg [6:0] segments_out 
);

    // Mapeo de BCD a 7-segmentos (g,f,e,d,c,b,a)
    // '0' = ENCIENDE el segmento
    // '1' = APAGA el segmento
    always @(*) begin
        case (bcd_in)
            4'h0: segments_out = 7'b1000000; // '0' (g apagado)
            4'h1: segments_out = 7'b1111001; // '1' (b,c encendidos)
            4'h2: segments_out = 7'b0100100; // '2'
            4'h3: segments_out = 7'b0110000; // '3'
            4'h4: segments_out = 7'b0011001; // '4'
            4'h5: segments_out = 7'b0010010; // '5'
            4'h6: segments_out = 7'b0000010; // '6'
            4'h7: segments_out = 7'b1111000; // '7'
            4'h8: segments_out = 7'b0000000; // '8' (todos encendidos)
            4'h9: segments_out = 7'b0010000; // '9'
            
            // Si la entrada no es 0-9 (ej. A-F), apaga todos
            default: segments_out = 7'b1111111; 
        endcase
    end

endmodule