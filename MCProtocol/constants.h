#include <Arduino.h>

enum MitsuPLCSeries
{
  QL_SERIES = 0,
  iQR_SERIES = 1
};

enum MitsuPLCRemoteControl
{
  RUN = 0,
  STOP,
  PAUSE,
  CLEAR,
  RESET,
  CPU_MODEL
};

enum MitsuRemoteControlMode
{
  DONT_EXECUTE_FORCIBLY = 0x01,
  EXECUTE_FORCIBLY = 0x03
};

enum MitsuRemoteControlClearMode
{
  DO_NOT_CLEAR = 0x01,
  CLEAR_ONLY_OUTSIDE_LATCH_RANGE = 0x02,
  ALL_CLEAR = 0x03

};

enum MitsuDeviceType
{
  SM = 0 /*SPECIAL RELAY*/,
  SD /*SPECIAL REGISTER*/,
  X /*INPUT*/,
  Y /*OUTPUT*/,
  M /*INTERNAL RELAY*/,
  L /*LATCH RELAY*/,
  F /*ANNUNCIATOR*/,
  V /*EDGE RELAY*/,
  B /*LINK RELAY*/,
  D /*DATA REGISTER*/,
  W /*LINK REGISTER*/,
  TS /*TIMER-CONTACT*/,
  TC /*TIMER-COIL*/,
  TN /*TIMER-CURRENT VALUE*/,
  LTS /*LONG TIMER-CONTACT*/,
  LTC /*LONG TIMER-COIL*/,
  LTN /*LONG TIMER-CURRENT VALUE*/,
  STS /*RETENTIVE TIMER-CONTACT*/,
  STC /*RETENTIVE TIMER-COIL*/,
  STN /*RETENTIVE TIMER-CURRENT VALUE*/,
  LSTS /*LONG RETENTIVE TIMER-CONTACT*/,
  LSTC /*LONG RETENTIVE TIMER-COIL*/,
  LSTN /*LONG RETENTIVE TIMER-CURRENT VALUE*/,
  CS /*COUNTER-CONTACT*/,
  CC /*COUNTER-COIL*/,
  CN /*COUNTER-CURRENT VALUE*/,
  LCS /*LONG COUNTER-CONTACT*/,
  LCC /*LONG COUNTER-COIL*/,
  LCN /*LONG COUNTER-CURRENT VALUE*/,
  SB /*LINK SPECIAL RELAY*/,
  SW /*LINK SPECIAL REGISTER*/,
  DX /*DIRECT ACCESS INPUT*/,
  DY /*DIRECT ACCESS OUTPUT*/,
  Z /*INDEX REGISTER-INDEX REGISTER*/,
  LZ /*INDEX REGISTER-LONG INDEX REGISTER */,
};

const char DeviceTypeQL[40][3] = {"SM", "SD", "X*", "Y*", "M*", "L*",
                                  "F*", "V*", "B*", "D*", "W*",
                                  "TS", "  ", "  ", "  ", "  ",
                                  "  ", "SS", "SC", "SN", "  ",
                                  "  ", "  ", "CS", "CC", "CN",
                                  "  ", "  ", "  ", "SB", "SW",
                                  "DX", "DY", "Z*", "  ", "R*",
                                  "ZR"};

const char DeviceTypeiQR[40][5] = {"SM**", "SD**", "X***", "Y***", "M***", "L***",
                                   "F***", "V***", "B***", "D***", "W***",
                                   "TS**", "TC**", "TN**", "LTS*", "LTC*",
                                   "LTN*", "STS*", "STC*", "STN*", "LSTS",
                                   "LSTC", "LSTN", "CS**", "CC**", "CN**",
                                   "LCS*", "LCC*", "LCN*", "SB**", "SW**",
                                   "DX**", "DY**", "Z***", "LZ**", "R***",
                                   "ZR**"};

const char dia_de_la_semana[7][10] = {"Domingo", "Lunes",
                                      "Martes", "Miercoles",
                                      "Jueves", "Viernes", "Sabado"};
