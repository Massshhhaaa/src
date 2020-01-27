#include <SPI.h>
#include <RF24.h>
#include <Arduino.h>

RF24 radio(2, 15);
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

int incoming_identification_request[2];
int request_for_identification[] = {111, 222};
int identification_telemetry[2];
int incoming_id[2];

bool identification_procedure_start = false;

void setup(){
  Serial.begin(115200);
  radio.begin();
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)
  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS);  //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);
  radio.powerUp();

  identification();
}

void loop(){
}


void identification(){
  radio.openReadingPipe(1, address[1]);
  radio.startListening();
      // branch for passive participants
     while (true){
       if (radio.available()){
         radio.read(&incoming_identification_request, sizeof(incoming_identification_request));
         Serial.print("принял");
         if (incoming_identification_request[0] == 111){
           Serial.print("этот пидорас получил запрос на идентификацию");
           identification_telemetry[0] = 20;
           identification_telemetry[1] = 30;
           radio.writeAckPayload(1, &identification_telemetry, sizeof(identification_telemetry));
           Serial.println("отправил пакет телеметрии с id");
       }}

        // branch for beginner
        if ((Serial.available() > 0) && (Serial.read() == 'S')){
        // identification_procedure_start = true;
         radio.openWritingPipe(address[1]);
         radio.stopListening();
         radio.write(&request_for_identification, sizeof(request_for_identification), true);
         Serial.println("отправил request ");
     }
}}
