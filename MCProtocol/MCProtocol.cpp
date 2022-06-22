#include <iostream>
#include <string.h>
#include "MCProtocol.h"
#include <pthread.h>

#include <iterator>
using namespace std;

const String request = "500000FF03FF00";

MC_PROTOCOL::MC_PROTOCOL()
{
}

MC_PROTOCOL::MC_PROTOCOL(const char *mitsubishi_plc_ip, uint16_t mc_port, MitsuPLCSeries plc_serie)
{
    _ip = mitsubishi_plc_ip;
    _port = mc_port;
    tcp.setIP(mitsubishi_plc_ip);
    tcp.setPort(mc_port);
    PLCSeries = plc_serie;
}

bool MC_PROTOCOL::init()
{
    tcp.setIP(_ip);
    tcp.setPort(_port);
    return tcp.testConexion();
}

bool MC_PROTOCOL::init(const char *mitsubishi_plc_ip, uint16_t mc_port, MitsuPLCSeries plc_serie)
{
    _ip = mitsubishi_plc_ip;
    _port = mc_port;
    PLCSeries = plc_serie;
    return init();
}

String MC_PROTOCOL::SendAndRecieve(String command)
{
    String ans = tcp.SendAndRecieve(command);
    if (ans.length() < 12)
    {
        Serial.println("ilegal or bad response to the comand 0x" + command);
        return String("ERROR");
    }
    String qhead = "";
    String responseLenght = "";
    String completeCode = "";
    String responseData = "";

    int i = 0;

    for (; i < 14; i++)
        qhead += (char)ans[i];

    if (qhead != "D00000FF03FF00")
    {
        Serial.println("No es un mensaje MC o no es para mi"); // ilegall response
        return String("ERROR");
    }

    for (; i < 18; i++)
        responseLenght += (char)ans[i];
    for (; i < 22; i++)
        completeCode += (char)ans[i];
    if (completeCode != "0000")
    {
        Serial.println("An error >" + completeCode + "< was returned for the command >" + command + "<");

        Serial.println("Error Description = " + getErrorDescription(completeCode));

        return String("ERROR");
    }
    for (; i < int(ans.length()); i++)
        responseData += (char)ans[i];

    // Serial.println("Ans= 0x" + ans + "  response lenght =" + responseLenght + " completeCode =" + completeCode);

    if (!responseData.length())
    {
        //   Serial.println("Te comand: 0x" + command + "< doesnt returns data");
        return "";
    }

    //   Serial.println("Ans With Response Data = 0x" + responseData + " len=" + String(responseData.length()));

    return responseData;
}

int MC_PROTOCOL::SendAndRecieve(String command, uint16_t dataResponse[])
{
    String ans = SendAndRecieve(command);
    if (ans == "ERROR")
        return 0;
    int words = StringToInt16(ans, dataResponse);
    return words;
}

int MC_PROTOCOL::StringToInt16(String data, uint16_t dataResponse[])
{
    if (!data.length())
        return 0;
    int words = data.length() / 4;
    if (data.length() % 4)
        words++;

    String data_ = fitStringToWords(data, words); // quitar si da problemas

    uint16_t wordsarray[words] = {0};
    int word = 0;

    for (unsigned int i = 0; i < data_.length(); i += 4)
    {
        wordsarray[word] = 0;
        for (int j = 0; j < 4; j++)
        {
            dataResponse[word] <<= 4;
            dataResponse[word] |= char2int(data_[i + j]);
        }
        word++;
    }
    return words;
}

uint16_t MC_PROTOCOL::char2int(char input)
{
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    return 0;
}

int MC_PROTOCOL::int16ToString(uint16_t data[], uint16 wordsNumber, String &ascii_data)
{
    char hex_info[5];
    ascii_data = "";
    int i = 0;
    for (; i < wordsNumber; i++)
    {
        memset(hex_info, 0, 5);
        sprintf(hex_info, "%.4X", data[i]);
        ascii_data += String(hex_info);
    }
    return i;
}

int MC_PROTOCOL::int32ToString(uint32_t data[], uint16 wordsNumber, String &ascii_data)
{
    char hex_info[5];
    ascii_data = "";
    int i = 0;

    for (; i < wordsNumber; i++)
    {
        memset(hex_info, 0, 5);
        sprintf(hex_info, "%.4X", data[i]);
        ascii_data += String(hex_info);
    }
    return i;
}

