#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iomanip>
#include <limits> // Include this header for std::numeric_limits

// Base Account Class
class Account {
protected:
    int accountNumber;
    std::string accountHolder;
    double balance;

public:
    Account(int number, const std::string& holder, double initialBalance)
        : accountNumber(number), accountHolder(holder), balance(initialBalance) {}

    virtual ~Account() {}

    int getNumber() const { return accountNumber; }
    std::string getHolder() const { return accountHolder; }
    double getBalance() const { return balance; }

    virtual void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        } else {
            throw std::invalid_argument("Deposit amount must be positive.");
        }
    }

    virtual void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
        } else {
            throw std::invalid_argument("Insufficient funds or invalid amount.");
        }
    }

    virtual void displayAccountInfo() const {
        std::cout << "Account Number: " << accountNumber << " | Holder: " << accountHolder
                  << " | Balance: $" << balance << std::endl;
    }

    virtual void saveToFile(std::ofstream& file) const {
        file << "A " << accountNumber << ' ' << accountHolder << ' ' << balance << '\n';
    }

    virtual void loadFromFile(std::ifstream& file) {
        file >> accountNumber >> accountHolder >> balance;
    }
};

// Derived Savings Account Class
class SavingsAccount : public Account {
public:
    SavingsAccount(int number, const std::string& holder, double initialBalance)
        : Account(number, holder, initialBalance) {}

    void displayAccountInfo() const override {
        std::cout << "[Savings] ";
        Account::displayAccountInfo();
    }

    void saveToFile(std::ofstream& file) const override {
        file << "S " << accountNumber << ' ' << accountHolder << ' ' << balance << '\n';
    }
};

// Derived Current Account Class
class CurrentAccount : public Account {
public:
    CurrentAccount(int number, const std::string& holder, double initialBalance)
        : Account(number, holder, initialBalance) {}

    void displayAccountInfo() const override {
        std::cout << "[Current] ";
        Account::displayAccountInfo();
    }

    void saveToFile(std::ofstream& file) const override {
        file << "C " << accountNumber << ' ' << accountHolder << ' ' << balance << '\n';
    }
};

class Customer {
    int id;
    std::vector<Account*> accounts;

public:
    Customer(int id) : id(id) {}

    int getID() const { return id; }

    void addAccount(const Account* account) {
        accounts.push_back(const_cast<Account*>(account));
    }

    Account* getSavingsAccount() const { // Marking the method as const
        for (auto& account : accounts) {
            if (dynamic_cast<SavingsAccount*>(account)) {
                return account;
            }
        }
        return nullptr;
    }

    Account* getCurrentAccount() const { // Marking the method as const
        for (auto& account : accounts) {
            if (dynamic_cast<CurrentAccount*>(account)) {
                return account;
            }
        }
        return nullptr;
    }

    void displayAccounts() const {
        for (const auto& account : accounts) {
            account->displayAccountInfo();
        }
    }
};

// Bank Class
class Bank {
    std::vector<Account*> accounts;
    std::vector<Customer> customers;

public:
    ~Bank() {
        for (auto account : accounts) {
            delete account;
        }
    }

    void addAccount(Account* account) {
        accounts.push_back(account);
    }

    void addCustomer(const Customer& customer) {
        customers.push_back(customer);
    }

    Customer* findCustomer(int customerID) {
    for (auto& customer : customers) {
        if (customer.getID() == customerID) {
            return &customer;
        }
    }
    return nullptr;
}

const Customer* findCustomer(int customerID) const {
    for (const auto& customer : customers) {
        if (customer.getID() == customerID) {
            return &customer;
        }
    }
    return nullptr;
}

    bool customerExists(int customerID) const {
    return findCustomer(customerID) != nullptr;
}


    const Account* findAccount(int accountNumber) const {
    for (const auto& account : accounts) {
        if (account->getNumber() == accountNumber) {
            return account;
        }
    }
    return nullptr;
}


