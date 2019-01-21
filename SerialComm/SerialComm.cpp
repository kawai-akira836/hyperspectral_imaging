//#include "pch.h"
#include "DeviceDriving.h"
#include <string>//良く知らん
#include <stdio.h>//printfなどの標準入出力関数を使うためのヘッダファイル
#include <windows.h>//Wi32APIを使うためのヘッダファイル
#include <tchar.h>//_T()使うのに要る
#include <iostream>
using namespace std;

//COMポート1を開くメンバ関数の実装
int SerialComm::open() {
  LPCSTR portName = "\\\\.\\COM1";
  this->hComPort = CreateFile(    //ファイルとしてポートを開く
    (LPCTSTR)portName,      // ポート名を指すバッファへのポインタ:COM1を開く（デバイスマネージャでどのポートが使えるか確認）
    GENERIC_READ | GENERIC_WRITE, // アクセスモード:読み書き両方する
    0,                      //ポートの共有方法を指定:オブジェクトは共有しない
    NULL,                   //セキュリティ属性:ハンドルを子プロセスへ継承しない
    OPEN_EXISTING,          //ポートを開き方を指定:既存のポートを開く
    0,                      //ポートの属性を指定:同期　非同期にしたいときはFILE_FLAG_OVERLAPPED
    NULL                    // テンプレートファイルへのハンドル:NULLって書け
  );
  if (this->hComPort == INVALID_HANDLE_VALUE) {//ポートの取得に失敗
    printf("指定COMポートが開けません.\n\r");
    CloseHandle(this->hComPort);//ポートを閉じる
    return 1;
  }
  else {
    printf("COMポートは正常に開けました.\n\r");
	return 0;
  }
}


//バッファを設定するメンバ関数の実装
int SerialComm::setBuffer() {
  int check;          //エラーチェック用の変数
  check = SetupComm(
    this->hComPort,         //COMポートのハンドラ
    1024,             //受信バッファサイズ:1024byte
    1024              //送信バッファ:1024byte
  );
  if (check == FALSE) {
    printf("送受信バッファの設定ができません.\r\n");
    CloseHandle(this->hComPort);
    return 1;
  }
  else {
    printf("送受信バッファの設定が完了しました.\r\n");
	return 0;
  }
}


//バッファを初期化するメンバ関数の実装
int SerialComm::initBuffer() {
  int check;
  check = PurgeComm(
    this->hComPort,//COMポートのハンドラ
    PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR//出入力バッファをすべてクリア
  );
  if (check == FALSE) {
    printf("送受信バッファの初期化ができません.\r\n");
    CloseHandle(this->hComPort);
    return 1;
  }
  else {
    printf("送受信バッファの初期化が完了しました.\r\n");
	return 0;
  }
}


//DCBの設定値を書きこむメンバ関数の実装
int SerialComm::writeDCB(DCB *dcb) {
	int check;
  check = SetCommState(this->hComPort, dcb);  //設定値の書き込み
  if (check == FALSE) {//エラーチェック
    printf("COMポート構成情報の変更に失敗しました.\r\n");
    CloseHandle(this->hComPort);
    return 1;
  }
  else {
    printf("COMポート構成情報を変更しました.\r\n");
	return 0;
  }
}


//タイムアウト情報を設定するメンバ関数の実装
int SerialComm::setTimeouts() {
  int check;
  COMMTIMEOUTS TimeOut; // COMMTIMEOUTS構造体の変数を宣言
  GetCommTimeouts(this->hComPort, &TimeOut); // タイムアウトの設定状態を取得

  TimeOut.ReadTotalTimeoutMultiplier = 0;//読込の１文字あたりの時間:タイムアウトなし
  TimeOut.ReadTotalTimeoutConstant = 1000;//読込エラー検出用のタイムアウト時間
  //(受信トータルタイムアウト) = ReadTotalTimeoutMultiplier × (受信予定バイト数) + ReadTotalTimeoutConstant
  TimeOut.WriteTotalTimeoutMultiplier = 0;//書き込み１文字あたりの待ち時間:タイムアウトなし
  TimeOut.WriteTotalTimeoutConstant = 1000;//書き込みエラー検出用のタイムアウト時間
  //(送信トータルタイムアウト) = WriteTotalTimeoutMultiplier ×(送信予定バイト数) + WriteTotalTimeoutConstant

  check = SetCommTimeouts(this->hComPort, &TimeOut);//タイムアウト設定の書き換え
  if (check == FALSE) {//エラーチェック
    printf("タイムアウトの設定に失敗しました.\r\n");
    CloseHandle(this->hComPort);
    return 1;
  }
  else {
    printf("タイムアウトの設定に成功しました.\r\n");
	return 0;
  }
}

//データを送信するメンバ関数の実装
void SerialComm::sendData(string data) {
	
  int toWriteBytes;
  DWORD index;
  DWORD writeBytes;//実際に送信したデータサイズ
  cout << data << "を送信開始" << endl;
  string data_temp = data + "\r\n";
  const char* sendData = data_temp.c_str();
  toWriteBytes = strlen(sendData) + 1;
  index = 0;
  while (toWriteBytes > 0) {
    WriteFile(this->hComPort,
      ((BYTE*)sendData) + index, toWriteBytes, &writeBytes, NULL);
    index += writeBytes;
    toWriteBytes -= writeBytes;
  }
  cout << data  << "を送信終了" << endl;
}


//ポートを閉じるメンバ関数の実装
void SerialComm::close() {
  CloseHandle(this->hComPort);
}

HANDLE SerialComm::gethComPort() {
	return this->hComPort;
}

SerialComm::SerialComm(){}