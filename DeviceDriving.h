class SerialComm {
protected:
  HANDLE hComPort;          //COMポートのハンドル
public:
  void
  void open();
  void setBuffer();
  void initBuffer();
  void writeDCB(*dcb);
  void setTimeouts();
  void close();

  void sendData(string data);

  HANDLE gethComPort();
  SerialComm();
};