   void deposit(int customerID, const std::string& accountType, double amount) {
    Customer* customer = findCustomer(customerID);
    if (customer) {
        Account* account = (accountType == "Savings") ? customer->getSavingsAccount() : customer->getCurrentAccount();
        if (account) {
            account->deposit(amount);
        } else {
            throw std::invalid_argument(accountType + " account not found for this customer.");
        }
    } else {
        throw std::invalid_argument("Customer not found.");
    }
}

void withdraw(int customerID, const std::string& accountType, double amount) {
    Customer* customer = findCustomer(customerID);
    if (customer) {
        Account* account = (accountType == "Savings") ? customer->getSavingsAccount() : customer->getCurrentAccount();
        if (account) {
            account->withdraw(amount);
        } else {
            throw std::invalid_argument(accountType + " account not found for this customer.");
        }
    } else {
        throw std::invalid_argument("Customer not found.");
    }
}

void transfer(int fromCustomerID, const std::string& fromAccountType, double amount, int toCustomerID = -1, const std::string& toAccountType = "") {
    Customer* fromCustomer = findCustomer(fromCustomerID);
    if (fromCustomer) {
        Account* fromAccount = (fromAccountType == "Savings") ? fromCustomer->getSavingsAccount() : fromCustomer->getCurrentAccount();

        if (!fromAccount) {
            throw std::invalid_argument(fromAccountType + " account not found for this customer.");
        }

        if (toCustomerID == -1) {  // Self-transfer case
            Account* toAccount = (toAccountType == "Savings") ? fromCustomer->getSavingsAccount() : fromCustomer->getCurrentAccount();

            if (!toAccount) {
                throw std::invalid_argument(toAccountType + " account not found for this customer.");
            }

            fromAccount->withdraw(amount);
            toAccount->deposit(amount);

            std::cout << "Transfer successful within the same customer.\n";

        } else {  // Transfer to another customer
            Customer* toCustomer = findCustomer(toCustomerID);
            if (!toCustomer) {
                throw std::invalid_argument("Target customer not found.");
            }

            Account* toAccount = (toAccountType == "Savings") ? toCustomer->getSavingsAccount() : toCustomer->getCurrentAccount();

            if (!toAccount) {
                throw std::invalid_argument(toAccountType + " account not found for the target customer.");
            }

            fromAccount->withdraw(amount);
            toAccount->deposit(amount);

            std::cout << "Transfer successful to another customer.\n";
        }
    } else {
        throw std::invalid_argument("Customer not found.");
    }
}


void viewBalance(int customerID, const std::string& accountType) const {
    const Customer* customer = findCustomer(customerID);
    if (customer) {
        const Account* account = (accountType == "Savings") ? customer->getSavingsAccount() : customer->getCurrentAccount();
        if (account) {
            std::cout << accountType << " Account Balance: $" << account->getBalance() << std::endl;
        } else {
            std::cout << accountType << " account not found for this customer.\n";
        }
    } else {
        std::cout << "Customer not found.\n";
    }
}




    void saveData(const std::string& filename) const {
        std::ofstream file(filename);
        for (const auto& account : accounts) {
            account->saveToFile(file);
        }
    }

    void loadData(const std::string& filename) {
        std::ifstream file(filename);
        char type;
        int number;
        std::string holder;
        double balance;
        while (file >> type >> number >> holder >> balance) {
            if (type == 'S') {
                addAccount(new SavingsAccount(number, holder, balance));
            } else if (type == 'C') {
                addAccount(new CurrentAccount(number, holder, balance));
            }
        }
    }

    void displayAllAccounts() const {
        for (const auto& account : accounts) {
            account->displayAccountInfo();
        }
    }
};

