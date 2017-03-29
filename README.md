# GPRSbee

This is the Arduino library for the [GPRSbee](http://gprsbee.com/).  The GPRSbee is made by [Sodaq](http://sodaq.com/) and can be purchased either directly through the [Sodaq store](https://shop.sodaq.com/en/gprsbee.html) or through [seeed studio](https://www.seeedstudio.com/GPRSbee-rev.-6-p-2445.html).

This library implements most of the functions of the Sodaq_GSM_Modem class, and a number of others besides.  It is slightly clunkier than strictly necessary to maintain both backward and forward compatibility.


## Initializing the GPRSbee
To use the GPRSbee, you must first begin the stream instance that will communicate with the bee and then call the initialization function.  Generally both of these things should be called from the setup() of an arduino program.  For backwards compatibility, there are several different init functions, which are based on whether the power can be toggled on or off to the GPRSBee and which version of the Bee is attached.  All older versions of the On-Off methods are supported!  (toggle, mbili_jp2, Ndogo, and Autonomo)

The general initialization method for any device or version of the bee is:
```cpp
initGPRS(Stream &stream, int vcc33Pin, int onoff_DTR_pin, int status_CTS_pin,
    GPRSVersion version, int bufferSize)
```
The GPRSVersion can be V04 (rev 4), V05, or V06.  The default buffer size is 64.  If the GPRSbee is continuously powered by your device, enter -1 for the vcc33Pin.  If the power pin and the DTR pin are the same pin, use that single pin number as the vcc33Pin and enter -1 for the onoff_DTR_pin.

## Using the GPRS modem
This library enables the GPRSbee to open TCP connections, open and read FTP files, attach HTTP headers, send HTTP GET and POST requests, and send SMS messages.  It can also access SIM card functions like checking the current network time. Examples are available in the examples folder and in the [wiki](https://github.com/SodaqMoja/GPRSbee/wiki).


## Lower level methods

There are a group of functions behind all of the http methods. These can be accessed when you want to do special things.  For example if you want to do multiple GETs in a row without turning the bee on and off and waiting for it to initialize, you can use the following sequence:
```cpp
  gprsbee.on();
  gprsbee.doHTTPprolog(const char *apn);
  gprsbee.doHTTPGETmiddle(const char *url1, char *buffer1, size_t len);
  gprsbee.doHTTPGETmiddle(const char *url2, char *buffer2, size_t len);
  gprsbee.doHTTPGETmiddle(const char *url3, char *buffer3, size_t len);
  gprsbee.doHTTPGETmiddle(const char *url4, char *buffer4, size_t len);
  gprsbee.doHTTPepilog();
  gprsbee.off();
```

These lower level GET functions can also be used to keep the GPRS connection up.


## Direct AT commands
Direct AT commands can also be sent to the SIM card by calling sendCommandWaitForOK(string or char).