int MC_PROTOCOL::read(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_to_read, uint16_t readed_data[])
{
    String ascii_data = BatchReadAndWrite(memoryType, memory_start_address, no_of_words_to_read, true);
    String ans = SendAndRecieve(ascii_data);
    int words = StringToInt16(ans, readed_data);
    return words;
}

int MC_PROTOCOL::read(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_to_read, String &readed_data)
{
    String ascii_data = BatchReadAndWrite(memoryType, memory_start_address, no_of_words_to_read, true);
    readed_data = SendAndRecieve(ascii_data);
    int words = readed_data.length() / 4;

    return words;
}

bool MC_PROTOCOL::write(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_to_write, uint16_t data_to_write[])
{
    String hex_data;
    int16ToString(data_to_write, no_of_words_to_write, hex_data);
    String ascii_data = BatchReadAndWrite(memoryType, memory_start_address, no_of_words_to_write, false, false, hex_data);
    String ans = SendAndRecieve(ascii_data);
    return (ans != "ERROR");
}

bool MC_PROTOCOL::write(MitsuDeviceType memoryType, uint32_t memory_address, String string_text_to_write)
{
    int no_of_words_to_write = (string_text_to_write.length() / 2);
    if (string_text_to_write.length() % 2)
        no_of_words_to_write++;
    String string_text = StringToHexASCII(string_text_to_write);
    String ascii_data = BatchReadAndWrite(memoryType, memory_address, no_of_words_to_write, false, false, string_text);
    String ans = SendAndRecieve(ascii_data);
    return (ans != "ERROR");
}

bool MC_PROTOCOL::write(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_to_write, String data_to_write)
{
    String hex_data = fitStringToWords(data_to_write, no_of_words_to_write);
    hex_data.toUpperCase();

    String ascii_data = BatchReadAndWrite(memoryType, memory_start_address, no_of_words_to_write, false, false, hex_data);
    String ans = SendAndRecieve(ascii_data);
    return (ans != "ERROR");
}

bool MC_PROTOCOL::write(MitsuDeviceType memoryType, uint32_t memory_address, uint16_t word_data_to_write)
{
    uint16_t word_data_to_write_[1] = {word_data_to_write};
    return write(memoryType, memory_address, 1, word_data_to_write_);
}

bool MC_PROTOCOL::writeBit(MitsuDeviceType memoryType, uint32_t memory_hex_bit_address, uint8_t new_bit_value)
{
    char value = new_bit_value ? '1' : '0';
    String ans = SendAndRecieve(BatchReadAndWrite(memoryType, memory_hex_bit_address, 1, false, true, String(value)));
    return (ans != "ERROR");
}

bool MC_PROTOCOL::write(MitsuDeviceType memoryType, uint32_t memory_word_address, float float_value)
{
    uint32_t en_hex;
    memcpy(&en_hex, &float_value, 4);
    uint16_t en_hex_[2] = {uint16_t(0xffff & (en_hex)), uint16_t(0xffff & (en_hex >> 16))};
    return write(memoryType, memory_word_address, 2, en_hex_);
}

bool MC_PROTOCOL::write(MitsuDeviceType memoryType, uint32_t memory_word_address, double double_value)
{
    uint64_t en_hex;
    memcpy(&en_hex, &double_value, 8);
    //  Serial.println("double " + String(double_value) + " in hex = 0x" + String(en_hex, HEX));
    uint16_t en_hex_[4] = {uint16_t((0xffff) & (en_hex)), uint16_t(0xffff & (en_hex >> 16)), uint16_t(0xffff & (en_hex >> 32)), uint16_t(0xffff & (en_hex >> 48))};

    return write(memoryType, memory_word_address, 4, en_hex_);
}

bool MC_PROTOCOL::writeInt16(MitsuDeviceType memoryType, uint32_t memory_word_address, int integer_value)
{
    if (integer_value >= -32768 && integer_value <= 0xFFFF) // 32768 16 bits integer
    {
        uint16_t en_hex;
        memcpy(&en_hex, &integer_value, 2);
        return write(memoryType, memory_word_address, en_hex);
    }

    Serial.println("Integer Number out of Range (data >= -32768 || data <= 0xFFFF)");
    return false;
}

bool MC_PROTOCOL::writeInt32(MitsuDeviceType memoryType, uint32_t memory_word_address, int integer_value)
{
    uint32_t en_hex; // 32 bits integer
    memcpy(&en_hex, &integer_value, 4);
    // Serial.println("int32 <" + String(integer_value) + "< in hex = 0x" + String(en_hex, HEX));
    uint16_t en_hex_[2] = {uint16_t(0xffff & (en_hex)), uint16_t(0xffff & (en_hex >> 16))};
    return write(memoryType, memory_word_address, 2, en_hex_);
}

