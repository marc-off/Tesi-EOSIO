# Tesi triennale per l'Università di Pisa (facoltà di informatica) sulla blockchain EOSIO

<p align="center">
  <img width="300" height="300" src="https://upload.wikimedia.org/wikipedia/it/7/72/Stemma_unipi.png">
</p>

[![Software License](https://img.shields.io/badge/license-MIT-lightgrey.svg)](./LICENSE)

Il software EOS.IO introduce una nuova generazione di architettura blockchain progettata per funzionare sull'algoritmo di consenso "Delegated Proof of Stake" (DPOS). Fornisce gli elementi costitutivi di base per lo sviluppo di applicazioni decentralizzate (DApp) aziendali, come account, autenticazione, database e comunicazione asincrona. Uno smart contract in EOS.IO è un programma C++ che può essere eseguito sulla blockchain come un calcolo affidabile e questa esecuzione fa parte della storia immutabile della blockchain. Il software EOS.IO fornisce un semplice sistema di gestione delle autorizzazioni basato sui ruoli che offre agli utenti un controllo di alto livello su chi può eseguire azioni su un altro smart contract.

## Obiettivo

L'obiettivo di questa tesi è migliorare il sistema di gestione dei permessi basato sui ruoli fornito da EOS.IO. In particolare, questo lavoro prevede la definizione e lo sviluppo di uno strumento di uso generale per la gestione dei permessi delle DApp. Lo strumento deve supportare almeno una forma di base comune di controllo degli accessi (come gruppi gerarchici e autorizzazioni multiple).

## Guida

Si assumerà d'ora in poi che l'utente abbia già installato il software EOSIO, seguendo *attentamente* tutti i passi descritti dalla [guida ufficiale](https://developers.eos.io/manuals/eos/latest/install/index). Si assumerà inoltre che l'utente abbia effettuato anche la build degli eosio.contracts, sempre seguendo *attentamente* i passi descritti dalla [documentazione del sito](https://developers.eos.io/manuals/eosio.contracts/latest/build-and-deploy).

### Struttura della tabella 

La struttura del database ha la seguente forma:

| Chiave primaria | Secondary key | Owner | Owner_Permission | Account | Account_Permission | Weight  | Attributes |
| --------------- | ------------- | ----- | ---------------- | ------- | ------------------ | ------- | ---------- |

### Aggiungere o modificare un record nel database delle tabelle di autorità

Per aggiungere un record in una determinata tabella di autorità all'interno del database, il proprietario dell'account deve eseguire
il seguente comando:

```shellsession
cleos push action authadmin upsert data -p account@perm
```

Dove: 

1) `authadmin` è l'account sul quale è deployato lo smart contract

2) `upsert` è l'azione relativa allo smart contract

3) `data` è il record che il proprietario aggiunge al database, di conseguenza dovrà fornire i seguenti argomenti:

    - Il nome del proprietario della tabella di autorità

    - Il nome del permesso al quale è riferita la tabella di autorità

    - Il nome dell'account al quale l'autorità è conferita, nel contesto della suddetta tabella di autorità

    - Il nome del permesso dell'account al quale l'autorità è conferita, nel contesto della suddetta tabella di autorità

    - Il peso dell'autorità conferita dal proprietario all'account

    - Gli attributi (formato JSON) assegnato dal proprietario all'account al quale l'autorità è conferita

# English translation 🏴󠁧󠁢󠁥󠁮󠁧󠁿

The EOS.IO software introduces a new generation of blockchain architecture designed to work on "Delegated Proof of Stake" (DPOS) consensus algorithm. It provides the basic building blocks for developing enterprise Decentralized Applications (DApps), such as accounts, authentication, databases, and asynchronous communication. A smart contract in EOS.IO is a C++ program that can be executed on the blockchain as a trusted computation and this execution takes part of the immutable history of the blockchain. The EOS.IO software provides a simple role-based permission management system that gives to users high-level control on who can execute actions over another smart contract.

## Target

The goal of this thesis is to enhance the role-based permission management system provided by EOS.IO. In particular, this work involves the definition and development of a general-purpose tool for managing permissions of DApps. The tool must support at least a basic common form of access control (such as, hierarchical groups and multiple permissions).


## Guide 

We'll assume from now onwards that the user already installed the EOSIO software, following *carefully* all the steps as described by the [official guide](https://developers.eos.io/manuals/eos/latest/install/index). We'll also assume that the user already executed the build of the eosio.contracts, once again *carefully* following the steps as described by the [site documentation](https://developers.eos.io/manuals/eosio.contracts/latest/build-and-deploy).

### Table structure

| Primary key   | Secondary key | Owner | Owner_Permission | Account | Account_Permission | Weight  | Attributes |
| ------------- | ------------- | ----- | ---------------- | ------- | ------------------ | ------- | ---------- |

### Adding or modifying a record in the authority tables database 

In way to add a record in a specific authority table in the database, the owner of the account has to execute the following command:

```shellsession
cleos push action authadmin upsert `data` -p `account`@`perm`
```

Where: 

1) `authadmin` is the account to which the smart contract is deployed to

2) `upsert` is the action related to the smart contract

3) `data` is the record that the owner adds to the database, hence it has to provide the following arguments: 

    - Name of the owner of the authority table 

    - Name of the permission to which the authority table is referred to

    - Name of the account to which authority is conferred, in context of the authority table

    - Name of the permission of the account to which authority is conferred, in context of such authority table

    - Weight of the authority conferred by the owner to such account

    - The attributes (JSON format) assigned by the owner to the account to which authority is conferred
