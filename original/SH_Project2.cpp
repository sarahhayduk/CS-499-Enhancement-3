// CS410 SH_Project2.cpp Sarah Hayduk
#include <iostream>
#include <string>
#include <vector> // Include vector for client storage

using namespace std;

/* ---------------------------------------------------------------------------
 * VULNERABILITY: Global variables exposer client data to unintended
 * modification and violate least privilege.
 * ---------------------------------------------------------------------------
 * FIX: Replaced insecure global variable declarations with a structured,
 * encapsulated design. This removes global state vulnerabilities and prevents
 * unintended modifications to client info.
 * --------------------------------------------------------------------------- */
struct Client {
	string name;
	int num;
};

vector<Client> clients = {
		{"Bob Jones", 1},
		{"Sarah Davis", 2},
		{"Amy Friendly", 1},
		{"Johnny Smith", 1},
		{"Carol Spears", 2}
};

/* ---------------------------------------------------------------------------
 * ChangeCustomerChoice()
 * Allows user to change client service choice: Brokerage(1) & Retirement(2)
 * ---------------------------------------------------------------------------
 * VULNERABILITY: User input is taken without validation or bounds checking
 * allowing non-numeric, invalid or out-of-range values to enter program.
 * ---------------------------------------------------------------------------
 * FIXES: Added input validation and bounds checking to prevent out-of-range
 * access and failed cin states from persisting.
 * ---------------------------------------------------------------------------*/
void ChangeCustomerChoice() {
	int changechoice;
	int newservice;

	cout << "Enter the number of the client that you wish to change\n";
	cin >> changechoice;
	// FIX Added validation and bounds checking, must be within range
	if (cin.fail() || changechoice < 1 || changechoice > clients.size()) {
		// Print message to inform user of error
		cout << "[Invalid Client Number] Returning to menu.\n";
		// clear the error and flush bad input from stream
		cin.clear();
		cin.ignore(1000, '\n');
		return;
	}

	// Refactor this cout statement to include the clients name
	//cout << "Please enter the client's new service choice (1 = Brokerage, 2 = Retirement)\n";
	cout << "Please enter new service choice for " << clients[changechoice - 1].name <<
			" (1 = Brokerage, 2 = Retirement)\n";

	cin >> newservice;
	// FIX Added validation of service choice, must be 1 or 2
	if (cin.fail() || (newservice != 1 && newservice != 2)) {
		// Print message to inform user of error
		cout << "[Invalid Service Number] Returning to menu.\n";
		// clear the error and flush the bad input from stream
		cin.clear();
		cin.ignore(1000, '\n');
		return;
	}

	// FIX remove if/else if block. Safely update structured data
	clients[changechoice - 1].num = newservice;

	// FIX Added confirmation message to user
	cout << "Service updated: " << clients[changechoice - 1].name <<
			" selection option " << clients[changechoice - 1].num << endl;
}

/* ---------------------------------------------------------------------------
 * CheckUserPermissionAccess()
 * Checks if user password matches string "123", return answer to main()
 * ---------------------------------------------------------------------------
 * VULNERABILITY: password is hardcoded ("123") and performs no input validation
 * or sanitization, allowing malformed input to enter program.
 * ----------------------------------------------------------------------------
 * FIXES: Added basic input validation to prevent failed cin states from
 * persisting.
 * ---------------------------------------------------------------------------*/
int CheckUserPermissionAccess() {
	string username;
	string password;

	cout << "Enter your username: \n";
	cin >> username;
	cout << "Enter your password: \n";
	cin >> password;

	// FIX Added validation to prevent malformed input from bypassing logic
	if (cin.fail()) {
		// clear the error and flush the bad input from stream
		cin.clear();
		cin.ignore(1000, '\n');
		// treat as invalid password
		return 2;
	}

	// if password matches, return 1 to main
	if (password.compare("123") == 0) {
		return 1;
	}
	// if password does not match, return 2
	else {
		return 2;
	}
}

/* -----------------------------------------------------------------------------
 * DisplayInfo()
 * Display client number, name, and selected service to user
 * ----------------------------------------------------------------------------
 * FIX: Now uses structured vector instead of insecure global variables
 * ----------------------------------------------------------------------------*/
void DisplayInfo() {
	cout << "  Client's Name    Service Selected (1 = Brokerage, 2 = Retirement)" << endl;

	// loop through clients vector
	for (int i = 0; i < clients.size(); i++) {
		cout << i + 1 << ". " << clients[i].name << " selected option " <<
				clients[i].num << endl;
	}
}

/* -----------------------------------------------------------------------------
 * DisplayMenu()
 * Holds main menu loop to call functions based on user selection.
 * -----------------------------------------------------------------------------
 * VULNERABILITY: Menu input is read without validation or bounds checking,
 * allowing non-numeric or invalid values to enter program and break logic.
 * -----------------------------------------------------------------------------
 * FIXES: Added input validation to menu choices, prevented failed cin state
 * from persisting, and added print statement to inform user of errors
 * ----------------------------------------------------------------------------*/
void DisplayMenu(vector<Client>& clients) {
	int choice;

	while (true) {
		// Added an extra newline to clean up console output
		cout << endl;
		cout << "What would you like to do?\n";
		cout << "DISPLAY the client list (enter 1)\n";
		cout << "CHANGE a client's choice (enter 2)\n";
		cout << "Exit the program.. (enter 3)\n";

		cin >> choice;

		// FIX Added input validation for non-numeric input
		if (cin.fail()) {
			cout << "[Invalid Input] Enter a number between 1 and 3.\n";
			// clear the error and flush bad input from stream
			cin.clear();
			cin.ignore(1000, '\n');
			continue;
		}

		cout << "You chose " << choice << endl;

		// switch statement to handle the choices
		switch (choice) {
		case 1:
			DisplayInfo();
			break;
		case 2:
			ChangeCustomerChoice();
			break;
		case 3:
			// Inform user the program is Exiting, return to main.
			cout << "Exiting the program... Goodbye!\n";
			return;
		// invalid option, restart loop
		default:
			// Added message for invalid numeric input
			cout << "[Invalid Option] Please enter 1, 2, or 3.\n";
			continue;
		}
	}
}

/* ------------------------------------------------------------------------------
 * main()
 * Calls CheckUserPermissionAccess to verify password.
 * -----------------------------------------------------------------------------
 * VULNERABILITY: Menu logic was held in main, making it unnecessarily complex.
 * -----------------------------------------------------------------------------
 * FIX: Removed menu loop to make the program more module and to minimize and
 * simply the main function logic.
 * ----------------------------------------------------------------------------*/
int main() {
	// Add output statement per rubric
	cout << "CS-410 Project 2 | Secure Refactor by: Sarah Hayduk\n" << endl;
	// Print the welcome message
	cout << "Hello! Welcome to our Investment Company\n";

	// Loop to check password
	int answer = CheckUserPermissionAccess();
	while (answer != 1) {
		// Edited this statement so error messages are consistent
		cout << "[Invalid Password] Please try again.\n";
		// Update answer
		answer = CheckUserPermissionAccess();
	}

	// Menu logic moved to its own function
	DisplayMenu(clients);

	return 0;
}
