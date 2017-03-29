/*
 * Copyright (c) 2013-2015 Kees Bakker.  All rights reserved.
 *
 * This file is part of GPRSbee.
 *
 * GPRSbee is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or(at your option) any later version.
 *
 * GPRSbee is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GPRSbee.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef GPRSBEE_H_
#define GPRSBEE_H_

#include <stdint.h>
#include <Arduino.h>
#include <Stream.h>

#include "Sodaq_GSM_Modem.h"

// Comment this line, or make it an undef to disable
// diagnostic
#define ENABLE_GPRSBEE_DIAG     1

/*!
 * \def SIM900_DEFAULT_BUFFER_SIZE
 *
 * The GPRSbee class uses an internal buffer to read lines from
 * the SIMx00. The buffer is allocated in .init() and the default
 * size is what this define is set to.
 *
 * The function .readline() is the only function that writes to this
 * internal buffer.
 *
 * Other functions (such as receiveLineTCP) can make use of .readline
 * and sometimes it is necessary that the buffer is much bigger. Please
 * be aware that the buffer is allocated once and never freed.
 *
 * You can make it allocate a bigger buffer by calling .setBufSize()
 * before doing the .init()
 */
#define SIM900_DEFAULT_BUFFER_SIZE      64

/*
 * \brief A class to store clock values
 */
class SIMDateTime
{
public:
  SIMDateTime(uint32_t ts=0);
  SIMDateTime(uint8_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss, int8_t tz=0);
  SIMDateTime(const char * cclk);

  enum _WEEK_DAYS_ {
    SUNDAY,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
  };

  uint16_t      year() { return _yOff + 2000; }
  uint8_t       month() { return _m + 1; }
  uint8_t       day() { return _d + 1; }
  uint8_t       hour() { return _hh; }
  uint8_t       minute() { return _mm; }
  uint8_t       second() { return _ss; }

  // 32-bit number of seconds since Unix epoch (1970-01-01)
  uint32_t getUnixEpoch() const;
  // 32-bit number of seconds since Y2K epoch (2000-01-01)
  uint32_t getY2KEpoch() const;

  void addToString(String & str) const;

private:
  uint8_t       conv1d(const char * txt);
  uint8_t       conv2d(const char * txt);

  uint8_t       _yOff;          // Year value minus 2000
  uint8_t       _m;             // month (0..11)
  uint8_t       _d;             // day (0..30)
  uint8_t       _hh;            // hour (0..23)
  uint8_t       _mm;            // minute (0..59)
  uint8_t       _ss;            // second (0..59)
  int8_t        _tz;            // timezone (multiple of 15 minutes)
};


// The versions of GPRSBees available
  typedef enum GPRSVersion {
    V04 = 0,
    V05,
    V06
} typedev;

// A specialized class to switch on/off the GPRSbee module
// The VCC3.3 pin is switched by the Autonomo BEE_VCC pin
// The DTR pin is the actual ON/OFF pin, it is A13 on Autonomo, D20 on Tatu
class GPRSbeeOnOff : public Sodaq_OnOffBee
{
public:
    GPRSbeeOnOff();
    void init(int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin, GPRSVersion version = V06);
    void on();
    void off();
    bool isOn();
private:
    int8_t _vcc33Pin;
    int8_t _onoff_DTR_pin;
    int8_t _status_CTS_pin;
    GPRSVersion _version;
};


class GPRSbeeClass : public Sodaq_GSM_Modem
{
public:

  ////////////////////////////////////////////
  // Higher Level Functions for the GPRSBee //
  ////////////////////////////////////////////
  // These are functions unique to the GPRSBee, not the GSM Modem class
  // Some are essentially identical to GSM Modem class functions, but with
  // different names.  For historical reasons, they are maintained.
  void init(Stream &stream, int status_CTS_pin, int onoff_DTR_pin,
      int bufferSize=SIM900_DEFAULT_BUFFER_SIZE);
  void setPowerSwitchedOnOff(bool x);
  void initNdogoSIM800(Stream &stream, int pwrkeyPin, int vbatPin, int status_CTS_pin,
      int bufferSize=SIM900_DEFAULT_BUFFER_SIZE);
  void initAutonomoSIM800(Stream &stream, int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin,
      int bufferSize=SIM900_DEFAULT_BUFFER_SIZE);
  void initGPRS(Stream &stream, int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin,
      GPRSVersion version=V06, int bufferSize=SIM900_DEFAULT_BUFFER_SIZE);

  // Allow to skip connecting to the GPRS Service
  void setSkipCGATT(bool x=true) { _skipCGATT = x; _changedSkipCGATT = true; }

  // Verifies the GPRSBee is on, there is sufficient signal, and you have
  // successfully registered on the network
  bool networkOn();

