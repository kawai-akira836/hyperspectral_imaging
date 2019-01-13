// ConsoleApplication2.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#include <string>//良く知らん
#include <stdio.h>//printfなどの標準入出力関数を使うためのヘッダファイル
#include <windows.h>//Wi32APIを使うためのヘッダファイル
#include <tchar.h>//_T()使うのに要る
#include <iostream>

int main()
{
	LPCSTR portName = "\\\\.\\COM1";
	HANDLE hComPort;//COMポートのハンドルいろいろ使うのでグローバル変数にしておくとよい
	hComPort = CreateFile(    //ファイルとしてポートを開く
		(LPCTSTR)portName,      // ポート名を指すバッファへのポインタ:COM1を開く（デバイスマネージャでどのポートが使えるか確認）
		GENERIC_READ | GENERIC_WRITE, // アクセスモード:読み書き両方する
		0,        //ポートの共有方法を指定:オブジェクトは共有しない
		NULL,       //セキュリティ属性:ハンドルを子プロセスへ継承しない
		OPEN_EXISTING,     //ポートを開き方を指定:既存のポートを開く
		0,   //ポートの属性を指定:同期　非同期にしたいときはFILE_FLAG_OVERLAPPED
		NULL       // テンプレートファイルへのハンドル:NULLって書け
	);
	if (hComPort == INVALID_HANDLE_VALUE) {//ポートの取得に失敗
		printf("指定COMポートが開けません.\n\r");
		CloseHandle(hComPort);//ポートを閉じる
		return 0;
	}
	else {
		printf("COMポートは正常に開けました.\n\r");
	}





	//バッファ設定
	int check;//エラーチェック用の変数
	check = SetupComm(
		hComPort,//COMポートのハンドラ
		1024,//受信バッファサイズ:1024byte
		1024//送信バッファ:1024byte
	);
	if (check == FALSE) {
		printf("送受信バッファの設定ができません.\r\n");
		CloseHandle(hComPort);
		return 0;
	}
	else {
		printf("送受信バッファの設定が完了しました.\r\n");
	}



	
	//バッファ初期化
	check = PurgeComm(
		hComPort,//COMポートのハンドラ
		PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR//出入力バッファをすべてクリア
	);
	if (check == FALSE) {
		printf("送受信バッファの初期化ができません.\r\n");
		CloseHandle(hComPort);
		return 0;
	}
	else {
		printf("送受信バッファの初期化が完了しました.\r\n");
	}



	
	DCB dcb;//構成情報を記録する構造体の生成
	GetCommState(hComPort, &dcb);//現在の設定値を読み込み
	dcb.DCBlength = sizeof(DCB);//DCBのサイズ
	dcb.BaudRate = 9600;//ボーレート:9600bps
	dcb.ByteSize = 8;//データサイズ:8bit
	dcb.fBinary = TRUE;//バイナリモード:通常TRUE
	dcb.fParity = NOPARITY;//パリティビット:パリティビットなし
	dcb.StopBits = ONESTOPBIT;//ストップビット:1bit
	dcb.fOutxCtsFlow = FALSE;//CTSフロー制御:フロー制御なし
	dcb.fOutxDsrFlow = FALSE;//DSRハードウェアフロー制御：使用しない
	dcb.fDtrControl = DTR_CONTROL_DISABLE;//DTR有効/無効:DTR無効
	dcb.fRtsControl = RTS_CONTROL_DISABLE;//RTSフロー制御:RTS制御なし

	dcb.fOutX = FALSE;//送信時XON/XOFF制御の有無:なし
	dcb.fInX = FALSE;//受信時XON/XOFF制御の有無:なし
	dcb.fTXContinueOnXoff = TRUE;// 受信バッファー満杯＆XOFF受信後の継続送信可否:送信可
	dcb.XonLim = 512;//XONが送られるまでに格納できる最小バイト数:512
	dcb.XoffLim = 512;//XOFFが送られるまでに格納できる最小バイト数:512
	dcb.XonChar = 0x11;//送信時XON文字 ( 送信可：ビジィ解除 ) の指定:XON文字として11H ( デバイス制御１：DC1 )
	dcb.XoffChar = 0x13;//XOFF文字（送信不可：ビジー通告）の指定:XOFF文字として13H ( デバイス制御3：DC3 )

	dcb.fNull = TRUE;// NULLバイトの破棄:破棄する
	dcb.fAbortOnError = TRUE;//エラー時の読み書き操作終了:終了する
	dcb.fErrorChar = FALSE;// パリティエラー発生時のキャラクタ（ErrorChar）置換:なし
	dcb.ErrorChar = 0x00;// パリティエラー発生時の置換キャラクタ
	dcb.EofChar = 0x03;// データ終了通知キャラクタ:一般に0x03(ETX)がよく使われます。
	dcb.EvtChar = 0x02;// イベント通知キャラクタ:一般に0x02(STX)がよく使われます

	check = SetCommState(hComPort, &dcb);  //設定値の書き込み
	if (check == FALSE) {//エラーチェック
		printf("COMポート構成情報の変更に失敗しました.\r\n");
		CloseHandle(hComPort);
		return 0;
	}
	else {
		printf("COMポート構成情報を変更しました.\r\n");
	}




	COMMTIMEOUTS TimeOut; // COMMTIMEOUTS構造体の変数を宣言
	GetCommTimeouts(hComPort, &TimeOut); // タイムアウトの設定状態を取得

	TimeOut.ReadTotalTimeoutMultiplier = 0;//読込の１文字あたりの時間:タイムアウトなし
	TimeOut.ReadTotalTimeoutConstant = 1000;//読込エラー検出用のタイムアウト時間
	//(受信トータルタイムアウト) = ReadTotalTimeoutMultiplier × (受信予定バイト数) + ReadTotalTimeoutConstant 
	TimeOut.WriteTotalTimeoutMultiplier = 0;//書き込み１文字あたりの待ち時間:タイムアウトなし
	TimeOut.WriteTotalTimeoutConstant = 1000;//書き込みエラー検出用のタイムアウト時間
	//(送信トータルタイムアウト) = WriteTotalTimeoutMultiplier ×(送信予定バイト数) + WriteTotalTimeoutConstant

	check = SetCommTimeouts(hComPort, &TimeOut);//タイムアウト設定の書き換え
	if (check == FALSE) {//エラーチェック
		printf("タイムアウトの設定に失敗しました.\r\n");
		CloseHandle(hComPort);
		return 0;
	}
	else {
		printf("タイムアウトの設定に成功しました.\r\n");
	}



	char SendData[] = "M:1+P500000\r\n";//送信データの用意
	char SendData2[] = "M:1-P500000\r\n";//送信データの用意
	char SendData3[] = "G:\r\n";//送信データの用意
	char SendData4[] = "T:M\r\n";//送信データの用意
	DWORD index;
	DWORD writeBytes;//実際に送信したデータサイズ
	DWORD toWriteBytes = strlen(SendData) + 1;
	DWORD toWriteBytes2 = strlen(SendData2) + 1;//送信データサイズを取得
	DWORD toWriteBytes3 = strlen(SendData3) + 1;//送信データサイズを取得

	
	
	int n = 0;
	while (n < 200) {
		
		printf("T:Mを送信開始\r\n");
		toWriteBytes = strlen(SendData4) + 1;
		index = 0;
		while (toWriteBytes > 0) {
			WriteFile(hComPort,
				((BYTE*)SendData4) + index, toWriteBytes, &writeBytes, NULL);
			index += writeBytes;
			toWriteBytes -= writeBytes;
		}
		printf("T:Mを送信終了\r\n");

		if (n == 0) {
			Sleep(10000);
		}
		else {
			Sleep(1000);
		}
		



		printf("M:1+P500000を送信開始\r\n");
		toWriteBytes = strlen(SendData) + 1;
		index = 0;
		while (toWriteBytes > 0) {
			WriteFile(hComPort,
				((BYTE*)SendData) + index, toWriteBytes, &writeBytes, NULL);
			index += writeBytes;
			toWriteBytes -= writeBytes;
		}
		printf("M:1+P500000を送信終了\r\n");

		Sleep(1000);
	


		printf("G:を送信開始\r\n");
		toWriteBytes3 = strlen(SendData3) + 1;
		index = 0;
		while (toWriteBytes3 > 0) {
			WriteFile(hComPort,
				((BYTE*)SendData3) + index, toWriteBytes3, &writeBytes, NULL);
			index += writeBytes;
			toWriteBytes3 -= writeBytes;
		}
		printf("G:を送信終了\r\n");

		Sleep(3000);
		



		printf("M:1-P500000を送信開始\r\n");
		toWriteBytes2 = strlen(SendData2) + 1;
		index = 0;
		while (toWriteBytes2 > 0) {
			WriteFile(hComPort,
				((BYTE*)SendData2) + index, toWriteBytes2, &writeBytes, NULL);
			index += writeBytes;
			toWriteBytes2 -= writeBytes;
		}
		printf("M:1-P500000を送信終了\r\n");

		Sleep(1000);



		printf("G:を送信開始\r\n");
		toWriteBytes3 = strlen(SendData3) + 1;
		index = 0;
		while (toWriteBytes3 > 0) {
			WriteFile(hComPort,
				((BYTE*)SendData3) + index, toWriteBytes3, &writeBytes, NULL);
			index += writeBytes;
			toWriteBytes3 -= writeBytes;
		}
		printf("G:を送信終了\r\n");

		Sleep(2000);







		n++;
	}
	/*
	int i = 0;
	int j = 0;
	char RecieveChar[1];//1文字受信のための変数
	char RecieveData[100];//受信文字列
	unsigned long nn;
	while (1) {
		printf("こんにちは-----------っ！！！！\r\n");
		ReadFile(hComPort, RecieveChar, 1, &nn, 0); // シリアルポートに対する読み込み
		if (nn == 1) {
			if (RecieveChar[0] == 10 || RecieveChar[0] == 13) { // '\r'や'\n'を受信すると文字列を閉じる
				if (i != 0) {
					RecieveData[i] = '\0';//最後にNULL文字を付加して
					i = 0;//文字列を先頭に戻す
					printf("%s\n", RecieveData);//受信した文字列を表示
				}
			}
			else {
				RecieveData[i] = RecieveChar[0];//受信した文字を文字列にする
				printf("%s\n", RecieveData);
				i++;
			}
		}
		else {
			printf("読み込めない…\r\n");
		}
	}

	*/

	CloseHandle(hComPort);//ポートを閉じる
	return 0;
	
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
