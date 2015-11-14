#ifndef _MESSAGE_PARSER_H
#define _MESSAGE_PARSER_H

#include "application.h"

struct Message
{
  String subject;
  String sender;
};

class MessageParser
{
public:	
	MessageParser();
	void parse(const String& text);
	int messageCount() {return _messageCount;}
	Message* messages() {return _messages;}
private:
	Message* _messages;
	int _messageCount;
};


#endif
