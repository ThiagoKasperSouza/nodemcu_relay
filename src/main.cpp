#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "config.h"
#include <FS.h> // Inclua a biblioteca SPIFFS

String ssid;
String password;

// Crie um servidor na porta 80
ESP8266WebServer server(80);

// pino digital do rele
const int relay1 = D1;

// Configurações de IP estático

IPAddress local_IP(192, 168, 1, 100); // IP estático
IPAddress gateway(192, 168, 1, 1);     // Gateway (normalmente o IP do roteador)
IPAddress subnet(255, 255, 255, 0);    // Máscara de sub-rede


void loadEnv();
void setupServer();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  loadEnv();
  serverSetup();
  Serial.println("Hello world");

  // Inicialize os relés como desligados
  digitalWrite(relay1, LOW);
}

void loop() {
  // Trata as requisições do servidor
  server.handleClient();
}

void serverSetup() {
   // Defina as rotas do servidor
  server.on("/ligar", []() {
    digitalWrite(relayPin, HIGH);
    server.send(200, "text/plain", "Relé ligado");

  });

  server.on("/desligar", []() {
    digitalWrite(relayPin, LOW);
    server.send(200, "text/plain", "Relé desligado");

  });

  server.begin();
}

void loadEnv() {
  // Inicie o SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Erro ao montar o sistema de arquivos");
    return;

  }

  // Abrir e ler o arquivo .env
  File file = SPIFFS.open("/.env", "r"); // Abre o arquivo para leitura
  if (!file) {
    Serial.println("Erro ao abrir o arquivo .env");
    return;

  }

  // Ler o conteúdo do arquivo
  while (file.available()) {
    String line = file.readStringUntil('\n'); // Lê até a nova linha
    int separatorIndex = line.indexOf('=');

    if (separatorIndex != -1) {
      String key = line.substring(0, separatorIndex);
      String value = line.substring(separatorIndex + 1);

      // Armazena as variáveis
      if (key == "SSID") {
        ssid = value;
      } else if (key == "PASSWORD") {
        password = value;
      }
    }
  }
  file.close(); // Fecha o arquivo

  // Configurar o IP estático
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Falha ao configurar IP estático");
  }


  // Conecte-se à rede Wi-Fi
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado!");

  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}
