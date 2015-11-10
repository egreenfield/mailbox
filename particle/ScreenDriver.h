#ifndef _SCREEN_DRIVER_H_
#define _SCREEN_DRIVER_H_

#include "application.h"

class String;
class ScreenLine;
class Adafruit_SSD1306;

struct Message
{
  String subject;
  String sender;
};

//------------------------------------------------------------------------------------------------
//************************************************************************************************
//------------------------------------------------------------------------------------------------

class ScreenDriver {
public:
  ScreenDriver();
  ~ScreenDriver();

  void parse(const String& text);
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
