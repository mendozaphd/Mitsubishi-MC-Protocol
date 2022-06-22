
#ifndef _MC_PROTOCOL_H_
#define _MC_PROTOCOL_H_

#include <Arduino.h>
// #include <SPI.h>
// #include <EthernetENC.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "constants.h"
#include <TransportTCP.h>

using namespace std;

class MC_PROTOCOL
{
private:
    uint16_t _port;
    const char *_ip;

    TCP_TRANSPORT tcp;

    int PLCSeries = 0;

    uint16_t char2int(char input);

    uint8_t monitoringTimer = 0x2; // seconds

    /*!
     *    @brief  Sends an string command to the defined ip and port , and registers the hex answer into an string
     *    @param  command string command to be sended
     *    @return nothing ("") if everything is ok, the response hex data from the plc or
     *     "ERROR" if something is bad(no answer,bad answer, incorrect size...)
     */
    String SendAndRecieve(String command);

    /*!
     *    @brief  Sends an string command to the defined ip and port , and registers the answer into an uint16_t array
     *    @param  command string command to be sended
     *    @param  dataResponse uint16_t array where to store the aswer from the PLC
     *    @return the number of words recieved, 0 if error or not reponse data
     */
    int SendAndRecieve(String command, uint16_t dataResponse[]);
    
    /*!
     *    @brief  converts an hex string to an uint16_t array
     *    @param  data input string hex data
     *    @param  dataResponse uint16_t array where to store the input data
     *    @return the number of words converted into the uint16_t array
     */
    int StringToInt16(String data, uint16_t dataResponse[]);

        /*!
     *    @brief  converts an uint16_t array to an String hex format
     *    @param  data[] input data array of 16 bits
     *    @param  wordsNumber number of words to be converted
     *    @param  ascii_data string object where to write the data
     *    @return the number of uint16_t elements converted in the string array
     */
    int int16ToString(uint16_t data[], uint16 wordsNumber, String &ascii_data);

    /*!
     *    @brief  converts an uint32_t array to an String hex format
     *    @param  data[] input data array of 32 bits
     *    @param  wordsNumber number of words to be converted
     *    @param  ascii_data string object where to write the data
     *    @return the number of uint32_t elements converted in the string array
     */
    int int32ToString(uint32_t data[], uint16 wordsNumber, String &ascii_data);

    /*!
     *    @brief  converts an uint16_t array to an int8_t array
     *    @param  int16_input_data input data of 16 bits
     *    @param  output_int8_data output array to split and store the 16 bits input array
     *    @param  number_of_16int_words number of words of the input array to be converted
     *    @return the number of uint8_t elements converted in the uint8_t array
     */
    int int16arrayToint8array(uint16_t int16_input_data[], uint8_t output_int8_data[], int number_of_16int_words);

    /*!
     *    @brief  fill one string to and 4 letters word format
     *    Ex. input = "AB", 1 word > Output = "00AB"
     *    Ex. input = "ABCDE", 2 words > Output = "000ABCDE"
     *    Ex. input = "ABCDE", 1 word > Output = "ABCD"
     *    @param  input input string
     *    @param  no_of_words no of words to fitt
     *    @return the input string fitted to 4 letters(per each specified word) format
     */
    String fitStringToWords(String input, int no_of_words);

    /*!
     *    @brief  converts an text string(non hex) to hex format(2 inverted letters per word)
     *    Ex. input = "HELLO" > returns = "45484C4C4F" 48 = 'H', 45= 'E', 4C = 'L', 4C = 'L', 4F = 'O'
     *    @param  input input text ascii string(coul be any ascii letter)
     *    @return te converted text to hex format asked by the CPU
     */
    String StringToHexASCII(String input);

    String ModuleControl(MitsuPLCRemoteControl kind_of_control, MitsuRemoteControlMode controlMode = DONT_EXECUTE_FORCIBLY, MitsuRemoteControlClearMode controlClearMode = DO_NOT_CLEAR);

public:
    MC_PROTOCOL();

