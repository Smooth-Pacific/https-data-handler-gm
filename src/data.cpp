#include "data.hpp"

// =========================
//          User
// =========================

// Constructor
User::User(unsigned int id, std::string fname, std::string lname,
    std::map<int,std::string>* cards):
    id(id), fname(fname), lname(lname), cards(cards) {
        email = fname + "." + lname + "@smoothceeplusplus.com";
    }

// Destructor
User::~User() {
    if(cards != nullptr)
        delete cards;
}


// Accessor Functions
unsigned int User::get_id() { return id; }
std::string  User::get_fname() { return fname; }
std::string  User::get_lname() { return lname; }
std::string  User::get_email() { return email; }
std::map<int,std::string>* User::get_cards() {return cards; }


// Member Functions
bool User::add_card(int card_id, std::string card) {
    auto it = cards->find(card_id);
    if(it != cards->end())
        return false;
    else
        cards->insert(std::make_pair(card_id, card));
    return true;
}




// =========================
//         Merchant
// =========================

// Constructor
Merchant::Merchant(unsigned long id, std::string name, std::string city,
    std::string state, int zip_code, int mcc): id(id), name(name),
    city(city), state(state), zip_code(zip_code), mcc(mcc) {
        if(mcc < 1500)
            merchant_type = "Agricultural Services";
        else if(mcc < 3000)
            merchant_type = "Contracted Services";
        else if(mcc < 4800)
            merchant_type = "Transportation Services";
        else if(mcc < 5000)
            merchant_type = "Utility Services";
        else if(mcc < 5600)
            merchant_type = "Retail Outlet Services";
        else if(mcc < 5700)
            merchant_type = "Clothing Stores";
        else if(mcc < 7300)
            merchant_type = "Miscellaneous Stores";
        else if(mcc < 8000)
            merchant_type = "Business Services";
        else if(mcc < 9000)
            merchant_type = "Professional Services and Membership Organizations";
        else if(mcc < 10000)
            merchant_type = "Government Services";
        else
            merchant_type = "Unspecified Merchant Type";
    }

// Destructor
Merchant::~Merchant() { }


// Accessor functions
unsigned long Merchant::get_id() { return id; }
std::string   Merchant::get_name() { return name; }
std::string   Merchant::get_city() { return city; }
std::string   Merchant::get_state() { return state; }
int           Merchant::get_zip_code() { return zip_code; }
int           Merchant::get_mcc() { return mcc; }
std::string   Merchant::get_merchant_type() { return merchant_type; }



// =========================
//          State
// =========================

// Constructor
State::State(std::string name, std::string capital, std::string abbr,
    std::map<int,int>* zip_codes, std::map<std::string,int>* cities):
    name(name), capital(capital), abbr(abbr), zip_codes(zip_codes),
    cities(cities) { }

// Destructor
State::~State() {
    if(cities != nullptr)
        delete cities;

    if(zip_codes != nullptr)
        delete zip_codes;
}


// Accessor functions
std::string         State::get_name() { return name; }
std::string         State::get_capital() { return capital; }
std::string         State::get_abbr() { return abbr; }
std::map<int,int>*  State::get_zip_codes() { return zip_codes; }
std::map<std::string,int>* State::get_cities() { return cities; }


// Member Functions
void State::add_zip(int zip) {
    auto it = zip_codes->find(zip);
    if(it != zip_codes->end())
        it->second++;
    else
        zip_codes->insert(std::make_pair(zip, 1));
}

void State::add_city(std::string city) {
    auto it = cities->find(city);
    if(it != cities->end())
        it->second++;
    else
        cities->insert(std::make_pair(city, 1));
}




// =========================
//        Transaction
// =========================

// Constructor
Transaction::Transaction(unsigned long long id, User* user, int card, int year, int month, 
                         int day, std::string time, float amount, std::string use_chip,
                         Merchant* merchant, std::string errors, bool is_fraud):
                         id(id), user(user), card(card), year(year), month(month), day(day), 
                         time(time), amount(amount), use_chip(use_chip), merchant(merchant),
                         errors(errors), is_fraud(is_fraud) { }

// Destructor
Transaction::~Transaction() { }


// Member functions
User*       Transaction::get_user() { return user; }
int         Transaction::get_card() { return card; }
int         Transaction::get_year() { return year; }
int         Transaction::get_month() { return month; }
int         Transaction::get_day() { return day; }
std::string Transaction::get_time() { return time; }
float        Transaction::get_amount() { return amount; }
std::string Transaction::get_use_chip() { return use_chip; }
Merchant*   Transaction::get_merchant() { return merchant; }
std::string Transaction::get_errors() { return errors; }
bool        Transaction::get_is_fraud() { return is_fraud; }




// =========================
//        Transaction
// =========================

// Destructor
Data_Holder::~Data_Holder() {
    for(auto state: states) {
        delete state.second;
    }

    for(auto user: users) {
        delete user.second;
    }

    for(auto merchant: merchants) {
        delete merchant.second;
    }

    for(auto transaction: transactions) {
        delete transaction.second;
    }
}