#ifndef DATA_HEADER
#define DATA_HEADER

#include <map>
#include <string>



class User {
private: // Member variables
    unsigned int id;
    std::string  fname;
    std::string  lname;
    std::string  email;
    std::map<int,std::string>* cards;

public: // Constructor/Destructor
    User(unsigned int id, std::string fname, std::string lname,
         std::map<int,std::string>* cards);
    ~User();

public: // Member Functions
    unsigned int get_id();
    std::string  get_fname();
    std::string  get_lname();
    std::string  get_email();
    std::map<int,std::string>* get_cards();

    bool add_card(int card_id, std::string card);
};


class Merchant {
private: // Member variables
    unsigned long id;
    std::string   name;
    std::string   city;
    std::string   state;
    int           zip_code;
    int           mcc;
    std::string   merchant_type;

public: // Constructor/Destructor
    Merchant(unsigned long id, std::string name, std::string city,
             std::string state, int zip_code, int mcc);
    ~Merchant();

public: // Member Functions
    unsigned long get_id();
    std::string   get_name();
    std::string   get_city();
    std::string   get_state();
    int           get_zip_code();
    int           get_mcc();
    std::string   get_merchant_type();
};


class State {
private: // Member variables
    std::string name;
    std::string capital;
    std::string abbr;
    std::map<int,int>* zip_codes;
    std::map<std::string,int>* cities;

public: // Constructor/Destructor
    State(std::string name, std::string captial, std::string abbr,
          std::map<int,int>* zip_codes, std::map<std::string,int>* cities);
    ~State();

public: // Member Functions
    std::string get_name();
    std::string get_capital();
    std::string get_abbr();
    std::map<int,int>* get_zip_codes();
    std::map<std::string,int>* get_cities();

    void add_zip(int zip);
    void add_city(std::string city);
};


class Transaction {
private: // Member variables
    unsigned long long id;
    User*       user;
    int         card;
    int         year;
    int         month;
    int         day;
    std::string time;
    float amount;
    std::string use_chip;
    Merchant*   merchant;
    std::string errors;
    bool        is_fraud;

public: // Constructor/Destructor
    Transaction(unsigned long long id, User* user, int card, int year,
                int month, int day, std::string time, float amount,
                std::string use_chip, Merchant* merchant, std::string errors,
                bool is_fraud);
    ~Transaction();

public: // Member Functions
    User*       get_user();
    int         get_card();
    int         get_year();
    int         get_month();
    int         get_day();
    std::string get_time();
    float       get_amount();
    std::string get_use_chip();
    Merchant*   get_merchant();
    std::string get_errors();
    bool        get_is_fraud();
};


struct Data_Holder {
    std::map<unsigned int,User*> users;
    std::map<unsigned long,Merchant*> merchants;
    std::map<std::string,State*> states;
    std::map<unsigned long long,Transaction*> transactions;

    ~Data_Holder();
};

#endif