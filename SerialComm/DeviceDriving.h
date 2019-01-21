#include <string>//良く知らん
#include <stdio.h>//printfなどの標準入出力関数を使うためのヘッダファイル
#include <windows.h>//Wi32APIを使うためのヘッダファイル
#include <tchar.h>//_T()使うのに要る
#include <iostream>
using namespace std;


class SerialComm {
protected:
  HANDLE hComPort;          //COMポートのハンドル
public:
  int open();
  int setBuffer();
  int initBuffer();
  int writeDCB(DCB *dcb);
  int setTimeouts();
  void close();

  void sendData(string data);

  HANDLE gethComPort();
  SerialComm();
};
