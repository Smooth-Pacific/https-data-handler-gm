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
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << xml_string;
    file.close();
}



// Public Member Functions
void Processor::run_querries() {
    largest_transactions_10();
    transactions_by_state();
    insufficient_balance();
    most_transactions_by_zip_5();
    largest_transactions_by_merchant_5();
    percentage_of_fraud_by_year();
    //transaction_types();
    bottom_online_transaction_months_5();
    top_cities_with_online();
    all_deposits();
    most_transactions_by_city_5();
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
        xml_string += "<transaction id=\"" + std::to_string(vec.at(i).first) + "\">";
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
    std::unordered_map<std::string, unsigned long long> t;

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
        xml_string += "<state id=\"" + p.first + "\">";
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


// top 5 zipcodes with most transactions
void Processor::most_transactions_by_zip_5() {
    std::string file_name = "most_transactions_by_zip_5.xml";
    std::map<int, unsigned long long> t;

    for(auto p: data->transactions) {
        if(p.second->get_merchant()->get_zip_code() == 0)    //skip non-zip_code entries
            continue;

        auto iter = t.find(p.second->get_merchant()->get_zip_code());

        if(iter != t.end())
            iter->second++;
        else
            t.insert(std::make_pair(p.second->get_merchant()->get_zip_code(), 1));
    }


    std::vector<std::pair<int, unsigned long long>> vec;
    for(auto p: t)
        vec.push_back(p);

    sort(vec.begin(), vec.end(), [](auto const& a, auto const& b) -> bool {
        return a.second > b.second;
    });


    std::string xml_string = "<transactions>";
    for(int i = 0; i < 5; i++) {
        xml_string += "<zip_code id=\"" + std::to_string(vec.at(i).first) + "\">";
        xml_string += "<count>" + std::to_string(vec.at(i).second) + "</count>";
        xml_string += "</zip_code>";
    }
    xml_string += "</transactions>";

    save_to_xml(xml_string, file_name);
}


// Top 5 transactions by merchant
void Processor::largest_transactions_by_merchant_5() {
    std::string file_name = "largest_transactions_by_merchant_5.xml";
    std::vector<std::pair<unsigned long long, Transaction*>> vec;

    for(auto p: data->transactions)
        vec.push_back(p);


    sort(vec.begin(), vec.end(), cmp);   


    std::string xml_string = "<transactions>";
    for(int i = 0; i < 5; i++) {
        xml_string += "<transaction id=\"" + std::to_string(vec.at(i).first) + "\">";
        xml_string += "<merchant>" + std::to_string(vec.at(i).second->get_merchant()->get_id()) + "</merchant>";
        xml_string += "</transaction>";
    }
    xml_string += "</transactions>";

    save_to_xml(xml_string, file_name);
}


// Fraud Percentage
void Processor::percentage_of_fraud_by_year() {
    std::string file_name = "percentage_of_fraud_by_year.xml";
    std::map<int, std::pair<unsigned long, unsigned long>> year_counts;
    int fraud_value;

    for(auto t: data->transactions) {
        fraud_value = t.second->get_is_fraud() ? 1 : 0;
        auto iter = year_counts.find(t.second->get_year());

        if(iter != year_counts.end()) {
            iter->second.first += 1;
            iter->second.second += fraud_value;
        }

        else {
            year_counts.insert(std::make_pair(t.second->get_year(), std::make_pair(1,fraud_value)));
        }
    }


    //write result to xml string
    std::string xml_string = "<fraudulent_transactions>";

    for(auto pair: year_counts) {
        float percent = 100. * pair.second.second / pair.second.first;

        xml_string += "<year id=\"" + std::to_string(pair.first) + "\">";
        xml_string += std::to_string(percent) + "%";
        xml_string += "</year>";
    }
    
    xml_string += "</fraudulent_transactions>";


    //save result
    save_to_xml(xml_string, file_name);
}



// Bottom 5 months of online transaction
void Processor::bottom_online_transaction_months_5() {
    std::string file_name = "bottom_online_transaction_months.xml";
    std::map<std::string, int> month_counts;
    std::vector<std::pair<std::string,int>> vec;

    for(auto t: data->transactions) {
        if(t.second->get_use_chip() != "Online Transaction")
            continue;

        std::string date = std::to_string(t.second->get_month()) + "/" + std::to_string(t.second->get_year());
        auto iter = month_counts.find(date);

        if(iter != month_counts.end())
            iter->second++;
        else
            month_counts.insert(std::make_pair(date, 1));
    }


    copy(std::execution::par, month_counts.begin(), month_counts.end(), back_inserter(vec));
    sort(std::execution::par, vec.begin(), vec.end(), 
        [](std::pair<std::string,int> const& a, std::pair<std::string,int> const& b) -> bool {
            return a.second < b.second;
        });

    //write result to xml string
    std::string xml_string = "<lowest_online_transactions>";

    for(int i = 0; i < 5; i++) {
        xml_string += "<date id=\"" + vec.at(i).first + "\">";
        xml_string += "<transaction_count>" + std::to_string(vec.at(i).second) + "</transaction_count>";
        xml_string += "</date>";
    }
    
    xml_string += "</lowest_online_transactions>";


    //save result
    save_to_xml(xml_string, file_name);
}


// containing the ten cities in which the largest number of merchants had at least one online transaction
// Top 10 cities with 
void Processor::top_cities_with_online() {
    std::string file_name = "top_cities_with_online.xml";
    std::map<Merchant*, int> merchants;
    std::map<std::string, int> city_counts;
    std::vector<std::pair<std::string,int>> vec;

    for(auto t: data->transactions) {
        if(t.second->get_use_chip() != "Online Transaction") continue;

        auto iter = merchants.find(t.second->get_merchant());
        if(iter == merchants.end())
            merchants.insert(std::make_pair(t.second->get_merchant(), 1));
    }

    for(auto t: data->transactions) {
        if(t.second->get_use_chip() == "Online Transaction") continue;

        auto iter = merchants.find(t.second->get_merchant());
        if(iter == merchants.end()) continue;

        auto iter2 = city_counts.find(t.second->get_merchant()->get_city());
        if(iter2 != city_counts.end())
            iter2->second++;
        else
            city_counts.insert(std::make_pair(t.second->get_merchant()->get_city(),1));
    }

    copy(std::execution::par, city_counts.begin(), city_counts.end(), back_inserter(vec));
    sort(std::execution::par, vec.begin(), vec.end(), 
        [](std::pair<std::string,int> const& a, std::pair<std::string,int> const& b) -> bool {
            return a.second > b.second;
        });


    //write result to xml string
    std::string xml_string = "<top_merchant_count_with_online_transactions>";

    for(int i = 1; i < 11; i++) {
        xml_string += "<city id=\"" + vec.at(i).first + "\">";
        xml_string += "<merchant_count>" + std::to_string(vec.at(i).second) + "</merchant_count>";
        xml_string += "</city>";
    }
    
    xml_string += "</top_merchant_count_with_online_transactions>";


    //save result
    save_to_xml(xml_string, file_name);
}




// All deposits by users 
void Processor::all_deposits() {
    std::string file_name = "all_deposits.xml";
    std::map<unsigned int, std::vector<std::pair<unsigned long long, float>>> deposits;

    for(auto t: data->transactions) {
        if(t.second->get_amount() >= 0) continue;

        auto iter = deposits.find(t.second->get_user()->get_id());
        if(iter != deposits.end())
            iter->second.push_back(std::make_pair(t.first, t.second->get_amount()));
        else
            deposits.insert(std::make_pair(t.second->get_user()->get_id(), std::vector<std::pair<unsigned long long, float>>{std::make_pair(t.first, t.second->get_amount())}));
    }


    //write result to xml string
    std::string xml_string = "<deposit_transactions>";

    for(auto d: deposits) {
        xml_string += "<user id=\"" + std::to_string(d.first) + "\">";

        for(auto t: d.second) {
            xml_string += "<transaction id=\"" + std::to_string(t.first) + "\">";
            xml_string += "<amount>$" + std::to_string(t.second).substr(0, std::to_string(t.second).find(".") + 2 + 1) + "</amount>";
            xml_string += "</transaction>";
        }
        xml_string += "</user>";
    }
    
    xml_string += "</deposit_transactions>";


    //save result
    save_to_xml(xml_string, file_name);
}



// Top 5 cities with most transactions
void Processor::most_transactions_by_city_5() {
    std::string file_name = "most_transactions_by_city.xml";
    std::vector<std::pair<std::string,int>> city_counts;

    for(auto s: data->states) {
        auto cities = s.second->get_cities();
        copy(std::execution::par, cities->begin(), cities->end(), back_inserter(city_counts));
    }

    sort(std::execution::par, city_counts.begin(), city_counts.end(),
        [](std::pair<std::string,int> const& a, std::pair<std::string,int> const& b) -> bool {
            return a.second > b.second;
        });


    //write result to xml string
    std::string xml_string = "<largest_transaction_count>";

    for(int i = 0; i < 5; i++) {
        xml_string += "<city id=\"" + city_counts.at(i).first + "\">";
        xml_string += "<count>" + std::to_string(city_counts.at(i).second) + "</count>";
        xml_string += "</city>";
    }
    
    xml_string += "</largest_transaction_count>";


    //save result
    save_to_xml(xml_string, file_name);
}


// XML Formatting
// XML Signing
// Look into better endpoint management
// investigation, design(back+UX), coding(implementation), testing
// More unit test coverage