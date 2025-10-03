/**
 * Ejemplo de sketch Processing para el laboratorio.
 *
 * Este sketch permite graficar funciones que evolucionan en el tiempo,
 * mostrar valores en labels y recibir datos de Arduino vía Serial.
 */

import processing.serial.*;

// Declaraciones para graficar funciones...
int cosVal;
int cantValues;
ScrollingFcnPlot f1, f2, f3, f4;

// Botones...
RectButton rectBtn1, rectBtn2, rectBtn3, rectBtn4;
PFont myFont;

// Modo de gráfico
int modo_grafico = 0; // 0: todos, 1: medicion, 2: max, 3: min, 4: prom
boolean[] en_modo_individual = new boolean[5]; // 1-4
boolean buttonPressed = false;

// Etiquetas textuales...
Label lbl1, lbl2, lbl3, lbl4, lbl5, lbl6, lbl7, lbl8, lbl9, lbl10;
Label yVal1, yVal2, yVal3;

// Ventana y viewports...
int pgFunctionViewportWidth = 700;
int pgControlViewportWidth = 120;
int pgViewportsHeight = 400;

// Puerto Serial
Serial myPort;

void setup() {
	size(1000, 400);

  	// Inicialización de gráficos
  	cantValues = pgFunctionViewportWidth;
	f1 = new ScrollingFcnPlot(cantValues, color(100, 0, 0), -10, 110);
	f2 = new ScrollingFcnPlot(cantValues, color(0, 100, 0), -10, 110);
	f3 = new ScrollingFcnPlot(cantValues, color(0, 0, 100), -10, 110);
	f4 = new ScrollingFcnPlot(cantValues, color(100, 100, 0), -10, 110);

	// Inicialización de botones
	rectBtn1 = new RectButton(pgFunctionViewportWidth+10, 50, 90, 40, color(102), color(50), color(255), "MEDICION");
	rectBtn2 = new RectButton(pgFunctionViewportWidth+150, 50, 90, 40, color(102), color(50), color(255), "MAX");
	rectBtn3 = new RectButton(pgFunctionViewportWidth+10, 110, 90, 40, color(102), color(50), color(255), "MIN");
	rectBtn4 = new RectButton(pgFunctionViewportWidth+150, 110, 90, 40, color(102), color(50), color(255), "PROM");

	// Inicialización de labels
	lbl1 = new Label(pgFunctionViewportWidth+60, 15, color(255), "MOSTRANDO: ");
	lbl2 = new Label(pgFunctionViewportWidth+170, 15, color(255), "TODOS");
	lbl3 = new Label(pgFunctionViewportWidth+60, 200, color(100, 0, 0), "LUX Actual: ");
	lbl4 = new Label(pgFunctionViewportWidth+150, 200, color(100, 0, 0), "30 LUX");
	lbl5 = new Label(pgFunctionViewportWidth+60, 240, color(0, 0, 100), "LUX Max: ");
	lbl6 = new Label(pgFunctionViewportWidth+150, 240, color(0, 0, 100), "30 LUX");
	lbl7 = new Label(pgFunctionViewportWidth+60, 280, color(100, 100, 0), "LUX Min: ");
	lbl8 = new Label(pgFunctionViewportWidth+150, 280, color(100, 100, 0), "30 LUX");
	lbl9 = new Label(pgFunctionViewportWidth+60, 320, color(0, 100, 0), "LUX Prom: ");
	lbl10 = new Label(pgFunctionViewportWidth+150, 320, color(0, 100, 0), "30 LUX");

	yVal1 = new Label(5, 5, color(255), "110");
	yVal3 = new Label(10, pgViewportsHeight-25, color(255), "0");

	// Fuente de la GUI
	myFont = createFont("Arial", 15);
	textFont(myFont);

	// Inicializar puerto Serial en COM10
	myPort = new Serial(this, "COM10", 9600);
	myPort.bufferUntil('\n');
	println("Puerto Serial abierto: COM10");
}

