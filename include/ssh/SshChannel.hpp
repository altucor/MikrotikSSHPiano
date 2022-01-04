#ifndef SSH_CHANNEL_HPP
#define SSH_CHANNEL_HPP

#include "libssh2.h"
#include "ssh/SshSession.hpp"

class SshChannel
{
public:
	SshChannel(SshSession &session, std::string ip, std::string port);
	~SshChannel();
private:
	LIBSSH2_CHANNEL *channel;
};

#endif // SSH_CHANNEL_HPP
