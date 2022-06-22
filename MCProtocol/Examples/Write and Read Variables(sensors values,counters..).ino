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
    /*  <<<<<<<<<<<<<<< TEST WRITE AND READ DOUBLE RANDOM VALUES >>>>>>>>>>>>>>>> */
    float entero = random(50, 1000);
    float decimal = random(0, 99);
    decimal /= 100;
    double sensorV = entero + decimal; // generates an random double number of 4 words

    if (plc1.write(D, 0x50, sensorV)) // write the double value into 4 words starting in the D50 register
        Serial.println("Se escribio el numero " + String(double(sensorV)));
    double sensorR = plc1.readDouble(D, 0x50); // read the double number

    if (sensorR == sensorV) // compare the generated number vs the readed
        Serial.println("Son iguales!!");

    delay(3000);

    /*  <<<<<<<<<<<<<<< TEST WRITE AND READ FLOAT RANDOM VALUES >>>>>>>>>>>>>>>> */

    entero = random(5, 1000);
    decimal = random(0, 99);
    decimal /= 100;
    float sensorF = entero + decimal; // generates an random double number of 4 words

    if (plc1.write(M, 0x200, sensorF)) // write the float value into 4 words starting in the M200 register
        Serial.println("Se escribio el numero " + String(double(sensorV)));
    float sensorFR = plc1.readDouble(M, 0x200); // read the float number into the M200 register

    if (sensorFR == sensorF) // compare the generated number vs the readed
        Serial.println("Son iguales!!");
    else
        Serial.println("El numero escrito es diferente del que lei!!"); // Something wrong happen

    delay(3000);

    /*  <<<<<<<<<<<<<<< TEST WRITE AND READ INTEGER RANDOM VALUES >>>>>>>>>>>>>>>> */

    int int16 = random(14, 3098);
    if (plc1.writeInt16(Y, 0x20, int16)) // write the number in the word Y20
        Serial.println("Se escribio el numero int16 " + String(int16));

    int inttt = plc1.readInt16(Y, 0x20); // read the int16 number in the word Y20

    if (int16 == inttt)
        Serial.println("Los enteros son iguales!!"); // compare the readed value vs the written value
    else
        Serial.println("El numero escrito es diferente del que lei!!"); // Something wrong happen

    delay(3000);

    int int32 = random(0xffff, 0xffffff); // generates a random number beetwen 0xffff and 0xffffff

    if (plc1.writeInt32(Y, 0x30, int32))
        Serial.println("Se escribio el numero int32 " + String(int32));

    int inttt32 = plc1.readInt32(Y, 0x30); // read the value in the same memory

    if (int32 == inttt32) // compare the readed value vs the written value
        Serial.println("Los entero 32s son iguales!!");
    else
        Serial.println("El numero escrito es diferente del que lei!!"); // Something wrong happen
}