void draw() {
	background(125);
	stroke(0);
	noFill();

	// Dibuja las funciones
	if (modo_grafico == 0) {
		f1.displayIntoRect(30, 10, pgFunctionViewportWidth-10, pgViewportsHeight-10);
		f2.displayIntoRect(30, 10, pgFunctionViewportWidth-10, pgViewportsHeight-10);
		f3.displayIntoRect(30, 10, pgFunctionViewportWidth-10, pgViewportsHeight-10);
		f4.displayIntoRect(30, 10, pgFunctionViewportWidth-10, pgViewportsHeight-10);
	} else if (modo_grafico == 1) {
		f1.displayIntoRect(30, 10, pgFunctionViewportWidth-10, pgViewportsHeight-10);
	} else if (modo_grafico == 2) {
		f3.displayIntoRect(30, 10, pgFunctionViewportWidth-10, pgViewportsHeight-10);
	} else if (modo_grafico == 3) {
		f4.displayIntoRect(30, 10, pgFunctionViewportWidth-10, pgViewportsHeight-10);
	} else if (modo_grafico == 4) {
		f2.displayIntoRect(30, 10, pgFunctionViewportWidth-10, pgViewportsHeight-10);
	}

	// Procesa eventos de MouseOver
	rectBtn1.update();
	rectBtn2.update();
	rectBtn3.update();
	rectBtn4.update();

	// Procesa las entradas (botones)
	if (mousePressed && !buttonPressed) {
		buttonPressed = true;

		if (rectBtn1.pressed()) {
			rectBtn1.currentcolor = color(102);
			if (en_modo_individual[1]) {
				modo_grafico = 0;
				lbl2.caption = "TODOS";
				en_modo_individual[1] = false;
			} else {
				modo_grafico = 1;
				lbl2.caption = "MEDICION";
				en_modo_individual[1] = true;
				for (int i=2; i<=4; i++) en_modo_individual[i] = false;
			}
			myPort.write('1');
		
		} else if (rectBtn2.pressed()) {
			rectBtn2.currentcolor = color(102);
			if (en_modo_individual[2]) {
				modo_grafico = 0;
				lbl2.caption = "TODOS";
				en_modo_individual[2] = false;
			} else {
				modo_grafico = 2;
				lbl2.caption = "MAX";
				en_modo_individual[2] = true;
				for (int i=1; i<=4; i++) if (i!=2) en_modo_individual[i] = false;
			}
			myPort.write('2');
		
		} else if (rectBtn3.pressed()) {
			rectBtn3.currentcolor = color(102);
			if (en_modo_individual[3]) {
				modo_grafico = 0;
				lbl2.caption = "TODOS";
				en_modo_individual[3] = false;
			} else {
				modo_grafico = 3;
				lbl2.caption = "MIN";
				en_modo_individual[3] = true;
				for (int i=1; i<=4; i++) if (i!=3) en_modo_individual[i] = false;
			}
			myPort.write('3');
		
		} else if (rectBtn4.pressed()) {
			rectBtn4.currentcolor = color(102);
			if (en_modo_individual[4]) {
				modo_grafico = 0;
				lbl2.caption = "TODOS";
				en_modo_individual[4] = false;
			} else {
				modo_grafico = 4;
				lbl2.caption = "PROM";
				en_modo_individual[4] = true;
				for (int i=1; i<=4; i++) if (i!=4) en_modo_individual[i] = false;
			}
			myPort.write('4');
		}
	}

	// Dibuja el eje X y el recuadro de los gráficos
	stroke(0);
	rect(30, 10, pgFunctionViewportWidth-40, pgViewportsHeight-20);

	// Dibujar botones
	rectBtn1.display();
	rectBtn2.display();
	rectBtn3.display();
	rectBtn4.display();

	// Dibujar labels
	lbl1.display();
	lbl2.display();
	lbl3.display();
	lbl4.display();
	lbl5.display();
	lbl6.display();
	lbl7.display();
	lbl8.display();
	lbl9.display();
	lbl10.display();
	yVal1.display();
	yVal3.display();
}

// Lectura de datos desde Arduino
void serialEvent(Serial p) {
	String line = p.readStringUntil('\n');
	if (line != null) {
		line = trim(line);
		
		if (line.startsWith("LUX:")) {
			line = line.substring(4); // quitar "LUX:"
			String[] vals = split(line, ',');
		
			if (vals.length == 4) {
				float lux_actual = float(vals[0]);
				float lux_max = float(vals[1]);
				float lux_min = float(vals[2]);
				float lux_avg = float(vals[3]);

				// Actualizar labels
				lbl4.caption = nf(lux_actual, 1, 1) + " LUX";
				lbl6.caption = nf(lux_max, 1, 1) + " LUX";
				lbl8.caption = nf(lux_min, 1, 1) + " LUX";
				lbl10.caption = nf(lux_avg, 1, 1) + " LUX";

				// Actualizar gráficos
				f1.updateValue(lux_actual);
				f2.updateValue(lux_avg);
				f3.updateValue(lux_max);
				f4.updateValue(lux_min);
			}
		}
	}
}

void mouseReleased() {
  	buttonPressed = false;
}

