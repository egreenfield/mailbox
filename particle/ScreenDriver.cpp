#include "ScreenDriver.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "util.h"
#include "MessageParser.h"

#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


#define FONT_SIZE 1
#define CHAR_PIXEL_SIZE 3*FONT_SIZE
#define VELOCITY 4
static const int kPauseTime = 2000;


typedef enum {
  kAlignment_Left,
  kAlignment_Right,
  kAlignment_Scrolling
} Alignment;
//------------------------------------------------------------------------------------------------
//************************************************************************************************
//------------------------------------------------------------------------------------------------

class ScreenLine
{
public:
  ScreenLine();  
  void init(Adafruit_SSD1306* display, int placement,Alignment align, const String& text);
  bool render();
  bool advance();
  void reset();

private:
    String _text;    
    int _width;
    int _placement;
    int _scrollPosition;
    int _leftBoundary;
    Alignment _align;
    Adafruit_SSD1306* _display;
};


//------------------------------------------------------------------------------------------------

enum {
  kScrolling,
  kPaused
} ScreenState;

ScreenLine::ScreenLine()
{
}

void ScreenLine::init(Adafruit_SSD1306* display,int placememnt,Alignment align,const String& text)
{
  _text = text;
  _placement = placememnt;
  _align = align;
  _display = display;
  _width = CHAR_PIXEL_SIZE * 2 * _text.length(); // 12 = 6 pixels/character * text size 2
  _scrollPosition    =  (_align == kAlignment_Left)? 0                            :
                        (_align == kAlignment_Right)? _display->width() - _width  :
                        (_align == kAlignment_Scrolling)? _display->width()       :
                        0;
  _leftBoundary = _display->width() - _width;
}

bool ScreenLine::render()
{
    _display->setCursor(_scrollPosition, _placement);
    _display->print(_text);
    return (_scrollPosition <= _leftBoundary);
}

bool ScreenLine::advance()
{
    if(_align == kAlignment_Scrolling) {
      _scrollPosition = max(_scrollPosition-VELOCITY,_leftBoundary);
      return (_scrollPosition <= _leftBoundary);
    }
    return true;
}

void ScreenLine::reset()
{
      _scrollPosition = _display->width();  
}
//------------------------------------------------------------------------------------------------
//************************************************************************************************
//------------------------------------------------------------------------------------------------

ScreenDriver::ScreenDriver()
:_on(false)
,_lines(NULL)
,_messages(NULL)
,_lineCount(0)
,_messageCount(0)
{}

ScreenDriver::~ScreenDriver() 
{
  delete [] _lines;
}

void ScreenDriver::init(void) {
  _display = new Adafruit_SSD1306(OLED_RESET);

  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  _display->begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);  // initialize with the I2C addr 0x3D (for the 128x64)

  _display->setTextSize(FONT_SIZE);
  _display->setTextColor(WHITE);
  _display->setTextWrap(false);
}

void ScreenDriver::setMessages(int msgCount, Message* messages) {
  

  _messages  = messages;


  _messageCount = msgCount;

  displayMessage(0);
  bool state = getState();
  setState(false);
  setState(state);
}


void ScreenDriver::displayMessage(int msgIdx) {
  delete [] _lines; 
  _lines = NULL;
  _lineCount = 0;
  if(msgIdx < _messageCount) {
    _lineCount = 5;  
    _lines = new ScreenLine[_lineCount];
    char buf[256];
    sprintf(buf,"%d/%d",msgIdx+1,_messageCount);
    _lines[0].init(_display,8,kAlignment_Left,String(buf));
    _lines[1].init(_display,16,kAlignment_Right,String("from"));
    _lines[2].init(_display,26,kAlignment_Scrolling,_messages[msgIdx].sender);
    _lines[3].init(_display,42,kAlignment_Right,String("subject"));
    _lines[4].init(_display,52,kAlignment_Scrolling,(_messages[msgIdx].subject.length() == 0)? "(blank)":_messages[msgIdx].subject);
    _currentMessage = msgIdx;
    _state = kScrolling;
  debug("displaying message %d:%s",msgIdx,(const char*)_messages[msgIdx].sender);
  }
}

void ScreenDriver::setState(bool on) {
  if(_on != on) {
    if(on) {
        _currentPosition = 0;//display->width();
    }
    _on = on;
  }

}
void ScreenDriver::nextMessage() {
  if(_messageCount > 0)
    displayMessage((_currentMessage+1)% _messageCount);
}

void ScreenDriver::pump() {
  _display->clearDisplay();
  if(_on) {
    for(int i=0;i<_lineCount;i++) {
      _lines[i].render();
    }
    if (_state == kScrolling) {
      bool finished = true;
      for(int i=0;i<_lineCount;i++) {
        finished = _lines[i].advance() && finished;
      }
      if(finished) {
        _state = kPaused;
        _pauseTime = millis();
      }
    }
    else if(_state == kPaused) {
      int t = millis();
      if(t - _pauseTime > kPauseTime) {
        nextMessage();
      }
    }
  }
  _display->display();
}
