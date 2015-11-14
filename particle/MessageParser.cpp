#include "MessageParser.h"

MessageParser::MessageParser()
: _messageCount(0), _messages(NULL)
{}

void MessageParser::parse(const String& msg) {
  
  delete [] _messages; 
  _messages  = NULL;


  int stop = msg.indexOf('\t',1);
  int msgCount = _messageCount = atoi(msg.substring(0,stop));

  _messages = new Message[msgCount];

  int start = stop+1;
  for(int i=0;i<msgCount;i++) {
    stop = msg.indexOf('\t',start);
    _messages[i].subject = msg.substring(start,stop);
    start = stop+1;
    stop = msg.indexOf('\t',start);
    _messages[i].sender = msg.substring(start,stop);
    start = stop+1;
    long millis =  atoi(msg.substring(start,stop));
    start = stop+1;
    //setText(_messages[i].subject);
  }
}