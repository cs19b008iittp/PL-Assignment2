#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

// Room Base Class
class Room {
protected:
    int roomNumber;
    std::string roomType;
    bool isAvailable;

public:
    Room(int roomNumber, const std::string& roomType) : roomNumber(roomNumber), roomType(roomType), isAvailable(true) {}

    virtual ~Room() {}

    int getRoomNumber() const { return roomNumber; }
    std::string getRoomType() const { return roomType; }
    bool getAvailability() const { return isAvailable; }

    void setAvailability(bool available) { isAvailable = available; }

    virtual void displayDetails() const {
        std::cout << "Room Number: " << roomNumber << " | Type: " << roomType << " | " 
                  << (isAvailable ? "Available" : "Not Available") << std::endl;
    }
};

// Derived Classes for Different Room Types
class SingleRoom : public Room {
public:
    SingleRoom(int roomNumber) : Room(roomNumber, "Single") {}
};

class DoubleRoom : public Room {
public:
    DoubleRoom(int roomNumber) : Room(roomNumber, "Double") {}
};

class SuiteRoom : public Room {
public:
    SuiteRoom(int roomNumber) : Room(roomNumber, "Suite") {}
};

class Customer {
    int customerID;
    std::string name;

public:
    Customer(int id, const std::string& name) : customerID(id), name(name) {}

    int getID() const { return customerID; }
    std::string getName() const { return name; }
};

// Booking Class
class Booking {
    int roomNumber;
    int customerID;
    bool isActive;

public:
    Booking(int roomNumber, int customerID) : roomNumber(roomNumber), customerID(customerID), isActive(true) {}

    int getRoomNumber() const { return roomNumber; }
    int getCustomerID() const { return customerID; }
    bool getStatus() const { return isActive; }

    void cancelBooking() { isActive = false; }
};

// Hotel Class
class Hotel {
    std::vector<Room*> rooms;
    std::vector<Customer> customers;
    std::vector<Booking> bookings;

public:
    ~Hotel() {
        for (auto room : rooms) {
            delete room;
        }
    }

    void addRoom(Room* room) {
        rooms.push_back(room);
    }

    void addCustomer(const Customer& customer) {
        customers.push_back(customer);
    }

    bool customerExists(int customerID) const {
        for (const auto& customer : customers) {
            if (customer.getID() == customerID) {
                return true;
            }
        }
        return false;
    }

    void showCustomers() const {
        std::cout << "\nList of Customers:\n";
        for (const auto& customer : customers) {
            std::cout << "Customer ID: " << customer.getID() << " | Name: " << customer.getName() << std::endl;
        }
    }

    void bookRoom(int roomNumber, int customerID) {
        if (!customerExists(customerID)) {
            std::cerr << "Error: Customer ID " << customerID << " does not exist.\n";
            return;
        }

        for (auto& room : rooms) {
            if (room->getRoomNumber() == roomNumber) {
                if (room->getAvailability()) {
                    room->setAvailability(false);
                    bookings.push_back(Booking(roomNumber, customerID));
                    std::cout << "Room " << roomNumber << " booked successfully for Customer ID " << customerID << ".\n";
                    return;
                } else {
                    throw std::runtime_error("Room is not available.");
                }
            }
        }
        throw std::runtime_error("Room not found.");
    }

    void cancelBooking(int roomNumber, int customerID) {
        for (auto& booking : bookings) {
            if (booking.getRoomNumber() == roomNumber && booking.getCustomerID() == customerID && booking.getStatus()) {
                booking.cancelBooking();
                for (auto& room : rooms) {
                    if (room->getRoomNumber() == roomNumber) {
                        room->setAvailability(true);
                        std::cout << "Booking canceled successfully.\n";
                        return;
                    }
                }
            }
        }
        throw std::runtime_error("Booking not found or already canceled.");
    }

    void checkAvailability() const {
        std::cout << "\nRoom Availability:\n";
        for (const auto& room : rooms) {
            room->displayDetails();
        }
    }

    void saveData() {
        std::ofstream file("hotel_data.txt");
        if (!file) {
            std::cerr << "Error opening file for saving data.\n";
            return;
        }

        // Save rooms
        file << "Rooms:\n";
        for (const auto& room : rooms) {
            file << room->getRoomNumber() << "|" << room->getRoomType() << "|" 
                 << (room->getAvailability() ? "1" : "0") << "\n";
        }

        // Save customers
        file << "Customers:\n";
        for (const auto& customer : customers) {
            file << customer.getID() << "|" << customer.getName() << "\n";
        }

        // Save bookings
        file << "Bookings:\n";
        for (const auto& booking : bookings) {
            file << booking.getRoomNumber() << "|" << booking.getCustomerID() 
                 << "|" << (booking.getStatus() ? "1" : "0") << "\n";
        }

        file.close();
        std::cout << "Data saved successfully.\n";
    }

