#include <iostream>
#include <string.h>
#include "TransportTCP.h"
#include <pthread.h>

#include <iterator>
using namespace std;


TCP_TRANSPORT::TCP_TRANSPORT()
{
}

TCP_TRANSPORT::TCP_TRANSPORT(const char *mitsubishi_plc_ip, uint16_t mc_port)
{
    _ip = mitsubishi_plc_ip;
    _port = mc_port;
}

String TCP_TRANSPORT::SendAndRecieve(String command)
{
    if (!conectado)
        Connect();
    if (!conectado)
        return "";
    String ans = "";
    unsigned long tiempo = millis();

    if (cliente.connect(_ip, _port))
    {
        int cmd_len = command.length();
        char cmd[cmd_len + 1];

        for (size_t i = 0; i < cmd_len; i++)
            cmd[i] = char(command[i]);

        int writt = cliente.write(cmd, cmd_len);
        if (!writt)
        {
            Serial.println("Hubo un problema al escribir =" + String(writt));
            if (writt != cmd_len)
                Serial.println("No se escribio todo el mensaje =" + String(writt));
            return "";
        }
        unsigned long timeout_ = _timeout + millis();

        while (timeout_ > millis() || !cliente.available())
            delay(1);

        uint8_t response[2048];
        int av = cliente.available();
        int lei = cliente.read(response, av);

        if (lei)
            for (int i = 0; i < lei; i++)
                ans += (char)response[i];
    }
    else
        Serial.println("no se encontro el plc");

    cliente.stop();
    return ans;
}

void TCP_TRANSPORT::setIP(const char *ip)
{
    _ip = ip;
}
void TCP_TRANSPORT::setPort(uint16_t port)
{
    _port = port;
}

void TCP_TRANSPORT::setTimeOut(unsigned long timeout)
{
    _timeout = timeout;
}

unsigned long TCP_TRANSPORT::getTimeOut(){
    return _timeout;
}

bool TCP_TRANSPORT::testConexion()
{
   if (conectado)
        return true;
    if (cliente.connect(_ip, _port))
        conectado = true;
    else
    {
        conectado = false;
        Serial.println("Algo salio mal con la conexion");
    }
    cliente.stop();
    return conectado;
}


bool TCP_TRANSPORT::Connect()
{
   if (conectado)
        return true;
    if (cliente.connect(_ip, _port))
    {
        conectado = true;
    }
    else
    {
        conectado = false;
        Serial.println("Algo salio mal con la conexion");
    }
    return conectado;
}

void TCP_TRANSPORT::Close()
{
    conectado = false;
    cliente.stop();
}

int TCP_TRANSPORT::Send(const uint8_t command[], int cmdLen)
{
    int writt = cliente.write(command, cmdLen);
    if (writt != cmdLen)
    {
        Serial.println("Hubo un problema al escribir =" + String(writt));
        return 0;
    }
    //  Serial.println("Escribi " + String(writt) + " de " + String(cmdLen) + " bytes 0x");
    return writt;
}

int TCP_TRANSPORT::Receive(uint8_t response[], int respLen)
{
    int maxloops = 0;

    // wait for the server's reply to become available
    // unsigned long timm = millis();
    if (!cliente.connected())
    {
        Serial.println("No hay un cliente conectado");
        Close();
        return 0;
    }

    while (!cliente.available() && maxloops < 1000)
    {
        maxloops++;
        delay(1); // delay 1 msec
    }

    int lei = cliente.read(response, respLen);
    if (!lei)
    {
        Serial.println("No habia datos en el buffer de entrada");
        return 0;
    }
    if (lei != respLen)
    {
        Serial.println("Error en la lectura >> Lei solamente " + String(lei) + " bytes de " + String(respLen));
    }
    // String fins_;
    // for (int i = 0; i < 4; i++)
    // for (int i = 0; i < 4; i++)
    //     fins_ += char(response[i]);

    // if (lei && (fins_ == "FINS"))
    // {
    //     timm = millis() - timm;
    //     int i = 0;
    //     Serial.print(" >>Lei=" + String(lei) + " de " + String(disp) + " tim= " + String(timm) + "ms=");
    //     // Serial.print(" >>Lei=" + String(lei) + " en tim= " + String(timm) + "ms=");
    //     for (; i < 4; i++)
    //         Serial.print(char(response[i]));
    //     Serial.print("<");
    //     Serial.print("  Length: 0x");
    //     for (; i < 8; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.print("<");
    //     Serial.print("  Reseved (0): 0x");
    //     for (; i < 11; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.print("<");
    //     Serial.print("  Data Type(): 0x");
    //     for (; i < 12; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.print("<");
    //     Serial.print("  Err_Code: 0x");
    //     for (; i < 16; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.print("<");
    //     Serial.print("  Data: 0x");
    //     for (; i < lei; i++)
    //         Serial.print(response[i], HEX);
    //     Serial.println("<");
    //     Serial.print("  Raw=0x");
    //     for (int u = 0; u < lei; u++)
    //     {
    //         Serial.print(response[u], HEX);
    //     }
    //     Serial.println("<");
    // }
    // else
    // {
    //     Serial.print(">>Raw Response [" + String(lei) + "] of [" + String(disp) + "] bytes=");
    //     for (int u = 0; u < lei; u++)
    //     {
    //         Serial.print(response[u], HEX);
    //     }
    //     Serial.print("< = >");
    //     for (int u = 0; u < lei; u++)
    //     {
    //         Serial.print(response[u], HEX);
    //         Serial.print("|");
    //     }
    //     Serial.println("<");
    // }
    return lei;
}
