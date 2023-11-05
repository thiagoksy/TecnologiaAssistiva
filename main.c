//Criado por: Thiago Yamamoto
#include <IRremote.h>

#define TempoAcionado 5000 // tempo acionado
#define FatorTempo   200
/*
   CH- BA45FF00
   CH  B946FF00
   CH+ B847FF00
   EQ F609FF00
*/

//pinos interrupções 2 e 3
//INPUTS
#define B0 2 //Botão de segurança B0
#define B1 A0 //Botão de subida   B1
#define B2 A1 //Botão de descida  B2
#define LDR 3 //Barreira laser    SW3

#define SW_UP 6// Fim de curso Superior  SW1
#define SW_DOWN 5//Fim de curso Inferior SW2

#define RECEPTOR 4

//OUTPUTS
#define BUZZER 7

#define RED_LIGHT 13
#define GREEN_LIGHT 12
#define WHITE_LIGHT 11

#define B1_ACIONA A4
#define B2_ACIONA A5

typedef enum estados {
  OFF,
  ON,
  SUBIDA,
  EMERGENCIA,
  DESCIDA,
} sts;

//Variáveis
IRrecv sensorIR(RECEPTOR);

unsigned long int tempo = 0;

//Funções
void Etapa0() {
  // Acende luz branca
  Serial.println("Etapa 0");
  digitalWrite(BUZZER, LOW);
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(WHITE_LIGHT, HIGH);
  digitalWrite(GREEN_LIGHT, LOW);
}

void Etapa1_2(int inversor, int direcao) { //etapa 1 e 2 nenhum fim de curso ativado
  // Acende luz verde
  Serial.println("Etapa 1 e 2");
  digitalWrite(BUZZER, LOW); //colocar barulho
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(WHITE_LIGHT, LOW);
  digitalWrite(GREEN_LIGHT, HIGH);
  if(inversor == OFF){
   tempo = millis();
   while (millis() - tempo < TempoAcionado) {
      if(direcao == SUBIDA){
        digitalWrite(B1_ACIONA,LOW);
        digitalWrite(B2_ACIONA,HIGH);
      }
      else{
        digitalWrite(B1_ACIONA,HIGH);
        digitalWrite(B2_ACIONA,LOW);
      }
    } 
  }
  digitalWrite(B1_ACIONA,LOW);
  digitalWrite(B2_ACIONA,LOW);

}

void Etapa3() { //EMERGENCIA
  // Acende luz vermelha e apaga as outras
  // Fica apitando (alarme)
  digitalWrite(B1_ACIONA,LOW);
  digitalWrite(B2_ACIONA,LOW);
  while (true) {
    Serial.println("Etapa 3");
    digitalWrite(BUZZER, HIGH);
    digitalWrite(RED_LIGHT, HIGH);
    digitalWrite(WHITE_LIGHT, LOW);
    digitalWrite(GREEN_LIGHT, LOW);
    delay(500);
    digitalWrite(BUZZER, LOW);
    digitalWrite(RED_LIGHT, HIGH);
    digitalWrite(WHITE_LIGHT, LOW);
    digitalWrite(GREEN_LIGHT, LOW);
    delay(500);
  }
}

void Etapa4(int inversor) { // Fins de curso  atingido
  // Pisca Luz branca e dá apito
  tempo = millis();
  int estado = 0;
  Serial.println("Etapa 4");
  while (millis() - tempo < TempoAcionado) {
    digitalWrite(B1_ACIONA, LOW);
    digitalWrite(B2_ACIONA, HIGH);
    if(millis()%FatorTempo == 0){
      estado = !estado;
    }
    digitalWrite(WHITE_LIGHT, estado);
    digitalWrite(BUZZER, estado);
  }
  digitalWrite(B1_ACIONA, LOW);
  digitalWrite(B2_ACIONA, LOW);
  digitalWrite(BUZZER, LOW);
  Serial.println("Fim da Etapa 5");
  digitalWrite(WHITE_LIGHT, LOW);
}

void Etapa5() { //Barreira
  tempo = millis();
  int estado = 0;
  Serial.println("Etapa 5");
  while (millis() - tempo < TempoAcionado) {
    digitalWrite(B1_ACIONA, HIGH);
    digitalWrite(B2_ACIONA, LOW);
    if(millis()%FatorTempo == 0){
      estado = !estado;
    }
    digitalWrite(WHITE_LIGHT, estado);
    digitalWrite(BUZZER, estado);
  }
  digitalWrite(B1_ACIONA, LOW);
  digitalWrite(B2_ACIONA, LOW);
  digitalWrite(BUZZER, LOW);
  Serial.println("Fim da Etapa 5");
  digitalWrite(WHITE_LIGHT, LOW);
}

void setup() {
  Serial.begin(9600);
  sensorIR.enableIRIn();


  pinMode(B0, INPUT);
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(LDR, INPUT);
  pinMode(SW_UP, INPUT);
  pinMode(SW_DOWN, INPUT);

  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LIGHT, OUTPUT);
  pinMode(WHITE_LIGHT, OUTPUT);
  pinMode(GREEN_LIGHT, OUTPUT);
  pinMode(B1_ACIONA, OUTPUT);
  pinMode(B2_ACIONA, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(WHITE_LIGHT, LOW);
  digitalWrite(GREEN_LIGHT, LOW);

  digitalWrite(B1_ACIONA, LOW);
  digitalWrite(B2_ACIONA, LOW);
  Etapa0();
  attachInterrupt(B0, Etapa3, FALLING);
  attachInterrupt(LDR, Etapa5, RISING);
}

void loop() {
  
  if (digitalRead(B0) == LOW) {
    Etapa3();
  }
  else {
    if (digitalRead(B1) == LOW || digitalRead(B2) == LOW) {
      Etapa1_2(ON,OFF);
    }
    else {
      Etapa0();
    }
  }
  if (digitalRead(SW_UP) == LOW) {
    Etapa4(ON);
  }
  if (digitalRead(SW_DOWN) == LOW) {
    Etapa5();
  }

  if(sensorIR.decode()){
     switch(sensorIR.decodedIRData.decodedRawData){
       case 0xBA45FF00:{ // descida
         Serial.println("CH-");
         Etapa1_2(OFF,DESCIDA);
         break;
       }
       case 0xB946FF00:{ // emergencia
         Etapa3();
         break;
       }
       case 0xB847FF00:{ // subida
         Serial.println("CH+");
         Etapa1_2(OFF,SUBIDA);
         break;
       }
       case 0xF609FF00:{
         Serial.println("EQ");
         break;
       }
       default:{
         Serial.println("NULO");
         break;
       }
      }
     Etapa0();
     sensorIR.resume();
    }
}
