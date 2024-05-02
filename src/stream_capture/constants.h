#include <libssh/libssh.h>
const int num_host = 8;
const char* addrs[] = {"192.168.51.101", "192.168.51.195", "192.168.51.196", "192.168.51.85", "192.168.51.148", "192.168.51.167", "192.168.51.161", "192.168.51.69"};
const char* usernames[] = {"pi", "pi", "pi", "pi", "pi", "pi", "pi", "pi"};
const char* keys[] = {"12345678", "12345678", "12345678", "12345678", "12345678", "12345678", "12345678", "12345678"};
const char* output_dir = "./outputs";
