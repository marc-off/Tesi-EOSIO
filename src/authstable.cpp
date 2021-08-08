#include <eosio/eosio.hpp>
#include <json.hpp>

using json = nlohmann::json;
using namespace eosio;
using namespace std;

class [[eosio::contract("authstable")]] authstable : public eosio::contract {
    
    public:

    /* Our constructor will be responsible for initially setting up the contract. 
    EOSIO contracts extend the contract class. 
    Initialize our parent contract class with the code name of the contract and the receiver. 
    The important parameter here is the code parameter which is the account on the blockchain that 
    the contract is being deployed to. */
    authstable(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds) {}

   /* The above declaration will extract the arguments of the action and 
    create necessary ABI struct descriptions in the generated ABI file. */
    [[eosio::action]]
    void subscribe( string account ) {
        /* We are converting account as lower string, as by eosio syntax rules over accounts */
        std::for_each(account.begin(), account.end(), [](char & c) { c = ::tolower(c); });
        require_auth( name(account) );
        /* To instantiate a table, two parameters are required:
        1. The first parameter specifies the owner of this table.
        2. The second parameter ensures the uniqueness of the table in the scope of this contract.*/
        auths_index records("authadmin"_n, get_first_receiver().value);
        hash<string> hasher; string s1 = account+"owner"; 
        uint64_t key = (uint64_t) hasher(s1); 
        auto iterator = records.find(key);
        check(iterator == records.end(), "Account "+account+" has already subscribed!");
        // The user isn't in the table
        /* Create a record in the table using the multi_index method emplace. 
        This method accepts two arguments, the "payer" of this record who pays the storage usage 
        and a callback function. 
        The callback function for the emplace method must use a lamba function to create a reference. 
        Inside the body assign the row's values with the ones provided to upsert. */
        records.emplace( name(account), [&]( auto& row ) {
            row.key = key;
            row.key_2 = name(account).value;
            row.account = account;
            row.perm = "owner";
            row.parent_perm = "";
            row.attr = "{}";
        });
        s1 = account+"active";  key = (uint64_t) hasher(s1);
        records.emplace( name(account), [&]( auto& row ) {
            row.key = key;
            row.key_2 = name(account).value;
            row.account = account;
            row.perm = "active";
            row.parent_perm = "owner";
            row.attr = "{}";
        });
    }

    /* The above declaration will extract the arguments of the action and 
    create necessary ABI struct descriptions in the generated ABI file. */
    [[eosio::action]]
    void upsert( string account, string perm, string parent_perm, string attr ) {
        /* We are converting account as lower string, as by eosio syntax rules over accounts */
        std::for_each(account.begin(), account.end(), [](char & c) { c = ::tolower(c); });
        require_auth( name(account) );
        /* Same thing above goes for perm */
        std::for_each(perm.begin(), perm.end(), [](char & c) { c = ::tolower(c); });
        check ( (perm!="owner" || perm!="active"), "You cannot upsert the "+perm+" permission record!");
        /* Checking for nonsense inputs */ 
        std::for_each(parent_perm.begin(), parent_perm.end(), [](char & c) { c = ::tolower(c); });
        check ( perm!=parent_perm, "A permission cannot be parent of itself!"); 
        /* Only JSON valid string are accepted as the attribute parameter */
        check( json::accept(attr), "Attributes are not in a valid JSON format!");
        /* To instantiate a table, two parameters are required:
        1. The first parameter specifies the owner of this table.
        2. The second parameter ensures the uniqueness of the table in the scope of this contract. */
        auths_index records("authadmin"_n, get_first_receiver().value);
        hash<string> hasher; string s1 = account+perm; 
        uint64_t key = (uint64_t) hasher(s1); 
        auto iterator = records.begin(); bool exists = false;
        /* A while loop is needed in way to find if the parent_perm exists already as a perm */
        while ( !exists && iterator != records.end() ) {
            /* Loop will stop whether we've reached the end of the iteration or if both 
            the parent permission and the record we've been looking for have been found */
            if ( iterator->perm == parent_perm) exists = true;
            else iterator++;
        }
        check (exists, "The parent permission does not exist in the database!");
        iterator = records.find(key);
        if( iterator == records.end() ) {   // The user isn't in the table
            /* Create a record in the table using the multi_index method emplace. 
            This method accepts two arguments, the "payer" of this record who pays the storage usage 
            and a callback function. 
            The callback function for the emplace method must use a lamba function to create a reference. 
            Inside the body assign the row's values with the ones provided to upsert. */
            records.emplace( name(account), [&]( auto& row ) {
                row.key = key;
                row.key_2 = name(account).value;
                row.account = account;
                row.perm = perm;
                row.parent_perm = parent_perm;
                row.attr = attr;
            });
        }
        else {      // The user is in the table
            /* Handle the modification, or update, case of the "upsert" function. 
            Use the modify method, passing a few arguments:
            1. The iterator defined earlier, presently set to the user as declared when calling this action.
            2. The "payer", who will pay for the storage cost of this row, in this case, the owner.
            3. The callback function that actually modifies the row. */
            records.modify( iterator, name(account), [&]( auto& row ) {
                row.key = key;
                row.key_2 = name(account).value;
                row.account = account;
                row.perm = perm;
                row.parent_perm = parent_perm;
                row.attr = attr;
            });
        }
    }