bool MC_PROTOCOL::readBit(MitsuDeviceType memoryType, uint32_t memory_hex_bit_address_to_read)
{

    String ans = SendAndRecieve(BatchReadAndWrite(memoryType, memory_hex_bit_address_to_read, 1, true, true));
    return (ans == "1");
}

float MC_PROTOCOL::readFloat(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read)
{
    uint16_t float_hex[2] = {0, 0};
    int two_words = read(memoryType, memory_hex_word_address_to_read, 2, float_hex);
    if (two_words != 2)
        return 0;

    uint32_t float_hex_32 = uint32_t(float_hex[1]);
    float_hex_32 <<= 16;
    float_hex_32 |= uint32_t(float_hex[0]);

    float float_value;
    memcpy(&float_value, &float_hex_32, 4);
    return float_value;
}

double MC_PROTOCOL::readDouble(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read)
{
    uint16_t double_hex[4] = {0, 0, 0, 0};
    int two_words = read(memoryType, memory_hex_word_address_to_read, 4, double_hex);
    if (two_words != 4)
        return 0;

    uint64_t double_hex_64 = uint32_t(double_hex[3]);
    double_hex_64 <<= 16;
    double_hex_64 |= uint32_t(double_hex[2]);
    double_hex_64 <<= 16;
    double_hex_64 |= uint32_t(double_hex[1]);
    double_hex_64 <<= 16;
    double_hex_64 |= uint32_t(double_hex[0]);

    double double_value;
    memcpy(&double_value, &double_hex_64, 8);
    return double_value;
}

int MC_PROTOCOL::readInt16(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read)
{
    uint16_t int_hex[1] = {0};
    int two_words = read(memoryType, memory_hex_word_address_to_read, 1, int_hex);
    if (two_words != 1)
        return 0;
    int int_value = 0;
    memcpy(&int_value, &int_hex[0], 2);

    return int_value;
}

int MC_PROTOCOL::readInt32(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read)
{
    uint16_t int_hex[2] = {0, 0};
    int two_words = read(memoryType, memory_hex_word_address_to_read, 2, int_hex);
    if (two_words != 2)
        return 0;

    uint32_t int_hex_32 = uint32_t(int_hex[1]);
    int_hex_32 <<= 16;
    int_hex_32 |= uint32_t(int_hex[0]);

    int int_value;
    memcpy(&int_value, &int_hex_32, 4);
    return int_value;
}

String MC_PROTOCOL::readString(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read, int no_of_words_to_read)
{
    uint16_t ans[no_of_words_to_read] = {0};
    int words = read(memoryType, memory_hex_word_address_to_read, no_of_words_to_read, ans);
    if (words != no_of_words_to_read)
        return "";

    uint8_t ansss[words * 2];
    int index = int16arrayToint8array(ans, ansss, words);

    String ans_ = "";
    for (int i = 0; i < index; i += 2)
    {
        ans_ += char(ansss[i + 1]);
        ans_ += char(ansss[i]);
    }

    return ans_;
}

String MC_PROTOCOL::fitStringToWords(String input, int no_of_words)
{
    String hex_data;
    int letras = no_of_words * 4;
    int inputLen = input.length();
    if (inputLen % 4 || letras != inputLen)
    {
        if (letras < inputLen)
        {
            hex_data = input;
            //   Serial.println("Sobran " + String(inputLen - letras) + " letras del string >" + input + "<");
            hex_data.remove(inputLen - 1, (inputLen - letras));
        }
        else
        {
            // Serial.println("Faltan " + String(letras - inputLen) + " letras del string >" + input + "<");
            for (int i = 0; i < (letras - inputLen); i++)
                hex_data += "0";
            hex_data += input;
        }
    }
    return hex_data;
}

int MC_PROTOCOL::int16arrayToint8array(uint16_t int16_input_data[], uint8_t output_int8_data[], int number_of_16int_words)
{
    uint8_t output[number_of_16int_words * 2];
    int index = 0;
    int i = 0;
    for (; i < number_of_16int_words; i++)
    {
        output[index++] = uint8_t(int16_input_data[i] >> 8);
        output[index++] = uint8_t(int16_input_data[i] & 0xff);
    }

    memcpy(output_int8_data, output, number_of_16int_words * 2);
    return index;
}

