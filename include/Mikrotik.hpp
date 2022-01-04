#ifndef MIKROTIK_HPP
#define MIKROTIK_HPP

#include "ssh/SshSession.hpp"

class Mikrotik
{
public:
	Mikrotik(
		std::string hostname, 
		const uint16_t port, 
		std::string username, 
		std::string password
	);
	int playNote(float freq, float length);
private:
	SshSession m_sshSession;
};


#endif // MIKROTIK_HPP
