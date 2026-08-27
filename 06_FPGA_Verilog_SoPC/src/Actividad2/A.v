//=======================================================
// Modulo: A.v
// Descripcion: Modulo combinacional que ordena dos 
//              entradas de 8 bits (N1, N2) en una 
//              salida minima (S1) y maxima (S2).
//=======================================================

module A (
    // --- Entradas ---
    input  [7:0] N1, // Numero 1
    input  [7:0] N2, // Numero 2
    
    // --- Salidas ---
    output reg [7:0] S1, // Salida Minima (min(N1, N2))
    output reg [7:0] S2  // Salida Maxima (max(N1, N2))
);

    // Bloque combinacional.
    // Se dispara automaticamente cada vez que N1 o N2 cambian.
    always @(*) begin
        if (N1 < N2) begin
            // N1 es el mas chico
            S1 = N1;
            S2 = N2;
        end
        else begin
            // N2 es el mas chico (o son iguales)
            S1 = N2;
            S2 = N1;
        end
    end

endmodule