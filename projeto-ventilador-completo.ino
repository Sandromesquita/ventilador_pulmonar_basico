/*
 * TREINAMENTO AUTOMAÇÃO RESIDENCIAL
 * PRÁTICA: Ventilador Automatizado
 * Período: 12/Dez/2020 a 02/Jan/2021
 * Ultima atualização do Código: 25/Dez/2020
 * Instrutores: Sandro Mesquita e Bianor Oliveira
 * Rede Social: 
 * Instagram: @roboticmente, @arduinoceara, @profsandromesquita
 * www.profsandromesquita.com
 * www.roboticmente.blogspot.com
 * www.arduinoceara.com
 */

#include <IRremote.h>     //Inclui a biblioteca IRremote ao código
long int teclaVermelha = 3795534161,
         teclaVerde = 3461693924,
         teclaAmarela = 3234147071,
         teclaAzul = 2046533204; //Valores lidos na clonagem no código anterior
byte botao = 2,         //Botao instalado na porta 2 do arduino
     sensorIR = 3,      //Pino do Arduino que tem o Sensor IR instalado
     releGeral = 4,     //Relé geral que habilita ou desliga o ventilador
     releVel01 = 5,     //Relé que liga o ventilador na Velocidade 01
     releVel02 = 6,     //Relé que liga o ventilador na Velocidade 02
     releVel03 = 7,     //Relé que liga o ventilador na Velocidade 03
     ledVermelho = 8,   //Led indicador que o Ventilador está habilitado
     ledVerde = 9,      //Led indicador que o Ventilador está ligado na velocidade 01
     ledAmarelo = 10,   //Led indicador que o Ventilador está ligado na velocidade 02
     ledAzul = 11;      //Led indicador que o Ventilador está ligado na velocidade 03
byte posBotao = 0;      //Variável da posição do botao em relação ao acionamento das velocidades
IRrecv sensorVentilador(sensorIR);  //Anexa a porta 03 do arduino no objeto sensorVentilador
decode_results codigo;              //Objeto codigo responsável para armazenar o codigo para ser decodificado
bool statusVermelho = 0,
     statusVerde = 0,
     statusAmarelo = 0,
     statusAzul = 0,
     ativaBotao = 0,
     desligou = 0;

void setup() {
  //Inicializa com todos os reles desligados
  digitalWrite(releGeral, !statusVermelho); //Lembrando que o estado de ligar o relé
  digitalWrite(releVel01, !statusVerde);    //é invertido de ligar o led
  digitalWrite(releVel02, !statusAmarelo);  //Led liga com HIGH e desliga com LOW
  digitalWrite(releVel03, !statusAzul);     //Rele liga com LOW e desliga com HIGH
  
  Serial.begin(9600);
  sensorVentilador.enableIRIn();

  pinMode(botao, INPUT_PULLUP); //Configurando pino 2 do Arduino como de entrada

  for (byte pin = 4; pin < 12; pin++) {
    pinMode(pin, OUTPUT);   //Configurando os pinos dos leds e relés como saída
  }

  //Testando leds
  for (byte led = 8; led < 12; led++) {
    digitalWrite(led, HIGH);
    delay(1000);
  }

  //Inicializa com todos os leds desligados
  digitalWrite(ledVermelho, statusVermelho);
  digitalWrite(ledVerde, statusVerde);
  digitalWrite(ledAmarelo, statusAmarelo);
  digitalWrite(ledAzul, statusAzul);

}

void loop() {
  desligou = 0;
  if (sensorVentilador.decode(&codigo)) {
    controleRemoto();
  }

  else if (!digitalRead(botao)) {
    unsigned long tempo1 = millis();
    while (!digitalRead(botao)) {
      int tempo = millis() - tempo1;
      if (tempo > 3000) {
        statusVermelho = 0;
        statusVerde = 0;
        statusAmarelo = 0;
        statusAzul = 0;
        acionamento(statusVermelho, statusVerde, statusAmarelo, statusAzul);
        delay(5000);
        posBotao=0;
        ativaBotao = 0;
        desligou=1;
        break;
      }
    }
    controleManual();
  }
}

void controleManual() {
  ativaBotao = 1;
  if ((posBotao == 0)&&(desligou == 0)) {
    controleRemoto();
  }
  else if ((posBotao == 1)&&(desligou == 0)) {
    controleRemoto();
  }
  else if ((posBotao == 2)&&(desligou == 0)) {
    controleRemoto();
  }
  else if ((posBotao == 3)&&(desligou == 0)) {
    controleRemoto();
  }
  delay(200);
}

void controleRemoto() {
  desligou=0;
  if ((codigo.value == teclaVermelha) or ((posBotao == 0) && (ativaBotao == 1))) {
    statusVermelho = !statusVermelho;
    statusVerde = 0;
    statusAmarelo = 0;
    statusAzul = 0;
    acionamento(statusVermelho, statusVerde, statusAmarelo, statusAzul);
    posBotao=1;
    ativaBotao = 0;
  }

  else if (((codigo.value == teclaVerde) && (statusVermelho == 1)) or ((posBotao == 1) && (ativaBotao == 1))) {
    posBotao = 2;
    statusVerde = 1;  //Liga led Verde e Velocidade 01 do ventilador
    statusAmarelo = 0;
    statusAzul = 0;
    acionamento(statusVermelho, statusVerde, statusAmarelo, statusAzul);
    ativaBotao = 0;
  }

  else if (((codigo.value == teclaAmarela) && (statusVermelho == 1)) or ((posBotao == 2) && (ativaBotao == 1))) {
    posBotao = 3;
    statusVerde = 0;
    statusAmarelo = 1; //Liga led Amarelo e Velocidade 02 do ventilador
    statusAzul = 0;
    acionamento(statusVermelho, statusVerde, statusAmarelo, statusAzul);
    ativaBotao = 0;
  }

  else if (((codigo.value == teclaAzul) && (statusVermelho == 1)) or ((posBotao == 3) && (ativaBotao == 1))) {
    posBotao = 1;
    statusVerde = 0;
    statusAmarelo = 0;
    statusAzul = 1;   //Liga led Azul e Velocidade 03 do ventilador
    acionamento(statusVermelho, statusVerde, statusAmarelo, statusAzul);
    ativaBotao = 0;
  }
  delay(200);
  sensorVentilador.resume();
}

void acionamento(bool statusVermelho, bool statusVerde, bool statusAmarelo, bool statusAzul) {
  //Liga ou desliga os Leds sinalizador
  digitalWrite(ledVermelho, statusVermelho);
  digitalWrite(ledVerde, statusVerde);
  digitalWrite(ledAmarelo, statusAmarelo);
  digitalWrite(ledAzul, statusAzul);

  //Liga ou desliga os relés
  digitalWrite(releGeral, !statusVermelho); //Lembrando que o estado de ligar o relé
  digitalWrite(releVel01, !statusVerde);    //é invertido de ligar o led
  digitalWrite(releVel02, !statusAmarelo);  //Led liga com HIGH e desliga com LOW
  digitalWrite(releVel03, !statusAzul);     //Rele liga com LOW e desliga com HIGH
  delay(200);
}
