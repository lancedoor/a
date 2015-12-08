#pragma once

class Receptionist : public Actor {
	Receptionist() {
		MessageManager.Get()->PostMessage()
	}
	void OnPacket(const string &s) {

	}
};