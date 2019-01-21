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

	SerialComm stage;
	stage.open();
  stage.setBuffer();
  stage.initBuffer();
  stage.writeDCB(*dcb);
  stage.setTimeouts();


	int n = 0;
	while (n < 200) {
		stage.sendData("T:M");

		if (n == 0) {
			Sleep(10000);
		}
		else {
			Sleep(1000);
		}

		stage.sendData("M:1+P500000");

		Sleep(1000);

		stage.sendData("G:");

		Sleep(3000);

		stage.sendData("M:1-P500000");

		Sleep(1000);

		stage.sendData("G:");

		Sleep(2000);

		n++;
	}

	stage.close();//ポートを閉じる
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
