#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

// ANSI color codes (works on most terminals)
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string UNDERLINE = "\033[4m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string BLUE = "\033[34m";
const std::string CYAN = "\033[36m";
// Book class
class Book {
private:
    int bookID;
    std::string title;
    std::string author;
    bool isAvailable;

public:
    Book(int id, std::string t, std::string a) : bookID(id), title(t), author(a), isAvailable(true) {}

    int getID() const { return bookID; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    bool getAvailability() const { return isAvailable; }

    void setAvailability(bool status) { isAvailable = status; }
    void display() const {
        std::cout << "Book ID: " << bookID << "\nTitle: " << title << "\nAuthor: " << author 
                  << "\nAvailable: " << (isAvailable ? "Yes" : "No") << std::endl;
    }
};

// Member class
class Member {
private:
    int memberID;
    std::string name;

public:
    Member(int id, std::string n) : memberID(id), name(n) {}

    int getID() const { return memberID; }
    std::string getName() const { return name; }

    void display() const {
        std::cout << "Member ID: " << memberID << "\nName: " << name << std::endl;
    }
};

// Loan class
class Loan {
private:
    int bookID;
    int memberID;
    bool isActive;

public:
    Loan(int bID, int mID) : bookID(bID), memberID(mID), isActive(true) {}

    int getBookID() const { return bookID; }
    int getMemberID() const { return memberID; }
    bool getStatus() const { return isActive; }

    void closeLoan() { isActive = false; }

    void display() const {
        std::cout << "Book ID: " << bookID << "\nMember ID: " << memberID 
                  << "\nActive: " << (isActive ? "Yes" : "No") << std::endl;
    }
};

// Library class
class Library {
private:
    std::vector<Book> books;
    std::vector<Member> members;
    std::vector<Loan> loans;

public:
    void addBook(const Book& book) {
        books.push_back(book);
        std::cout << "Book added successfully.\n";
    }

    void addMember(const Member& member) {
        members.push_back(member);
        std::cout << "Member added successfully.\n";
    }

    void issueBook(int bookID, int memberID) {
    // Check if the member exists
    bool memberExists = false;
    for (const auto& member : members) {
        if (member.getID() == memberID) {
            memberExists = true;
            break;
        }
    }

    // If the member is not found, print a message and return
    if (!memberExists) {
        std::cerr << RED << "Error: Member not found. Please register the member first." << RESET << std::endl;
        return;
    }

    // Proceed with book issuance if the member exists
    for (auto& book : books) {
        if (book.getID() == bookID) {
            if (!book.getAvailability()) {
                throw std::runtime_error("Book is currently unavailable.");
            }
            book.setAvailability(false);
            loans.push_back(Loan(bookID, memberID));
            std::cout << GREEN << "Book issued successfully." << RESET << std::endl;
            return;
        }
    }

    // If the book is not found, throw an error
    throw std::runtime_error("Book not found.");
}


    void returnBook(int bookID, int memberID) {
        for (auto& loan : loans) {
            if (loan.getBookID() == bookID && loan.getMemberID() == memberID && loan.getStatus()) {
                loan.closeLoan();
                for (auto& book : books) {
                    if (book.getID() == bookID) {
                        book.setAvailability(true);
                        std::cout << "Book returned successfully.\n";
                        return;
                    }
                }
            }
        }
        throw std::runtime_error("No active loan found for the given book and member.");
    }

    void saveData() {
    std::ofstream file("library_data.txt");
    if (!file) {
        std::cerr << "Error opening file for saving data.\n";
        return;
    }

    // Save books
    file << "Books:\n";
    for (const auto& book : books) {
        file << book.getID() << "|" << book.getTitle() << "|" << book.getAuthor() 
             << "|" << (book.getAvailability() ? "1" : "0") << "\n";
    }

    // Save members
    file << "Members:\n";
    for (const auto& member : members) {
        file << member.getID() << "|" << member.getName() << "\n";
    }

    // Save loans
    file << "Loans:\n";
    for (const auto& loan : loans) {
        file << loan.getBookID() << "|" << loan.getMemberID() 
             << "|" << (loan.getStatus() ? "1" : "0") << "\n";
    }

    file.close();
    std::cout << "Data saved successfully.\n";
}


    void loadData() {
    std::ifstream file("library_data.txt");
    if (!file) {
        std::cerr << "Error opening file for loading data.\n";
        return;
    }
    
    std::string line;
    enum Section { NONE, BOOKS, MEMBERS, LOANS };
    Section currentSection = NONE;

    while (std::getline(file, line)) {
        if (line == "Books:") {
            currentSection = BOOKS;
            continue;
        } else if (line == "Members:") {
            currentSection = MEMBERS;
            continue;
        } else if (line == "Loans:") {
            currentSection = LOANS;
            continue;
        }

        if (currentSection == BOOKS) {
            // Parse book data
            int bookID;
            std::string title, author;
            bool isAvailable;

            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            size_t pos3 = line.find('|', pos2 + 1);

            bookID = std::stoi(line.substr(0, pos1));
            title = line.substr(pos1 + 1, pos2 - pos1 - 1);
            author = line.substr(pos2 + 1, pos3 - pos2 - 1);
            isAvailable = line.substr(pos3 + 1) == "1";

            books.push_back(Book(bookID, title, author));
            books.back().setAvailability(isAvailable);

        } else if (currentSection == MEMBERS) {
            // Parse member data
            int memberID;
            std::string name;

            size_t pos = line.find('|');
            memberID = std::stoi(line.substr(0, pos));
            name = line.substr(pos + 1);

            members.push_back(Member(memberID, name));

        } else if (currentSection == LOANS) {
            // Parse loan data
            int bookID, memberID;
            bool isActive;

            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);

            bookID = std::stoi(line.substr(0, pos1));
            memberID = std::stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
            isActive = line.substr(pos2 + 1) == "1";

            loans.push_back(Loan(bookID, memberID));
            if (!isActive) {
                loans.back().closeLoan();
            }
        }
    }

    file.close();
    std::cout << "Data loaded successfully.\n";
}


    void displayBooks() const {
        for (const auto& book : books) {
            book.display();
            std::cout << "-------------------------\n";
        }
    }

    void displayMembers() const {
        for (const auto& member : members) {
            member.display();
            std::cout << "-------------------------\n";
        }
    }

    void displayLoans() const {
        for (const auto& loan : loans) {
            loan.display();
            std::cout << "-------------------------\n";
        }
    }
};

// Main function with console interface
int main() {
    Library lib;
    int choice;

    do {
        // Styling the header
        std::cout << BOLD << BLUE << "\n===== Library Management System =====\n" << RESET;
        std::cout << BOLD << UNDERLINE << "Menu Options:\n" << RESET;

        // Displaying options with some color and formatting
        std::cout << CYAN << "1. Add Book\n" << RESET;
        std::cout << CYAN << "2. Add Member\n" << RESET;
        std::cout << CYAN << "3. Issue Book\n" << RESET;
        std::cout << CYAN << "4. Return Book\n" << RESET;
        std::cout << CYAN << "5. Display All Books\n" << RESET;
        std::cout << CYAN << "6. Display All Members\n" << RESET;
        std::cout << CYAN << "7. Display All Loans\n" << RESET;
        std::cout << CYAN << "8. Save Data\n" << RESET;
        std::cout << CYAN << "9. Load Data\n" << RESET;
        std::cout << CYAN << "0. Exit\n" << RESET;

        std::cout << BOLD << "Enter your choice: " << RESET;
        std::cin >> choice;

        // Handling invalid input
        if (std::cin.fail()) {
            std::cin.clear();
          //  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << RED << "Invalid input! Please enter a number.\n" << RESET;
            continue;
        }

        switch (choice) {
        case 1: {
            int id;
            std::string title, author;
            std::cout << BOLD << GREEN << "\nAdding a New Book\n" << RESET;
            std::cout << "Enter Book ID: ";
            std::cin >> id;
            std::cin.ignore(); // To clear newline from the input buffer
            std::cout << "Enter Book Title: ";
            std::getline(std::cin, title);
            std::cout << "Enter Author Name: ";
            std::getline(std::cin, author);
            lib.addBook(Book(id, title, author));
            break;
        }
        case 2: {
            int id;
            std::string name;
            std::cout << BOLD << GREEN << "\nAdding a New Member\n" << RESET;
            std::cout << "Enter Member ID: ";
            std::cin >> id;
            std::cin.ignore(); // To clear newline from the input buffer
            std::cout << "Enter Member Name: ";
            std::getline(std::cin, name);
            lib.addMember(Member(id, name));
            break;
        }
        case 3: {
            int bookID, memberID;
            std::cout << BOLD << GREEN << "\nIssuing a Book\n" << RESET;
            std::cout << "Enter Book ID to Issue: ";
            std::cin >> bookID;
            std::cout << "Enter Member ID: ";
            std::cin >> memberID;
            try {
                lib.issueBook(bookID, memberID);
            } catch (const std::exception& e) {
                std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
            }
            break;
        }
        case 4: {
            int bookID, memberID;
            std::cout << BOLD << GREEN << "\nReturning a Book\n" << RESET;
            std::cout << "Enter Book ID to Return: ";
            std::cin >> bookID;
            std::cout << "Enter Member ID: ";
            std::cin >> memberID;
            try {
                lib.returnBook(bookID, memberID);
            } catch (const std::exception& e) {
                std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
            }
            break;
        }
        case 5: {
            std::cout << BOLD << GREEN << "\nDisplaying All Books\n" << RESET;
            lib.displayBooks();
            break;
        }
        case 6: {
            std::cout << BOLD << GREEN << "\nDisplaying All Members\n" << RESET;
            lib.displayMembers();
            break;
        }
        case 7: {
            std::cout << BOLD << GREEN << "\nDisplaying All Loans\n" << RESET;
            lib.displayLoans();
            break;
        }
        case 8: {
            std::cout << BOLD << GREEN << "\nSaving Data...\n" << RESET;
            lib.saveData();
            break;
        }
        case 9: {
            std::cout << BOLD << GREEN << "\nLoading Data...\n" << RESET;
            lib.loadData();
            break;
        }
        case 0: {
            std::cout << BOLD << GREEN << "Exiting the system. Goodbye!\n" << RESET;
            break;
        }
        default:
            std::cout << RED << "Invalid choice! Please try again.\n" << RESET;
        }
    } while (choice != 0);

    return 0;
}
