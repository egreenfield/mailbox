#ifndef _SCREEN_DRIVER_H_
#define _SCREEN_DRIVER_H_


class ScreenDriver {
public:
  ScreenDriver();
  ~ScreenDriver();

  void setText(char* text);
  void init();
  void pump();
  void setState(bool on);
private:
  char* _text;
  int _currentPosition;
  int _textWidth;
  void* _display;
  bool _on;
};



#endif
