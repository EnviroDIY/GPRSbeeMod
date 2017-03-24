#include <GPRSbee.h>

#define APN "yourAPNhere"
#define SERVER "ftp.yourserverhere"
#define USERNAME "username"
#define PASSWORD "password"
#define FTPPATH "/"

int BEE_VCC_PIN = -1;
int BEE_DTR_PIN = 23;  // Bee DTR Pin (Data Terminal Ready - used to turn on/off)
int BEE_CTS_PIN = 19;   // Bee CTS Pin (Clear to Send)

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  gprsbee.initGPRS(Serial1, BEE_VCC_PIN, BEE_CTS_PIN, BEE_DTR_PIN, V04);
  gprsbee.setDiag(Serial); // To see for debugging

  delay(4000);
  if (!gprsbee.openFTP(APN, SERVER, USERNAME, PASSWORD)) {
    // Failed to open connection
    return;
  }

  if (!gprsbee.openFTPfile("test_ftp.dat", FTPPATH)) {
    // Failed to open file on FTP server
    gprsbee.off();
  }

  // Append 100 lines with "Hello, world\n" to the FTP file.
  char myData[60];
  strncpy(myData, "Hello, world\n", sizeof(myData));
  for (int i = 0; i < 100; ++i) {
    if (!gprsbee.sendFTPdata((uint8_t *)myData, strlen(myData))) {
      // Failed to upload the data
      gprsbee.off();
      return;
    }
  }

  if (!gprsbee.closeFTPfile()) {
    // Failed to close file. The file upload may still have succeeded.
    gprsbee.off();
    return;
  }

  if (!gprsbee.closeFTP()) {
    // Failed to close the connection. The file upload may still have succeeded.
    gprsbee.off();
    return;
  }

  Serial.println("That's all folks.");
}

void loop()
{
  // Do nothing in this simple example.
}
