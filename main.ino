#include <SPI.h>
#include <RF24.h>
#include <Arduino.h>
#include <EEPROM.h>

RF24 radio(2, 15);
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

int incoming_identification_request;
int request_for_identification = 1111;
int identification_telemetry = 123123;

int incoming_id;
int identificators_list[];

bool identification_procedure_start = false;
bool ident_telemetry_accep = false;

//read eeprom memory for identification and ID sending delay
int id;
int queue_delay;

void setup(){
  Serial.begin(115200);
  radio.begin();
  EEPROM.begin(512);
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)
  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS);  //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(2, 15);    //(время между попыткой достучаться, число попыток)
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
  EEPROM.get(0, id); EEPROM.get(2; queue_delay);

     while (true){
       // byte b = Serial.read(); расставление задержки на идентификацию через сом
       // int temp_delay = 1;
       // //если получен символ '1', то светодиод включается
       //  if (b == 49) temp_delay = 1000;


        // branch for passive participants
       if (radio.available()){
         radio.read(&incoming_identification_request, sizeof(incoming_identification_request));
         if (incoming_identification_request == 1111){
           Serial.println("этот пидорас получил запрос на идентификацию");
           radio.closeReadingPipe(1);
           radio.openWritingPipe(address[1]);//актуально будет поменять трубу для телеметрии
           radio.stopListening();
           delay(queue_delay);
           radio.write(&identification_telemetry, sizeof(identification_telemetry));
           Serial.println("identification_telemetry was be seng beginner");
           radio.startListening();
           break;
       }}

        // branch for beginner
        if ((Serial.available() > 0) && (Serial.read() == 'S')){
        // identification_procedure_start = true;
         radio.stopListening();
         radio.openWritingPipe(address[1]);
         radio.write(&request_for_identification, sizeof(request_for_identification), true);
         Serial.println("sended request for ident");

         ident_telemetry_acceptance(true);
         for (int i = 0; i < 10; i = i + 1) {
            Serial.println(identificators_list[i]);
          }

     }
}}

// identification telemetry work
void ident_telemetry_acceptance(bool ident_telemetry_accep){
  uint8_t time = millis();
  int i = 0;
  radio.openReadingPipe(1, address[1]);
  radio.startListening();

  while((ident_telemetry_accep) && (millis() - time < 5000)){
    if (radio.available()){
      radio.read(&incoming_id, sizeof(incoming_id));
      Serial.println("id получен, придурок");
      identificators_list[i] = incoming_id;
      i++;
    }
  }
}
