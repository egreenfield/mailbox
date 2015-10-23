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


ScreenDriver::ScreenDriver()
:_on(false)
,_text(NULL)
{}
ScreenDriver::~ScreenDriver() {}

void ScreenDriver::init(void) {
  Adafruit_SSD1306* display = new Adafruit_SSD1306(OLED_RESET);

  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display->begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);  // initialize with the I2C addr 0x3D (for the 128x64)

  display->setTextSize(FONT_SIZE);
  display->setTextColor(WHITE);
  display->setTextWrap(false);
  _display = display;
  _currentPosition    = display->width();
  _text = new String("hello, world");
  _textWidth = CHAR_PIXEL_SIZE * 2 * _text->length(); // 12 = 6 pixels/character * text size 2
}

void ScreenDriver::setText(const String& text)
{
  delete _text;
  _text = new String(text);
  _textWidth = CHAR_PIXEL_SIZE * 2 * _text->length(); // 12 = 6 pixels/character * text size 2

  bool state = getState();
  setState(false);
  setState(true);
}


void ScreenDriver::parse(const String& msg) {
  int stop = msg.indexOf('\t');
  int msgCount = atoi(msg.substring(0,stop));
  int start = stop+1;
  for(int i=0;i<msgCount;i++) {
    stop = msg.indexOf('\t',start);
    String subject = msg.substring(start,stop);
    start = stop+1;
    stop = msg.indexOf('\t',start);
    String from = msg.substring(start,stop);
    start = stop+1;
    setText(subject);
  }
}
void ScreenDriver::setState(bool on) {
  Adafruit_SSD1306* display = (Adafruit_SSD1306*)_display;

  if(_on != on) {
    if(on) {
        _currentPosition = 0;//display->width();
    }
    _on = on;
  }

}
void ScreenDriver::pump() {
  Adafruit_SSD1306* display = (Adafruit_SSD1306*)_display;
  display->clearDisplay();
  if(_on) {
    display->setCursor(_currentPosition, 20);
    display->print(*_text);
    _currentPosition -= VELOCITY;
    if(_currentPosition < -_textWidth)
      _currentPosition = display->width();
  }
  display->display();
}
