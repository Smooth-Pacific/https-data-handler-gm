#ifndef GENERATOR_HEADER
#define GENERATOR_HEADER

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include "data.hpp"
#include "string.h"
#include <sw/redis++/redis++.h>

class Generator {
private: // Member variables
    Data_Holder* data;


public: // Constructor/Destructor
    Generator(Data_Holder* data);
    ~Generator();

private: // Private Member Functions
    void load_file(std::string file_name, std::vector<std::string> &lines);
    void parse_states(std::vector<std::string> &raw_states);

    void write_user_redis(std::string address);
    void write_merchant_redis(std::string address);
    void write_state_redis(std::string address);
    void write_transaction_redis(std::string address);

public: // Public Member Functions
    void run(int chunk_size, std::string file_name);
    void save_to_redis();
};

#endif