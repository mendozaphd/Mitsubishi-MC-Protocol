
#ifndef _TCP_TRANSPORT_H_
#define _TCP_TRANSPORT_H_

#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

using namespace std;

class TCP_TRANSPORT
{
private:
    // struct sockaddr_in _serveraddr;
    uint16_t _port = 1000;
    const char *_ip;
    WiFiClient cliente;

    bool conectado = false;

public:
    TCP_TRANSPORT();
    TCP_TRANSPORT(const char *mitsubishi_plc_ip, uint16_t mc_port);

    String SendAndRecieve(String command);
    unsigned long _timeout = 100; //max time to wait answer from client
    void setTimeOut(unsigned long timeout);
    void setIP(const char *ip);
    void setPort(uint16_t port);
bool testConexion();
    unsigned long getTimeOut();
    bool Connect();
    void Close();
    int Send(const uint8_t command[], int cmdLen);
    int Receive(uint8_t response[], int respLen);
};

#endif
