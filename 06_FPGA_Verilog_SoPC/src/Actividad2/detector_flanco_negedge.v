//=======================================================
// Modulo: detector_flanco_negedge.v
// Descripcion: Detecta un flanco de bajada (1 -> 0) de
//              un pulsador (con anti-rebote simple).
//              Genera un pulso limpio de 1 ciclo de reloj.
//=======================================================

module detector_flanco_negedge (
    input clk,      // Reloj (CLOCK_50)
    input boton_n,  // Pulsador 
    
    output reg pulso_limpio // Pulso de 1 ciclo
);

    // Registros para sincronizar y detectar el flanco
    reg q1_n, q2_n;

    always @(posedge clk) begin
        // 1. Sincronizacion: Pasa la senal del boton
        //    por dos registros para estabilizarla.
        q1_n <= boton_n;
        q2_n <= q1_n;
        
        // 2. Deteccion de Flanco:
        //    Buscamos un flanco de bajada (1 -> 0).
        //    Esto ocurre cuando q2_n era '1' y q1_n ahora es '0'.
        //    (La senal negada !q1_n sera '1' y !q2_n sera '0')
        //    El pulso solo dura 1 ciclo de reloj.
        pulso_limpio <= q2_n & ~q1_n; 
    end

endmodule