    /*!
     *    @brief Constructor sets the communications parameters to get communication with the CPU and makes an connection test
     *    @param  mitsubishi_plc_ip IP address from the Mitsubishi PLC
     *    @param  mc_port port specified in the cpu to get MC Protocol Calls
     *    @param  plc_serie   QL_SERIES or iQR_SERIES
     */
    MC_PROTOCOL(const char *mitsubishi_plc_ip, uint16_t mc_port, MitsuPLCSeries plc_serie = QL_SERIES);

    /*!
     *    @brief Sets the communications parameters to get communication with the CPU and makes an connection test
     *    @param  mitsubishi_plc_ip IP address from the Mitsubishi PLC
     *    @param  mc_port port specified in the cpu to get MC Protocol Calls
     *    @param  plc_serie   QL_SERIES or iQR_SERIES
     *    @return True if plc responds
     */
    bool init(const char *mitsubishi_plc_ip, uint16_t mc_port, MitsuPLCSeries plc_serie = QL_SERIES);

    /*!
     *    @brief Sets the communications parameters(previusly assigned in the constructor) to get communication with the CPU and makes an connection test
     *    @return True if plc responds
     */
    bool init();

    /*!
     *    @brief Write words to an specify deviceType(memory type) from a String
     *    the function aligns automatically the string size with the number of words specified
     *    CAUTION. if the number of words exceds the string lengh * 4, the algorithm will automatically add the data in zeros of the words that were not specified in the string
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write(inputs X, Ouyputs Y, D registers..)
     *    @param  memory_start_address Memory address in hex where to write the data
     *    @param  no_of_words_or_bits_to_write Number of 16 bits words to write in the specified memory and address
     *    @param data_to_write Words data hex values in string format
     *    @return True if write was successful, otherwise false.
     */
    bool write(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_or_bits_to_write, String data_to_write);

    /*!
     *    @brief Write words to an specify deviceType(memory type) from an unsigned 16 bits words array
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write(inputs X, Ouyputs Y, D registers..)
     *    @param  memory_start_address Memory address in hex where to write the data
     *    @param  no_of_words_or_bits_to_write Number of 16 bits words to write in the specified memory and address
     *    @param data_to_write Words data hex values in 16 bits unsigned array
     *    @return True if write was successful, otherwise false.
     */
    bool write(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_or_bits_to_write, uint16_t data_to_write[]);

    /*!
     *    @brief Write one word to an specify deviceType(memory type) from an unsigned 16 bits word
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write(inputs X, Ouyputs Y, D registers..)
     *    @param  memory_address Memory address in hex where to write the data
     *    @param data_to_write Word data hex value in 16 bits unsigned
     *    @return True if write was successful, otherwise false.
     */
    bool write(MitsuDeviceType memoryType, uint32_t memory_address, uint16_t word_data_to_write);

    /*!
     *    @brief Write one bit to an specify deviceType(memory type)
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write(inputs X, Ouyputs Y, D registers..)
     *    @param  memory_hex_bit_address Memory address in hex to write
     *    @param new_bit_value  1(true) to set the bit, 0(false) to reset the bit or coil
     *    @return True if write was successful, otherwise false.
     *
     */
    bool writeBit(MitsuDeviceType memoryType, uint32_t memory_hex_bit_address, uint8_t new_bit_value);

    /*!
     *    @brief Write a float value to an specified deviceType address
     *    Caution. float values takes 2 consecutive words to be stored in the plc
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write(inputs X, Ouyputs Y, D registers..)
     *    @param  mmemory_word_address Memory address in hex to start to write the float value
     *    @param float_value float value to write
     *    @return True if write was successful, otherwise false.
     *
     */
    bool write(MitsuDeviceType memoryType, uint32_t memory_word_address, float float_value);

    /*!
     *    @brief Write a double value to an specified deviceType address
     *    Caution. double values takes 4 consecutive words to be stored in the plc
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write(inputs X, Ouyputs Y, D registers..)
     *    @param  mmemory_word_address Memory address in hex to start to write the float value
     *    @param double_value duoble value to write
     *    @return True if write was successful, otherwise false.
     *
     */
    bool write(MitsuDeviceType memoryType, uint32_t memory_word_address, double double_value);

