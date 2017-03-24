/*
 * This is an example to show how to use HTTP GET with GPRSbee
 * We're doing a GET of the SODAQ time service to set the RTC
 * of the SODAQ board.
 *
 * If you want to analyse the AT commands sent to the GPRSbee
 * you can connect a UART to the D4/D5 grove of the SODAQ board.
 *
 * Note. After uploading the program to the SODAQ board you must
 * disconnect the USB, because the GPRSbee uses the same RX/TX
 * of the MCU. It will disrupt the GPRSbee connection.
 */

#define APN "internet.access.nl"
#define TIMEURL "http://time.sodaq.net/"

#include <Sodaq_DS3231.h>
#include <GPRSbee.h>
#include <Wire.h>

int BEE_VCC_PIN = -1;
int BEE_DTR_PIN = 23;  // Bee DTR Pin (Data Terminal Ready - used to turn on/off)
int BEE_CTS_PIN = 19;   // Bee CTS Pin (Clear to Send)


//######### forward declare #############
void syncRTCwithServer();

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  gprsbee.initGPRS(Serial1, BEE_VCC_PIN, BEE_CTS_PIN, BEE_DTR_PIN, V04);
  gprsbee.setDiag(Serial); // To see for debugging
  Serial.println(F("Program start"));

  // Make sure the GPRSbee is switched off
  gprsbee.off();
  Serial.println(F("Please disconnect USB"));
  delay(4000);

  syncRTCwithServer();
}

void loop()
{
    // Do nothing.
}

void syncRTCwithServer()
{
  char buffer[20];
  if (gprsbee.doHTTPGET(APN, TIMEURL, buffer, sizeof(buffer))) {
    Serial.print("HTTP GET: ");
    Serial.println(buffer);
    char *ptr;
    uint32_t newTs = strtoul(buffer, &ptr, 0);
    // Tweak the timestamp a little because doHTTPGET took a few second
    // to close the connection after getting the time from the server
    newTs += 3;
    if (ptr != buffer) {
      uint32_t oldTs = rtc.now().getEpoch();
      int32_t diffTs = abs(newTs - oldTs);
      if (diffTs > 30) {
        Serial.print("Updating RTC, old=");
        Serial.print(oldTs);
        Serial.print(" new=");
        Serial.println(newTs);
        rtc.setEpoch(newTs);
      }
    }
  }
}
