#include "generator.hpp"
#include <algorithm>
#include <random>
#include <stdlib.h>
#include <time.h>

using sw::redis::Redis;
// =========================
//        Generator
// =========================

// Constructor
Generator::Generator(Data_Holder* data): data(data) { }

// Destructor
Generator::~Generator() { }


// Member Functions

// main call for the generator
void Generator::run(int chunk_size, std::string file_name) {

    std::vector<std::string> raw_transactions;
    std::vector<std::string> raw_states;
    std::vector<std::string> rand_fnames;
    std::vector<std::string> rand_lnames;
    std::vector<std::string> rand_merchants;
    std::vector<std::string> rand_cards;
    std::vector<std::thread> threads(5);

    //load prep files
    threads[0] = std::thread(&Generator::load_file, this, "resources/states.csv", std::ref(raw_states));
    threads[1] = std::thread(&Generator::load_file, this, "resources/random_fnames.txt", std::ref(rand_fnames));
    threads[2] = std::thread(&Generator::load_file, this, "resources/random_lnames.txt", std::ref(rand_lnames));
    threads[3] = std::thread(&Generator::load_file, this, "resources/random_business_names.txt", std::ref(rand_merchants));
    threads[4] = std::thread(&Generator::load_file, this, "resources/random_card_numbers.txt", std::ref(rand_cards));


    for(auto& t: threads)
        t.join();
    threads.clear();

    //parse states
    parse_states(std::ref(raw_states));


    std::ifstream file;
    file.open(file_name);

    std::string transaction;
    std::getline(file,transaction); // Eats header line


    unsigned long long i = 0;
    std::string s_user_id;
    std::string s_card_id;
    std::string s_year;
    std::string s_month;
    std::string s_day;
    std::string s_time;
    std::string s_amount;
    std::string s_use_chip;
    std::string s_merchant_id;
    std::string city;
    std::string state;
    std::string s_zip;
    std::string s_mcc;
    std::string errors;
    std::string s_is_fraud;

    unsigned int  user_id;
    unsigned int  card_id;
    int           year;
    int           month;
    int           day;
    float         amount;
    unsigned long merchant_id;
    int zip;
    int mcc;
    bool is_fraud;

    Merchant* mer;
    User* use;

    //raw_transactions.erase(raw_transactions.begin());
    srand(time(0));

    while(std::getline(file, transaction)) {
        i++; 
        
        // Parse out transaction
        std::stringstream ss(transaction);
        getline(ss, s_user_id, ',');
        getline(ss, s_card_id, ',');
        getline(ss, s_year, ',');       
        getline(ss, s_month, ',');       
        getline(ss, s_day, ',');       
        getline(ss, s_time, ',');       
        getline(ss, s_amount, ',');       
        getline(ss, s_use_chip, ',');       
        getline(ss, s_merchant_id, ',');       
        getline(ss, city, ',');       
        getline(ss, state, ',');       
        getline(ss, s_zip, ',');       
        getline(ss, s_mcc, ',');       
        getline(ss, errors, ',');
        if(errors != "") {
            errors = errors.erase(0,1);
            getline(ss, s_is_fraud, ','); //eats extra comma
        }
        getline(ss, s_is_fraud, ','); 

        if(s_is_fraud != "No")
            is_fraud = true;
        else
            is_fraud = false;

        try {
        if(s_user_id != "")
            user_id = std::stoi(s_user_id);
        else
            break;

        if(s_card_id != "")
            card_id = std::stoi(s_card_id);
        else
            card_id = 0;

        if(s_merchant_id != "")
            merchant_id = std::stol(s_merchant_id);
        else
            break;

        if(s_year != "")
            year = std::stoi(s_year);
        else
            break;

        if(s_month != "")
            month = std::stoi(s_month);
        else
            break;

        if(s_day != "")
            day = std::stoi(s_day);
        else
            break;            

        if(s_zip != "")
            zip = std::stoi(s_zip);
        else
            zip = 0;

        if(s_mcc != "")
            mcc = std::stoi(s_mcc);
        else 
            mcc = 100000;

        if(state == "")
            state = "None";


        // get Amount out
        amount = std::stof(s_amount.erase(0,1));
        

        // User
        auto u_it = data->users.find(user_id);
        if(u_it != data->users.end()) {
            if(u_it->second->add_card(card_id, rand_cards.back()))
                rand_cards.pop_back();
        }

        else {
            int first = rand() % rand_fnames.size();
            int last = rand() % rand_lnames.size();
            auto cards = new std::map<int,std::string>();
            cards->insert(std::make_pair(card_id, rand_cards.back()));

            use = new User(user_id, rand_fnames.at(first), rand_lnames.at(last), cards);

            data->users.insert(std::make_pair(user_id, use));

            rand_cards.pop_back();
        }



        // State
        auto s_it = data->states.find(state);
        if(s_it != data->states.end()) {
            s_it->second->add_city(city);
            s_it->second->add_zip(zip);
        }


        // Merchant
        auto b_it = data->merchants.find(merchant_id);
        if(b_it == data->merchants.end()) {
            mer = new Merchant(merchant_id, rand_merchants.back(), city, state, zip, mcc);
            data->merchants.insert(std::make_pair(merchant_id, mer));
            rand_merchants.pop_back();
        }

        // Transaction
        Transaction* trans = new Transaction(i, use, card_id, year, month, day, s_time,
                                             amount, s_use_chip, mer, errors, is_fraud);
        data->transactions.insert(std::make_pair(i, trans));

        //std::cout << "success - " + std::to_string(i) << std::endl;
        }

        catch(...) {
            //std::cout << "error - " + std::to_string(i) << std::endl;
            //raw_transactions.erase(raw_transactions.begin());
            continue;
        }
    }
}


