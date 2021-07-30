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
    void upsert( string owner, string perm, string account, string accperm, string attr) {
        /* Only the owner has control over their own record (authority table), as this contract is opt-in. 
        This method accepts an name type argument and asserts that the account executing the 
        transaction equals the provided value and has the proper permissions to do so. */
        require_auth( name(owner) );
        // require_auth2 (owner, perm)
        /* To instantiate a table, two parameters are required:
        1. The first parameter specifies the owner of this table.
        2. The second parameter ensures the uniqueness of the table in the scope of this contract.*/
        address_index records("authadmin"_n, get_first_receiver().value);
        hash<string> hasher; string s = owner+perm+account;
        uint64_t key = (uint64_t) hasher(s);
        auto iterator = records.find(key);
        if( iterator == records.end() ) {   // The user isn't in the table
            /* Create a record in the table using the multi_index method emplace. 
            This method accepts two arguments, the "payer" of this record who pays the storage usage 
            and a callback function. 
            The callback function for the emplace method must use a lamba function to create a reference. 
            Inside the body assign the row's values with the ones provided to upsert. */
            records.emplace( name(owner), [&]( auto& row ) {
                row.key = key;
                row.owner = owner;
                row.perm = perm;
                row.account = account;
                row.accperm = accperm;
                row.attr = attr;
            });
        }
        else {      // The user is in the table
            /* Handle the modification, or update, case of the "upsert" function. 
            Use the modify method, passing a few arguments:
            1. The iterator defined earlier, presently set to the user as declared when calling this action.
            2. The "payer", who will pay for the storage cost of this row, in this case, the owner.
            3. The callback function that actually modifies the row. */
            records.modify( iterator, name(owner), [&]( auto& row ) {
                row.key = key;
                row.owner = owner;
                row.perm = perm;
                row.account = account;
                row.accperm = accperm;
                row.attr = attr;
            });
        }
    }

    /* The above declaration will extract the arguments of the action and 
    create necessary ABI struct descriptions in the generated ABI file. */
    [[eosio::action]]
    void erase( string owner, string perm, string account, string accperm ) {
        /* A require_auth that tests against the action's argument user to verify only the 
        owner of a record can modify their account. */
        require_auth( name(owner) );
        address_index records("authadmin"_n, get_first_receiver().value);
        hash<string> hasher; string s = owner+perm+account;
        uint64_t key = (uint64_t) hasher(s);
        auto iterator = records.find(key);
        /* A contract cannot erase a record that doesn't exist, 
        so check that the record indeed exists before proceeding. */
        check( iterator != records.end(), "Record does not exist!" );
        /* Finally, call the erase method, to erase the iterator. 
        Once the row is erased, the storage space will be free up for the original payer. */
        records.erase(iterator);
    }

    [[eosio::action]]
    void viewattr( string owner, string perm, string account, string accperm ) {
    
        require_auth ( name(owner) );
        address_index records("authadmin"_n, get_first_receiver().value);
        hash<string> hasher; string s = owner+perm+account;
        uint64_t key = (uint64_t) hasher(s);
        auto iterator = records.find(key);
        check( iterator != records.end(), "Record does not exist!" );
        // special iterator member functions for objects
        auto& field = records.get(key); string attr = field.attr;
        json j = json::parse(attr);
        print(j.dump());
    }

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
            1. Nome dell'owner della authority table
            2. Permesso a cui l'authority table si riferisce
            3. Nome/Chiave dell'account a cui si delega l'autorità
            La correttezza è garantita dalla proprietà di iniettività delle funzioni hash,
            Dove per input diversi corrispondono risultati diversi. */
            uint64_t key;
            string owner;
            string perm;
            string account;
            string accperm;
            string attr;

            /* Every multi_index struct requires a primary key method. 
            This method is used according to the index specification of your multi_index instantiation.*/
            uint64_t primary_key() const { return key;}
        /* A table's data structure cannot be modified while it has data in it. 
        If you need to make changes to a table's data structure in any way, 
        you first need to remove all its rows */
        };
        using address_index = eosio::multi_index<"authstable"_n, authority>;
        /* With the above multi_index configuration there is a table named authstable, that
        1. Uses the _n operator to define an eosio::name type and uses that to name the table. 
        2. Pass in the singular authority struct defined in the previous step.
        3. Declare this table's type. This type will be used to instantiate this table later. */
};
