#include "ScreenDriver.h"
Servo myservo;  // create servo object to control a servo

int ledPin = D7; // Instead of writing D7 over and over again, we'll write led2
const int buttonPin = D2;     // the number of the pushbutton pin
const int servoPin = A4;

int buttonState = 0;
int previousButtonState = 0;
unsigned long startTime = 0;
unsigned long PING_TIMEOUT = 20*1000;
String _message;

ScreenDriver screen;
void setup()
{
    Particle.publish("setup begin");
//    Particle.function("setSignal", setSignalP);
    Particle.function("clearSignal", clearSignalP);
    Particle.function("endMsg", finishMessageP);
    Particle.function("clearMsg",clearMessageP);
    Particle.function("appendMsg",appendMessageP);

    pinMode(ledPin,OUTPUT);
    digitalWrite(ledPin, HIGH);
    pinMode(buttonPin, INPUT);

    myservo.attach(servoPin);  // attaches the servo on the A0 pin to the servo object

    screen.init();
    setFlag(false);
    startTime = millis();
}

void loop()
{
    checkIfButtonChanged();
    tryToPingServer();
    screen.setState(buttonState == LOW);
    screen.pump();
}

void tryToPingServer() {
    unsigned long currentTime = millis();
    if(currentTime - startTime > PING_TIMEOUT) {
        startTime = currentTime;
        Spark.publish("ping-mailbox");

    }
}
void checkIfButtonChanged() {

    buttonState = digitalRead(buttonPin);
    if(buttonState == HIGH)
        digitalWrite(ledPin, LOW);
    else
        digitalWrite(ledPin, HIGH);

    if(buttonState != previousButtonState) {
        Particle.publish("button state changed");
        previousButtonState = buttonState;
    }
}

void setFlag(bool up) {
    myservo.write(up? 91:0);
}

void clearMessage()
{
  _message = "";
}

void appendMessage(const String& message)
{
  _message.concat(message);
}

void finishMessage()
{
  screen.parse(_message);
}
void setMessage(const String& m)
{
    clearMessage();
    appendMessage(m);
    finishMessage();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

int appendMessageP(String message) {
  Particle.publish("append message: " + message);
  appendMessage(message);
  return 0;
}

int clearMessageP(String param) {
  Particle.publish("clear message");
  setFlag(true);
  clearMessage();
  return 0;
}

int finishMessageP(String param) {
  Particle.publish("finish message",_message);
  setFlag(true);
  finishMessage();
  return 0;
}
int setSignalP(String count) {
    Particle.publish("set signal received");
    setFlag(true);
    return 0;
}


int clearSignalP(String s) {
    Particle.publish("clear signal received");
    setFlag(false);
    setMessage("");
    return 0;
}
