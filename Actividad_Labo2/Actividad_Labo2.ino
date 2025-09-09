// === Librerías necesarias ===
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// === Configuración OLED ===
#define i2c_Address 0x3c     // Dirección I2C de la pantalla (prueba 0x3d si no funciona)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === Configuración LM35 ===
const int pinLM35 = A0;        // Pin analógico donde está conectado LM35 Vout
const float Vref = 5.0;        // Voltaje de referencia del ADC

void setup() {
  Serial.begin(9600);

  // Inicializa la pantalla
  delay(250);
  display.begin(i2c_Address, true);
  display.setContrast(255);
  display.clearDisplay();
  display.display();

  Serial.println("Iniciando lectura LM35 + OLED...");
}

void loop() {
  // --- Lectura del LM35 ---
  int lectura = analogRead(pinLM35);              // valor entre 0 y 1023
  float voltaje = lectura * (Vref / 1023.0);      // conversión a voltios
  float tempC = voltaje * 100.0;                  // LM35 = 10mV/°C → factor 100

  // --- Enviar por Serial ---
  Serial.print("Temperatura: ");
  Serial.print(tempC, 2);
  Serial.println(" °C");

  // --- Mostrar en OLED ---
  display.clearDisplay();
  display.setTextSize(2);                // Tamaño de letra
  display.setTextColor(SH110X_WHITE);
  display.setCursor(10, 20);             // Posición en pantalla
  display.print("Temp:");
  display.setCursor(10, 45);
  display.print(tempC, 1);               // Mostrar con 1 decimal
  display.print(" C");
  display.display();

  delay(1000); // Actualiza cada segundo
}
