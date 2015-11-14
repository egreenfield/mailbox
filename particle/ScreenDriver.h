#ifndef _SCREEN_DRIVER_H_
#define _SCREEN_DRIVER_H_

#include "application.h"

class String;
class Message;
class ScreenLine;
class Adafruit_SSD1306;


//------------------------------------------------------------------------------------------------
//************************************************************************************************
//------------------------------------------------------------------------------------------------

class ScreenDriver {
public:
  ScreenDriver();
  ~ScreenDriver();

  void setMessages(int messageCount,Message* messages);
  void init();
  void pump();
  void setState(bool on);
  bool getState() {return _on;}
  Message* getMessages() { return _messages; } 
private:
  void displayMessage(int msgIdx);
  void nextMessage();
  ScreenLine* _lines;  
  Message* _messages;
  int _messageCount;
  int _lineCount;

  int _currentPosition;
  int _textWidth;
  int _currentMessage;
  Adafruit_SSD1306* _display;
  bool _on;
  int _state;
  int _pauseTime;
};



#endif
