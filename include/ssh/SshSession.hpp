#ifndef SSH_SESSION_HPP
#define SSH_SESSION_HPP

#include "libssh2.h"
#include <string>

class SshSession
{
public:
	SshSession(std::string hostname, const uint16_t port, std::string username, std::string password);
	~SshSession();
	int runCmd(std::string cmd, std::string &response);
private:
	int m_sock = 0;
	LIBSSH2_SESSION *m_session = NULL;
};

#endif // SSH_SESSION_HPP
