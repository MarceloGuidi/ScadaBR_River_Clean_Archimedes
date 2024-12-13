// Código_Planta_Usina_Geradora_Energia_TCC
// Abaixo a declaração das bibliotecas utilizadas:
#include <ThingSpeak.h>       //Biblioteca para acesso a nuvem IoT via ESP32
#include <WiFi.h>             //Biblioteca para conexão WiFi
#include <Ultrasonic.h>       // Biblioteca para fuções do Ultrasom
#include <ESP_FlexyStepper.h> //Biblioteca do Motor de Passo

int PassosMotor = 25;    // Criação da variável de tipo inteiro "PassosMotor" e definindo pino 25
int SentidoRotacao = 26; // Criação da variável de tipo inteiro "SentidoRotação" e definindo pino 26
int motorLixo = 15;      // Criação da variável de tipo inteiro "motorLixo" e definindo pino 15
int RPME = 5;            // Criação da variável de tipo inteiro "RPME" e definindo pino 5
int rpm;                 // Criação da variável de tipo inteiro "rpm"
int Bomba2300 = 18;      // Criação da variável de tipo inteiro "Bomba2300" e definindo pino 18
int Bomba1950 = 19;      // Criação da variável de tipo inteiro "Bomba1950" e definindo pino 19
int Bomba800 = 33;       // Criação da variável de tipo inteiro "Bomba800" e definindo pino 33
int Barreira = 32;       // Criação da variável de tipo inteiro "Barreira" e definindo pino 32
int posicao;             // Criação da variável de tipo inteiro "posição"
// Declaração dos pino 16 e 17 para serem usados no sensor ultrasom
Ultrasonic ultrassom(16, 17);       // Criação da variável de tipo Ultrasonic "Ultrasom" e definindo os pinos 16 e 17
ESP_FlexyStepper stepper;           // Criação da variável "stepper" de tipo ESP_FlexyStepper .
long distancia;                     // Criação da variável de tipo long "distancia"
volatile byte pulsos;               // Criação da variável de tipo volatile byte "pulsos"
unsigned long millis_old;           // Criação da variável de tipo unsigned long "millis_old"
unsigned int pulsos_por_volta = 20; // Criação da váriável de tipo inteiro longo e atribuindo o valor 20 a ela
//-----------------------------------------------------------------------------
// Função "contador" somador de pulsos
void contador()
{
  pulsos++; // Acrecenta valor à variável pulsos somando mais 1 a cada contagem
}
WiFiClient client;                           // Criação da variável cliente do tipo WIFiCliente
unsigned long ChannelID = 2303823;           // Atribuição de valor à variável "ChannelID" do tipo unsigned long
const char *ReadAPIKey = "CBLZJF25WVCIBCHL"; // Atribuição de valor à variável ReadAPIKey do tipo cosnt char
const int FieldNumber1 = 1;                  // Atribuição do valor 1 à variável FieldNumber1 do tipo const int
const int FieldNumber2 = 2;                  // Atribuição do valor  2 à variável FieldNumber2 do tipo const int
//--------------------------------------------------------------------------------------------
// Função setup sem valor retorno "Setup"
void setup()
{
	pinMode(2, INPUT);          // Define o pino 2 como entrada
	pinMode(4, INPUT);          // Defini o pino 4 como entrada
	pinMode(motorLixo, OUTPUT); // Define o pino 15 como saída
	pinMode(RPME, INPUT);       // Define o pino 5 como entrada
	pinMode(Bomba2300, OUTPUT); // Define o pino 18 como saída
	pinMode(Bomba1950, OUTPUT); // Define o pino 19 como saída
	pinMode(Bomba800, OUTPUT);  // Define o pino 33 como saída
	pinMode(Barreira, OUTPUT);  // Define o pino 32 como saída
  //------------------------------------------------------------------------------------------
  // O modo LOW dispara o evento continuamente
  // O modo CHANGE dispara o evento apenas uma vez
  // O modo RISING dispara o evento: Sai de LOW e parte para HIGH
  // O modo FALLING dispara o evento: Saindo de HIGH e vai para LOW
  attachInterrupt(5, contador, FALLING); // Definição de comando de interrupção no pino 5 e ativação da função "contador"
  //------------------------------------------------------------------------------------------
  // Chamada dos métodos da função stepper "Motor de Passo"
  stepper.connectToPins(PassosMotor, SentidoRotacao); // Método connectToPins
  stepper.setSpeedInStepsPerSecond(2000);             // Método setSpeedInStepsPerSecond
  // Método setAccelerationInStepsPerSecondPerSecond
  stepper.setAccelerationInStepsPerSecondPerSecond(1000);
  //------------------------------------------------------------------------------------------
  pulsos = 0;           // Atribuição do valor '0' a variável 'pulsos'
  rpm = 0;              // Atribuição do valor '0' a variável 'rpm'
  millis_old = 0;       // Atribuição do valor '0' a variável 'millis_old'
  posicao = 0;          // Atribuição do valor '0' a variável 'posição'
  Serial.begin(115200); // Inicialização da conexão serial com velocidade definida em '115200'
  Serial.println("");     // Imprime linha em branco no terminal Serial
  //-------------------------------------------------------------
  // Enquanto pino 4 e pino 2 estiverem ligados executa o código dentro da condicional
  while (digitalRead(4) == HIGH && digitalRead(2) == HIGH)
  {
    // Imprime no monitor serial a mensagem de execução de movimento do motor de passo
    Serial.println("Executando Homing");
    delay(300);                        // tempo de 1/2 segundo
    stepper.moveRelativeInSteps(-100); // Movimenta o motor em '-100'
  }
  stepper.moveRelativeInSteps(0);
  // Imprime no monitor serial a mensagem de movimento executado "ok"
  Serial.println("Homing Ok");
  delay(300); // tempo de 2 segundos
  //---------------------------------------------------------------
  while (digitalRead(4) != LOW) // Enquanto pino 4 estiver desligado
  {
    Serial.println("Executando Homing"); // Imprime no monitor serial a mensagem de execução de movimento do motor de passo
    delay(300);                          // tempo de 1/2 segundo
    stepper.moveRelativeInSteps(-1000);  // Movimenta o motor em '-100'
    stepper.moveRelativeInSteps(0);
    delay(300); // tempo de 1/2 segundo
  }
  stepper.moveRelativeInSteps(0);
  Serial.println("Homing Ok");
  delay(300); // tempo de 2 segundos
  //=============================== CONEXÃO WIFI  ============================================
  // Chamada ao método 'begin' da função WiFi passando os valores "DESKTOP-F3R7F79 6901","87944>Cm" como parâmetros
  WiFi.begin("River_Clean_1", "12345678");
  // Imprime a mensagem "conectado" se não houver erros de conexão à rede WiFi
  Serial.println("Conectando a rede: River_Clean_1");
  // Execução de condicional caso haja erro de conexão à rede WiFi
  while (WiFi.status() != WL_CONNECTED) // Enquanto não estiver conectado
  {
    delay(300);        // tempo de 1/2 segundo
    Serial.println("..."); // Imprime no monitor serial o caractere "." durante as tentativas de conexão sem sucesso
    Serial.println("Rede não conectada!");

  }
  Serial.println();                             // Imprime linha em branco
  Serial.println("Conectado a rede River_Clean..."); // imprime mensagem a "DESKTOP-F3R7F79 6901","87944>Cm"  no monitor serial
  Serial.println("===============================");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());               // Imprime o número de ip da rede conectada no monitor serial
  Serial.println("===============================");
  ThingSpeak.begin(client);                     // Chamada da função begin(cliente) do método ThingSpeak
  analogWrite(motorLixo, 25);                   // Escrita de valores analógicos coletados no pino 25
}
//================================ FUNÇÃO lOOPING  ===================================
// Função looping sem valor de retorno
void loop()
{
  while (WiFi.status() != WL_CONNECTED) // Enquanto não estiver conectado
  {
    delay(300);        // tempo de 1/2 segundo
    Serial.println("Rede não conectada!"); // Imprime no monitor serial o caractere "." durante as tentativas de conexão sem sucesso
  }
  Serial.println("Conectado a rede River_Clean..."); // imprime mensagem a "DESKTOP-F3R7F79 6901","87944>Cm"  no monitor serial
  Serial.println("===============================");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());               // Imprime o número de ip da rede conectada no monitor serial
  Serial.println("===============================");
  delay(300);
