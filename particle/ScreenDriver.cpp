#include "ScreenDriver.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"


#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


#define FONT_SIZE 1
#define CHAR_PIXEL_SIZE 3*FONT_SIZE
#define VELOCITY 4

//------------------------------------------------------------------------------------------------
//************************************************************************************************
//------------------------------------------------------------------------------------------------
struct Message
{
  String subject;
  String sender;
};

//------------------------------------------------------------------------------------------------
//************************************************************************************************
//------------------------------------------------------------------------------------------------

class ScreenLine
{
public:
  ScreenLine();  
  void init(Adafruit_SSD1306* display, int placement,const String& text);
  void update();

private:
    String _text;    
    int _width;
    int _placement;
    int _scrollPosition;
    Adafruit_SSD1306* _display;
};


//------------------------------------------------------------------------------------------------

ScreenLine::ScreenLine()
{
}

void ScreenLine::init(Adafruit_SSD1306* display,int placememnt,const String& text)
{
  _text = text;
  _placement = placememnt;
  _display = display;
  _width = CHAR_PIXEL_SIZE * 2 * _text.length(); // 12 = 6 pixels/character * text size 2
  _scrollPosition    = _display->width();
}

void ScreenLine::update()
{
    _display->setCursor(_scrollPosition, 20);
    _display->print(_text);
    _scrollPosition -= VELOCITY;
    if(_scrollPosition < -_width)
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
{}

ScreenDriver::~ScreenDriver() 
{
  delete [] _lines;
  delete [] _messages;
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

void ScreenDriver::parse(const String& msg) {
  
  Particle.publish(String("parsing"));

  delete [] _messages; _messages  = NULL;
  delete [] _lines; _lines = NULL;


  int stop = msg.indexOf('\t');
  int msgCount = atoi(msg.substring(0,stop));

  _messages = new Message[msgCount];

  int start = stop+1;
  for(int i=0;i<msgCount;i++) {
    stop = msg.indexOf('\t',start);
    _messages[i].subject = msg.substring(start,stop);
    start = stop+1;
    stop = msg.indexOf('\t',start);
    _messages[i].sender = msg.substring(start,stop);
    start = stop+1;
//    setText(_messages[i].subject);
  }
  _lines = new ScreenLine[1];
  Particle.publish(String("parsed: ") + _messages[0].subject);

  _lines[0].init(_display,20,_messages[0].subject);
  _lineCount = 1;

  bool state = getState();
  setState(false);
  setState(state);
}

void ScreenDriver::setState(bool on) {
  if(_on != on) {
    if(on) {
        _currentPosition = 0;//display->width();
    }
    _on = on;
  }

}
void ScreenDriver::pump() {
  _display->clearDisplay();
  if(_on) {
    for(int i=0;i<_lineCount;i++)
    {
      _lines[i].update();
    }
  }
  _display->display();
}
