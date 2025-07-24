#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <limits>
#include <climits>
#include <ctime>

using namespace std;

// Structure to store book information
struct Book {
    string id;
    string title;
    string author;
    string category;
    double price;
    int quantity;
    string dateAdded;
};

// Structure to store sales information
struct Sale {
    string saleId;
    string bookId;
    string bookTitle;
    int quantity;
    double totalAmount;
    string date;
    string customerName;
};

// Structure to store user information
struct User {
    string username;
    string password;
    string role;
};

class BookshopManager {
private:
    vector<Book> books;
    vector<Sale> sales;
    vector<User> users;
    bool isLoggedIn;
    string currentUser;
    string currentRole;
    
    // File names for data storage
    const string BOOKS_FILE = "books.txt";
    const string SALES_FILE = "sales.txt";
    const string USERS_FILE = "users.txt";

public:
    BookshopManager() : isLoggedIn(false), currentUser(""), currentRole("") {
        initializeDefaultUsers();
        loadData();
    }

    // Initialize default users
    void initializeDefaultUsers() {
        users.clear();
        users.push_back({"admin", "admin123", "admin"});
        users.push_back({"staff", "staff123", "staff"});
        users.push_back({"manager", "manager123", "manager"});
        saveUsers();
    }

    // Get current date and time
    string getCurrentDateTime() {
        time_t now = time(0);
        char* dt = ctime(&now);
        string dateTime = dt;
        dateTime.pop_back(); // Remove newline character
        return dateTime;
    }

