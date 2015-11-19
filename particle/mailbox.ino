#include "ScreenDriver.h"
#include "MessageParser.h"
#include "Button.h"
#include "util.h"


Servo myservo;  // create servo object to control a servo

int ledPin = D7; // Instead of writing D7 over and over again, we'll write led2
const int buttonPin = D2;     // the number of the pushbutton pin
const int servoPin = A4;
const char * kTerminator = "\t\t\t";


unsigned long startTime = 0;
unsigned long PING_TIMEOUT = 20*1000;
unsigned long long lastReadTime = 0;
String _message;
Button doorSwitch(buttonPin);
ScreenDriver screen;
MessageParser parser;

void setup()
{
    Particle.publish("setup begin");
    Particle.function("clearSignal", clearSignalP);

    Particle.subscribe("hook-response/ping-mailbox", mailboxPingResponse, MY_DEVICES);

    pinMode(ledPin,OUTPUT);
    digitalWrite(ledPin, HIGH);
    doorSwitch.init();

    myservo.attach(servoPin);  // attaches the servo on the A0 pin to the servo object

    screen.init();
    setFlag(false);
    startTime = 0;
}

void loop()
{
    doorSwitch.poll();
    tryToPingServer();
    screen.setState(doorSwitch.on() == false);
    digitalWrite(ledPin,(doorSwitch.on() == false)? HIGH:LOW);
    screen.pump();
    if(doorSwitch.changed() && doorSwitch.on()) {
      if(parser.latestMessage())
        lastReadTime = parser.latestMessage()->sendTime;
//      debug("reset","setting last read time to %llu",lastReadTime);
      setFlag(false);
    }
}

void tryToPingServer() {
    unsigned long currentTime = millis();
    if(currentTime - startTime > PING_TIMEOUT) {
        startTime = currentTime;
        Spark.publish("ping-mailbox");

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
    if(parser.messageCount() == 0) {
//      debug("nothing_unread");
      setFlag(false);  
    } else {
//      debug("unread","compare %llu - %llu",parser.latestMessage()->sendTime,lastReadTime);
      if (parser.latestMessage()->sendTime <= lastReadTime) {
        setFlag(false);  
      } else {
        setFlag(true);  
      }
    }
}

void mailboxPingResponse(const char * name,const char * data)
{
  String eventName(name);

  if(eventName.endsWith("0"))  {
    clearMessage();
  }

  String fragment(data);
  _message.concat(fragment);
  if(fragment.endsWith(kTerminator))
  {
    finishMessage();
  }
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

int setSignalP(String count) {
    setFlag(true);
    return 0;
}


int clearSignalP(String s) {
    setFlag(false);
    clearMessage();
    finishMessage();
    return 0;
}
