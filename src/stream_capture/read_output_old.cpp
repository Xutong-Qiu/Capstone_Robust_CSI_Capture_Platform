#include <libssh/libssh.h>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <future>
#include <fstream>

int execute_command(ssh_channel channel, const char* command){
    return ssh_channel_request_exec(channel, command);
}
int main() {
    ssh_session pi1_session, pi2_session;
    int verbosity = SSH_LOG_PROTOCOL;
    int port = 22;
    int pi1_rc, pi2_rc;

    // Initialize session
    pi1_session = ssh_new();
    if (pi1_session == nullptr) {
        std::cerr << "Error creating SSH session." << std::endl;
        exit(-1);
    }
    pi2_session = ssh_new();
    if (pi2_session == nullptr) {
        std::cerr << "Error creating SSH session." << std::endl;
        exit(-1);
    }

    // Set options
    ssh_options_set(pi1_session, SSH_OPTIONS_HOST, "192.168.51.98");
    ssh_options_set(pi1_session, SSH_OPTIONS_USER, "pi");
    ssh_options_set(pi1_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(pi1_session, SSH_OPTIONS_PORT, &port);

    ssh_options_set(pi2_session, SSH_OPTIONS_HOST, "192.168.51.125");
    ssh_options_set(pi2_session, SSH_OPTIONS_USER, "pi");
    ssh_options_set(pi2_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(pi2_session, SSH_OPTIONS_PORT, &port);

    // Connect to server
    pi1_rc = ssh_connect(pi1_session);
    pi2_rc = ssh_connect(pi2_session);
    if (pi1_rc != SSH_OK) {
        std::cerr << "Error connecting: " << ssh_get_error(pi1_session) << std::endl;
        ssh_free(pi1_session);
        exit(-1);
    }
    if (pi2_rc != SSH_OK) {
        std::cerr << "Error connecting: " << ssh_get_error(pi2_session) << std::endl;
        ssh_free(pi2_session);
        exit(-1);
    }

    // Authenticate
    pi1_rc = ssh_userauth_password(pi1_session, nullptr, "1234");
    if (pi1_rc != SSH_AUTH_SUCCESS) {
        std::cerr << "Authentication failed: " << ssh_get_error(pi1_session) << std::endl;
        ssh_disconnect(pi1_session);
        ssh_free(pi1_session);
        exit(-1);
    }

    pi2_rc = ssh_userauth_password(pi2_session, nullptr, "neslrocks!");
    if (pi2_rc != SSH_AUTH_SUCCESS) {
        std::cerr << "Authentication failed: " << ssh_get_error(pi2_session) << std::endl;
        ssh_disconnect(pi2_session);
        ssh_free(pi2_session);
        exit(-1);
    }
    // Execute a command
    ssh_channel pi1_channel = ssh_channel_new(pi1_session);
    if (pi1_channel == nullptr) return SSH_ERROR;

    ssh_channel pi2_channel = ssh_channel_new(pi2_session);
    if (pi2_channel == nullptr) return SSH_ERROR;

    pi1_rc = ssh_channel_open_session(pi1_channel);
    if (pi1_rc != SSH_OK) {
        ssh_channel_free(pi1_channel);
        return pi1_rc;
    }

    pi2_rc = ssh_channel_open_session(pi2_channel);
    if (pi2_rc != SSH_OK) {
        ssh_channel_free(pi2_channel);
        return pi2_rc;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::future<int> pi1_rc_future = std::async(execute_command, pi1_channel, "cat output.txt");
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> latency = end - start;
    std::cout << "Command execution request latency: " << latency.count() << " ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    std::future<int> pi2_rc_future = std::async(execute_command, pi2_channel, "cat output.txt");
    end = std::chrono::high_resolution_clock::now();

    latency = end - start;
    std::cout << "Command execution request latency: " << latency.count() << " ms" << std::endl;

    pi1_rc = pi1_rc_future.get();
    if (pi1_rc != SSH_OK) {
        ssh_channel_close(pi1_channel);
        ssh_channel_free(pi1_channel);
        return pi1_rc;
    }

    //get output
    std::ofstream pi1_outfile("pi1_output.txt");

    char buffer[256];
    int nbytes;

    nbytes = ssh_channel_read(pi1_channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        // Write buffer to file instead of stdout
        pi1_outfile.write(buffer, nbytes);
        nbytes = ssh_channel_read(pi1_channel, buffer, sizeof(buffer), 0);
    }
    pi1_outfile.close();
    
    std::ofstream pi2_outfile("pi2_output.txt");

    nbytes = ssh_channel_read(pi2_channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        // Write buffer to file instead of stdout
        pi2_outfile.write(buffer, nbytes);
        nbytes = ssh_channel_read(pi2_channel, buffer, sizeof(buffer), 0);
    }
    pi2_outfile.close();

    // Cleanup
    ssh_channel_send_eof(pi1_channel);
    ssh_channel_close(pi1_channel);
    ssh_channel_free(pi1_channel);

    ssh_disconnect(pi1_session);
    ssh_free(pi1_session);

    ssh_channel_send_eof(pi2_channel);
    ssh_channel_close(pi2_channel);
    ssh_channel_free(pi2_channel);

    ssh_disconnect(pi2_session);
    ssh_free(pi2_session);
}
