//=======================================================
// Modulo: Multiplexor.v
// Descripcion: Este modulo es un wrapper
//              para la IP LPM_MUX (mux_2a1_4bits) generada
//              por el IP Catalog de Quartus.
//=======================================================

module multiplexor (
    input  [3:0] data0x,
    input  [3:0] data1x,
    input        sel,
    output [3:0] result
);

    //--- Instanciacion de la IP generada ---
    
    mux_2a1 ip_mux_core (
        .data0x   (data0x),    // Conecta entrada 'data0x' al puerto 'data0' de la IP
        .data1x   (data1x),    // Conecta entrada 'data1x' al puerto 'data1' de la IP
        .sel     (sel),       // Conecta entrada 'sel' al puerto 'sel' de la IP
        .result  (result)     // Conecta salida 'result' al puerto 'result' de la IP
    );

endmodule