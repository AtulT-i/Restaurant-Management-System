#include "RestaurantSystem.h"

// --- Utility function for string splitting ---
vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// --- MenuItem ---
MenuItem::MenuItem() {}
MenuItem::MenuItem(string cat, string n, double p) : category(cat), name(n), price(p) {}

// --- Order ---
Order::Order(int id, string mobile, int wId) : orderId(id), mobileNumber(mobile), waiterId(wId), totalAmount(0.0) {}

void Order::addItem(const MenuItem& item) {
    items.push_back(item);
    totalAmount += item.price;
}

void Order::displayOrder() const {
    cout << "Order ID: " << orderId << " | Total: $" << fixed << setprecision(2) << totalAmount;
    if (waiterId != 0) {
        cout << " | Served By Waiter ID: " << waiterId;
    }
    cout << "\n";
    for (const auto& item : items) {
        cout << "  - " << item.name << " ($" << item.price << ")\n";
    }
}

// --- User ---
User::User() {}
User::User(string n, string m) : name(n), mobileNumber(m) {}

void User::addOrder(const Order& order) {
    orderHistory.push_back(order);
}

void User::displayProfile() const {
    cout << "\n--- User Profile ---\n";
    cout << "Name: " << name << "\n";
    cout << "Mobile: " << mobileNumber << "\n";
    cout << "\n--- Order History ---\n";
    if (orderHistory.empty()) {
        cout << "No past orders.\n";
    } else {
        for (const auto& o : orderHistory) {
            o.displayOrder();
        }
    }
    cout << "--------------------\n";
}

// --- Waiter ---
Waiter::Waiter() {}
Waiter::Waiter(int id, string n) : id(id), name(n) {}

void Waiter::addOrder(const Order& order) {
    orderHistory.push_back(order);
}

void Waiter::displayProfile() const {
    cout << "\n--- Waiter Profile ---\n";
    cout << "ID: " << id << "\n";
    cout << "Name: " << name << "\n";
    cout << "\n--- Served Orders History ---\n";
    if (orderHistory.empty()) {
        cout << "No past orders served.\n";
    } else {
        for (const auto& o : orderHistory) {
            o.displayOrder();
        }
    }
    cout << "--------------------\n";
}

// --- MenuManager ---
void MenuManager::loadMenu(const string& filename) {
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        vector<string> data = split(line, '|');
        if (data.size() == 3) {
            MenuItem item(data[0], data[1], stod(data[2]));
            menuItems.push_back(item);
        }
    }
    file.close();
}

void MenuManager::displayMenu() const {
    cout << "\n===== RESTAURANT MENU =====\n";
    string currentCategory = "";
    int index = 1;
    for (const auto& item : menuItems) {
        if (item.category != currentCategory) {
            currentCategory = item.category;
            cout << "\n[" << currentCategory << "]\n";
        }
        cout << index++ << ". " << left << setw(25) << item.name 
             << " $" << fixed << setprecision(2) << item.price << "\n";
    }
    cout << "===========================\n";
}

MenuItem MenuManager::getItem(int index) const {
    if (index > 0 && index <= menuItems.size()) {
        return menuItems[index - 1]; // Return correct item, ignoring category separation internally
    }
    return MenuItem("", "", -1); // Invalid item
}

int MenuManager::getMenuSize() const {
    return menuItems.size();
}

// --- RestaurantSystem ---
RestaurantSystem::RestaurantSystem() {
    nextOrderId = 1;
    loadData();
}

RestaurantSystem::~RestaurantSystem() {
    saveData();
}

RestaurantSystem& RestaurantSystem::getInstance() {
    static RestaurantSystem instance;
    return instance;
}

