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
    /*  <<<<<<<<<<<<<<< Test module control functions >>>>>>>>>>>>>>>> */

    Serial.println("CPU_MODEL >>>"); // Test module control functions

    Serial.println("CPU MODEL =" + plc1.getCPUModel()); // get the cpu Model
    delay(1000);

    Serial.println("STOP >>>");  //Stops the CPU
    plc1.stop();

    delay(1000);

    Serial.println("RESET >>>"); //Resets the CPU
    plc1.reset();

    delay(1000);

    Serial.println("PAUSE >>>");
    plc1.pause(true);

    delay(1000);

    Serial.println("CLEAR >>>");
    plc1.clear();

    delay(1000);

    Serial.println("RUN >>>"); 
    plc1.run(true, ALL_CLEAR);
}