    void loadData() {
        std::ifstream file("hotel_data.txt");
        if (!file) {
            std::cerr << "Error opening file for loading data.\n";
            return;
        }

        std::string line;
        enum Section { NONE, ROOMS, CUSTOMERS, BOOKINGS };
        Section currentSection = NONE;

        while (std::getline(file, line)) {
            if (line == "Rooms:") {
                currentSection = ROOMS;
                continue;
            } else if (line == "Customers:") {
                currentSection = CUSTOMERS;
                continue;
            } else if (line == "Bookings:") {
                currentSection = BOOKINGS;
                continue;
            }

            if (currentSection == ROOMS) {
                // Parse room data
                int roomNumber;
                std::string roomType;
                bool isAvailable;

                size_t pos1 = line.find('|');
                size_t pos2 = line.find('|', pos1 + 1);
                size_t pos3 = line.find('|', pos2 + 1);

                roomNumber = std::stoi(line.substr(0, pos1));
                roomType = line.substr(pos1 + 1, pos2 - pos1 - 1);
                isAvailable = line.substr(pos3 + 1) == "1";

                if (roomType == "Single") {
                    rooms.push_back(new SingleRoom(roomNumber));
                } else if (roomType == "Double") {
                    rooms.push_back(new DoubleRoom(roomNumber));
                } else if (roomType == "Suite") {
                    rooms.push_back(new SuiteRoom(roomNumber));
                }
                rooms.back()->setAvailability(isAvailable);

            } else if (currentSection == CUSTOMERS) {
                // Parse customer data
                int customerID;
                std::string name;

                size_t pos = line.find('|');
                customerID = std::stoi(line.substr(0, pos));
                name = line.substr(pos + 1);

                customers.push_back(Customer(customerID, name));

            } else if (currentSection == BOOKINGS) {
                // Parse booking data
                int roomNumber, customerID;
                bool isActive;

                size_t pos1 = line.find('|');
                size_t pos2 = line.find('|', pos1 + 1);

                roomNumber = std::stoi(line.substr(0, pos1));
                customerID = std::stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
                isActive = line.substr(pos2 + 1) == "1";

                bookings.push_back(Booking(roomNumber, customerID));
                if (!isActive) {
                    bookings.back().cancelBooking();
                }
            }
        }

        file.close();
        std::cout << "Data loaded successfully.\n";
    }
};

// Main Function with Console Interface
int main() {
    Hotel hotel;
    int choice;

    do {
        std::cout << "\n===== Hotel Booking System =====\n";
        std::cout << "1. Add Room\n";
        std::cout << "2. Add Customer\n";
        std::cout << "3. Book Room\n";
        std::cout << "4. Cancel Booking\n";
        std::cout << "5. Check Room Availability\n";
        std::cout << "6. Show List of Customers\n";
        std::cout << "7. Save Data\n";
        std::cout << "8. Load Data\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            int roomNumber;
            std::string roomType;
            std::cout << "Enter Room Number: ";
            std::cin >> roomNumber;
                        std::cout << "Enter Room Type (Single/Double/Suite): ";
            std::cin >> roomType;

            if (roomType == "Single") {
                hotel.addRoom(new SingleRoom(roomNumber));
            } else if (roomType == "Double") {
                hotel.addRoom(new DoubleRoom(roomNumber));
            } else if (roomType == "Suite") {
                hotel.addRoom(new SuiteRoom(roomNumber));
            } else {
                std::cout << "Invalid room type!\n";
            }
            break;
        }
        case 2: {
            int customerID;
            std::string name;
            std::cout << "Enter Customer ID: ";
            std::cin >> customerID;
            std::cin.ignore(); // Clear the newline character from the buffer
            std::cout << "Enter Customer Name: ";
            std::getline(std::cin, name);
            hotel.addCustomer(Customer(customerID, name));
            break;
        }
        case 3: {
            int roomNumber, customerID;
            std::cout << "Enter Room Number: ";
            std::cin >> roomNumber;
            std::cout << "Enter Customer ID: ";
            std::cin >> customerID;
            try {
                hotel.bookRoom(roomNumber, customerID);
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
            break;
        }
        case 4: {
            int roomNumber, customerID;
            std::cout << "Enter Room Number: ";
            std::cin >> roomNumber;
            std::cout << "Enter Customer ID: ";
            std::cin >> customerID;
            try {
                hotel.cancelBooking(roomNumber, customerID);
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
            break;
        }
        case 5:
            hotel.checkAvailability();
            break;
        case 6:
            hotel.showCustomers();
            break;
        case 7:
            hotel.saveData();
            break;
        case 8:
            hotel.loadData();
            break;
        case 0:
            std::cout << "Exiting...\n";
            break;
        default:
            std::cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}

