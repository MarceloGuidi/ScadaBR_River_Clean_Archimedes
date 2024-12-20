//By Marcelo Guidi
//Programa TCC-ETEC
//ModbusIP + WiFiManager
// Exemplo abstraído de https://github.com/tzapu/WiFiManager
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else ESP32
#include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
#include <WiFiManager.h>
//Modbus Registradores Offsets
const int RELE_COIL = 1;
//Pino utilizado
const int relePin = 13;  //GPIO13
//ModbusIP object
ModbusIP mb;
//================ Função Setup =================================
void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);                       //Iniciação da porta Serial
  WiFiManager wm;                             //Criação do objeto "wm" do tipo WIFiManager
  bool res;                                   // Variável para controle de resposta
  res = wm.autoConnect();                     //Auto geração de Ap
  res = wm.autoConnect("AutoConnectAP");      // AP Anônimo
  res = wm.autoConnect("AutoConnectAP", "");  // Proteção por senha do AP
  //======== Bloco de verificação de conexão de Rede ================
  if (!res) {  // Se não estiver conectado a rede...
    Serial.println("============================================");
    Serial.println("");
    Serial.println("A tentativa de conexao falhou...");
    Serial.println("");
    Serial.println("============================================");
    ESP.restart();
  } else {  // Senão, exibe mensagem de conexão realizada
    Serial.println("============================================");
    Serial.println("WiFi Conectado com sucesso!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("============================================");
    delay(1000);
  }
  //Saindo do looping executa os métodos e instruções abaixo
  mb.server();               // Chamada de método server modbus
  pinMode(relePin, OUTPUT);  // Definindo pino 13 como saída
  mb.addCoil(RELE_COIL);     // Chamada do método de inserção através da passagem de parâmetro
}
//================== Função Looping ===========================
void loop() {
  mb.task();                                     // Chamada do método task do objeto modbus
  digitalWrite(relePin, !(mb.Coil(RELE_COIL)));  //Escrita por passagem de parâmetro
  delay(10);                                     // Tempo de resposta em milisegundos
}