    // Clear screen function
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    // Input validation functions
    int getValidatedInt(const string& prompt, int min = 0, int max = INT_MAX) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value && value >= min && value <= max) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            } else {
                cout << "Invalid input! Please enter a number between " << min << " and " << max << ".\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
    }

    double getValidatedDouble(const string& prompt, double min = 0.0) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value && value >= min) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            } else {
                cout << "Invalid input! Please enter a valid number >= " << min << ".\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
    }

    string getValidatedString(const string& prompt) {
        string value;
        while (true) {
            cout << prompt;
            getline(cin, value);
            if (!value.empty()) {
                return value;
            }
            cout << "Input cannot be empty! Please try again.\n";
        }
    }

    // File I/O functions
    void loadData() {
        loadBooks();
        loadSales();
        loadUsers();
    }

    void saveData() {
        saveBooks();
        saveSales();
        saveUsers();
    }

    void loadBooks() {
        ifstream file(BOOKS_FILE);
        books.clear();
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    stringstream ss(line);
                    Book book;
                    getline(ss, book.id, '|');
                    getline(ss, book.title, '|');
                    getline(ss, book.author, '|');
                    getline(ss, book.category, '|');
                    ss >> book.price;
                    ss.ignore();
                    ss >> book.quantity;
                    ss.ignore();
                    getline(ss, book.dateAdded);
                    books.push_back(book);
                }
            }
            file.close();
        }
    }

    void saveBooks() {
        ofstream file(BOOKS_FILE);
        if (file.is_open()) {
            for (const auto& book : books) {
                file << book.id << "|" << book.title << "|" << book.author << "|"
                     << book.category << "|" << book.price << "|" << book.quantity
                     << "|" << book.dateAdded << "\n";
            }
            file.close();
        }
    }

    void loadSales() {
        ifstream file(SALES_FILE);
        sales.clear();
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    stringstream ss(line);
                    Sale sale;
                    getline(ss, sale.saleId, '|');
                    getline(ss, sale.bookId, '|');
                    getline(ss, sale.bookTitle, '|');
                    ss >> sale.quantity;
                    ss.ignore();
                    ss >> sale.totalAmount;
                    ss.ignore();
                    getline(ss, sale.date, '|');
                    getline(ss, sale.customerName);
                    sales.push_back(sale);
                }
            }
            file.close();
        }
    }

    void saveSales() {
        ofstream file(SALES_FILE);
        if (file.is_open()) {
            for (const auto& sale : sales) {
                file << sale.saleId << "|" << sale.bookId << "|" << sale.bookTitle
                     << "|" << sale.quantity << "|" << sale.totalAmount << "|"
                     << sale.date << "|" << sale.customerName << "\n";
            }
            file.close();
        }
    }

    void loadUsers() {
        ifstream file(USERS_FILE);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    stringstream ss(line);
                    User user;
                    getline(ss, user.username, '|');
                    getline(ss, user.password, '|');
                    getline(ss, user.role);
                    
                    // Check if user already exists in default users
                    bool exists = false;
                    for (const auto& existingUser : users) {
                        if (existingUser.username == user.username) {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists) {
                        users.push_back(user);
                    }
                }
            }
            file.close();
        }
    }

    void saveUsers() {
        ofstream file(USERS_FILE);
        if (file.is_open()) {
            for (const auto& user : users) {
                file << user.username << "|" << user.password << "|" << user.role << "\n";
            }
            file.close();
        }
    }

    // Authentication functions
    bool login() {
        clearScreen();
        cout << "\n" << string(60, '=') << "\n";
        cout << "           GENIUS BOOKS - LOGIN SYSTEM\n";
        cout << string(60, '=') << "\n\n";
        
        string username, password;
        cout << "Enter Username: ";
        getline(cin, username);
        cout << "Enter Password: ";
        getline(cin, password);
        
        for (const auto& user : users) {
            if (user.username == username && user.password == password) {
                isLoggedIn = true;
                currentUser = username;
                currentRole = user.role;
                cout << "\n✓ Login successful! Welcome, " << username << "!\n";
                cout << "Role: " << user.role << "\n";
                cout << "Press Enter to continue...";
                cin.get();
                return true;
            }
        }
        
        cout << "\n✗ Invalid username or password!\n";
        cout << "Press Enter to continue...";
        cin.get();
        return false;
    }

    void logout() {
        isLoggedIn = false;
        currentUser = "";
        currentRole = "";
        cout << "\n✓ Logged out successfully!\n";
        cout << "Press Enter to continue...";
        cin.get();
    }

    // Book management functions
    void viewBooks() {
        clearScreen();
        cout << "\n" << string(80, '=') << "\n";
        cout << "                    AVAILABLE BOOKS\n";
        cout << string(80, '=') << "\n";
        
        if (books.empty()) {
            cout << "\nNo books available in the system.\n";
            cout << "Press Enter to continue...";
            cin.get();
            return;
        }
        
        cout << left << setw(8) << "ID" << setw(25) << "Title" << setw(20) << "Author"
             << setw(15) << "Category" << setw(10) << "Price" << setw(8) << "Qty"
             << "Date Added\n";
        cout << string(80, '-') << "\n";
        
        for (const auto& book : books) {
            cout << left << setw(8) << book.id << setw(25) << book.title.substr(0, 24)
                 << setw(20) << book.author.substr(0, 19) << setw(15) << book.category.substr(0, 14)
                 << setw(10) << fixed << setprecision(2) << book.price
                 << setw(8) << book.quantity << book.dateAdded << "\n";
        }
        
        cout << "\nTotal Books: " << books.size() << "\n";
        cout << "Press Enter to continue...";
        cin.get();
    }

    void addBook() {
        clearScreen();
        cout << "\n" << string(60, '=') << "\n";
        cout << "                ADD NEW BOOK\n";
        cout << string(60, '=') << "\n\n";
        
        Book newBook;
        newBook.id = getValidatedString("Enter Book ID: ");
        
        // Check if book ID already exists
        for (const auto& book : books) {
            if (book.id == newBook.id) {
                cout << "\n✗ Book ID already exists! Please use a different ID.\n";
                cout << "Press Enter to continue...";
                cin.get();
                return;
            }
        }
        
        newBook.title = getValidatedString("Enter Book Title: ");
        newBook.author = getValidatedString("Enter Author Name: ");
        newBook.category = getValidatedString("Enter Category: ");
        newBook.price = getValidatedDouble("Enter Price: $", 0.01);
        newBook.quantity = getValidatedInt("Enter Quantity: ", 0);
        newBook.dateAdded = getCurrentDateTime();
        
        books.push_back(newBook);
        saveBooks();
        
        cout << "\n✓ Book added successfully!\n";
        cout << "Press Enter to continue...";
        cin.get();
    }

    void updateBook() {
        clearScreen();
        cout << "\n" << string(60, '=') << "\n";
        cout << "                UPDATE BOOK\n";
        cout << string(60, '=') << "\n\n";
        
        string bookId = getValidatedString("Enter Book ID to update: ");
        
        auto it = find_if(books.begin(), books.end(),
                         [&bookId](const Book& book) { return book.id == bookId; });
        
        if (it == books.end()) {
            cout << "\n✗ Book not found!\n";
            cout << "Press Enter to continue...";
            cin.get();
            return;
        }
        
        cout << "\nCurrent Book Details:\n";
        cout << "ID: " << it->id << "\n";
        cout << "Title: " << it->title << "\n";
        cout << "Author: " << it->author << "\n";
        cout << "Category: " << it->category << "\n";
        cout << "Price: $" << fixed << setprecision(2) << it->price << "\n";
        cout << "Quantity: " << it->quantity << "\n\n";
        
        int choice = getValidatedInt("What would you like to update?\n1. Title\n2. Author\n3. Category\n4. Price\n5. Quantity\n6. All Details\nEnter choice: ", 1, 6);
        
        switch (choice) {
            case 1:
                it->title = getValidatedString("Enter new title: ");
                break;
            case 2:
                it->author = getValidatedString("Enter new author: ");
                break;
            case 3:
                it->category = getValidatedString("Enter new category: ");
                break;
            case 4:
                it->price = getValidatedDouble("Enter new price: $", 0.01);
                break;
            case 5:
                it->quantity = getValidatedInt("Enter new quantity: ", 0);
                break;
            case 6:
                it->title = getValidatedString("Enter new title: ");
                it->author = getValidatedString("Enter new author: ");
                it->category = getValidatedString("Enter new category: ");
                it->price = getValidatedDouble("Enter new price: $", 0.01);
                it->quantity = getValidatedInt("Enter new quantity: ", 0);
                break;
        }
        
        saveBooks();
        cout << "\n✓ Book updated successfully!\n";
        cout << "Press Enter to continue...";
        cin.get();
    }

    void deleteBook() {
        clearScreen();
        cout << "\n" << string(60, '=') << "\n";
        cout << "                DELETE BOOK\n";
        cout << string(60, '=') << "\n\n";
        
        string bookId = getValidatedString("Enter Book ID to delete: ");
        
        auto it = find_if(books.begin(), books.end(),
                         [&bookId](const Book& book) { return book.id == bookId; });
        
        if (it == books.end()) {
            cout << "\n✗ Book not found!\n";
            cout << "Press Enter to continue...";
            cin.get();
            return;
        }
        
        cout << "\nBook Details:\n";
        cout << "ID: " << it->id << "\n";
        cout << "Title: " << it->title << "\n";
        cout << "Author: " << it->author << "\n";
        cout << "Price: $" << fixed << setprecision(2) << it->price << "\n";
        
        char confirm;
        cout << "\nAre you sure you want to delete this book? (y/n): ";
        cin >> confirm;
        cin.ignore();
        
        if (confirm == 'y' || confirm == 'Y') {
            books.erase(it);
            saveBooks();
            cout << "\n✓ Book deleted successfully!\n";
        } else {
            cout << "\n✗ Deletion cancelled!\n";
        }
        
        cout << "Press Enter to continue...";
        cin.get();
    }

    // Sales management functions
    void makeSale() {
        clearScreen();
        cout << "\n" << string(60, '=') << "\n";
        cout << "                MAKE SALE\n";
        cout << string(60, '=') << "\n\n";
        
        string bookId = getValidatedString("Enter Book ID: ");
        
        auto it = find_if(books.begin(), books.end(),
                         [&bookId](const Book& book) { return book.id == bookId; });
        
        if (it == books.end()) {
            cout << "\n✗ Book not found!\n";
            cout << "Press Enter to continue...";
            cin.get();
            return;
        }
        
        cout << "\nBook Details:\n";
        cout << "Title: " << it->title << "\n";
        cout << "Author: " << it->author << "\n";
        cout << "Price: $" << fixed << setprecision(2) << it->price << "\n";
        cout << "Available Quantity: " << it->quantity << "\n\n";
        
        if (it->quantity == 0) {
            cout << "✗ Book is out of stock!\n";
            cout << "Press Enter to continue...";
            cin.get();
            return;
        }
        
        int quantity = getValidatedInt("Enter quantity to sell: ", 1, it->quantity);
        string customerName = getValidatedString("Enter customer name: ");
        
        Sale newSale;
        newSale.saleId = "S" + to_string(sales.size() + 1);
        newSale.bookId = bookId;
        newSale.bookTitle = it->title;
        newSale.quantity = quantity;
        newSale.totalAmount = quantity * it->price;
        newSale.date = getCurrentDateTime();
        newSale.customerName = customerName;
        
        // Update book quantity
        it->quantity -= quantity;
        
        sales.push_back(newSale);
        saveBooks();
        saveSales();
        
        cout << "\n" << string(40, '-') << "\n";
        cout << "         SALES RECEIPT\n";
        cout << string(40, '-') << "\n";
        cout << "Sale ID: " << newSale.saleId << "\n";
        cout << "Book: " << newSale.bookTitle << "\n";
        cout << "Quantity: " << newSale.quantity << "\n";
        cout << "Unit Price: $" << fixed << setprecision(2) << it->price << "\n";
        cout << "Total Amount: $" << fixed << setprecision(2) << newSale.totalAmount << "\n";
        cout << "Customer: " << newSale.customerName << "\n";
        cout << "Date: " << newSale.date << "\n";
        cout << string(40, '-') << "\n";
        
        cout << "\n✓ Sale completed successfully!\n";
        cout << "Press Enter to continue...";
        cin.get();
    }

    void viewSales() {
        clearScreen();
        cout << "\n" << string(80, '=') << "\n";
        cout << "                    SALES HISTORY\n";
        cout << string(80, '=') << "\n";
        
        if (sales.empty()) {
            cout << "\nNo sales records found.\n";
            cout << "Press Enter to continue...";
            cin.get();
            return;
        }
        
        cout << left << setw(8) << "Sale ID" << setw(8) << "Book ID" << setw(25) << "Book Title"
             << setw(5) << "Qty" << setw(10) << "Amount" << setw(15) << "Customer"
             << "Date\n";
        cout << string(80, '-') << "\n";
        
        double totalSales = 0;
        for (const auto& sale : sales) {
            cout << left << setw(8) << sale.saleId << setw(8) << sale.bookId
                 << setw(25) << sale.bookTitle.substr(0, 24) << setw(5) << sale.quantity
                 << setw(10) << fixed << setprecision(2) << sale.totalAmount
                 << setw(15) << sale.customerName.substr(0, 14) << sale.date << "\n";
            totalSales += sale.totalAmount;
        }
        
        cout << string(80, '-') << "\n";
        cout << "Total Sales: $" << fixed << setprecision(2) << totalSales << "\n";
        cout << "Total Transactions: " << sales.size() << "\n";
        cout << "Press Enter to continue...";
        cin.get();
    }

    // Company information
    void viewCompanyDetails() {
        clearScreen();
        cout << "\n" << string(60, '=') << "\n";
        cout << "              COMPANY INFORMATION\n";
        cout << string(60, '=') << "\n\n";
        
        cout << "Company Name: GENIUS BOOKS\n";
        cout << "Established: 2020\n";
        cout << "Address: 123 Main Street, City Center\n";
        cout << "Phone: +1 (555) 123-4567\n";
        cout << "Email: info@geniusbooks.com\n";
        cout << "Website: www.geniusbooks.com\n\n";
        
        cout << "About Us:\n";
        cout << "GENIUS BOOKS is a leading bookshop in the city, providing\n";
        cout << "a wide variety of books across all genres. We pride ourselves\n";
        cout << "on excellent customer service and competitive prices.\n\n";
        
        cout << "Our Mission:\n";
        cout << "To promote reading culture and provide easy access to\n";
        cout << "quality books for everyone in our community.\n\n";
        
        cout << "Current Statistics:\n";
        cout << "Total Books in Stock: " << books.size() << "\n";
        cout << "Total Sales Made: " << sales.size() << "\n";
        cout << "System Users: " << users.size() << "\n";
        
        cout << "\nPress Enter to continue...";
        cin.get();
    }

    // Main menu functions
    void displayMainMenu() {
        clearScreen();
        cout << "\n" << string(60, '=') << "\n";
        cout << "           GENIUS BOOKS MANAGEMENT SYSTEM\n";
        cout << string(60, '=') << "\n";
        
        if (isLoggedIn) {
            cout << "Logged in as: " << currentUser << " (" << currentRole << ")\n";
            cout << string(60, '-') << "\n";
            cout << "1. View Available Books\n";
            cout << "2. Add New Book\n";
            cout << "3. Update Book Details\n";
            cout << "4. Delete Book\n";
            cout << "5. Make Sale\n";
            cout << "6. View Sales History\n";
            cout << "7. View Company Details\n";
            cout << "8. Logout\n";
            cout << "9. Exit\n";
        } else {
            cout << "Please login to access the system\n";
            cout << string(60, '-') << "\n";
            cout << "1. Login\n";
            cout << "2. View Company Details\n";
            cout << "3. Exit\n";
        }
        
        cout << string(60, '=') << "\n";
    }

    void run() {
        cout << "\n" << string(60, '=') << "\n";
        cout << "     WELCOME TO GENIUS BOOKS MANAGEMENT SYSTEM\n";
        cout << string(60, '=') << "\n";
        cout << "Loading system... Please wait.\n";
        
        // Simulate loading
        for (int i = 0; i < 3; i++) {
            cout << ".";
            cout.flush();
            // Small delay simulation
            for (int j = 0; j < 100000000; j++);
        }
        
        cout << "\n✓ System loaded successfully!\n";
        cout << "Press Enter to continue...";
        cin.get();
        
        while (true) {
            displayMainMenu();
            
            int choice;
            if (isLoggedIn) {
                choice = getValidatedInt("Enter your choice (1-9): ", 1, 9);
            } else {
                choice = getValidatedInt("Enter your choice (1-3): ", 1, 3);
            }
            
            if (isLoggedIn) {
                switch (choice) {
                    case 1:
                        viewBooks();
                        break;
                    case 2:
                        if (currentRole == "admin" || currentRole == "manager") {
                            addBook();
                        } else {
                            cout << "\n✗ Access denied! Only admin and manager can add books.\n";
                            cout << "Press Enter to continue...";
                            cin.get();
                        }
                        break;
                    case 3:
                        if (currentRole == "admin" || currentRole == "manager") {
                            updateBook();
                        } else {
                            cout << "\n✗ Access denied! Only admin and manager can update books.\n";
                            cout << "Press Enter to continue...";
                            cin.get();
                        }
                        break;
                    case 4:
                        if (currentRole == "admin") {
                            deleteBook();
                        } else {
                            cout << "\n✗ Access denied! Only admin can delete books.\n";
                            cout << "Press Enter to continue...";
                            cin.get();
                        }
                        break;
                    case 5:
                        makeSale();
                        break;
                    case 6:
                        viewSales();
                        break;
                    case 7:
                        viewCompanyDetails();
                        break;
                    case 8:
                        logout();
                        break;
                    case 9:
                        cout << "\n✓ Thank you for using GENIUS BOOKS Management System!\n";
                        saveData();
                        exit(0);
                        break;
                }
            } else {
                switch (choice) {
                    case 1:
                        login();
                        break;
                    case 2:
                        viewCompanyDetails();
                        break;
                    case 3:
                        cout << "\n✓ Thank you for using GENIUS BOOKS Management System!\n";
                        saveData();
                        exit(0);
                        break;
                }
            }
        }
    }
};

int main() {
    BookshopManager system;
    system.run();
    return 0;
}