String MC_PROTOCOL::StringToHexASCII(String input)
{
    int len = input.length();
    String result = "";
    for (int i = 0; i < len; i++)
    {
        char hex_info[3];
        memset(hex_info, 0, 2);
        sprintf(hex_info, "%2X", int(input[i]));
        result += String(hex_info);
    }
    while (result.length() % 4)
        result += "0";

    /* invert the letters */
    len = result.length();
    String result_ = "";
    for (int i = 0; i < len; i += 4)
    {
        result_ += result[i + 2];
        result_ += result[i + 3];
        result_ += result[i];
        result_ += result[i + 1];
    }

    return result_;
}

uint8_t MC_PROTOCOL::getMonitoringTimer()
{
    return monitoringTimer;
}

void MC_PROTOCOL::setMonitoringTimer(uint8_t newTime)
{
    monitoringTimer = newTime;
}

bool MC_PROTOCOL::run(bool forced_executed, MitsuRemoteControlClearMode controlClearMode)
{
    // MitsuRemoteControlMode forced = forced_executed ? EXECUTE_FORCIBLY : DONT_EXECUTE_FORCIBLY;
    String ans = ModuleControl(RUN, forced_executed ? EXECUTE_FORCIBLY : DONT_EXECUTE_FORCIBLY, controlClearMode);
    return (ans != "ERROR");
}

bool MC_PROTOCOL::pause(bool forced_executed)
{
    String ans = ModuleControl(PAUSE, forced_executed ? EXECUTE_FORCIBLY : DONT_EXECUTE_FORCIBLY);
    return (ans != "ERROR");
}

bool MC_PROTOCOL::stop()
{
    String ans = ModuleControl(STOP);
    return (ans != "ERROR");
}

bool MC_PROTOCOL::clear()
{
    String ans = ModuleControl(CLEAR);
    return (ans != "ERROR");
}
bool MC_PROTOCOL::reset()
{
    String ans = ModuleControl(RESET);
    return (ans != "ERROR");
}

String MC_PROTOCOL::getCPUModel()
{
    return ModuleControl(CPU_MODEL);
}

String MC_PROTOCOL::getErrorDescription(String error)
{
    uint16_t xx[1];
    StringToInt16(error, xx);
    uint16 error_code = xx[0];

    if (error_code >= 0x4000 && error_code <= 0x4FFF)
        return "Errors detected by the CPU module (Errors occurred in other than MC protocol communication)";
    else if (error_code == 0x0050)
        return "A code other than specified ones is set to command/response typeof subheader";
    else if (error_code == 0x0055)
        return "Although online change is disabled, the connected device requested the RUN-state CPU module for data writing.";
    else if (error_code == 0xC050)
        return "When ´Communication Data Code´ is set to ASCII Code, ASCII code data that cannot be converted to binary were received.";
    if (error_code >= 0xC051 && error_code <= 0xC054)
        return "The number of read or write points is outside the allowable range.";
    else if (error_code == 0xC056)
        return "The read or write request exceeds the maximum address.";
    else if (error_code == 0xC058)
        return "The request data length after ASCII-to-binary conversion does not match the data size of the character area (a part of text data).";
    else if (error_code == 0xC059)
        return "• The command and/or subcommand are specified incorrectly. || • The CPU module does not support the command and/or subcommand.";
    else if (error_code == 0xC05B)
        return "The CPU module cannot read data from or write data to the specified device.";
    else if (error_code == 0xC05C)
        return "The request data is incorrect. (e.g. reading or writing data in units of bits from or to a word device)";
    else if (error_code == 0xC05D)
        return "No monitor registration";
    else if (error_code == 0xC05F)
        return "The request cannot be executed to the CPU module.";
    else if (error_code == 0xC05C)
        return "The request data is incorrect. (ex. incorrect specification of data for bit devices)";
    else if (error_code == 0xC060)
        return "The request data is incorrect. (ex. incorrect specification of data for bit devices)";
    else if (error_code == 0xC061)
        return "The request data length does not match the number of data in the character area (a part of text data).";
    else if (error_code == 0xC06F)
        return "The CPU module received a request message in ASCII format when ´Communication Data Code´ is set to Binary Code, or Received it in binary format when the setting is set to ASCII Code. (This error code is only registered to the error history, and no abnormal response is returned.)";
    else if (error_code == 0xC070)
        return "The device memory extension cannot be specified for the target Station.";
    else if (error_code == 0xC0B5)
        return "The CPU module cannot handle the data specified.";
    else if (error_code == 0xC200)
        return "The remote password is incorrect.";
    else if (error_code == 0xC201)
        return "The port used for communication is locked with the remote password. Or, because of the remote password lock status with ´Communication Data Code´ set to ASCII Code, the subcommand and later part cannot be converted to a binary code.";
    else if (error_code == 0xC240)
        return "The connected device is different from the one that requested for unlock processing of the remote password.";

    return "UNKNOWN ERROR";
}

