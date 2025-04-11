#include <iostream>
#include <string>
#include <pqxx/pqxx> // Библиотека для работы с PostgreSQL
#include <fstream>
#include <vector>
#include <memory>
#include <ctime>

// Базовый класс для работы с авторами
class Author {
public:
    Author(int id, const std::string& name, int birth_year)
        : id_(id), name_(name), birth_year_(birth_year) {}

    void save(pqxx::connection& C) {
        pqxx::work W(C);
        W.exec("INSERT INTO authors (id_, name_, birth_year) VALUES (" + W.quote(id_) + ", " + W.quote(name_) + ", " + W.quote(birth_year_) + ")");
        W.commit();
    }

    std::string getName() const { return name_; }

private:
    int id_;
    std::string name_;
    int birth_year_;
};

// Базовый класс для работы с книгами
class Book {
public:
    Book(int id, const std::string& title, int author_id, int publication_year, const std::string& genre)
        : id_(id), title_(title), author_id_(author_id), publication_year_(publication_year), genre_(genre) {}

    void save(pqxx::connection& C) {
        pqxx::work W(C);
        W.exec("INSERT INTO books (id_, title, author_id, publication_year, genre) VALUES (" +
            W.quote(id_) + ", " + W.quote(title_) + ", " + W.quote(author_id_) + ", " +
            W.quote(publication_year_) + ", " + W.quote(genre_) + ")");
        W.commit();
    }

    std::string getTitle() const { return title_; }

private:
    int id_;
    std::string title_;
    int author_id_;
    int publication_year_;
    std::string genre_;
};

// Класс для работы с пользователями
class User {
public:
    User(int id, const std::string& name, const std::string& registration_date)
        : id_(id), name_(name), registration_date_(registration_date) {}

    void save(pqxx::connection& C) {
        pqxx::work W(C);
        W.exec("INSERT INTO users (id_, name_, registration_date) VALUES (" +
            W.quote(id_) + ", " + W.quote(name_) + ", " + W.quote(registration_date_) + ")");
        W.commit();
    }

private:
    int id_;
    std::string name_;
    std::string registration_date_;
};

// Класс для работы с заёмами книг
class BorrowedBook {
public:
    BorrowedBook(int user_id, int book_id, const std::string& borrow_date, const std::string& return_date)
        : user_id_(user_id), book_id_(book_id), borrow_date_(borrow_date), return_date_(return_date) {}

    void save(pqxx::connection& C) {
        pqxx::work W(C);
        W.exec("INSERT INTO borrowed_books (user_id, book_id, borrow_date, return_date) VALUES (" +
            W.quote(user_id_) + ", " + W.quote(book_id_) + ", " + W.quote(borrow_date_) + ", " +
            W.quote(return_date_) + ")");
        W.commit();
    }

private:
    int user_id_;
    int book_id_;
    std::string borrow_date_;
    std::string return_date_;
};

// Функция для записи операций в журнал
void logOperation(const std::string& operation) {
    std::ofstream logFile("log.txt", std::ios_base::app);
    logFile << operation << std::endl;
}

// Функции для добавления авторов, книг и пользователей
void addAuthor(pqxx::connection& C) {
    int id;
    std::string name;
    int birth_year;

    std::cout << "Enter author ID: ";
    std::cin >> id;
    std::cin.ignore();

    std::cout << "Enter author name: ";
    std::getline(std::cin, name);

    std::cout << "Enter author birth year: ";
    std::cin >> birth_year;

    Author author(id, name, birth_year);

    try {
        author.save(C);
        logOperation("Added author: " + name);
        std::cout << "Author added: " << name << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error adding author: " << e.what() << "\n";
    }
}

void addBook(pqxx::connection& C) {
    int id;
    std::string title;
    int author_id, publication_year;
    std::string genre;
    std::cout << "Enter book ID: ";
    std::cin >> id;

    std::cin.ignore(); // Игнорируем остаток строки
    std::cout << "Enter book title: ";
    std::getline(std::cin, title);

    std::cout << "Enter author ID: ";
    std::cin >> author_id;

    std::cout << "Enter publication year: ";
    std::cin >> publication_year;

    std::cin.ignore(); // Игнорируем остаток строки
    std::cout << "Enter genre: ";
    std::getline(std::cin, genre);

    Book book(id, title, author_id, publication_year, genre);

    try {
        book.save(C);
        logOperation("Added book: " + title);
        std::cout << "Book added: " << title << " by author ID " << author_id << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error adding book: " << e.what() << "\n";
    }
}

void addUser(pqxx::connection& C) {
    int id;
    std::string name;
    std::string registration_date;

    std::cout << "Enter user ID: ";
    std::cin >> id;

    std::cin.ignore(); // Игнорируем остаток строки
    std::cout << "Enter user name: ";
    std::getline(std::cin, name);

    std::cout << "Enter registration date (YYYY-MM-DD): ";
    std::getline(std::cin, registration_date);

    User user(id, name, registration_date);

    try {
        user.save(C);
        logOperation("Added user: " + name);
        std::cout << "User added: " << name << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error adding user: " << e.what() << "\n";
    }
}

void registerBorrowedBook(pqxx::connection& C) {
    int user_id, book_id;
    std::string borrow_date;

    std::cout << "Enter user ID: ";
    std::cin >> user_id;

    std::cout << "Enter book ID: ";
    std::cin >> book_id;

    std::cout << "Enter borrow date (YYYY-MM-DD): ";
    std::cin >> borrow_date;

    // Возврат книги может быть пустым на момент заимствования
    BorrowedBook borrowedBook(user_id, book_id, borrow_date, "");

    try {
        borrowedBook.save(C);
        logOperation("Registered borrowed book for user ID " + std::to_string(user_id));
        std::cout << "Borrowed book registered for user ID " << user_id << " and book ID " << book_id << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error registering borrowed book: " << e.what() << "\n";
    }
}

// Функция для выполнения аналитических запросов
void performAnalytics(pqxx::connection& C) {
    // Пример аналитического запроса: список книг определённого автора
    int author_id;

    std::cout << "Enter author ID to list their books: ";
    std::cin >> author_id;

    try {
        pqxx::nontransaction N(C);
        pqxx::result R = N.exec("SELECT title FROM books WHERE author_id = " + N.quote(author_id));

        for (auto row : R) {
            std::cout << "Book Title: " << row[0].c_str() << std::endl;
        }

        logOperation("Listed books for author ID: " + std::to_string(author_id));

    }
    catch (const std::exception& e) {
        std::cerr << "Error performing analytics: " << e.what() << "\n";
    }
}

// Функция для отображения меню
void showMenu(pqxx::connection& C) {
    int choice;

    do {
        std::cout << "\nMenu:\n";
        std::cout << "1. Add Author\n";
        std::cout << "2. Add Book\n";
        std::cout << "3. Add User\n";
        std::cout << "4. Register Borrowed Book\n";
        std::cout << "5. Perform Analytics\n";
        std::cout << "6. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1: addAuthor(C); break;
        case 2: addBook(C); break;
        case 3: addUser(C); break;
        case 4: registerBorrowedBook(C); break;
        case 5: performAnalytics(C); break;
        case 6: break; // Выход
        default: std::cout << "Invalid option. Try again.\n"; break;
        }

    } while (choice != 6);
}

// Основная функция
int main() {
    try {
        pqxx::connection C("dbname=library_ user=postgres password=pupa host=localhost");

        showMenu(C);

        C.disconnect();

    }
    catch (const pqxx::sql_error& e) {
        std::cerr << "SQL error: " << e.what() << "\n";
        return 1;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;

    }

    return 0;
}
