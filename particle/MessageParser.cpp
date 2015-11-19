#include "MessageParser.h"
#include "util.h"
MessageParser::MessageParser()
: _messageCount(0), _messages(NULL)
{}

void MessageParser::parse(const String& msgString) {
  
  delete [] _messages; 
  _messages  = NULL;


  int stop = msgString.indexOf('\t',1);
  int msgCount = _messageCount = atoi(msgString.substring(0,stop));
  _latestMessage = NULL;

  _messages = new Message[msgCount];
  Message* msg = _messages;
  int start = stop+1;
  for(int i=0;i<msgCount;i++) {
    stop = msgString.indexOf('\t',start);
    msg->subject = msgString.substring(start,stop);
    start = stop+1;
    stop = msgString.indexOf('\t',start);
    msg->sender = msgString.substring(start,stop);
    start = stop+1;
    stop = msgString.indexOf('\t',start);
    msg->sendTime = atoll(msgString.substring(start,stop));
    start = stop+1;

    if(_latestMessage == NULL || msg->sendTime >= _latestMessage->sendTime) {
    	_latestMessage = msg;
    }
    msg++;
  }
}