  // Allows to set headers for tokens, security, etc
  void addHTTPHeaders(const String headers) { _HTTPHeaders = headers; }
  // Allows to set content type for posts
  void addContentType(const String content) { _contentType = content; }

  // HTTP POST Requests, no reply desired
  bool doHTTPPOST(const char *apn, const char *url, const char *postdata, size_t pdlen);
  bool doHTTPPOST(const char *apn, const String & url, const char *postdata, size_t pdlen);
  bool doHTTPPOST(const char *apn, const char *apnuser, const char *apnpwd,
      const char *url, const char *postdata, size_t pdlen);

  // HTTP POST Requests, with reply
  bool doHTTPPOSTWithReply(const char *apn, const char *url, const char *postdata, size_t pdlen, char *buffer, size_t len);
  bool doHTTPPOSTWithReply(const char *apn, const String & url, const char *postdata, size_t pdlen, char *buffer, size_t len);
  bool doHTTPPOSTWithReply(const char *apn, const char *apnuser, const char *apnpwd,
      const char *url, const char *postdata, size_t pdlen, char *buffer, size_t len);

  // HTTP Get Requests
  bool doHTTPGET(const char *apn, const char *url, char *buffer, size_t len);
  bool doHTTPGET(const char *apn, const String & url, char *buffer, size_t len);
  bool doHTTPGET(const char *apn, const char *apnuser, const char *apnpwd,
      const char *url, char *buffer, size_t len);

  // TCP Connections
  bool openTCP(const char *apn, const char *server, int port, bool transMode=false);
  bool openTCP(const char *apn, const char *apnuser, const char *apnpwd,
      const char *server, int port, bool transMode=false);
  void closeTCP(bool switchOff=true);
  bool isTCPConnected();
  bool sendDataTCP(const uint8_t *data, size_t data_len);
  bool receiveDataTCP(uint8_t *data, size_t data_len, uint16_t timeout=4000);
  bool receiveLineTCP(const char **buffer, uint16_t timeout=4000);

// FTP Connections
  bool openFTP(const char *apn, const char *server,
      const char *username, const char *password);
  bool openFTP(const char *apn, const char *apnuser, const char *apnpwd,
      const char *server, const char *username, const char *password);
  bool closeFTP();
  bool openFTPfile(const char *fname, const char *path);
  bool sendFTPdata(uint8_t *data, size_t size);
  bool sendFTPdata(uint8_t (*read)(), size_t size);
  bool closeFTPfile();

  // SMS (Text Messages)
  bool sendSMS(const char *telno, const char *text);



  ///////////////////////////////
  // Sodaq GSM Modem Functions //
  ///////////////////////////////
  // These functions are needed to complete the virtual functions in the GSM Modem class
  // Not all of these functions are implemented in GPRS, and for backwards
  // compatibility, many are just references to the functions above.  (ie, the
  // functions above could have been re-named, but instead of doing that, they
  // are referenced here.)

  // Returns the default baud rate of the modem.
  // To be used when initializing the modem stream for the first time.
  uint32_t getDefaultBaudrate() { return 9600; }

  // Sets the apn, apn username and apn password to the modem.
  bool sendAPN(const char* apn, const char* username, const char* password);

  // Turns on and initializes the modem, then connects to the network and activates the data connection.
  bool connect(void);

  // Disconnects the modem from the network.
  bool disconnect();

  // Returns true if the modem is connected to the network and has an activated data connection.
  bool isConnected();

  // Functions for different types of signal strength indicators
  int8_t convertCSQ2RSSI(uint8_t csq) const;
  uint8_t convertRSSI2CSQ(int8_t rssi) const;

  NetworkRegistrationStatuses getNetworkStatus();

  NetworkTechnologies getNetworkTechnology() { return UnknownNetworkTechnology; }

  // Get the Received Signal Strength Indication and Bit Error Rate
  bool getRSSIAndBER(int8_t* rssi, uint8_t* ber);

  // Get the Operator Name - NOT IMPLEMENTED
  bool getOperatorName(char* buffer, size_t size) { return false; }
  bool selectBestOperator(Stream & verbose_stream) { return false; }
  bool selectOperatorWithRSSI(const String & oper_long, const String & oper_num,
          int8_t & lastRSSI, Stream & verbose_stream) { return false; }

  // Get Mobile Directory Number - NOT IMPLEMENTED
  bool getMobileDirectoryNumber(char* buffer, size_t size) { return false; }

  // Gets International Mobile Equipment Identity (IMEI)
  // Should be provided with a buffer of at least 16 bytes.
  // Returns true if successful.
  bool getIMEI(char *buffer, size_t buflen);

