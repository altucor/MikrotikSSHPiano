#include <iostream>
#include <string>

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#include "SshWrapper.h"

int verbosity = SSH_LOG_WARNING;

SshWrapper::SshWrapper(std::string &user, std::string &ip, unsigned int port)
	: m_user(user),
	m_ip(ip),
	m_port(port)
{
	initialize();
}


SshWrapper::~SshWrapper()
{
	ssh_free(m_session);
}

int SshWrapper::initialize() {
	m_session = ssh_new();

	if (m_session == NULL) {
		this->~SshWrapper();
		exit(-1);
	}

	makeSession();

	std::cout << " >> Connecting to host\n";
	int connection;
	connection = ssh_connect(m_session);
	if (connection != SSH_OK) {
		std::cout << " >> Error connecting: " << ssh_get_error(m_session) << std::endl;
		exit(-1);
	}
	else {
		std::cout << " >> Connected\n";
	}

	int rc;
	std::string password;

	password = m_getpass(" >> Please enter your password: ", true);
	rc = ssh_userauth_password(m_session, NULL, password.c_str());

	if (rc != SSH_AUTH_SUCCESS) {
		std::cout << " >> Error authentication. " << ssh_get_error(m_session) << std::endl;
		ssh_disconnect(m_session);
		ssh_free(m_session);
		exit(-1);
	}
	return 0;
}

void SshWrapper::makeSession() {
	ssh_options_set(m_session, SSH_OPTIONS_HOST, m_ip.c_str());
	ssh_options_set(m_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
	ssh_options_set(m_session, SSH_OPTIONS_PORT, &m_port);
	ssh_options_set(m_session, SSH_OPTIONS_USER, m_user.c_str());
}

std::string SshWrapper::m_getpass(const char *prompt, bool show_asterisk)
{
	const char BACKSPACE = 8;
	const char RETURN = 13;

	std::string password;
	unsigned char ch = 0;

	std::cout << prompt << std::endl;

	DWORD con_mode;
	DWORD dwRead;

	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

	GetConsoleMode(hIn, &con_mode);
	SetConsoleMode(hIn, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

	while (ReadConsoleA(hIn, &ch, 1, &dwRead, NULL) && ch != RETURN)
	{
		if (ch == BACKSPACE)
		{
			if (password.length() != 0)
			{
				if (show_asterisk)
					std::cout << "\b \b";
				password.resize(password.length() - 1);
			}
		}
		else
		{
			password += ch;
			if (show_asterisk)
				std::cout << '*';
		}
	}
	std::cout << std::endl;
	return password;
}

int SshWrapper::m_runCmd(std::string &cmd) {
	ssh_channel channel;
	int rc;
	char buffer[256];
	int nbytes;

	channel = ssh_channel_new(m_session);
	if (channel == NULL)
		return SSH_ERROR;

	rc = ssh_channel_open_session(channel);
	if (rc != SSH_OK) {
		ssh_channel_free(channel);
		return rc;
	}

	rc = ssh_channel_request_exec(channel, cmd.c_str());
	if (rc != SSH_OK) {
		ssh_channel_close(channel);
		ssh_channel_free(channel);
		return rc;
	}

	nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
	while (nbytes > 0) {
		if (_write(1, buffer, nbytes) != (unsigned int)nbytes) {
			ssh_channel_close(channel);
			ssh_channel_free(channel);
		}
		nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
	}

	if (nbytes < 0) {
		ssh_channel_close(channel);
		ssh_channel_free(channel);
		return SSH_ERROR;
	}
	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);
	return SSH_OK;
}

void SshWrapper::playNote(float freq, float length) {
	std::string cmd = "beep";
	cmd.append(" frequency=" + std::to_string(freq));
	cmd.append(" length=" + std::to_string(length));

	std::cout << cmd << std::endl;
	m_runCmd(cmd);
}




