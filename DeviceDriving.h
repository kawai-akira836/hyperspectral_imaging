class SerialComm {
protected:
  DCB dcb;
public:
  void setDCBlength
  void setBaudRate
  void setByteSize
  void setfBinary
  void setfParity
  void setStopBits
  void setfOutxCtsFlow
  void setfOutxDsrFlow
  void setfDtrControl
  void setfRtsControl

  void setfOutX
  void setfInX
  void setfTXContinueOnXoff
  void setXonLim
  void setXoffLim
  void setXonChar
  void setXoffChar

  void setfNull
  void setfAbortOnError
  void setfErrorChar
  void setErrorChar
  void setEofChar
  void setEvtChar

  void open();
  void setBuffer();
  void initBuffer();

  SerialComm();
}
