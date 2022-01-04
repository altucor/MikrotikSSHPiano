#include "Mikrotik.hpp"

Mikrotik::Mikrotik(
		std::string hostname, 
		const uint16_t port, 
		std::string username, 
		std::string password
) : m_sshSession(hostname, port, username, password)
{
	
}

int Mikrotik::playNote(float freq, float length)
{
	std::string cmd = "beep";
	cmd.append(" frequency=" + std::to_string(freq));
	cmd.append(" length=" + std::to_string(length));

	std::string reposnse;
	return m_sshSession.runCmd(cmd, reposnse);
}
