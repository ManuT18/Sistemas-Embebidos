//=======================================================
// Modulo: DE1_SOC_Act2.v
// Descripcion: Modulo Top-Level (Actividad 2)
//              Logica de Carga/Visualizacion con MUX y
//              detectores de flanco.
//=======================================================

module DE1_SOC_Act2 (

    input               CLOCK_50,

    input      [3:0]    KEY,

    input      [9:0]    SW,

    output     [9:0]    LEDR,

    output     [6:0]    HEX0,
    output     [6:0]    HEX1,
    output     [6:0]    HEX2,

    output     [6:0]    HEX3,
    output     [6:0]    HEX4,
    output     [6:0]    HEX5
);

    // ===============================================
    // 1. DECLARACIONES INTERNAS Y REGISTROS
    // ===============================================

    // Registros para guardar los 4 valores
    reg [7:0] val0 = 8'd0;
    reg [7:0] val1 = 8'd0;
    reg [7:0] val2 = 8'd0;
    reg [7:0] val3 = 8'd0;

    // Contadores para los modos
    reg [1:0] count_load = 2'd0; // Contador de Carga (0-3)
    reg [1:0] sel_out = 2'd0;    // Contador de Visualizacion (0-3)

    // Cables para el Sorter
    wire [7:0] sorted_out_1, sorted_out_2, sorted_out_3, sorted_out_4;

    // Cable para el valor ORDENADO seleccionado
    wire [7:0] value_to_display_sorted;

    // Cable para el valor FINAL a mostrar (MUX principal)
    wire [7:0] valor_final_display;

    // Reset (Activo-Bajo)
    wire reset_n = KEY[0];

    // Cables para los pulsos limpios de 1 ciclo
    wire pulso_key1_n;
    wire pulso_key2_n;

    // Cables intermedios para la salida normal de los displays
    wire [6:0] hex0_normal, hex1_normal, hex2_normal;

    // Valores para "RES"
    wire [6:0] CHAR_R     = 7'b0101000; // 'r'
    wire [6:0] CHAR_E     = 7'b0000110; // 'E'
    wire [6:0] CHAR_S     = 7'b0010010; // 'S'
    wire [6:0] CHAR_BLANK = 7'b1111111; // Apagado

    // Muestra de valor del registro
    wire [6:0] hex_load_digit_out;
    wire [6:0] hex_show_digit_out;

    // ===============================================
    // 2. INSTANCIAS DE DETECTORES DE FLANCO
    // ===============================================

    // Instancia para KEY[1] (Confirmar Carga)
    detector_flanco_negedge det_key1 (
        .clk(CLOCK_50),
        .boton_n(KEY[1]),
        .pulso_limpio(pulso_key1_n)
    );

    // Instancia para KEY[2] (Ciclar Visualizacion)
    detector_flanco_negedge det_key2 (
        .clk(CLOCK_50),
        .boton_n(KEY[2]),
        .pulso_limpio(pulso_key2_n)
    );

    // Instancia para mostrar el registro en que se carga
    BCD2HEX inst_load_digit (
        .bcd_in(count_load + 4'd1), // Muestra 1, 2, 3 o 4
        .segments_out(hex_load_digit_out)
    );

    // Instancia para mostrar el registro en modo MUESTRA
    BCD2HEX inst_show_digit (
        .bcd_in(sel_out + 4'd1), // Muestra 1, 2, 3 o 4
        .segments_out(hex_show_digit_out)
    );

    // ===============================================
    // 3. LOGICA SECUENCIAL (Contadores y Registros)
    // ===============================================

    always @(posedge CLOCK_50 or negedge reset_n) begin
        if (!reset_n) begin
            val0 <= 8'd0;
            val1 <= 8'd0;
            val2 <= 8'd0;
            val3 <= 8'd0;
            count_load <= 2'd0;
            sel_out    <= 2'd0;
        end
        else begin
            // --- Logica de Carga ---
            // Si SW[8] esta ARRIBA (Modo Carga) Y 
            // se detecta un pulso limpio de KEY[1]
            if (SW[8] && pulso_key1_n) begin
                case (count_load)
                    2'd0: val0 <= SW[7:0];
                    2'd1: val1 <= SW[7:0];
                    2'd2: val2 <= SW[7:0];
                    2'd3: val3 <= SW[7:0];
                endcase
                count_load <= count_load + 2'd1; // Cicla al siguiente registro
            end

            // --- Logica de Visualizacion ---
            // Si SW[9] esta ARRIBA (Modo Visualizacion) Y
            // se detecta un pulso limpio de KEY[2]
            if (SW[9] && pulso_key2_n) begin
                sel_out <= sel_out + 2'd1; // Cicla al siguiente valor guardado
            end
        end
    end

    // ===============================================
    // 4. LOGICA COMBINACIONAL (Sorters y MUXes)
    // ===============================================

    // Instancia del Sorter4
    Sorter4 inst_sorter (
        .in_1 (val0),
        .in_2 (val1),
        .in_3 (val2),
        .in_4 (val3),
        .out_1 (sorted_out_1),
        .out_2 (sorted_out_2),
        .out_3 (sorted_out_3),
        .out_4 (sorted_out_4)
    );

    // MUX para seleccionar el valor ORDENADO 
    assign value_to_display_sorted = (sel_out == 2'd0) ? sorted_out_1 :
                                     (sel_out == 2'd1) ? sorted_out_2 :
                                     (sel_out == 2'd2) ? sorted_out_3 :
                                     sorted_out_4;

    // --- MUX PRINCIPAL ---
    // El selector es SW[9] (Modo Visualizacion)
    assign valor_final_display = (SW[9]) ? value_to_display_sorted : // '1': Muestra guardados
                                           SW[7:0];                 	// '0': Muestra SW en vivo

    // ===============================================
    // 5. INSTANCIAS DE SALIDA (Displays y LEDs)
    // ===============================================

    // Conecta el MUX principal a los displays
    Bin8_to_3DigitHEX inst_display (
        .bin_in (valor_final_display),
        .HEX0   (hex0_normal), // Escribe al cable intermedio
        .HEX1   (hex1_normal), // Escribe al cable intermedio
        .HEX2   (hex2_normal)  // Escribe al cable intermedio
    );

    // Si KEY[0] esta presionado (!reset_n), anula la salida normal y muestra "RES"
    assign HEX0 = (!reset_n) ? CHAR_S : hex0_normal;
    assign HEX1 = (!reset_n) ? CHAR_E : hex1_normal;
    assign HEX2 = (!reset_n) ? CHAR_R : hex2_normal;

    assign HEX5 = (SW[8]) 		? hex_load_digit_out : 			// Modo Carga muestra el registro a cargar
                  (SW[9])     ? hex_show_digit_out : 			// Modo Muestra muestra el registro mostrado actualmente
                  CHAR_BLANK;                          		// Default: Apagado

    // Apagamos los displays no usados (enviandoles '1' logico)
    assign HEX3 = 7'b1111111;
    assign HEX4 = 7'b1111111;

    // ===============================================
    // 6. DEBUG (LEDs)
    // ===============================================

    // Muestra el valor FINAL (binario) en los LEDs
    assign LEDR[7:0] = valor_final_display;

    // LED[8] indica Modo Carga (se enciende con SW[8])
    assign LEDR[8] = SW[8];
    // LED[9] indica Modo Visualizacion (se enciende con SW[9])
    assign LEDR[9] = SW[9];

endmodule