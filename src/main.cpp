#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h> 

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
void devEnv(ssid, password);
void serverSetup();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  loadEnv();
  //devEnv("your_ssid", "your_password");
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
    digitalWrite(relay1, HIGH);
    server.send(200, "text/plain", "Relé ligado");

  });

  server.on("/desligar", []() {
    digitalWrite(relay1, LOW);
    server.send(200, "text/plain", "Relé desligado");

  });

  server.begin();
  Serial.println("Server configurado!");
}

void devEnv(String ssid,String password) {
  // Configurar o IP estático
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Falha ao configurar IP estático");
  }


  // Conecta à rede Wi-Fi
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado!");

  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
};

void loadEnv() {

  // Inicia o LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Erro ao montar o sistema de arquivos LittleFS");
    return;
  }

  // Abrir e ler o arquivo .env
  File file = LittleFS.open("/.env", "r"); // Abre o arquivo para leitura

  if (!file) {
    Serial.println("Erro ao abrir o arquivo .env");
    return;
  }

  // Ler o conteúdo do arquivo
  while (file.available()) {
    String line = file.readStringUntil('\n'); // Lê até a nova linha
    int separatorIndex = line.indexOf('=');
    //pega key e value separados
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

  file.close();

  // Configurar o IP estático
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Falha ao configurar IP estático");
  }


  // Conecta à rede Wi-Fi
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado!");

  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}
