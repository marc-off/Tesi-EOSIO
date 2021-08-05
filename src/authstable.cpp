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
    void upsert( string owner, string perm, string account, string accperm, 
        uint64_t weight, string attr) {
        require_auth(permission_level( name("authadmin"), name("custom") ));
        /* To instantiate a table, two parameters are required:
        1. The first parameter specifies the owner of this table.
        2. The second parameter ensures the uniqueness of the table in the scope of this contract.*/
        auths_index records("authadmin"_n, get_first_receiver().value);
        hash<string> hasher; 
        string s2 = owner+perm; string s1 = s2+account;
        uint64_t key = (uint64_t) hasher(s1); uint64_t key_2 = (uint64_t) hasher(s2);
        auto iterator = records.find(key);
        if( iterator == records.end() ) {   // The user isn't in the table
            /* Create a record in the table using the multi_index method emplace. 
            This method accepts two arguments, the "payer" of this record who pays the storage usage 
            and a callback function. 
            The callback function for the emplace method must use a lamba function to create a reference. 
            Inside the body assign the row's values with the ones provided to upsert. */
            records.emplace( name(owner), [&]( auto& row ) {
                row.key = key;
                row.key_2 = key_2;
                row.owner = owner;
                row.perm = perm;
                row.account = account;
                row.accperm = accperm;
                row.weight = weight;
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
                row.key_2 = key_2;
                row.owner = owner;
                row.perm = perm;
                row.account = account;
                row.accperm = accperm;
                row.weight = weight;
                row.attr = attr;
            });
        }
    }

    /* The above declaration will extract the arguments of the action and 
    create necessary ABI struct descriptions in the generated ABI file. */
    [[eosio::action]]
    void erase( string owner, string perm, string account, string accperm ) {
        /* A require_auth that tests against the action's argument user to verify only the 
        owner of a record with such permission can modify their account. */
        require_auth( permission_level(name(owner), name(perm)) );
        auths_index records("authadmin"_n, get_first_receiver().value);
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

    /* The above declaration will extract the arguments of the action and 
    create necessary ABI struct descriptions in the generated ABI file. */
    [[eosio::action]]
    void erasetable( string owner, string perm ) {
        /* A require_auth that tests against the action's argument user to verify only the 
        owner of a record with such permission can modify their account. */
        require_auth( permission_level(name(owner), name(perm)) );
        auths_index records("authadmin"_n, get_first_receiver().value);
        hash<string> hasher; string s = owner+perm;
        uint64_t key_2 = (uint64_t) hasher(s);
        auto iterator = records.begin();
        /* A while loop is needed in way to erase all the records related to an authority table */
        while ( iterator != records.end() ) {
            /* Finally, call the erase method, to erase the iterator. 
            Once the row is erased, the storage space will be free up for the original payer. */
            if ( iterator->key_2 == key_2) iterator = records.erase(iterator);
            else iterator++;
        }
    }


    [[eosio::action]]
    void viewattr( string owner, string perm, string account, string accperm ) {
    
        require_auth( permission_level(name(owner), name(perm)) );
        auths_index records("authadmin"_n, get_first_receiver().value);
        hash<string> hasher; string s = owner+perm+account;
        uint64_t key = (uint64_t) hasher(s);
        auto iterator = records.find(key);
        check( iterator != records.end(), "Record does not exist!" );
        // special iterator member functions for objects
        auto& field = records.get(key); string attr = field.attr;
        json j = json::parse(attr);
        print(j.dump());
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
            1. Nome dell'owner della authority table
            2. Permesso a cui l'authority table si riferisce
            3. Nome/Chiave dell'account a cui si delega l'autorità
            La correttezza è garantita dalla proprietà di iniettività delle funzioni hash,
            Dove per input diversi corrispondono risultati diversi. */
            uint64_t key;
            /* Since the secondary index needs to be numeric field, a uint64_t altkey variable is added. 
            This will represent the identity of an authtable starting from an owner and a perm,
            in hash fashion */
            uint64_t key_2;
            string owner;
            string perm;
            string account;
            string accperm;
            uint64_t weight;
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
