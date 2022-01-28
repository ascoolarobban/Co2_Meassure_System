

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include <MQ2.h>
#include "LowPower.h"

#define FONA_RX 9
#define FONA_TX 8
#define FONA_RST 7
#define lipo A3

long randNumber;
//double atm_pressure;
int half_hour = 60000 * 30;

SoftwareSerial SIM800ss = SoftwareSerial(8, 9);
Adafruit_FONA SIM800 = Adafruit_FONA(FONA_RST);
Adafruit_BMP085 bmp;

unsigned long nextUpdate;
int count = 0;

int RED = 2;
int GREEN = 3;
int mq2Pin = 0;

MQ2 mq2(mq2Pin);
char http_cmd[80];
char url_string[] = "api.thingspeak.com/update?api_key=xxxxxxxxxxxxxxx";
char atm_pressure_string[20];

int net_status;

uint16_t statuscode;
int16_t length;
String response = "";
char buffer[512];
int creg_count = 0;
boolean gprs_on = false;
boolean tcp_on = false;
void(* resetFunc) (void) = 0;
void setup() {
  mq2.begin();
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  while (!Serial);

  Serial.begin(115200);

  Serial.println("STARTAR UPP!");
  Serial.println("Initializing SIM800L....");

  SIM800ss.begin(4800);

  if (! SIM800.begin(SIM800ss)) {
    Serial.println("Couldn't find SIM800L");
    while (1);
  }

  Serial.println("SIM800L is OK");
  delay(1000);



  Serial.println("Waiting to be registered to network...");
  SIM800ss.println("AT+CREG=1");
  net_status = SIM800.getNetworkStatus();
  while (net_status != 1) {
    net_status = SIM800.getNetworkStatus();
    delay(2000);
    Serial.println("TEST RESPONS: ");
    Serial.println(SIM800.getNetworkStatus());
    if (SIM800.getNetworkStatus() == 0) {
      creg_count ++;
      Serial.print("CREG counter: ");
      Serial.println(creg_count);
    }
    if (creg_count >= 5) {
      creg_count = 0;
      resetFunc();
    }


  }
  Serial.println("Registered to home network!");
  Serial.print("Turning on GPRS... ");
  delay(2000);
  while (!gprs_on) {
    if (!SIM800.enableGPRS(true)) {
      Serial.println("Failed to turn on GPRS");
      Serial.println("Trying again...");
      delay(2000);
      gprs_on = false;
      resetFunc();

    } else {
      Serial.println("GPRS now turned on");
      delay(2000);
      gprs_on = true;
      blinkGreen();
    }
  }
}

void loop() {
  double atm_pressure;
  if (millis() >= nextUpdate)
  {
    Serial.print("Minute: ");
    Serial.println(count++);
    nextUpdate = millis() + 60000UL;
    if (count > 60) {
      count = 0;
    }
  }
  if (count == 60) {
    Serial.println("Initializing process");
    int lipoV = analogRead(lipo);
    Serial.println(response);
    lipoValue(lipoV);
    //float co = mq2.readCO();
    delay(5000);

    atm_pressure = random(1000000, 2000000);
    dtostrf(atm_pressure, 5, 0, atm_pressure_string);
    sprintf(http_cmd, "%s=%s", url_string, atm_pressure_string);
    delay(2000);


    while (!tcp_on) {
      if (!SIM800.HTTP_GET_start(http_cmd, &statuscode, (uint16_t *)&length)) {
        digitalWrite(RED, HIGH);
        Serial.println("Failed!");
        Serial.println("Trying again...");
        tcp_on = false;
        digitalWrite(RED, LOW);

        Serial.println("Resetting");
        blinkRed();
        resetFunc();
      } else {

        tcp_on = true;
        atm_pressure = random(1000000, 2000000);
        dtostrf(atm_pressure, 5, 0, atm_pressure_string);
        sprintf(http_cmd, "%s=%s", url_string, atm_pressure_string);
        digitalWrite(GREEN, HIGH);
        while (length > 0) {
          while (SIM800.available()) {
            char c = SIM800.read();
            response += c;
            length--;
          }
        }
      }
      Serial.println(response);
      if (statuscode == 200) {
        Serial.print("Value: ");
        Serial.println(atm_pressure);
        Serial.println("Success!");
        tcp_on = false;
        delay(5000);
        digitalWrite(GREEN, LOW);
      }

    }

  }




  // LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
void blinkRed() {
  digitalWrite(RED, HIGH);
  delay(100);
  digitalWrite(RED, LOW);
  delay(100);
  digitalWrite(RED, HIGH);
  delay(100);
  digitalWrite(RED, LOW);
  delay(100);
  digitalWrite(RED, HIGH);
  delay(100);
  digitalWrite(RED, LOW);
  delay(100);
}


void blinkGreen() {
  digitalWrite(GREEN, HIGH);
  delay(100);
  digitalWrite(GREEN, LOW);
  delay(100);
  digitalWrite(GREEN, HIGH);
  delay(100);
  digitalWrite(GREEN, LOW);
  delay(100);
  digitalWrite(GREEN, HIGH);
  delay(100);
  digitalWrite(GREEN, LOW);
  delay(100);
}
void lipoValue(int lipoV) {
  if (lipoV <= 650) {
    Serial.println("Lipo level: LOW");
  }
  if (lipoV > 800 && lipoV <= 950) {
    Serial.println("Lipo level: 50%");
  }
  if (lipoV > 800 && lipoV <= 950) {
    Serial.println("Lipo level: 50%");
  }
  if (lipoV > 950) {
    Serial.println("Lipo level: Full");
  }


}