String MC_PROTOCOL::BatchReadAndWrite(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_or_or_points_or_bits_to_write_or_read, bool read, bool bit, String data_to_write)
{
    String command = "";
    char hex_info[7];
    memset(hex_info, 0, 6);
    // add monitoring timer
    sprintf(hex_info, "%.4X", monitoringTimer);
    command += String(hex_info);
    // add comand 1401 or 0401(read or write?)
    if (read)
        command += "0401";
    else
        command += "1401";

    // add subcomand depends of the plc series (iQR or QL)
    if (PLCSeries)
    {
        if (bit) // read/write bit
            command += "0003";
        else // read/write word
            command += "0002";

        command += String(DeviceTypeiQR[memoryType]); // add deviceType
    }
    else
    {
        if (bit) // read/write bit
            command += "0001";
        else // read/write word
            command += "0000";

        command += String(DeviceTypeQL[memoryType]); // add deviceType
    }

    // adding Head device number(address)

    memset(hex_info, 0, 6);
    sprintf(hex_info, "%.6X", memory_start_address);
    command += String(hex_info);

    // adding Number of device points
    memset(hex_info, 0, 6);
    sprintf(hex_info, "%.4X", no_of_words_or_or_points_or_bits_to_write_or_read);
    command += String(hex_info);

    // adding request data
    // String ascii_data = "";
    // int palabras = int16ToString(data_to_write, no_of_words_to_write, ascii_data);
    // if (palabras == no_of_words_to_write)
    command += data_to_write;

    // generating Request data length
    int data_len = command.length();
    memset(hex_info, 0, 6);
    sprintf(hex_info, "%.4X", data_len);

    // joint all the components
    // adding the basic Subheader + Access route
    String ascii_data = request;

    // add Request data length
    ascii_data += String(hex_info);

    // add Request data
    ascii_data += command;
    // Serial.println("resultado = 0x" + ascii_data);

    // String ans = SendAndRecieve(ascii_data);
    return ascii_data;
}

String MC_PROTOCOL::ModuleControl(MitsuPLCRemoteControl kind_of_control, MitsuRemoteControlMode controlMode, MitsuRemoteControlClearMode controlClearMode)
{
    String command = "";
    char hex_info[7];
    memset(hex_info, 0, 6);
    // add monitoring timer
    sprintf(hex_info, "%.4X", monitoringTimer);
    command += String(hex_info);
    // add comand
    switch (kind_of_control)
    {
    case RUN:
        command += "1001"; // comand
        command += "0000"; // subcomand
        // add control mode
        memset(hex_info, 0, 6);
        sprintf(hex_info, "%.4X", uint8_t(controlMode));
        command += String(hex_info);
        // add clear mode
        memset(hex_info, 0, 6);
        sprintf(hex_info, "%.2X", uint8_t(controlClearMode));
        command += String(hex_info);

        command += "00"; // ad fix value 00

        break;

    case STOP:
        command += "1002"; // comand
        command += "0000"; // subcomand
        command += "0001"; // add fixed value 0001

        break;

    case PAUSE:
        command += "1003"; // comand
        command += "0000"; // subcomand
        // add control mode
        memset(hex_info, 0, 6);
        sprintf(hex_info, "%.4X", uint8_t(controlMode));
        command += String(hex_info);

        break;

    case CLEAR:
        command += "1005"; // comand
        command += "0000"; // subcomand
        command += "0001"; // add fixed value 0001

        break;

    case RESET:
        command += "1006"; // comand
        command += "0000"; // subcomand
        command += "0001"; // add fixed value 0001

        break;
    case CPU_MODEL:
        command += "1001"; // comand
        command += "0000"; // subcomand
        break;
    }

    // generating Request data length
    int data_len = command.length();
    memset(hex_info, 0, 6);
    sprintf(hex_info, "%.4X", data_len);

    // joint all the components
    // adding the basic Subheader + Access route
    String ascii_data = request;

    // add Request data length
    ascii_data += String(hex_info);

    // add Request data
    ascii_data += command;

    // Serial.println("Command result = 0x" + ascii_data);

    return SendAndRecieve(ascii_data);
}