    /*!
     *    @brief Write an integer of 16 bits into 1 word address, the range of values is (data >= -32768 || data <= 0xFFFF)
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write(inputs X, Ouyputs Y, D registers..)
     *    @param  mmemory_word_address Memory address in hex to write the int value
     *    @param integer_value int value to write
     *    @return True if write was successful and if value is in the well range, otherwise false.
     *
     */
    bool writeInt16(MitsuDeviceType memoryType, uint32_t memory_word_address, int integer_value);

    /*!
     *    @brief Write a 32 bits integer in two consecutive words
     *    Caution. Integer of 32 bits values takes 2 consecutive words to be stored in the plc
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write(inputs X, Ouyputs Y, D registers..)
     *    @param  mmemory_word_address Memory address in hex to start to write the int32 value
     *    @param integer_value int32 value to write
     *    @return True if write was successful, otherwise false.
     *
     */
    bool writeInt32(MitsuDeviceType memoryType, uint32_t memory_word_address, int integer_value);

    /*!
     *    @brief Write an string message text into the plc and stores it to the specified word, the algoritm converts the string into hex data ascii chars
     *    Caution. each word just can store 2 ascii characteres
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write(inputs X, Ouyputs Y, D registers..)
     *    @param  mmemory_word_address Memory address in hex to start to write the string text
     *    @param string_text_to_write string message to write, acept all the ascii chars
     *    @return True if write was successful, otherwise false.
     *
     */
    bool write(MitsuDeviceType memoryType, uint32_t memory_address, String string_text_to_write);

    /*!
     *    @brief Generates the basics read and write frames with the BatchReadAndWrite commands
     *    @param  memoryType is an MitsuDeviceType enum wich correspond to an specified memory area to write or read(inputs X, Ouyputs Y, D registers..)
     *    @param  memory_start_address Memory address where start to read or write
     *    @param  no_of_words_or_or_points_or_bits_to_write_or_read numbers of words or bits to be readed or writted
     *    @param read if is true the comand is focus to get info from the plc, false to write
     *    @param bit if is true the write/read bits, if false the command will be to write/read complete words
     *    @param  data_to_write string data to be written
     *    @return Returns the complete 4E command frame ready to be sended.
     *
     */
    String BatchReadAndWrite(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_or_or_points_or_bits_to_write_or_read, bool read, bool bit = false, String data_to_write = "");

    /*!
     *    @brief change the status of PLC to RUN mode
     *    @param  forced_executed true to force the PLC to change
     *    @param controlClearMode use one of this ->>  DO_NOT_CLEAR, CLEAR_ONLY_OUTSIDE_LATCH_RANGE, ALL_CLEAR
     *    @return True if was successful and if the the CPU module does support the command and/or subcommand.
     */
    bool run(bool forced_executed, MitsuRemoteControlClearMode controlClearMode);

    /*!
     *    @brief change the status of PLC to PAUSE mode
     *    @param  forced_executed true to force the PLC to change to the PAUSE mode
     *    @return True if was successful and if the the CPU module does support the command and/or subcommand.
     */
    bool pause(bool forced_executed);

    /*!
     *    @brief change the status of PLC to STOP mode
     *    @return True if was successful and if the the CPU module does support the command and/or subcommand.
     */
    bool stop();

    /*!
     *    @brief change the status of PLC to Clear mode
     *    @return True if was successful and if the the CPU module does support the command and/or subcommand.
     */
    bool clear();

    /*!
     *    @brief RESETs the CPU
     *    @return True if was successful and if the the CPU module does support the command and/or subcommand.
     */
    bool reset();

    /*!
     *    @brief Get the model /name of the cpu
     *    @return The model/name of the cpu if the the CPU module does support the command and/or subcommand.
     */
    String getCPUModel();

    /*!
     *    @brief get one description of one hex error code
     *    @return the error description.
     *
     */
    String getErrorDescription(String error);

