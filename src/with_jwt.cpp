#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <JWT.h> // Biblioteca para manipulação de JWT

// Defina suas credenciais Wi-Fi
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";

// Crie um servidor na porta 80
ESP8266WebServer server(80);

// Pino do relé
const int relayPin = D1;

// Segredo para assinar o JWT
const char* jwtSecret = "SEU_SEGREDO_JWT";

// Função para gerar o token JWT
String generateToken(String username) {
  JWT jwt;
  jwt.setHeader("alg", "HS256");
  jwt.setHeader("typ", "JWT");
  jwt.setClaim("sub", username);
  jwt.setClaim("exp", millis() + 3600000); // Expira em 1 hora
  return jwt.sign(jwtSecret);
}

// Função para validar o token JWT
bool validateToken(String token) {
  JWT jwt;
  return jwt.verify(token, jwtSecret);
}

void setup() {
  Serial.begin(115200);
  
  // Configure o pino do relé como saída
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Desligado inicialmente

  // Conecte-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado!");

  // Rota para autenticação
  server.on("/login", HTTP_POST, []() {
    String username = server.arg("username");
    String password = server.arg("password");

    // Aqui você deve validar o usuário e a senha
    if (username == "admin" && password == "senha") { // Exemplo de validação
      String token = generateToken(username);
      server.send(200, "application/json", "{\"token\":\"" + token + "\"}");
    } else {
      server.send(401, "application/json", "{\"error\":\"Unauthorized\"}");
    }
  });

  // Rota para controlar o relé
  server.on("/relay", HTTP_POST, []() {
    String authHeader = server.header("Authorization");
    if (authHeader.startsWith("Bearer ")) {
      String token = authHeader.substring(7);
      if (validateToken(token)) {
        String action = server.arg("action");
        if (action == "on") {
          digitalWrite(relayPin, HIGH); // Liga o relé
          server.send(200, "text/plain", "Relé ligado");
        } else if (action == "off") {
          digitalWrite(relayPin, LOW); // Desliga o relé
          server.send(200, "text/plain", "Relé desligado");
        } else {
          server.send(400, "text/plain", "Ação inválida");
        }
      } else {
        server.send(401, "application/json", "{\"error\":\"Invalid Token\"}");
      }
    } else {
      server.send(401, "application/json", "{\"error\":\"Authorization header missing\"}");
    }
  });

  // Inicie o servidor
  server.begin();
}

void loop() {
  server.handleClient();
}