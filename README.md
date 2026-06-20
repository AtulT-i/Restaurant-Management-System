# Restaurant Management System (C++)

A terminal-based restaurant management system built in C++ using Object-Oriented Programming (OOP) principles, File I/O for database simulation, and the Singleton design pattern.

## 🌟 Features
- **Customer & Waiter Portals**: Distinct application flows for customers to order themselves and for waiters to manage orders for customers.
- **User Authentication**: Simple login and registration system using mobile numbers as unique identifiers.
- **Order Tracking**: Comprehensive order history tracking for both customers and waiters.
- **Dynamic Menu**: Real-time loading and categorized display of the restaurant menu.
- **Persistent Data**: Order, user, and menu data are saved to local text files (`users.txt`, `waiters.txt`, `orders.txt`, `menu.txt`) avoiding data loss between application restarts.

## 🏗️ Technical Concepts Used
- **Object-Oriented Programming (OOP)**: Use of classes (`User`, `Waiter`, `Order`, `MenuItem`, `MenuManager`) for encapsulation and cleaner architecture.
- **Singleton Design Pattern**: The core `RestaurantSystem` allows only one central instance to manage memory effectively and prevent concurrent file writes.
- **Data Structures**: Extensive use of standard template library (STL) structures like `std::vector` (for dynamic ordered lists) and `std::map` (for fast ID lookups).
- **File Management (`fstream`)**: File Streams are used to read structured data upon initialization and write/persist data automatically.

## 🚀 How to Run

### Prerequisites
Make sure you have a C++ compiler installed (like `g++` via MinGW on Windows, or standard `clang`/`gcc` on macOS/Linux).

### Compilation
Navigate to the project root directory and compile the source files:
```bash
g++ main.cpp RestaurantSystem.cpp -o restaurant
```

### Execution
Run the compiled executable:
- **Windows**: `.\restaurant.exe`
- **Linux/Mac**: `./restaurant`

## 📂 Project Structure
- `main.cpp` - Entry point to launch the application.
- `RestaurantSystem.h` - Header file containing class definitions.
- `RestaurantSystem.cpp` - Implementation of the classes and their logic.
- `*.txt` - Simple text-based databases acting as persistent storage.