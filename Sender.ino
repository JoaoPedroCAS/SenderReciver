#include <SPI.h>  //responsável pela comunicação serial
#include <LoRa.h>  //responsável pela comunicação com o WIFI Lora
#include <Wire.h>  //responsável pela comunicação i2c
#include "SSD1306.h" //responsável pela comunicação com o display
#include "DHT.h" //responsavel pelo sensor DHT11

// Definição dos pinos 
#define SCK 5 // GPIO5 -- SX127x's SCK
#define MISO 19 // GPIO19 -- SX127x's MISO
#define MOSI 27 // GPIO27 -- SX127x's MOSI
#define CS 18 // GPIO18 -- SX127x's CS
#define RST 14 // GPIO14 -- SX127x's RESET
#define DI00 26 // GPIO26 -- SX127x's IRQ (Interrupt Request)
#define DHTPIN 3 //Pino responsavel por ler os dados do DHT11
#define DHTTYPE DHT11 //Utilizamos o DHT11

#define BAND 915E6 //Frequencia do radio - podemos utilizar ainda : 433E6, 868E6, 915E6

//variável responsável por armazenar o valor do contador (enviaremos esse valor para o outro Lora)
unsigned int counter = 0;

//parametros: address,SDA,SCL 
SSD1306 display(0x3c, 4, 15); //construtor do objeto que controlaremos o display
DHT dht(DHTPIN, DHTTYPE); //Construtor do objeto dht

String rssi = "RSSI --";
String packSize = "--";
String packet ;

void setup()
{
Serial.begin(115200);
Serial.println("Entrando no Setup");
dht.begin(); //Iniciando o DHT11
delay(1000);
//configura os pinos como saida
pinMode(16,OUTPUT); //RST do oled
pinMode(25,OUTPUT);
digitalWrite(16, LOW); // reseta o OLED
delay(50); 
digitalWrite(16, HIGH); // enquanto o OLED estiver ligado, GPIO16 deve estar HIGH

display.init(); //inicializa o display
display.flipScreenVertically(); 
display.setFont(ArialMT_Plain_10); //configura a fonte para um tamanho maior

delay(1500);
display.clear(); //apaga todo o conteúdo da tela do display

SPI.begin(SCK,MISO,MOSI,CS); //inicia a comunicação serial com o Lora
LoRa.setPins(CS,RST,DI00); //configura os pinos que serão utlizados pela biblioteca (deve ser chamado antes do LoRa.begin)
//inicializa o Lora com a frequencia específica.
if (!LoRa.begin(BAND))
{
display.drawString(0, 0, "Starting LoRa failed!");
display.display(); //mostra o conteúdo na tela
while (1);
}
//indica no display que inicilizou corretamente.
display.drawString(0, 0, "LoRa Initial success!");
display.display(); //mostra o conteúdo na tela
delay(1000);
Serial.println("Saindo do Setup");
}

void loop()
{
display.clear();////apaga o conteúdo do display
display.setTextAlignment(TEXT_ALIGN_LEFT); //Alinhamento do texto
display.setFont(ArialMT_Plain_16); //Ajustes da fonte

float h = dht.readHumidity(); //lendo a umidade
float t = dht.readTemperature(); //lendo a temperatura
display.drawString(0,0,"Umi: "+ String(h)+"%"); //Escrevendo o conteudo a ser mostrado no display
display.drawString(0, 15, "Temp: "+ String(t)+"ºC");
display.display(); //Mostrando o conteudo no display

//beginPacket : abre um pacote para adicionarmos os dados para envio
LoRa.beginPacket();
//print: adiciona os dados no pacote
LoRa.print("Umi: ");
LoRa.print(h);
LoRa.println("%");
LoRa.print("Temp: ");
LoRa.print(t);
LoRa.println("ºC");
//endPacket : fecha o pacote e envia
LoRa.endPacket(); //retorno= 1:sucesso | 0: falha
delay(60000); //Delay entre leituras de 1 minuto
}
