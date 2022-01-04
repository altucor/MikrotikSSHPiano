#include "ssh/SshSession.hpp"

#include <stdexcept>

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
	struct timeval timeout;
	int rc;
	fd_set fd;
	fd_set *writefd = NULL;
	fd_set *readfd = NULL;
	int dir;

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	FD_ZERO(&fd);
	FD_SET(socket_fd, &fd);

	/* now make sure we wait in the correct direction */
	dir = libssh2_session_block_directions(session);
	if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
		readfd = &fd;
	if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
		writefd = &fd;
	rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);
	return rc;
}

SshSession::SshSession(std::string hostname, const uint16_t port, std::string username, std::string password)
{
#ifdef WIN32
	WSADATA wsadata;
	int err = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if(err != 0)
		throw std::runtime_error("WSAStartup failed");
#endif
	uint32_t hostaddr = inet_addr(hostname.data());
	m_sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = hostaddr;
	if(connect(m_sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0)
	{
		throw std::runtime_error("Error failed to connect to socket");
	}

	m_session = libssh2_session_init();
	if(!m_session)
		throw std::runtime_error("Error libssh2_session_init");
	libssh2_session_set_blocking(m_session, 0);
	int rc = 0;
	while((rc = libssh2_session_handshake(m_session, m_sock)) == LIBSSH2_ERROR_EAGAIN);
	if(rc)
		throw std::runtime_error("Failure establishing SSH session");
	
	LIBSSH2_KNOWNHOSTS *nh = libssh2_knownhost_init(m_session);
	if(!nh)
		throw std::runtime_error("Failure libssh2_knownhost_init");

	/* read all hosts from here */
	libssh2_knownhost_readfile(nh, "known_hosts", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

	/* store all known hosts to here */
	libssh2_knownhost_writefile(nh, "dumpfile", LIBSSH2_KNOWNHOST_FILE_OPENSSH);

	int type;
	std::size_t len;
	const char *fingerprint = libssh2_session_hostkey(m_session, &len, &type);
	if(fingerprint)
	{
		struct libssh2_knownhost *host;
		int check = libssh2_knownhost_checkp(
			nh,
			hostname.data(), 
			port,
			fingerprint, 
			len,
			LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW,
			&host
		);

		//fprintf(stderr, "Host check: %d, key: %s\n", check,
		//		(check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
		//		host->key:"<none>");

		/*****
		 * At this point, we could verify that 'check' tells us the key is
		 * fine or bail out.
		 *****/
	}
	else
	{
		throw std::runtime_error("Failure libssh2_session_hostkey");
	}
	libssh2_knownhost_free(nh);

	if(password.size() != 0) {
		/* We could authenticate via password */
		while((rc = libssh2_userauth_password(m_session, username.data(), password.data())) ==
			   LIBSSH2_ERROR_EAGAIN);
		if(rc) {
			throw std::runtime_error("Authentication by password failed");
		}
	}
	else {
		/* Or by public key */
		while((rc = libssh2_userauth_publickey_fromfile(
			m_session, username.data(),
			"/home/user/"
			".ssh/id_rsa.pub",
			"/home/user/"
			".ssh/id_rsa",
			password.data())) == LIBSSH2_ERROR_EAGAIN);
		if(rc) {
			throw std::runtime_error("Authentication by public key failed");
		}
	}

}

SshSession::~SshSession()
{
	libssh2_session_disconnect(m_session, "Normal Shutdown, Thank you for playing");
	libssh2_session_free(m_session);
#ifdef WIN32
	closesocket(m_sock);
#else
	close(m_sock);
#endif
}

int SshSession::runCmd(std::string cmd, std::string &response)
{
	char *exitsignal = (char *)"none";
	int exitcode = 0;
	LIBSSH2_CHANNEL *channel;
	while((channel = libssh2_channel_open_session(m_session)) == NULL &&
		  libssh2_session_last_error(m_session, NULL, NULL, 0) ==
		  LIBSSH2_ERROR_EAGAIN) {
		waitsocket(m_sock, m_session);
	}
	if(channel == NULL) {
		fprintf(stderr, "Error\n");
		exit(1);
	}
	int rc = 0;
	while((rc = libssh2_channel_exec(channel, cmd.data())) ==
		   LIBSSH2_ERROR_EAGAIN) {
		waitsocket(m_sock, m_session);
	}
	if(rc != 0) {
		fprintf(stderr, "Error\n");
		exit(1);
	}
	int bytecount = 0;
	for(;;) {
		/* loop until we block */
		int rc;
		do {
			char buffer[0x4000];
			rc = libssh2_channel_read(channel, buffer, sizeof(buffer) );
			if(rc > 0) {
				int i;
				bytecount += rc;
				fprintf(stderr, "We read:\n");
				for(i = 0; i < rc; ++i)
					fputc(buffer[i], stderr);
				fprintf(stderr, "\n");
			}
			else {
				if(rc != LIBSSH2_ERROR_EAGAIN)
					/* no need to output this for the EAGAIN case */
					fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
			}
		}
		while(rc > 0);

		/* this is due to blocking that would occur otherwise so we loop on
		   this condition */
		if(rc == LIBSSH2_ERROR_EAGAIN) {
			waitsocket(m_sock, m_session);
		}
		else
			break;
	}
	exitcode = 127;
	while((rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN)
		waitsocket(m_sock, m_session);

	if(rc == 0) {
		exitcode = libssh2_channel_get_exit_status(channel);
		libssh2_channel_get_exit_signal(channel, &exitsignal,
										NULL, NULL, NULL, NULL, NULL);
	}

	if(exitsignal)
		fprintf(stderr, "\nGot signal: %s\n", exitsignal);
	else
		fprintf(stderr, "\nEXIT: %d bytecount: %d\n", exitcode, bytecount);

	libssh2_channel_free(channel);
	channel = NULL;
	return 0;
}
