#ifndef RESTAURANT_SYSTEM_H
#define RESTAURANT_SYSTEM_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

using namespace std;

// --- Utility function for string splitting ---
vector<string> split(const string &s, char delimiter);

// --- Classes ---

class MenuItem {
public:
    string category;
    string name;
    double price;

    MenuItem();
    MenuItem(string cat, string n, double p);
};

class Order {
public:
    int orderId;
    string mobileNumber;
    int waiterId;
    vector<MenuItem> items;
    double totalAmount;

    Order(int id, string mobile, int wId = 0);

    void addItem(const MenuItem& item);
    void displayOrder() const;
};

class User {
public:
    string name;
    string mobileNumber; // Unique ID
    vector<Order> orderHistory;

    User();
    User(string n, string m);

    void addOrder(const Order& order);
    void displayProfile() const;
};

class Waiter {
public:
    int id;
    string name;
    vector<Order> orderHistory;

    Waiter();
    Waiter(int id, string n);

    void addOrder(const Order& order);
    void displayProfile() const;
};

class MenuManager {
private:
    vector<MenuItem> menuItems;

public:
    void loadMenu(const string& filename);
    void displayMenu() const;
    MenuItem getItem(int index) const;
    int getMenuSize() const;
};

class RestaurantSystem {
private:
    map<string, User> users;
    map<int, Waiter> waiters;
    vector<Order> allOrders;
    MenuManager menuManager;
    int nextOrderId;

    // Private constructor and destructor for Singleton
    RestaurantSystem();
    ~RestaurantSystem();

    // Prevent copy constructor and assignment operator
    RestaurantSystem(const RestaurantSystem&) = delete;
    RestaurantSystem& operator=(const RestaurantSystem&) = delete;

public:
    // Singleton access method
    static RestaurantSystem& getInstance();

    void loadData();
    void saveData();
    void registerUser();
    User* loginUser();
    void placeOrderForUser(User* user, Waiter* waiter = nullptr);
    void customerFlow();
    void waiterFlow();
    void start();
};

#endif