    /*!
     *    @brief Get the number of words specified into one String in hex format(4 chars for each word)
     *    @param  memoryType the device type where start to read words consecutibily(ex X,Y, D... watch constants.h enum MitsuDeviceType)
     *    @param  memory_start_address start address where the information starts
     *    @param no_of_words_to_read number of words to be readed
     *    @param  readed_data string var where to write the requiered data
     *    @return the numbers of words readed(it should be the same of o_of_words_to_read), zero if something is bad
     */
    int read(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_to_read, String &readed_data);

    /*!
     *    @brief Get the number of words specified into one uint16_t (unsigned 16 bits integer) array
     *    @param  memoryType the device type where start to read words consecutibily(ex X,Y, D... watch constants.h enum MitsuDeviceType)
     *    @param  memory_start_address start address where the information starts
     *    @param no_of_words_to_read number of words to be readed
     *    @param  readed_data uint16 array where to write the requiered data
     *    @return the numbers of words readed(it should be the same of o_of_words_to_read), zero if something is bad
     */
    int read(MitsuDeviceType memoryType, uint32_t memory_start_address, int no_of_words_to_read, uint16_t readed_data[]);

    /*!
     *    @brief Get the booleam value of one specified bit.
     *    Example readbit(X, 0x2A); returns the boolean value of the bit stored in the INPUT X2A || X2.10;
     *    @param  memoryType the device type where start to read words consecutibily(ex X,Y, D... watch constants.h enum MitsuDeviceType)
     *    @param  memory_hex_bit_address_to_read bit to be readed
     *    @return the numbers of words readed(it should be the same of o_of_words_to_read), zero if something is bad
     */
    bool readBit(MitsuDeviceType memoryType, uint32_t memory_hex_bit_address_to_read);

    /*!
     *    @brief Get the float value stored in an specified start word
     *    @param  memoryType the device type where start to read the float value(ex X,Y, D... watch constants.h enum MitsuDeviceType)
     *    @param  memory_hex_word_address_to_read address where is stored the float value
     *    @return the float value of the specified 32 bits word
     */
    float readFloat(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read);

    /*!
     *    @brief Get the double value stored in an specified start word
     *    @param  memoryType the device type where start to read the float value(ex X,Y, D... watch constants.h enum MitsuDeviceType)
     *    @param  memory_hex_word_address_to_read address where is stored the double value
     *    @return the double value of the specified 64 bits word
     */
    double readDouble(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read);

    /*!
     *    @brief Get the integer 16 bits value stored in an specified start word
     *    @param  memoryType the device type where start to read the float value(ex X,Y, D... watch constants.h enum MitsuDeviceType)
     *    @param  memory_hex_word_address_to_read address where is stored the int16 value
     *    @return the integer value of the specified 16 bits word
     */
    int readInt16(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read);

    /*!
     *    @brief Get the integer 32 bits value stored in an specified start word
     *    @param  memoryType the device type where start to read the float value(ex X,Y, D... watch constants.h enum MitsuDeviceType)
     *    @param  memory_hex_word_address_to_read address where is stored the int32 value
     *    @return the integer value of the specified 32 bits word
     */
    int readInt32(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read);

    /*!
     *    @brief Get the ascii string values starting for the specified register(converts the hex data into string ascii text message)
     *    @param  memoryType the device type where start to read the float value(ex X,Y, D... watch constants.h enum MitsuDeviceType)
     *    @param  memory_hex_word_address_to_read address where is stored the int32 value
     *    @param  no_of_words_to_read number of words to be readed
     *    @return the ascii string text alocated in the specified address and device
     */
    String readString(MitsuDeviceType memoryType, uint32_t memory_hex_word_address_to_read, int no_of_words_to_read);

    /*!
     *    @brief Get the actual value of monitoring timmer
     *    @return the actual value of monitoring timmer
     */
    uint8_t getMonitoringTimer();

    /*!
     *    @brief Set the value of the MonitoringTimer parameter
     *    @return nothing
     */
    void setMonitoringTimer(uint8_t newTime);
};

#endif