// Function to clear input buffer
void clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    Bank bank;
    int choice;

    do {
        std::cout << "\n===== Banking System =====\n";
        std::cout << "1. Add Savings Account\n";
        std::cout << "2. Add Current Account\n";
        std::cout << "3. Add Customer\n";
        std::cout << "4. Deposit\n";
        std::cout << "5. Withdraw\n";
        std::cout << "6. Transfer\n";
        std::cout << "7. View Balance\n";
        std::cout << "8. Save Data\n";
        std::cout << "9. Load Data\n";
        std::cout << "10.View All Accounts\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            int customerID, number;
            std::string holder;
            double balance;
            std::cout << "Enter Customer ID: ";
            std::cin >> customerID;
            if (!bank.customerExists(customerID)) {
                std::cout << "Customer does not exist. Please add the customer first.\n";
                break;
            }
            std::cout << "Enter Savings Account Number: ";
            std::cin >> number;
            clearInputBuffer(); // Clear buffer before getline
            std::cout << "Enter Account Holder's Name: ";
            std::getline(std::cin, holder);
            std::cout << "Enter Initial Balance: ";
            std::cin >> balance;
            bank.addAccount(new SavingsAccount(number, holder, balance));
            // Associate account with customer
            Customer* customer = bank.findCustomer(customerID);
            // if (customer) {
            //     customer->addAccount(bank.findAccount(number));
            // }
            const Account* account = bank.findAccount(number);
if (account) {
    customer->addAccount(const_cast<Account*>(account));
}

            std::cout << "Savings Account added successfully.\n";
            break;
        }
        case 2: {
            int customerID, number;
            std::string holder;
            double balance;
            std::cout << "Enter Customer ID: ";
            std::cin >> customerID;
            if (!bank.customerExists(customerID)) {
                std::cout << "Customer does not exist. Please add the customer first.\n";
                break;
            }
            std::cout << "Enter Current Account Number: ";
            std::cin >> number;
            clearInputBuffer(); // Clear buffer before getline
            std::cout << "Enter Account Holder's Name: ";
            std::getline(std::cin, holder);
            std::cout << "Enter Initial Balance: ";
            std::cin >> balance;
            bank.addAccount(new CurrentAccount(number, holder, balance));
            // Associate account with customer
            Customer* customer = bank.findCustomer(customerID);
            // if (customer) {
            //     customer->addAccount(bank.findAccount(number));
            // }
            const Account* account = bank.findAccount(number);
if (account) {
    customer->addAccount(const_cast<Account*>(account));
}

            std::cout << "Current Account added successfully.\n";
            break;
        }
        case 3: {
            int id;
            std::string name;
            std::cout << "Enter Customer ID: ";
            std::cin >> id;
            clearInputBuffer(); // Clear buffer before getline
            std::cout << "Enter Customer Name: ";
            std::getline(std::cin, name);
            bank.addCustomer(Customer(id));
            std::cout << "Customer added successfully.\n";
            break;
        }
 case 4: { // Deposit
    int customerID;
    std::string accountType;
    double amount;
    std::cout << "Enter Customer ID: ";
    std::cin >> customerID;
    std::cout << "Enter Account Type (Savings/Current): ";
    std::cin >> accountType;
    std::cout << "Enter Amount to Deposit: ";
    std::cin >> amount;
    try {
        bank.deposit(customerID, accountType, amount);
        std::cout << "Deposit successful.\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    break;
}

case 5: { // Withdraw
    int customerID;
    std::string accountType;
    double amount;
    std::cout << "Enter Customer ID: ";
    std::cin >> customerID;
    std::cout << "Enter Account Type (Savings/Current): ";
    std::cin >> accountType;
    std::cout << "Enter Amount to Withdraw: ";
    std::cin >> amount;
    try {
        bank.withdraw(customerID, accountType, amount);
        std::cout << "Withdrawal successful.\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    break;
}

case 6: { // Transfer
    int fromCustomerID, toCustomerID;
    std::string fromAccountType, toAccountType;
    double amount;
    char isSelfTransfer;

    std::cout << "Is this a self-transfer? (y/n): ";
    std::cin >> isSelfTransfer;

    if (isSelfTransfer == 'y' || isSelfTransfer == 'Y') {
        std::cout << "Enter Customer ID: ";
        std::cin >> fromCustomerID;
        std::cout << "Enter From Account Type (Savings/Current): ";
        std::cin >> fromAccountType;
        std::cout << "Enter To Account Type (Savings/Current): ";
        std::cin >> toAccountType;
        std::cout << "Enter Amount to Transfer: ";
        std::cin >> amount;
        try {
            bank.transfer(fromCustomerID, fromAccountType, amount, -1, toAccountType);
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Enter Your Customer ID: ";
        std::cin >> fromCustomerID;
        std::cout << "Enter From Account Type (Savings/Current): ";
        std::cin >> fromAccountType;
        std::cout << "Enter Target Customer ID: ";
        std::cin >> toCustomerID;
        std::cout << "Enter Target Account Type (Savings/Current): ";
        std::cin >> toAccountType;
        std::cout << "Enter Amount to Transfer: ";
        std::cin >> amount;
        try {
            bank.transfer(fromCustomerID, fromAccountType, amount, toCustomerID, toAccountType);
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
    break;
}


case 7: { // View Balance
    int customerID;
    std::string accountType;
    std::cout << "Enter Customer ID: ";
    std::cin >> customerID;
    std::cout << "Enter Account Type (Savings/Current): ";
    std::cin >> accountType;
    bank.viewBalance(customerID, accountType);
    break;
}

        case 10: {
            std::cout << "Displaying all accounts:\n";
            bank.displayAllAccounts();
            break;
        }
        case 8: {
            std::string filename;
            std::cout << "Enter filename to save data: ";
            std::cin >> filename;
            bank.saveData(filename);
            std::cout << "Data saved successfully.\n";
            break;
        }
        case 9: {
            std::string filename;
            std::cout << "Enter filename to load data: ";
            std::cin >> filename;
            bank.loadData(filename);
            std::cout << "Data loaded successfully.\n";
            break;
        }
        case 0:
            std::cout << "Exiting...\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 0);

    return 0;
}
