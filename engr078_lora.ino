/*
  RadioLib SX1276 Transmit Example using the 1W LoRa Module @ 915MHz

  Modified by Thomas Makin, 2024

  Gotchas:
      The RadioLib defaults the SX1276 to 434MHz so the reception is pretty
      poor. This is fixed with a radio.begin(915.0);
      The RadioLib really requires DIO0 and DIO1 be connected. Reset is optional.
      Avoid ESP pins 34/35/36/39 as they are inputs only
*/

#include <RadioLib.h>  //Click here to get the library: http://librarymanager/All#RadioLib

// SX1276 requires the following connections:
int pin_cs = D10;
int pin_dio0 = D7;
int pin_nrst = PA0;
int pin_dio1 = D6;
SX1276 radio = new Module(pin_cs, pin_dio0, pin_nrst, pin_dio1);

void setup() {
  Serial.begin(115200);
  delay(500);  //Wait for ESP32 to be able to print

  Serial.print(F("[SX1276] Initializing ... "));
  //int state = radio.begin(); //-121dBm
  //int state = radio.begin(868.0); //-20dBm
  int state = radio.begin(915.0, 31.25, 12, 8, 18, 20, 8);
  //int state = radio.beginFSK(915.0, 4.8, 5.0, 31.25, 20);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("init success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
      ;
  }

  // set output power to 10 dBm (accepted range is -3 - 17 dBm)
  // NOTE: 20 dBm value allows high power operation, but transmission
  //       duty cycle MUST NOT exceed 1%
  // if (radio.setOutputPower(20) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
  //   Serial.println(F("Selected output power is invalid for this module!"));
  //   while (true)
  //     ;
  // }

  // some modules have an external RF switch
  // controlled via two pins (RX enable, TX enable)
  // to enable automatic control of the switch,
  // call the following method
  int pin_rx_enable = D8;
  int pin_tx_enable = D9;
  radio.setRfSwitchPins(pin_rx_enable, pin_tx_enable);

  radio.setCRC(true);
}

int counter = 0;

void loop() {
  Serial.print(F("[SX1276] Transmitting packet ... "));

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  // NOTE: transmit() is a blocking method!
  //       See example SX127x_Transmit_Interrupt for details
  //       on non-blocking transmission method.

  // char output[40];
  // sprintf(output, "Counter: %d", counter++);

  // int state = radio.transmit(output);

  // you can also transmit byte array up to 256 bytes long
  
  byte byteArr[] = {0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff};
  int state = radio.transmit(byteArr, 13);
  

  if (state == RADIOLIB_ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F(" success!"));

    // print measured data rate
    Serial.print(F("[SX1276] Datarate:\t"));
    Serial.print(radio.getDataRate());
    Serial.println(F(" bps"));

  } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F("too long!"));

  } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
    // timeout occurred while transmitting packet
    Serial.println(F("timeout!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);
  }

  // wait for a second before transmitting again
  delay(1000);
}
