#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "config.h"


// ===== CONFIGURACIÓN DE RED WiFi =====

#define WIFI_SSID     "your_wifi"      // <-- valor de ejemplo
#define WIFI_PASS     "your_password"  // <-- valor de ejemplo

// ===== CONFIGURACIÓN DE ADAFRUIT.IO =====
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883

#define AIO_USERNAME    "your_username"   // <-- valor de ejemplo
#define AIO_KEY         "your_aio_key"    // <-- valor de ejemplo

// ===== CLIENTE WIFI =====
WiFiClient client;

// ===== CLIENTE MQTT =====
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// ===== FEEDS DE ADAFRUIT =====
// Botón para LED
Adafruit_MQTT_Subscribe boton_led = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/boton_led");
// Sensor
Adafruit_MQTT_Publish sensorFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensor");


// ===== PINES DEL CIRCUITO =====
#define LED_PIN 2      // LED en pin D2 (GPIO2)
#define SENSOR_PIN 34  // Sensor analógico en pin GPIO34

// ===== FUNCIONES =====
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(LED_PIN, OUTPUT);

  // Conectar a WiFi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado!");

  // Suscribirse al botón
  mqtt.subscribe(&boton_led);
}

void loop() {
  // Conectar a MQTT si se desconecta
  MQTT_connect();

  // Verificar mensajes de Adafruit (botón)
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    if (subscription == &boton_led) {
      char *value = (char *)boton_led.lastread;
      Serial.print("Estado del botón: ");
      Serial.println(value);

      if (!strcmp(value, "ON")) {
        digitalWrite(LED_PIN, LOW);   // LED APAGADO (activo en bajo)
      } else {
        digitalWrite(LED_PIN, HIGH);  // LED ENCENDIDO
      }
    }
  }

  // Leer sensor analógico
  int lectura = analogRead(SENSOR_PIN);
  float voltaje = (lectura * 3.3) / 4095.0; // Conversión a voltaje (ESP32 usa 12 bits -> 4095)
  float temperatura = voltaje * 100;        // Fórmula para LM35 (10mV/°C)
  // Si es LDR puedes enviar la lectura cruda (lectura)

  Serial.print("Temperatura/Valor Sensor: ");
  Serial.println(temperatura);

  // Publicar dato en Adafruit
  if (!sensorFeed.publish(temperatura)) {
    Serial.println("Error al enviar dato");
  } else {
    Serial.println("Dato enviado a Adafruit");
  }

  delay(3000); // cada 3 segundos
}

// ===== FUNCIÓN PARA CONECTAR A MQTT =====
void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Conectando a MQTT... ");

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintentando en 5 segundos...");
    mqtt.disconnect();
    delay(5000);
  }
  Serial.println("Conectado a MQTT!");
}
