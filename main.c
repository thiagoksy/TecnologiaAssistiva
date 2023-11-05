//Criado por: Thiago Yamamoto
#include <IRremote.h>

#define TempoAcionado 5000 // tempo acionado

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
int inversor = 0;

//Funções
void etapa0() {
  // Acende luz branca
  Serial.println("Etapa 0");
  digitalWrite(BUZZER, LOW);
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(WHITE_LIGHT, HIGH);
  digitalWrite(GREEN_LIGHT, LOW);
}

void etapa1_2() { //etapa 1 e 2 nenhum fim de curso ativado
  // Acende luz verde
  Serial.println("Etapa 1 e 2");
  digitalWrite(BUZZER, LOW); //colocar barulho
  digitalWrite(RED_LIGHT, LOW);
  digitalWrite(WHITE_LIGHT, LOW);
  digitalWrite(GREEN_LIGHT, HIGH);

}

void etapa3() { //EMERGENCIA
  // Acende luz vermelha e apaga as outras
  // Fica apitando (alarme)
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

void etapa4() { // Fins de curso  atingido
  // Pisca Luz branca e dá apito
  tempo = millis();
  int estado = 0;
  Serial.println("Etapa 4");
  while (millis() - tempo < TempoAcionado) {
    if(millis()%200 == 0){
      estado = !estado;
    }
    digitalWrite(WHITE_LIGHT, estado);
    digitalWrite(BUZZER, estado);
  }
  digitalWrite(BUZZER, LOW); //colcoar barulho
  digitalWrite(WHITE_LIGHT, LOW);
}

void etapa5() { //Barreira
  tempo = millis();
  int estado = 0;
  Serial.println("Etapa 5");
  while (millis() - tempo < TempoAcionado) {
    digitalWrite(B1_ACIONA, HIGH);
    if(millis()%100 == 0){
      estado = !estado;
    }
    digitalWrite(WHITE_LIGHT, estado);
    digitalWrite(BUZZER, estado);
  }
  digitalWrite(B1_ACIONA, LOW);
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
  etapa0;
  attachInterrupt(B0, etapa3, FALLING);
  attachInterrupt(LDR, etapa5, RISING);
  inversor = 0;

}

void loop() {
  if (digitalRead(B0) == LOW) {
    inversor = 1;
    etapa3();
  }
  else {
    if (digitalRead(B1) == LOW || digitalRead(B2) == LOW) {
      inversor = 1;
      etapa1_2();
    }
    else {
      etapa0();
    }
  }
  if (digitalRead(SW_UP) == LOW) {
    etapa4();
  }
  if (digitalRead(SW_DOWN) == LOW) {
    etapa5();
  }
  /*
    if(sensorIR.decode() && inversor == 0){
       switch(sensorIR.decodedIRData.decodedRawData){
         case 0xF609BF00:{ // descida
           Serial.println("CH-");
           digitalWrite(B1_ACIONA,LOW);
           digitalWrite(B2_ACIONA,HIGH);
           etapa1_2();
           digitalWrite(B1_ACIONA,LOW);
           digitalWrite(B2_ACIONA,LOW);
           break;
         }
         case 0xFF00BF00:{ // emergencia
           digitalWrite(B1_ACIONA,LOW);
           digitalWrite(B2_ACIONA,LOW);
           etapa_3();
           break;
         }
         case 0xFE01BF00:{ // subida
           digitalWrite(B1_ACIONA,HIGH);
           digitalWrite(B2_ACIONA,LOW);
           Serial.println("CH+");
           etapa1_2();
           digitalWrite(B1_ACIONA,LOW);
           digitalWrite(B2_ACIONA,LOW);
           break;
         }
         case 0xF609FF00:{
           Serial.println("EQ");
           break;
         }
         default:{
            etapa0();
           Serial.println("NULO");
           break;
         }

        }
     delay(5000);
     sensorIR.resume();
    }*/
}
