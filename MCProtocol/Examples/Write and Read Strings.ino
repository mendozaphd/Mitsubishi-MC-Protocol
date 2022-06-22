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

    String saludos = "ESTE ES UN MENSAJE ENVIADO POR EL MICROCONTROLADOR ESP32";

    if (plc1.write(D, 0x300, saludos)) // write the String into the D300 register
        Serial.println("Se saludo correctamente con un >" + saludos);

    String saludo = plc1.readString(D, 0x300, 28); // read the same string in the D300 register with the half of length of the sended message
    if (saludo.length())
        Serial.println("Se leyo el saludo  " + saludo);  //it should be the same text written
}