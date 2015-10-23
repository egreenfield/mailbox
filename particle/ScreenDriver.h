#ifndef _SCREEN_DRIVER_H_
#define _SCREEN_DRIVER_H_

class String;

class ScreenDriver {
public:
  ScreenDriver();
  ~ScreenDriver();

  void setText(const String& text);
  void parse(const String& text);
  void init();
  void pump();
  void setState(bool on);
  bool getState() {return _on;}
private:
  String* _text;
  int _currentPosition;
  int _textWidth;
  void* _display;
  bool _on;
};



#endif
