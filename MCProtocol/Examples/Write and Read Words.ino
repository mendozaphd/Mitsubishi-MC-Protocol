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

    while (!Serial)
        ;

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
    /*  <<<<<<<<<<<<<<< TEST WRITE AND READ RANDOM VALUES >>>>>>>>>>>>>>>> */
    delay(2000);
    uint16_t array1[] = {0b1010101001010101, 0b1010101001010101}; // generate an array of two words
    plc1.write(Y, 0x20, 2, array1);                               // write the array of two words starting in the Output Y20
    delay(2000);

    uint16_t words_written[] = {0b0101010110101010, 0b0101010110101010}; // generate an array of two words 
    plc1.write(Y, 0x20, 2, words_written);                               // write the array of two words starting in the Output Y20

    uint16_t words_readed[2];
    plc1.read(Y, 0x20, 2, words_readed); // read the same 2 words

    bool are_the_same = true;

    for (size_t i = 0; i < 2; i++)
    {
        if (words_readed[i] != words_written[i])  //compare the readed words with the written words
            are_the_same = false;
    }

    if (are_the_same)
        Serial.println("The words readed are the same that the words written");

    



}