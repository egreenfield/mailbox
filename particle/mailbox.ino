#include "ScreenDriver.h"
#include "MessageParser.h"
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
MessageParser parser;
void setup()
{
    Particle.publish("setup begin");
    Particle.function("clearSignal", clearSignalP);

    Particle.subscribe("hook-response/ping-mailbox", mailboxPingResponse, MY_DEVICES);

    pinMode(ledPin,OUTPUT);
    digitalWrite(ledPin, HIGH);
    pinMode(buttonPin, INPUT);

    myservo.attach(servoPin);  // attaches the servo on the A0 pin to the servo object

    screen.init();
    setFlag(false);
    startTime = 0;
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

void finishMessage() {
    parser.parse(_message);
    screen.setMessages(parser.messageCount(),parser.messages());
    Particle.publish("ending_response");
    if(parser.messageCount() > 0)
      setFlag(true);  
    else
      setFlag(false);  
}

void mailboxPingResponse(const char * name,const char * data)
{
  String eventName(name);

  if(eventName.endsWith("0"))  {
    Particle.publish("first_response_clearing");
    setFlag(true);
    clearMessage();
  }

  String fragment(data);
  _message.concat(fragment);
  Particle.publish("appending_response",fragment);
  if(fragment.endsWith("\t\t\t"))
  {
    finishMessage();
  }
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

int setSignalP(String count) {
    Particle.publish("set signal received");
    setFlag(true);
    return 0;
}


int clearSignalP(String s) {
    Particle.publish("clear signal received");
    setFlag(false);
    clearMessage();
    finishMessage();
    return 0;
}
