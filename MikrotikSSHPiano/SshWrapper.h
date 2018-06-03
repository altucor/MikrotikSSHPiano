#pragma once

#include <libssh/libssh.h>

class SshWrapper
{
public:
	explicit SshWrapper(std::string &user, std::string &ip, unsigned int port = 22);
	~SshWrapper();
	void playNote(float freq, float length);

private:
	std::string m_user = "";
	std::string m_ip = "";
	int m_port = 22;
	ssh_session m_session;


private:
	std::string m_getpass(const char *prompt, bool show_asterisk = true);
	int m_runCmd(std::string &cmd);

	int initialize();
	void makeSession();
};