    /* The above declaration will extract the arguments of the action and 
    create necessary ABI struct descriptions in the generated ABI file. */
    [[eosio::action]]
    void erase( string account, string perm) {
        /* A require_auth that tests against the action's argument user to verify only the 
        account corresponding to a record can erase his record. */
        std::for_each(account.begin(), account.end(), [](char & c) { c = ::tolower(c); });
        require_auth( name(account) );
        auths_index records("authadmin"_n, get_first_receiver().value);
        std::for_each(perm.begin(), perm.end(), [](char & c) { c = ::tolower(c); });
        auto iterator = records.begin();
        /* A while loop is needed in way to erase all the records related to an account */
        while ( iterator != records.end() ) {
            /* Finally, call the erase method to erase the iterator, if the record is associate to such permission (whether parent or not). 
            Once the row is erased, the storage space will be free up for the original payer. */
            if ( iterator->parent_perm == perm || iterator->perm == perm ) iterator = records.erase(iterator);
            /* This is possible because  */ 
            iterator++;
        }
    }

    /* The above declaration will extract the arguments of the action and 
    create necessary ABI struct descriptions in the generated ABI file. */
    [[eosio::action]]
    void eraseacc( string account ) {
        /* A require_auth that tests against the action's argument user to verify only the 
        account corresponding to a record can erase all records associated to his account. */
        std::for_each(account.begin(), account.end(), [](char & c) { c = ::tolower(c); });
        require_auth( name(account) );
        auths_index records("authadmin"_n, get_first_receiver().value);
        uint64_t key_2 = name(account).value;
        auto iterator = records.begin();
        /* A while loop is needed in way to erase all the records related to an account */
        while ( iterator != records.end() ) {
            /* Finally, call the erase method to erase the iterator, if the record is associate to such account. 
            Once the row is erased, the storage space will be free up for the original payer. */
            if ( iterator->key_2 == key_2) iterator = records.erase(iterator);
            iterator++;
        }
    }


    [[eosio::action]]
    void viewattr( string account, string perm ) {
        /* Based on this semantic, only the account corresponding to the parameter 
        can view his own attributes */
        std::for_each(account.begin(), account.end(), [](char & c) { c = ::tolower(c); });
        require_auth( name(account) );
        auths_index records("authadmin"_n, get_first_receiver().value);
        std::for_each(perm.begin(), perm.end(), [](char & c) { c = ::tolower(c); });
        hash<string> hasher; string s = account+perm;
        uint64_t key = (uint64_t) hasher(s);
        auto iterator = records.find(key);
        check( iterator != records.end(), "Record does not exist!" );
        // special iterator member functions for objects
        auto& field = records.get(key); string attr = field.attr;
        json j = json::parse(attr);
        print(j.dump());
    }