void RestaurantSystem::loadData() {
    menuManager.loadMenu("menu.txt");

    // Load Users
    ifstream userFile("users.txt");
    string line;
    while (getline(userFile, line)) {
        vector<string> data = split(line, '|');
        if (data.size() == 2) {
            users[data[0]] = User(data[1], data[0]); // mobile, name
        }
    }
    userFile.close();

    // Load Waiters
    ifstream waiterFile("waiters.txt");
    while (getline(waiterFile, line)) {
        vector<string> data = split(line, '|');
        if (data.size() == 2) {
            waiters[stoi(data[0])] = Waiter(stoi(data[0]), data[1]);
        }
    }
    waiterFile.close();

    // Load Orders
    ifstream orderFile("orders.txt");
    while (getline(orderFile, line)) {
        vector<string> data = split(line, '|');
        if (data.size() >= 4) {
            int oId = stoi(data[0]);
            string mobile = data[1];
            int wId = 0;
            string itemName;
            double price;

            // Handle both older format (no waiter id) and new format
            if (data.size() >= 5) {
                wId = stoi(data[2]);
                itemName = data[3];
                price = stod(data[4]);
            } else {
                itemName = data[2];
                price = stod(data[3]);
            }
            
            // Keep track of the max order ID
            if (oId >= nextOrderId) nextOrderId = oId + 1;

            if (users.find(mobile) != users.end()) {
                bool found = false;
                for (auto& order : users[mobile].orderHistory) {
                    if (order.orderId == oId) {
                        order.addItem(MenuItem("", itemName, price));
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    Order newOrder(oId, mobile, wId);
                    newOrder.addItem(MenuItem("", itemName, price));
                    users[mobile].addOrder(newOrder);
                }
            }

            if (wId != 0 && waiters.find(wId) != waiters.end()) {
                bool found = false;
                for (auto& order : waiters[wId].orderHistory) {
                    if (order.orderId == oId) {
                        order.addItem(MenuItem("", itemName, price));
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    Order newOrder(oId, mobile, wId);
                    newOrder.addItem(MenuItem("", itemName, price));
                    waiters[wId].addOrder(newOrder);
                }
            }
        }
    }
    orderFile.close();
}

void RestaurantSystem::saveData() {
    // Save Users
    ofstream userFile("users.txt");
    for (const auto& pair : users) {
        userFile << pair.second.mobileNumber << "|" << pair.second.name << "\n";
    }
    userFile.close();

    // Save Waiters
    ofstream waiterFile("waiters.txt");
    for (const auto& pair : waiters) {
        waiterFile << pair.second.id << "|" << pair.second.name << "\n";
    }
    waiterFile.close();

    // Save Orders
    ofstream orderFile("orders.txt");
    for (const auto& pair : users) {
        for (const auto& order : pair.second.orderHistory) {
            for (const auto& item : order.items) {
                orderFile << order.orderId << "|" << pair.first << "|" << order.waiterId << "|" << item.name << "|" << item.price << "\n";
            }
        }
    }
    orderFile.close();
}

void RestaurantSystem::registerUser() {
    string name, mobile;
    cout << "\nEnter Mobile Number: "; cin >> mobile;
    if (users.find(mobile) != users.end()) {
        cout << "User already exists! Please login instead.\n";
        return;
    }
    cin.ignore();
    cout << "Enter Name: "; getline(cin, name);
    users[mobile] = User(name, mobile);
    cout << "Profile created successfully!\n";
}

User* RestaurantSystem::loginUser() {
    string mobile;
    cout << "\nEnter Mobile Number to Login: "; cin >> mobile;
    if (users.find(mobile) != users.end()) {
        cout << "Welcome back, " << users[mobile].name << "!\n";
        return &users[mobile];
    }
    cout << "User not found. Please register first.\n";
    return nullptr;
}

void RestaurantSystem::placeOrderForUser(User* user, Waiter* waiter) {
    menuManager.displayMenu();
    int wId = (waiter != nullptr) ? waiter->id : 0;
    Order newOrder(nextOrderId++, user->mobileNumber, wId);
    int choice;

    while (true) {
        cout << "\nEnter Item Number to add to order (0 to finish): ";
        cin >> choice;
        if (choice == 0) break;

        MenuItem item = menuManager.getItem(choice);
        if (item.price != -1) {
            newOrder.addItem(item);
            cout << item.name << " added to order.\n";
        } else {
            cout << "Invalid item number!\n";
        }
    }

    if (newOrder.items.size() > 0) {
        user->addOrder(newOrder);
        if (waiter != nullptr) {
            waiter->addOrder(newOrder);
        }
        allOrders.push_back(newOrder);
        cout << "\nOrder placed successfully!\n";
        newOrder.displayOrder();
        saveData(); // Save instantly
    } else {
        cout << "\nOrder cancelled (no items selected).\n";
        nextOrderId--; // rollback ID
    }
}

void RestaurantSystem::customerFlow() {
    User* user = nullptr;
    int choice;
    do {
        cout << "\n--- Customer Menu ---\n";
        cout << "1. Register\n2. Login\n0. Back\nChoice: ";
        cin >> choice;
        if (choice == 1) registerUser();
        else if (choice == 2) user = loginUser();

        if (user != nullptr) break;
    } while (choice != 0);

    if (user == nullptr) return;

    do {
        cout << "\n--- Hello " << user->name << " ---\n";
        cout << "1. View Menu\n2. Place Order\n3. View Profile & Order History\n0. Log out\nChoice: ";
        cin >> choice;

        switch (choice) {
            case 1: menuManager.displayMenu(); break;
            case 2: placeOrderForUser(user); break;
            case 3: user->displayProfile(); break;
            case 0: cout << "Logged out.\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void RestaurantSystem::waiterFlow() {
    int wId;
    cout << "\n--- Waiter Login ---\n";
    cout << "Enter Waiter ID (or 0 to register as new waiter): ";
    cin >> wId;

    if (wId == 0) {
        string wName;
        cin.ignore();
        cout << "Enter Waiter Name: ";
        getline(cin, wName);
        wId = waiters.empty() ? 101 : waiters.rbegin()->first + 1; // start IDs from 101
        waiters[wId] = Waiter(wId, wName);
        cout << "Registered successfully! Your new Waiter ID is " << wId << "\n";
        saveData();
    } else if (waiters.find(wId) == waiters.end()) {
        cout << "Waiter ID not found!\n";
        return;
    }

    Waiter* waiter = &waiters[wId];
    cout << "\nWelcome, Waiter " << waiter->name << " (ID: " << waiter->id << ")\n";

    string mobile;
    cout << "\n--- Waiter Terminal ---\n";
    cout << "Enter Customer Mobile Number: "; cin >> mobile;

    if (users.find(mobile) == users.end()) {
        cout << "Customer not found. Creating a quick profile...\n";
        string name;
        cin.ignore();
        cout << "Enter Customer Name: "; getline(cin, name);
        users[mobile] = User(name, mobile);
        saveData();
    }

    User* user = &users[mobile];
    cout << "Serving customer: " << user->name << "\n";
    int choice;
    do {
        cout << "\n1. View Menu\n2. Place Order for Customer\n3. View My Served Orders\n0. Back\nChoice: ";
        cin >> choice;
        switch(choice) {
            case 1: menuManager.displayMenu(); break;
            case 2: placeOrderForUser(user, waiter); break;
            case 3: waiter->displayProfile(); break;
        }
    } while (choice != 0);
}

void RestaurantSystem::start() {
    int choice;
    do {
        cout << "\n=== RESTAURANT MANAGEMENT SYSTEM ===\n";
        cout << "1. Customer Portal\n";
        cout << "2. Waiter Portal\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: customerFlow(); break;
            case 2: waiterFlow(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 0);
}
