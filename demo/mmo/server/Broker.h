#pragma once

class Broker : public Actor {
	void Init();
	void Term();


	void OnPacket(const string &s) {
		cout << s << endl;
	}
}