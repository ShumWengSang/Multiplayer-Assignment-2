#pragma once
#include "ChatBackground.h"
#include <hge.h>
#include <string>
#include <iostream>
#include "hgefont.h"
class hgeFont;

class ChatSystem
{
public:
	ChatSystem();
	~ChatSystem();

	int x, y;

	//True means continue input. False means send and stop input.
	bool ChatTime;
	bool KeyboardInput(bool&);
	ChatBackground theBackground;
	std::auto_ptr<hgeFont> font_;

	//Call upon this to get the string to send.
	std::string StringtoSend();

	//Call this with the string received.
	void ReceiveString(std::string);

	void Render();

private:
	//This is the string
	std::string StringtoRender;
	//Render the string as it is typed.
	std::string ToSendBuffer;
	//String that is meant to be sent over.
	std::string SendString;
	//The counter to how many end lines are there
	unsigned int EndLineCounter;
	bool keys[256];
	bool BackSpace, Space;
	bool FirstRun;

	bool AllKeys[256];
	
};

