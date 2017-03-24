// Fill in your own APN here
const char *APN = "apn.konekt.io";

#include <Arduino.h>
#include <GPRSbee.h>

int BEE_VCC_PIN = -1;
int BEE_DTR_PIN = 23;  // Bee DTR Pin (Data Terminal Ready - used to turn on/off)
int BEE_CTS_PIN = 19;   // Bee CTS Pin (Clear to Send)

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  gprsbee.initGPRS(Serial1, BEE_VCC_PIN, BEE_CTS_PIN, BEE_DTR_PIN, V04);
  gprsbee.setDiag(Serial); // To see for debugging

  // Make sure the GPRSbee is switched off to begin with
  gprsbee.off();

  // Some "data" to send
  const char testData[] = "testdata3 = hello world with newline\n";
  Serial.println(testData);

  // Add a header
  char header[45] = "TOKEN: 12345678-abcd-1234-efgh-1234567890ab";
  gprsbee.addHTTPHeaders(header);

  // Set up the buffer for the recieved data to go into
  char buffer[50];
  memset(buffer, 0, sizeof(buffer));
  bool retval = gprsbee.doHTTPPOSTWithReply(APN, "http://httpbin.org/post",
      testData, strlen(testData), buffer, sizeof(buffer));
  if (retval) {
    Serial.print(("Post result: "));
    Serial.print('"');
    Serial.print(buffer);
    Serial.println('"');
  }
}
void loop()
{
}
