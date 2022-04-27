#include "processor.hpp"


// =========================
//        Processor
// =========================

// Constructor
Processor::Processor(Data_Holder* data, std::string destination): data(data), destination(destination) { }

// Destructor
Processor::~Processor() { }



// Private Member Functions
void Processor::save_to_xml(std::string xml_string, std::string file_name) {
    std::string target = destination + "/" + file_name;
    std::ofstream file(target);
    file << xml_string;
    file.close();
}



// Public Member Functions
void Processor::run_querries() {
    largest_transactions_10();
}


// =========================
//         Querries
// =========================

// Top 10 Largest Transactions
bool cmp(std::pair<unsigned long long, Transaction*> const& a, 
         std::pair<unsigned long long, Transaction*> const& b) {
    return a.second->get_amount() > b.second->get_amount(); 
}

void Processor::largest_transactions_10() {
    std::string file_name = "largest_transactions_10.xml";
    std::vector<std::pair<unsigned long long, Transaction*>> vec;
    for(auto& it : data->transactions)
        vec.push_back(it);

    sort(vec.begin(), vec.end(), cmp);       


    std::string xml_string = "<top_transactions>";
    for(int i = 0; i < 10; i++) {
        xml_string += "<transaction id=" + std::to_string(vec.at(i).first) + ">";
        xml_string += "<amount>$" 
            + std::to_string(vec.at(i).second->get_amount()).substr(0, std::to_string(vec.at(i).second->get_amount()).find(".") + 2 + 1)
            + "</amount>";
        xml_string += "</transaction>";
    }
    xml_string += "</top_transactions>";

    save_to_xml(xml_string, file_name);
}