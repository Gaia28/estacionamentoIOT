// ----------------------
// Projeto: Estacionamento Inteligente (ESP32 + HC-SR04)
// Autor: Matheus Gaia
// ----------------------

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "GAIA_CLARO";
const char* password = "Naldogaia";

// Endereço do servidor (troque pelo IP da sua máquina com XAMPP)
// Exemplo: http://192.168.0.10/receber.php
const char* serverUrl = "http://192.168.0.21/sensor/receber.php";
// Estrutura para representar um sensor ultrassônico
struct SensorUltrassonico {
  int trigPin;
  int echoPin;
  String nome;
};

// Lista de sensores (basta adicionar novos)
SensorUltrassonico sensores[] = {
  {32, 34, "Vaga 1"},
  // {25, 26, "Vaga 2"},  // exemplo de expansão
};

const int numSensores = sizeof(sensores) / sizeof(sensores[0]);

// ----------------------
// Função: Inicializa sensores
// ----------------------
void inicializarSensores() {
  for (int i = 0; i < numSensores; i++) {
    pinMode(sensores[i].trigPin, OUTPUT);
    pinMode(sensores[i].echoPin, INPUT);
    digitalWrite(sensores[i].trigPin, LOW);
  }
  delay(100);
}

void conectarWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando...");
  }
  Serial.println("Conectado ao WiFi");
  WiFi.mode(WIFI_MODE_STA);
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());
}

// ----------------------
// Função: Medir distância em metros
// ----------------------
float medirDistancia(SensorUltrassonico s) {
  digitalWrite(s.trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(s.trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(s.trigPin, LOW);

  long duration = pulseIn(s.echoPin, HIGH, 30000); // timeout 30ms (~5m máx)

  if (duration == 0) return -1; // sem leitura

  float distanciaMetros = (duration * 0.0343) / 200.0;
  return distanciaMetros;
}

// ----------------------
// Função: Média de N leituras
// ----------------------
float medirDistanciaMedia(SensorUltrassonico s, int n = 5) {
  float soma = 0;
  int validas = 0;

  for (int i = 0; i < n; i++) {
    float d = medirDistancia(s);
    if (d > 0) { // só considera leituras válidas
      soma += d;
      validas++;
    }
    delay(50);
  }

  if (validas == 0) return -1; // todas falharam
  return soma / validas;
}

// ----------------------
// Função: Verificar vaga
// ----------------------
String verificarVaga(SensorUltrassonico s) {
  float d = medirDistanciaMedia(s);

  if (d == -1) {
    return "Sem leitura";
  }

  if (d >= 0.20 && d <= 1.50) {
    return "OCUPADA";
  } else {
    return "LIVRE";
  }
}

// ----------------------
// Setup
// ----------------------
void setup() {
  Serial.begin(115200);
  conectarWifi();
  inicializarSensores();
}

// ----------------------
// Loop principal
// ----------------------
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    for (int i = 0; i < numSensores; i++) {
      String status = verificarVaga(sensores[i]);

      // Exibe no Serial
      Serial.print(sensores[i].nome);
      Serial.print(": ");
      Serial.println(status);

      // Envia para o servidor PHP
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      String json = "{\"vaga\":\"" + sensores[i].nome + "\",\"status\":\"" + status + "\"}";

      int httpResponseCode = http.POST(json);

      Serial.print("Resposta do servidor: ");
      Serial.println(httpResponseCode);

      http.end();
    }
  } else {
    Serial.println("WiFi desconectado, tentando reconectar...");
    conectarWifi();
  }

  Serial.println("---------------------");
  delay(5000); // envia a cada 5 segundos
}