// stores each line of a file in a vector
void Generator::load_file(std::string file_name, std::vector<std::string> &lines) {
    std::ifstream file;
    file.open(file_name);

    if(!file)
        throw std::invalid_argument("Failed to open file \'" + file_name + "\'");

    std::string line;
    while(std::getline(file, line)) {
        lines.push_back(line);
    }
}

void Generator::parse_states(std::vector<std::string> &raw_states) {
    // Clear title row
    raw_states.erase(raw_states.begin());

    // Get State info
    std::string name;
    std::string capital;
    std::string abbr;

    for(auto state: raw_states) {
        std::stringstream ss(state);
        getline(ss, name, ','); //eats the id column
        getline(ss, name, ',');
        getline(ss, abbr, ',');
        getline(ss, capital);

        State* s = new State(name, capital, abbr, new std::map<int,int>(),
            new std::map<std::string,int>());

        data->states.insert(std::make_pair(abbr, s));
    }
}


// Writes objects to Redis server
void Generator::save_to_redis() {
    std::string redis_address = "tcp://127.0.0.1:6379";
    
    write_user_redis(redis_address);
    write_merchant_redis(redis_address);
    write_state_redis(redis_address);
    write_transaction_redis(redis_address);
}


void Generator::write_user_redis(std::string address) {
    auto redis = Redis(address);

    for(auto p: data->users) {
        std::string designation = "user-" + std::to_string(p.first);
    
        std::unordered_map<std::string, std::string> map = {
            {"fname", p.second->get_fname()},
            {"lname", p.second->get_lname()},
            {"email", p.second->get_email()}
        };

        for(auto p2: *p.second->get_cards()) {
            map.insert(std::make_pair(("cards[" + std::to_string(p2.first) + "]"), p2.second));
        }

        redis.hmset(designation, map.begin(), map.end());
    }
}

void Generator::write_merchant_redis(std::string address) {
    auto redis = Redis(address);

    for(auto m: data->merchants) {
        std::string designation = "merchant-" + std::to_string(m.first);

        std::unordered_map<std::string, std::string> map = {
            {"name",  m.second->get_name()},
            {"city", m.second->get_city()},
            {"state", m.second->get_state()},
            {"zipcode", std::to_string(m.second->get_zip_code())},
            {"mcc", std::to_string(m.second->get_mcc())},
            {"merchant_type", m.second->get_merchant_type()}
        };

        redis.hmset(designation, map.begin(), map.end());
    }
}

void Generator::write_state_redis(std::string address) {
    auto redis = Redis(address);
    unsigned long i = 0;

    for(auto s: data->states) {
        std::string designation = "state-" + s.first;

        std::unordered_map<std::string, std::string> map = {
            {"name",  s.second->get_name()},
            {"capital", s.second->get_capital()},
            {"abbr", s.second->get_abbr()}
        };

        i = 0;
        for(auto zip_codes: *s.second->get_zip_codes()) {
            map.insert(std::make_pair("zip_codes[" + std::to_string(i) + "]", std::to_string(zip_codes.first)));
            i++;
        }

        i = 0;
        for(auto city: *s.second->get_cities()) {
            map.insert(std::make_pair("cards[" + std::to_string(i) + "]", city.first));
            i++;
        }

        redis.hmset(designation, map.begin(), map.end());       
    }
}


void Generator::write_transaction_redis(std::string address) {
    auto redis = Redis(address);
    std::string is_fraud;

    for(auto t: data->transactions) {
        std::string designation = "transaction-" + std::to_string(t.first);
        is_fraud = t.second->get_is_fraud() ? "true" : "false";

        std::unordered_map<std::string, std::string> m = {
            {"user", std::to_string(t.second->get_user()->get_id())},
            {"card", std::to_string(t.second->get_card())},
            {"year", std::to_string(t.second->get_year())},
            {"month", std::to_string(t.second->get_month())},
            {"day", std::to_string(t.second->get_day())},
            {"time", t.second->get_time()},
            {"amount", std::to_string(t.second->get_amount())},
            {"use_chip", t.second->get_use_chip()},
            {"merchant", std::to_string(t.second->get_merchant()->get_id())},
            {"errors", t.second->get_errors()},
            {"is_fraud", is_fraud}
        };

        redis.hmset(designation, m.begin(), m.end());
    }
}