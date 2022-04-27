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


// count of transactions by state
void Processor::transactions_by_state() {
    std::string file_name = "transactions_by_state.xml";
    std::map<std::string, unsigned long long> t;

    for(auto p: data->transactions) {
        if(p.second->get_is_fraud())    //skip fraud
            continue;

        auto iter = t.find(p.second->get_merchant()->get_state());

        if(iter != t.end())
            iter->second++;
        else
            t.insert(std::make_pair(p.second->get_merchant()->get_state(), 1));
    }


    std::string xml_string = "<transactions>";
    for(auto p: t) {
        xml_string += "<state id=" + p.first + ">";
        xml_string += "<count>" + std::to_string(p.second) + "</count>";
        xml_string += "</state>";
    }
    xml_string += "</transactions>";

    save_to_xml(xml_string, file_name);
}


// insufficient balance error count
void Processor::insufficient_balance() {
    std::string file_name = "insufficient_balance.xml";
    std::map<User*, bool> problem_users;

    for(auto p: data->transactions) {
        if(p.second->get_errors() != "Insufficient Balance")
            continue;

        auto iter = problem_users.find(p.second->get_user());
        if(iter == problem_users.end())
            problem_users.insert(std::make_pair(p.second->get_user(), true));
    }


    std::string xml_string = "<insufficient_balance>";
    xml_string += "<total_user_count>" + std::to_string(data->users.size()) + "</total_user_count>";
    xml_string += "<problem_user_count>" + std::to_string(problem_users.size()) + "</problem_user_count>";
    std::string percent = std::to_string(100. * problem_users.size() / data->users.size());
    xml_string += "<percentage_of_users>" + percent.substr(0, percent.find(".") + 2 + 1) + "%</percentage_of_users>";
    xml_string += "</insufficient_balance>";

    save_to_xml(xml_string, file_name);
}