  // Gets Integrated Circuit Card ID (SIM card number)
  // Should be provided with a buffer of at least 21 bytes.
  // Returns true if successful.
  bool getCCID(char *buffer, size_t buflen);

  // Gets the International Mobile Station Identity.
  // Should be provided with a buffer of at least 16 bytes.
  // Returns true if successful.
  bool getIMSI(char* buffer, size_t buflen);

  // Get SIM status - NOT IMPLEMENTED
  SimStatuses getSimStatus() { return SimStatusUnknown; }

  // Get IP Address - NOT IMPLEMENTED
  IP_t getLocalIP() { return 0; }

  // Get Host IP - NOT IMPLEMENTED
  IP_t getHostIP(const char* host) { return 0; }

  // Returns the sent and received counters - NOT IMPLEMENTED
  bool getSessionCounters(uint32_t* sent_count, uint32_t* recv_count) { return false; }
  // bool getTotalCounters(uint32_t* sent_count, uint32_t* recv_count) { return false; }



  // ==== Sockets
  // None are implemented for GPRS

  int createSocket(Protocols protocol, uint16_t localPort = 0) { return false; }
  bool connectSocket(uint8_t socket, const char* host, uint16_t port) { return false; }
  bool socketSend(uint8_t socket, const uint8_t* buffer, size_t size) { return false; }
  size_t socketReceive(uint8_t socket, uint8_t* buffer, size_t size) { return 0; }
  size_t socketBytesPending(uint8_t socket) { return 0; }
  bool closeSocket(uint8_t socket) { return false; }

  // ==== HTTP

  size_t httpRequest(const char* url, uint16_t port,
          const char* endpoint, HttpRequestTypes requestType = GET,
          char* responseBuffer = NULL, size_t responseSize = 0,
          const char* sendBuffer = NULL, size_t sendSize = 0);

  // ==== TCP
  // All functions implemented above

  // ==== FTP

  bool openFtpConnection(const char* server, const char* username, const char* password, FtpModes ftpMode);
  bool closeFtpConnection();
  bool openFtpFile(const char* filename, const char* path = NULL);
  bool ftpSend(const char* buffer) { return false; }  // NOT IMPLEMENTED
  bool ftpSend(const uint8_t* buffer, size_t size);
  int ftpReceive(char* buffer, size_t size) { return 0; }  // NOT IMPLEMENTED
  bool closeFtpFile();

  // ==== SMS
  int getSmsList(const char* statusFilter = "ALL", int* indexList = NULL, size_t size = 0) { return 0; }
  bool readSms(uint8_t index, char* phoneNumber, char* buffer, size_t size) { return false; }
  bool deleteSms(uint8_t index) { return false; }
  bool sendSms(const char* phoneNumber, const char* buffer) { return sendSMS(phoneNumber, buffer); }

  // MQTT (using this class as a transport)
  bool openMQTT(const char * server, uint16_t port = 1883);
  bool closeMQTT(bool switchOff=true);
  bool sendMQTTPacket(uint8_t * pckt, size_t len);
  size_t receiveMQTTPacket(uint8_t * pckt, size_t size, uint32_t timeout = 20000);
  size_t availableMQTTPacket() { return 0; }  // NOT IMPLEMENTED
  bool isAliveMQTT() { return 0; }  // NOT IMPLEMENTED




  ////////////////////////////////////////////
  // Lower Level Functions for the GPRSBee //
  ////////////////////////////////////////////
  // These are functions unique to the GPRSBee, not the GSM Modem class.

  // Gets Complete TA Capabilities List
  bool getGCAP(char *buffer, size_t buflen);
  // Gets international mobile subscriber identity
  bool getCIMI(char *buffer, size_t buflen);
  // Gets the calling line identity (CLI) of the calling party
  bool getCLIP(char *buffer, size_t buflen);
  // Gets the calling line indentification restriction
  bool getCLIR(char *buffer, size_t buflen);
  // Gets the connected line identity
  bool getCOLP(char *buffer, size_t buflen);
  // Gets the current network operator
  bool getCOPS(char *buffer, size_t buflen);
  // Sets the sim card clock
  bool setCCLK(const SIMDateTime & dt);
  // Gets the network time
  bool getCCLK(char *buffer, size_t buflen);
  // Gets the service provider name from SIM
  bool getCSPN(char *buffer, size_t buflen);
  // Gets the SIM card group identifier
  bool getCGID(char *buffer, size_t buflen);
  // Sets/gets Unsolicited Result Codes (URC) presentation value (0 disables, 1 enables)
  bool setCIURC(uint8_t value);
  bool getCIURC(char *buffer, size_t buflen);
  // Sets/gets the phone functionality mode
  bool setCFUN(uint8_t value);
  bool getCFUN(uint8_t * value);

