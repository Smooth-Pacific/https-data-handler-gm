#ifndef PROCESSOR_HEADER
#define PROCESSOR_HEADER

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
//#include <execution>

#include "data.hpp"


class Processor {
private: // Member variables
    Data_Holder* data;
    std::string destination;

public: // Constructor/Destructor
    Processor(Data_Holder* data, std::string destination);
    ~Processor();

private: // Private Member Functions
    void save_to_xml(std::string xml_string, std::string file_name);


private: // Querries
    void largest_transactions_10();
    void transactions_by_state();
    void insufficient_balance();

public: // Public Member Functions
    void run_querries();
};

#endif