#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <future>
#include <fstream>
#include <string>
#include "constants.h"

int execute_command(ssh_channel channel, const char* command){
    return ssh_channel_request_exec(channel, command);
}

void init_sessions(ssh_session* sessions, size_t size){
    for(size_t i = 0; i < size; ++i){
        sessions[i] = ssh_new();
        if (sessions[i] == nullptr) {
            std::cerr << "Error creating SSH session." << std::endl;
            exit(-1);
        }
    }
}

void set_options(ssh_session* sessions, size_t size, const char** addrs, const char** usernames){
    for(size_t i = 0; i < size; ++i){
        ssh_options_set(sessions[i], SSH_OPTIONS_HOST, addrs[i]);
        ssh_options_set(sessions[i], SSH_OPTIONS_USER, usernames[i]);
    }
}

void connect_and_authenticate(ssh_session* sessions, size_t size, int* rc, const char** keys){
    for(size_t i = 0; i< size; ++i){
        rc[i] = ssh_connect(sessions[i]);
        if (rc[i] != SSH_OK) {
            std::cerr << "Error connecting: " << ssh_get_error(sessions[i]) << std::endl;
            ssh_free(sessions[i]);
            exit(-1);
        }
        rc[i] = ssh_userauth_password(sessions[i], nullptr, keys[i]);
        if (rc[i] != SSH_AUTH_SUCCESS) {
            std::cerr << "Authentication failed: " << ssh_get_error(sessions[i]) << std::endl;
            ssh_disconnect(sessions[i]);
            ssh_free(sessions[i]);
            exit(-1);
        }
    }
}

void free_resources(ssh_session* sessions,  ssh_channel* channels, size_t size){
    for(size_t i = 0; i< size; ++i){
        ssh_channel_send_eof(channels[i]);
        ssh_channel_close(channels[i]);
        ssh_channel_free(channels[i]);

        ssh_disconnect(sessions[i]);
        ssh_free(sessions[i]);
    }
}
int main() {
    ssh_session sessions[num_host];

    // Initialize session
    init_sessions(sessions, num_host);

    // Set options
    set_options(sessions, num_host, addrs, usernames);

    // Connect to server
    int rc[num_host];
    connect_and_authenticate(sessions, num_host, rc, keys);

    // Execute a command
    ssh_channel channels[num_host];
    for(size_t i = 0; i< num_host; ++i){
        channels[i] = ssh_channel_new(sessions[i]);
        if (channels[i] == nullptr) return SSH_ERROR;
        rc[i] = ssh_channel_open_session(channels[i]);
        if (rc[i] != SSH_OK) {
            ssh_channel_free(channels[i]);
            return rc[i];
        }
    }
    for(size_t i = 0; i< num_host; ++i){
        // auto start = std::chrono::high_resolution_clock::now();
        std::future<int> pi1_rc_future = std::async(execute_command, channels[i], "cat output.txt");
        // auto end = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double, std::milli> latency = end - start;
        // std::cout << "Command execution request latency: " << latency.count() << " ms" << std::endl;
    }
    //get output
    std::ofstream outfiles[num_host];
    for(size_t i = 0; i < num_host; ++i){
        std::string fileName = std::string(output_dir)+ "/output"+ std::to_string(i+1) +".txt";
        outfiles[i].open(fileName);
        char buffer[256];
        int nbytes;
        nbytes = ssh_channel_read(channels[i], buffer, sizeof(buffer), 0);
        while (nbytes > 0) {
            // Write buffer to file instead of stdout
            outfiles[i].write(buffer, nbytes);
            nbytes = ssh_channel_read(channels[i], buffer, sizeof(buffer), 0);
        }
        outfiles[i].close();
    }

    // Cleanup
    free_resources(sessions, channels, num_host);
}
