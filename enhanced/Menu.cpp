// Menu.cpp : This file contains the menu-related functions.

#include "Menu.h"
#include "Database.h"
#include <iostream>
#include <iomanip>
#include <string>

/*--------------------------------------------------------------------------------
 * Menu.cpp
 *
 * This module centralizes all menu-related operations for the Client Service
 * Management System. It provides a clean separation between UI logic and the program
 * entry point by moving menu flow, prompts, and user interaction out of ClientService
 * where main() lives. Moving the menu system to a dedicated module improved clarity
 * and maintainability.
 *
 * Internal errors are routed to the diagnostics log to avoid exposing sensitive
 * details to the user interface, all database related cout statements were changed
 * to generic messages, supporting secure and defensive error handling practices.
 * ------------------------------------------------------------------------------*/

using namespace std;

/*---------------------------------------------------------------------------------
 * DisplayInfo
 *
 * This function prints the list of clients loaded from the database. It uses the
 * database-backed vector instead of an insecure hardcoded global vector. iomanip
 * is used to make an easy to read table of output in the console.
 * -------------------------------------------------------------------------------*/
void DisplayInfo(const vector<Client>& clients) {

    cout << left
        << setw(5) << "ID"
        << setw(25) << "Client Name"
        << setw(10) << "Service (1 = Brokerage, 2 = Retirement)"
        << endl;

    cout << string(69, '-') << endl;

    for (const auto& c : clients) {
        cout << left
            << setw(5) << c.id
            << setw(25) << c.name
            << setw(10) << c.service
            << "\n"
            << string(69, '-')
            << endl;
    }
}

/*---------------------------------------------------------------------------------
 * ChangeCustomerChoice
 *
 * This function prompts the user for a client index and a new service value,
 * validates input, updates the database, and reloads the client list.
 * -------------------------------------------------------------------------------*/
void ChangeCustomerChoice(vector<Client>& clients) {
    int changechoice;
    int newservice;

    cout << "Enter the number of the client that you wish to change\n";
    cin >> changechoice;

    // Input validation and bounds checking, must be within range
    if (cin.fail() || changechoice < 1 || changechoice > clients.size()) {
        cout << "[Invalid Client Number] Returning to menu.\n";
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }

    // Ask for new service
    cout << "Please enter the new service choice for " << clients[changechoice - 1].name
        << " (1 = Brokerage, 2 = Retirement)\n";

    cin >> newservice;

    // Validate service number
    if (cin.fail() || (newservice != 1 && newservice != 2)) {
        cout << "[Invalid Service Number] Returning to menu.\n";
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }

    // Update the database
    int id = clients[changechoice - 1].id;
    string name = clients[changechoice - 1].name;

    if (!UpdateClient(id, name, newservice)) {
        cout << "[Error] Database operation failed.\n";
        return;
    }

    // Reload the updated list
    LoadClients(clients);

    cout << "Service updated: " << name << " selected option " << newservice << endl;
}

/*---------------------------------------------------------------------------------
 * AddNewClient
 *
 * This function prompts the user for a new client's name and service selection,
 * validates input, inserts the new client into the database, and reloads the
 * client list.
 * -------------------------------------------------------------------------------*/
void AddNewClient(vector<Client>& clients) {
    string name;
    int service;

    cout << "Enter the new client's name: ";
    cin.ignore(); // clear leftover newline
    getline(cin, name);

    cout << "Enter service (1 = Brokerage, 2 = Retirement): ";
    cin >> service;

    // Validate service number
    if (cin.fail() || service != 1 && service != 2) {
        cout << "[Invalid Service Number] Returning to menu.\n";
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }

    if (!AddClient(name, service)) {
        cout << "[Error] Database operation failed.\n";
        return;
    }

    // Reload the updated list
    LoadClients(clients);

    cout << "New client added: " << name << " selected option " << service << endl;
}

/*---------------------------------------------------------------------------------
 * DisplayMenu
 *
 * This function holds the menu loop to call functions based on user selection.
 * All operations now interact with the database
 * -------------------------------------------------------------------------------*/
void DisplayMenu(vector<Client>& clients) {
    int choice;

    while (true) {
        cout << endl;
        cout << "What would you like to do?\n";
        cout << "DISPLAY the client list (enter 1)\n";
        cout << "CHANGE a client's choice (enter 2)\n";
        cout << "ADD a new client (enter 3)\n";
        cout << "Exit the program.. (enter 4)\n";

        cin >> choice;

        // Validate non-numeric input
        if (cin.fail()) {
            cout << "[Invalid Input] Enter a number between 1 and 4.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        cout << "You chose " << choice << "\n" << endl;

        // Switch statement to handle menu choice
        switch (choice) {
        case 1:
            DisplayInfo(clients);
            break;
        case 2:
            ChangeCustomerChoice(clients);
            break;
        case 3:
            AddNewClient(clients);
            break;
        case 4:
            cout << "Exiting the program... Goodbye!" << endl;;
            cout << "\nCS-499 Computer Science Capstone\nRefactor by: Sarah Hayduk" << endl;
            return;
            // Default case handles invalid numeric input
        default:
            cout << "[Invalid Option] Please enter 1, 2, 3, or 4.\n";
            continue;
        }
    }
}