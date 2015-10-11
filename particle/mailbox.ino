Servo myservo;  // create servo object to control a servo

int ledPin = D7; // Instead of writing D7 over and over again, we'll write led2
const int buttonPin = D2;     // the number of the pushbutton pin
const int servoPin = A4;

int buttonState = 0;
int previousButtonState = 0;
unsigned long startTime = 0;
unsigned long PING_TIMEOUT = 20*1000;
void setup()
{
    Particle.publish("setup begin");
    Particle.function("setSignal", setSignalP);
    Particle.function("clearSignal", clearSignalP);

    pinMode(ledPin,OUTPUT);
    digitalWrite(ledPin, HIGH);
    pinMode(buttonPin, INPUT);

    myservo.attach(servoPin);  // attaches the servo on the A0 pin to the servo object

    setFlag(false);
    startTime = millis();
}

void loop()
{
    checkIfButtonChanged();
    tryToPingServer();
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
        setFlag(false);
    }
}

void setFlag(bool up) {
    myservo.write(up? 91:0);
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
    return 0;
}