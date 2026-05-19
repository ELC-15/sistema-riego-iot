#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Configuración de Red
const char* ssid = "INFINITUMA2B5";
const char* password = "AGVdKpyK7p";

// Configuración MQTT (Usaremos el broker público de HiveMQ para la prueba)
const char* mqtt_server = "192.168.1.111";
const int mqtt_port = 1883;
const char* mqtt_topic = "riego/sensores";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.println("\nConectando a WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado. Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    // Creamos un ID de cliente único para el broker
    String clientId = "ESP32Client-" + String(random(0, 0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("¡Conectado al Broker!");
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) { // Enviar cada 5 segundos
    lastMsg = now;

    // Simulación de sensores (Aquí luego pondrás las lecturas reales del DHT11 y RS485)
    float tempSimulada = random(200, 350) / 10.0; // 20.0 a 35.0
    int humedadSimulada = random(40, 80);        // 40% a 80%

    // Crear el documento JSON
    StaticJsonDocument<200> doc;
    doc["nodo_id"] = "ESP32_Modulo_1"; // ID para identificar este nodo entre muchos
    doc["temperatura"] = tempSimulada;
    doc["humedad_suelo"] = humedadSimulada;

    // Convertir JSON a String para enviarlo
    char buffer[256];
    serializeJson(doc, buffer);

    Serial.print("Publicando mensaje: ");
    Serial.println(buffer);
    
    client.publish(mqtt_topic, buffer);
  }
}