    [[eosio::action]]
    template <typename T> void checkval( string account, string perm, string key, T value) {
        /* Based on this semantic, only the account corresponding to the parameter 
        can check on his own attributes */
        std::for_each(account.begin(), account.end(), [](char & c) { c = ::tolower(c); });
        require_auth( name(account) );
        auths_index records("authadmin"_n, get_first_receiver().value);
        std::for_each(perm.begin(), perm.end(), [](char & c) { c = ::tolower(c); });
        hash<string> hasher; string s = account+perm;
        uint64_t pkey = (uint64_t) hasher(s);
        auto iterator = records.find(pkey);
        check( iterator != records.end(), "Record does not exist!" );
        // special iterator member functions for objects
        auto& field = records.get(pkey); string attr = field.attr;
        json j = json::parse(attr);
        std::for_each(key.begin(), key.end(), [](char & c) { c = ::tolower(c); });
        check(j.contains(key), "The JSON attribute field does not contain the key '"+key+"'!");
        std::for_each(value.begin(), value.end(), [](char & c) { c = ::tolower(c); });
        check(j[key] == value, "The key '"+attr+"' does not match the value "+value+"!");
        print("The value of the key checks in with the value provided as by argument!");
    }

    [[eosio::action]]
    void setattr( string account, string perm, string attr ) {
        /* Function example of how to delegate the modify of attributes to an attribute manager
        account, named "attrmanager" */
        require_auth( name("attrmanager") );
        check( json::accept(attr), "Attributes are not in a valid JSON format!" );
        auths_index records("authadmin"_n, get_first_receiver().value);
        std::for_each(account.begin(), account.end(), [](char & c) { c = ::tolower(c); });
        std::for_each(perm.begin(), perm.end(), [](char & c) { c = ::tolower(c); });
        hash<string> hasher; string s = account+perm;
        uint64_t key = (uint64_t) hasher(s);
        auto iterator = records.find(key);
        check( iterator != records.end(), "Record does not exist!" );
        records.modify( iterator, name("attrmanager"), [&]( auto& row ) {
            row.attr = attr;
        });
    }

    /*
    void subscribe (string user, string perm) {
    
        transaction txn{};
        txn.actions.emplace_back(
            permission_level(name(user), name(perm)),
            "eosio"_n,
            ""
        );

    }
    ...inserire un meccanismo per il quale authadmin aggiunge alla propria tabella di autorità
    l'account alice con weight 1, per una threshold di 2*/

    private:

        /*
        struct attributes {

            string nome;
            string cognome;
            string codiceFiscale;
            string dataNascita;
        };
        */

        /* Before a table can be configured and instantiated, we need to define a struct that 
        represents the data structure of the authority tables.
        The [[eosio.table]] declaration will add the necessary descriptions to the ABI file. */
        struct [[eosio::table]] authority {

            /* La chiave sarà il risutato di una funzione hash che prende in ingresso
            1. Nome dell'account
            2. Permesso dell'account
            La correttezza è garantita dalla proprietà di iniettività delle funzioni hash,
            dove per input diversi corrispondono risultati diversi, che a sua volta deriva 
            dalla proprietà di unicità degli acocunt. */
            uint64_t key;
            /* Since the secondary index needs to be numeric field, a uint64_t altkey variable is added. 
            This will represent the value of name(account) */
            uint64_t key_2;
            string account;
            string perm;
            string parent_perm;
            string attr;

            /* Every multi_index struct requires a primary key method. 
            This method is used according to the index specification of your multi_index instantiation.*/
            uint64_t primary_key() const { return key;}
            /* Getter for the secondary key */
            uint64_t get_secondary_1() const { return key_2;}

        /* A table's data structure cannot be modified while it has data in it. 
        If you need to make changes to a table's data structure in any way, 
        you first need to remove all its rows */
        };
        using auths_index = eosio::multi_index<"authstable"_n, authority, 
            indexed_by<"byauthtable"_n, const_mem_fun<authority, uint64_t, &authority::get_secondary_1>>>;
        /* With the above multi_index configuration there is a table named authstable, that
        1. Uses the _n operator to define an eosio::name type and uses that to name the table. 
        2. Pass in the singular authority struct defined in the previous step.
        3. Declare this table's type. This type will be used to instantiate this table later. 
        ––---------
        In the third parameter, we pass a indexed_by struct which is used to instantiate a index.
        In that indexed_by struct, we specify the name of index as "byauthtable" and the second type 
        parameter as a function call operator which extracts a const value as an index key. 
        In this case, we point it to the getter we created earlier so this multiple index table 
        will index records by the altkey variable.*/
};
