//=======================================================
// Modulo: Sorter4.v
// Descripcion: Modulo combinacional que ordena 4 entradas
//              de 8 bits utilizando 5 instancias del
//              modulo 'A' (comparador min/max).
//=======================================================

module Sorter4 (
    // --- Entradas ---
    input  [7:0] in_1,
    input  [7:0] in_2,
    input  [7:0] in_3,
    input  [7:0] in_4,
    
    // --- Salidas (Ordenadas) ---
    output [7:0] out_1, // Minimo
    output [7:0] out_2,
    output [7:0] out_3,
    output [7:0] out_4  // Maximo
);

    //--- Cables para las conexiones intermedias ---
    
    // Salidas de la Etapa 1
    wire [7:0] w_min_12; // Minimo entre in_1, in_2
    wire [7:0] w_max_12; // Maximo entre in_1, in_2
    wire [7:0] w_min_34; // Minimo entre in_3, in_4
    wire [7:0] w_max_34; // Maximo entre in_3, in_4
    
    // Salidas de la Etapa 2
    wire [7:0] w_mid_min; // El maximo de los minimos
    wire [7:0] w_mid_max; // El minimo de los maximos

    //--- ETAPA 1: Comparamos los pares de entrada ---

    // Instancia 1: Compara in_1 con in_2
    A comp_par_12 (
        .N1 (in_1),
        .N2 (in_2),
        .S1 (w_min_12), // min(in_1, in_2)
        .S2 (w_max_12)  // max(in_1, in_2)
    );
    
    // Instancia 2: Compara in_3 con in_4
    A comp_par_34 (
        .N1 (in_3),
        .N2 (in_4),
        .S1 (w_min_34), // min(in_3, in_4)
        .S2 (w_max_34)  // max(in_3, in_4)
    );

    //--- ETAPA 2: Encontramos el minimo y maximo absolutos ---
    
    // Instancia 3: Compara los dos minimos de la Etapa 1
    A comp_min (
        .N1 (w_min_12),
        .N2 (w_min_34),
        .S1 (out_1),    		// El minimo de los minimos es el MINIMO ABSOLUTO (out_1)
        .S2 (w_mid_min) 		// Este es uno de los valores del medio
    );
    
    // Instancia 4: Compara los dos maximos de la Etapa 1
    A comp_max (
        .N1 (w_max_12),
        .N2 (w_max_34),
        .S1 (w_mid_max), 		// Este es el otro valor del medio
        .S2 (out_4)     		// El maximo de los maximos es el MAXIMO ABSOLUTO (out_4)
    );
    
    //--- ETAPA 3: Ordenamos los dos valores del medio ---
    
    // Instancia 5: Compara los dos valores "del medio" de la Etapa 2
    A comp_medios (
        .N1 (w_mid_min),
        .N2 (w_mid_max),
        .S1 (out_2),    // El minimo de los del medio es out_2
        .S2 (out_3)     // El maximo de los del medio es out_3
    );

endmodule