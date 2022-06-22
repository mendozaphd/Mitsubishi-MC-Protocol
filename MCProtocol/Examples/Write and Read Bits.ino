#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
//#include <SPI.h>

#include <MCProtocol.h>

// tested on Q series

WiFiClient mitsu;

// WiFi network name and password
const char *networkName = "TP-Link_3500";
const char *networkPswd = "32289459";

uint16_t mc_port = 3001;
const char *mitsu_ip = "192.168.0.40";

// declare a FINS_TCP object to control the Omron PLC
MC_PROTOCOL plc1(mitsu_ip, mc_port);

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.print("Waiting for WiFi..");
    WiFi.disconnect();
    WiFi.begin(networkName, networkPswd);
    Serial.print("..");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    if (plc1.init())
        Serial.println("Se encontro el PLC Mitsubichi");
}

void loop()
{
    /*  <<<<<<<<<<<<<<< TEST WRITE AND READ STRING TEXT VALUES >>>>>>>>>>>>>>>> */

    for (uint32_t bit_address = 0x20; bit_address < 0x3F; bit_address++)
        plc1.writeBit(Y, bit_address, 1); // set to 1(true) the bits into the outputs between the Y20 and Y3F

    bool bit_status = plc1.readBit(Y, 0x2A); // read the value of the Y2A bit

    Serial.println("The bit Y20 is " + String(bit_status ? "ON" : "OFF")); // prints the bit status

    delay(1000);

    for (uint32_t bit_address = 0x20; bit_address < 0x3F; bit_address++)
        plc1.writeBit(Y, bit_address, 0); // reset to 0(true) the bits into the outputs between the Y20 and Y3F

    bit_status = plc1.readBit(Y, 0x2A); // read the value of the Y2A bit

    Serial.println("The bit Y20 is " + String(bit_status ? "ON" : "OFF")); // prints the bit status
    
    delay(1000);
}