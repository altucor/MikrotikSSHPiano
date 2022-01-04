#ifndef SSH_WRAPPER_HPP
#define SSH_WRAPPER_HPP

#include "libssh2.h"

class SshWrapper
{
public:
	explicit SshWrapper(std::string &user, std::string &ip, uint16_t port = 22);
	~SshWrapper();
	void playNote(float freq, float length);

private:
	std::string m_user = "";
	std::string m_ip = "";
	uint16_t m_port = 22;
	LIBSSH2_SESSION *m_session;

private:
	std::string m_getpass(const char *prompt, bool show_asterisk = true);
	int m_runCmd(std::string &cmd);
};

#endif // SSH_WRAPPER_HPP
