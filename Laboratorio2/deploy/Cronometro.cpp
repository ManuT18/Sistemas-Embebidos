// #include <Arduino.h>
// #include <LiquidCrystal.h>

// bool running = false;
// unsigned long decimas = 0;
// bool flag_update = true;

// void iniciar() {
//     running = true;
//     flag_update = true;
// }

// void pausar() {
//     running = false;
//     flag_update = true;
// }

// void reiniciar() {
//     decimas = 0;
//     flag_update = true;
// }

// void incrementar() {
//     if (running) {
//         decimas++;
//         flag_update = true;
//     }
// }

// bool estaCorriendo() {
//     return running;
// }

// void actualizar() {
//     if (!flag_update) return;

//     flag_update = false;

//     unsigned long d = decimas;
//     unsigned int minutos = d / 600;
//     unsigned int segundos = (d / 10) % 60;
//     unsigned int dec = d % 10;

//     lcd.setCursor(0,0);
//     lcd.print("                "); // limpiar línea 0
//     lcd.setCursor(0,0);
//     lcd.print(minutos);
//     lcd.print(".");
//     if (segundos < 10) lcd.print("0");
//     lcd.print(segundos);
//     lcd.print(".");
//     lcd.print(dec);

//     lcd.setCursor(0,1);
//     lcd.print("                "); // limpiar línea 1
//     lcd.setCursor(0,1);
//     if (running)
//         lcd.print("MCA (corriendo)");
//     else
//         lcd.print("MP (pausado)  ");
// }