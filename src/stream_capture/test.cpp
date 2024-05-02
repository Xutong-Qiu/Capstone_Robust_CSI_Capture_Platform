#include <libssh/libssh.h>
#include <iostream>
#include <cstdlib>

int execute_ssh_command(const char* host, const char* user, const char* password, const char* command) {
    ssh_session my_ssh_session = ssh_new();
    if (my_ssh_session == nullptr) return -1;

    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, host);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, user);

    int rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK) {
        std::cerr << "Error connecting to host: " << ssh_get_error(my_ssh_session) << std::endl;
        ssh_free(my_ssh_session);
        return -1;
    }

    rc = ssh_userauth_password(my_ssh_session, nullptr, password);
    if (rc != SSH_AUTH_SUCCESS) {
        std::cerr << "Authentication failed: " << ssh_get_error(my_ssh_session) << std::endl;
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return -1;
    }

    ssh_channel channel = ssh_channel_new(my_ssh_session);
    if (channel == nullptr) {
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return -1;
    }

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        ssh_channel_free(channel);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return -1;
    }

    rc = ssh_channel_request_exec(channel, command);
    if (rc != SSH_OK) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return -1;
    }

    char buffer[256];
    int nbytes;
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        std::cout.write(buffer, nbytes);
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);

    return 0;
}

int main() {
    const char* host = "192.168.51.98";
    const char* user = "pi";
    const char* password = "12345678";
    const char* command = "ls";

    if (execute_ssh_command(host, user, password, command) != 0) {
        std::cerr << "SSH command execution failed." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