  // Enables/disables the local time stamp functions
  // When these are enabled getCCLK can be used to get the network time
  void enableLTS();
  void disableLTS();
  // Enables/disables initial Unsolicited Result Codes (URC) presentation value
  // These functions are equivalent to using setCUIRC(1) and setCUIRC(0)
  void enableCIURC();
  void disableCIURC();

  // Basic functions to send an AT command to the SIM and wait for it
  // to return an OK
  bool sendCommandWaitForOK(const char *cmd, uint16_t timeout=4000);
  bool sendCommandWaitForOK(const String & cmd, uint16_t timeout=4000);
  bool sendCommandWaitForOK_P(const char *cmd, uint16_t timeout=4000);

  // Using CCLK, get 32-bit number of seconds since Unix epoch (1970-01-01)
  uint32_t getUnixEpoch() const;
  // Using CCLK, get 32-bit number of seconds since Y2K epoch (2000-01-01)
  uint32_t getY2KEpoch() const;

  // Getters of diagnostic values
  uint32_t getTimeToOpenTCP() { return _timeToOpenTCP; }
  uint32_t getTimeToCloseTCP() { return _timeToCloseTCP; }


  // Middle functions unlikely to be needed, but left public
  bool doHTTPPOSTmiddle(const char *url, const char *postdata, size_t pdlen);
  bool doHTTPPOSTmiddleWithReply(const char *url, const char *postdata, size_t pdlen, char *buffer, size_t len);
  bool doHTTPGETmiddle(const char *url, char *buffer, size_t len);

  bool doHTTPREAD(char *buffer, size_t len);
  bool doHTTPACTION(char num);

  bool doHTTPprolog(const char *apn);
  bool doHTTPprolog(const char *apn, const char *apnuser, const char *apnpwd);
  void doHTTPepilog();

private:
  void initProlog(Stream &stream, size_t bufferSize);

  bool isAlive();

  void switchEchoOff();
  void flushInput();
  int readLine(uint32_t ts_max);
  int readBytes(size_t len, uint8_t *buffer, size_t buflen, uint32_t ts_max);
  bool waitForOK(uint16_t timeout=4000);
  bool waitForMessage(const char *msg, uint32_t ts_max);
  bool waitForMessage_P(const char *msg, uint32_t ts_max);
  int waitForMessages(const char *msgs[], size_t nrMsgs, uint32_t ts_max);
  bool waitForPrompt(const char *prompt, uint32_t ts_max);

  void sendCommandProlog();
  void sendCommandAdd(char c);
  void sendCommandAdd(int i);
  void sendCommandAdd(const char *cmd);
  void sendCommandAdd(const String & cmd);
  void sendCommandAdd_P(const char *cmd);
  void sendCommandEpilog();

  void sendCommand(const char *cmd);
  void sendCommand_P(const char *cmd);

  bool getIntValue(const char *cmd, const char *reply, int * value, uint32_t ts_max);
  bool getIntValue_P(const char *cmd, const char *reply, int * value, uint32_t ts_max);
  bool getStrValue(const char *cmd, const char *reply, char * str, size_t size, uint32_t ts_max);
  bool getStrValue_P(const char *cmd, const char *reply, char * str, size_t size, uint32_t ts_max);
  bool getStrValue(const char *cmd, char * str, size_t size, uint32_t ts_max);

  bool connectProlog();
  bool waitForSignalQuality();
  bool waitForCREG();
  bool setBearerParms(const char *apn, const char *user, const char *pwd);

  bool getPII(char *buffer, size_t buflen);
  void setProductId();

  // Small utility to see if we timed out
  bool isTimedOut(uint32_t ts) { return (long)(millis() - ts) >= 0; }

  const char * skipWhiteSpace(const char * txt);

  bool sendFTPdata_low(uint8_t *buffer, size_t size);
  bool sendFTPdata_low(uint8_t (*read)(), size_t size);

  ResponseTypes readResponse(char* buffer, size_t size, size_t* outSize,
          uint32_t timeout = DEFAULT_READ_MS) { return ResponseNotFound; }


  int _onoff_DTR_pin;
  int _status_CTS_pin;
  size_t _ftpMaxLength;
  bool _transMode;
  bool _skipCGATT;
  bool _changedSkipCGATT;		// This is set when the user has changed it.
  enum productIdKind {
    prodid_unknown,
    prodid_SIM900,
    prodid_SIM800,
  };
  enum productIdKind _productId;

  uint32_t _timeToOpenTCP;
  uint32_t _timeToCloseTCP;

  String _HTTPHeaders;
  String _contentType;
};

extern GPRSbeeClass gprsbee;

#endif /* GPRSBEE_H_ */
