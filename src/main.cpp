#include <iostream>
#include "../include/generator.hpp"
#include "../include/data.hpp"

int main() {
    //std::string file_name = "resources/card_transaction.v1.csv";
    //std::string file_name = "resources/test.csv";
    std::string file_name = "resources/long_test.csv";

    int chunk_size = 10'000;
    Data_Holder* data = new Data_Holder();

    Generator g(data);
    g.run(chunk_size, file_name);
    g.save_to_redis();

    
    // Cleanup
    delete(data);

    return 0;
}