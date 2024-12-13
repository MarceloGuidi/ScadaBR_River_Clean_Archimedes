#include <ThingSpeak.h>
#include <WiFi.h>
#include <Ultrasonic.h>
#include <ESP_FlexyStepper.h>
int PassosMotor = 25;
int SentidoRotacao= 26;
int motorLixo = 15 ;
int RPME = 5;
int rpm;
int Bomba2300 = 18;
int Bomba1950 = 19;
int Bomba800 = 33;
int Barreira = 32;
int posicao;
ESP_FlexyStepper stepper;
Ultrasonic ultrassom(16,17); 
long distancia;
volatile byte pulsos;
unsigned long millis_old;
unsigned int pulsos_por_volta = 20;
void contador()
{
 pulsos++;
}
WiFiClient client;
unsigned long ChannelID = 2324816;
const char * ReadAPIKey = "F2ZIKBUD4A4OCGQS";
const int FieldNumber1= 1;
const int FieldNumber2= 2;
void setup ()
{ pinMode(2,INPUT);
 pinMode(4,INPUT);
 pinMode(motorLixo,OUTPUT);
 pinMode(RPME, INPUT);
 pinMode(Bomba2300,OUTPUT);
 pinMode(Bomba1950,OUTPUT);
 pinMode(Bomba800,OUTPUT);
 pinMode(Barreira,OUTPUT);
 attachInterrupt(5, contador, FALLING);

 stepper.connectToPins(PassosMotor, SentidoRotacao);
 stepper.setSpeedInStepsPerSecond(2000);
 stepper.setAccelerationInStepsPerSecondPerSecond(1000);

 pulsos = 0;
 rpm = 0;
 millis_old = 0;
 posicao = 0;
 Serial.begin (115200);
 Serial.println();

 while (digitalRead(4)== HIGH && digitalRead(2)==HIGH)
 {
 Serial.println("Executando Homing");
 delay(500);
 stepper.moveRelativeInSteps(-100);
 }
 stepper.moveRelativeInSteps(0);
 Serial.println("Homing Ok");
 delay(2000);

 while (digitalRead(4) != LOW)
 {
 Serial.println("Executando Homing");
 delay(500);
 stepper.moveRelativeInSteps(-1000);
 stepper.moveRelativeInSteps(0);
 delay(500);
 }
 stepper.moveRelativeInSteps(0);
 Serial.println("Homing Ok");
 delay(2000);
 WiFi.begin("Poco X5","Jean1103/05");
 Serial.println("conectando");
 while (WiFi.status() != WL_CONNECTED)
 {
 delay(500);
 Serial.print (".");
 }
 Serial.println();
 Serial.print ("conectado, Endereço de IP: ");
 Serial.println (WiFi.localIP());
 ThingSpeak.begin(client);

 analogWrite(motorLixo, 25);
}
void loop (){

 int A = ThingSpeak.readLongField(2303823, FieldNumber1,"CBLZJF25WVCIBCHL");
if (A == 1)
{
 if (digitalRead(4) == 1)
 {
 while (digitalRead(4)==1 && digitalRead(2)==0){
 stepper.moveRelativeInSteps(-1000);
 delay(500);
 } 
  }
 if (digitalRead(2)==1)
{
 while (digitalRead(4)==1 && digitalRead(2)==1)
 {
 stepper.moveRelativeInSteps(-100);
 delay(500);
 }
}
 stepper.moveRelativeInSteps(0);
 distancia = ultrassom.Ranging(CM);
 Serial.println(A);
 digitalWrite(Bomba2300,A);
 digitalWrite(Bomba1950,A);
 digitalWrite(Bomba800,A);
 digitalWrite(Barreira,A);
 delay(1000);

 if (millis() - millis_old >= 1100)
 {

 detachInterrupt(5);
 rpm = (60 * 1000 / pulsos_por_volta ) / (millis() - millis_old) * pulsos;
 millis_old = millis();
 pulsos = 0;

 Serial.print("RPM = ");
 Serial.println(rpm);
 attachInterrupt(5, contador, FALLING);
 }
 Serial.print(distancia);
 Serial.println("cm");
 delay(100);

 int A = ThingSpeak.readLongField(2303823, FieldNumber1,"CBLZJF25WVCIBCHL");
}
if (A == 0 )
{
 int A = ThingSpeak.readLongField(2303823, FieldNumber1,"CBLZJF25WVCIBCHL");
 distancia = ultrassom.Ranging(CM);
if (millis() - millis_old >= 1100)
 {
 //Desabilita interrupcao enquanto faz o calculo
 detachInterrupt(5);
 rpm = (60 * 1000 / pulsos_por_volta ) / (millis() - millis_old) * pulsos;
 millis_old = millis();
 pulsos = 0;

 Serial.print("RPM = ");
 Serial.println(rpm);
 attachInterrupt(5, contador, FALLING);
 }
 Serial.print(distancia);
 Serial.println("cm");
 delay(100);
 Serial.println(A);
 digitalWrite(Bomba2300,A);
 digitalWrite(Bomba1950,A);
 digitalWrite(Bomba800,A);
 digitalWrite(Barreira,A);
 delay(1000);
 if (distancia >=17) {
 if(digitalRead(2)== HIGH && digitalRead(4)==LOW)

{
 stepper.moveRelativeInSteps(1050);
 delay(500);

}
 stepper.moveRelativeInSteps(0);

if (digitalRead(2)==1 && digitalRead(4)==1) {
 stepper.moveRelativeInSteps(700);

}
 stepper.moveRelativeInSteps(0); }
else if (distancia < 17)
{
 if(digitalRead(4)== 0)

{
 stepper.moveRelativeInSteps(300);
 delay(500);

}
 stepper.moveRelativeInSteps(0);
 if(digitalRead(2)== 0)

{
 stepper.moveRelativeInSteps(
-700);
 delay(500);

}
 stepper.moveRelativeInSteps(0);

delay(500);
}
Serial.println(A);
Serial.print("RPM = ");
Serial.println(rpm);
attachInterrupt(5, contador, FALLING);
Serial.print(distancia);
Serial.println("cm");
delay(100);
}}