// Variável 'A' recebe o retorno so servidor ThingSpeak
  int A = ThingSpeak.readLongField(2303823, FieldNumber1, "CBLZJF25WVCIBCHL");

  if (A == 1)
  {
    if (digitalRead(4) == 1)
    {
      while (digitalRead(4) == 1 && digitalRead(2) == 0)
      {
        stepper.moveRelativeInSteps(-1000);
        delay(300);
      }
    }
    if (digitalRead(2) == 1)
    {
      while (digitalRead(4) == 1 && digitalRead(2) == 1)
      {
        stepper.moveRelativeInSteps(-100);
        delay(300);
      }
    }
    stepper.moveRelativeInSteps(0);
    distancia = ultrassom.Ranging(CM);
    Serial.println(A);
    digitalWrite(Bomba2300, A);
    digitalWrite(Bomba1950, A);
    digitalWrite(Bomba800, A);
    digitalWrite(Barreira, A);
    delay(300);
    //================================================================================
    if (millis() - millis_old >= 1100)
    {
      detachInterrupt(5);
      rpm = (60 * 1000 / pulsos_por_volta) / (millis() - millis_old) * pulsos;
      millis_old = millis();
      pulsos = 0;
      Serial.print("RPM =  ");
      Serial.println(rpm);
      attachInterrupt(5, contador, FALLING);
    }
    Serial.print(distancia);
    Serial.println(" cm");
    delay(300);
    int A = ThingSpeak.readLongField(2303823, FieldNumber1, "CBLZJF25WVCIBCHL");
  }
  if (A == 0)
  {
    int A = ThingSpeak.readLongField(2303823, FieldNumber1, "CBLZJF25WVCIBCHL");
    distancia = ultrassom.Ranging(CM);
    if (millis() - millis_old >= 1100)
    {
      // Desabilita interrupcao enquanto faz o cálculo
      detachInterrupt(5);
      rpm = (60 * 1000 / pulsos_por_volta) / (millis() - millis_old) * pulsos;
      millis_old = millis();
      pulsos = 0;
      Serial.print("RPM =  ");
      Serial.println(rpm);
      attachInterrupt(5, contador, FALLING);
    }
    Serial.print(distancia);
    Serial.println(" cm");
    delay(300);
    Serial.println(A);
    digitalWrite(Bomba2300, A);
    digitalWrite(Bomba1950, A);
    digitalWrite(Bomba800, A);
    digitalWrite(Barreira, A);
    delay(300);
    if (distancia >= 17)
    {
      if (digitalRead(2) == HIGH && digitalRead(4) == LOW)
      {
        stepper.moveRelativeInSteps(1050);
        delay(300);
      }
      stepper.moveRelativeInSteps(0);
      if (digitalRead(2) == 1 && digitalRead(4) == 1)
      {
        stepper.moveRelativeInSteps(700);
      }
      stepper.moveRelativeInSteps(0);
    }
    else if (distancia < 17)
    {
      if (digitalRead(4) == 0)
      {
        stepper.moveRelativeInSteps(300);
        delay(300);
      }
      stepper.moveRelativeInSteps(0);
      if (digitalRead(2) == 0)
      {
        stepper.moveRelativeInSteps(-700);
        delay(300);
      }
      stepper.moveRelativeInSteps(0);
      delay(300);
    }
    Serial.println(A);       // Imprimi no prompt Serial o conteúdo da variável 'A'
    Serial.print("RPM =  "); // Imprimi no prompt Serial o conteúdo da variável 'RPM'
    Serial.println(rpm);     // Imprimi no prompt Serial o conteúdo da variável 'rpm'
    // Abaixo o modo FALLING dispara o evento: Saindo de HIGH e vai para LOW
    attachInterrupt(5, contador, FALLING);
    Serial.print(distancia); // Imprimi no prompt Serial o conteúdo da variável 'distância'
    Serial.println(" cm");    // Imprimi no prompt Serial o conteúdo da variável 'cm'
    delay(300);              // Aguarda um tempo de 100 milisegundos de interrupção de execução do código
  }
}