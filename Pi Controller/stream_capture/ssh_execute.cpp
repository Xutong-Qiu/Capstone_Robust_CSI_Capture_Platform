#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <future>
#include <fstream>
#include <sstream>
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
    for(size_t i = 0; i< size; ++i){
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
        //std::cout<<"Connected to "<<keys[i]<<std::endl;
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
    std::cout << "Initializing ssh sessions..." << std::endl;
    init_sessions(sessions, num_host);

    // Set options
    std::cout << "Setting paras..." << std::endl;
    set_options(sessions, num_host, addrs, usernames);

    // Connect to server
    int rc[num_host];
    std::cout << "Connecting..." << std::endl;
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

    std::string input;
    std::cout << "\nConnected!";
    std::cout << "\nWaiting for starting capturing:(any inputs) ";
    std::getline(std::cin, input); // Waiting for input
    
    std::time_t now = std::time(nullptr);
    std::cout << std::ctime(&now);
    std::cout << "\nCapturing and Livestreaming..." << std::endl;

    for(size_t i = 0; i< num_host; ++i){
        // auto start = std::chrono::high_resolution_clock::now();
        std::future<int> pi1_rc_future;// = std::async(execute_command, channels[i], "sudo tcpdump -i wlan0 dst port 5500 -vv -w output.pcap -c 100");
        
        // setup sniffer
        // if((i+1 == 1) || (i+1 == 4)){
        //     pi1_rc_future = std::async(execute_command, channels[i], "sudo ./quick_setup_livestream_5GHz.sh 36 80 1 1 JCAS3 > output.txt 2>&1 ");
        // }else if((i+1 == 2) || (i+1 == 5)){
        //     pi1_rc_future = std::async(execute_command, channels[i], "sudo ./quick_setup_livestream_5GHz.sh 3 20 1 1 JCAS3 > output.txt 2>&1 ");
        // }else{
        //     pi1_rc_future = std::async(execute_command, channels[i], "sudo ./quick_setup_livestream_5GHz.sh 149 80 1 1 JCAS4 > output.txt 2>&1 ");
        // }

        // give permission to execute the script
        // pi1_rc_future = std::async(execute_command, channels[i], "chmod +x ./livestream_autostop.sh");

        // capture and livestream
        const char* command = "sudo ./livestream.sh > output.txt 2>&1";
        // const char* cString = command.c_str();
        // std::cout << cString << std::endl;
        pi1_rc_future = std::async(execute_command, channels[i], command);

        // shutdown all sniffers
        // pi1_rc_future = std::async(execute_command, channels[i], "sudo shutdown now");


        // auto end = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double, std::milli> latency = end - start;
        // std::cout << "Command execution request latency: " << latency.count() << " ms" << std::endl;
    }


    // Cleanup
    free_resources(sessions, channels, num_host);
}
