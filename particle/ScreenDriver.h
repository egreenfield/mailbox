#ifndef _SCREEN_DRIVER_H_
#define _SCREEN_DRIVER_H_

class String;
class ScreenLine;
class Adafruit_SSD1306;
struct Message;

class ScreenDriver {
public:
  ScreenDriver();
  ~ScreenDriver();

  void parse(const String& text);
  void init();
  void pump();
  void setState(bool on);
  bool getState() {return _on;}
private:
  ScreenLine* _lines;  
  Message* _messages;
  int _lineCount;

  int _currentPosition;
  int _textWidth;
  Adafruit_SSD1306* _display;
  bool _on;
};



#endif
