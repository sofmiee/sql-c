CREATE DATABASE library_

CREATE TABLE authors(
id_ INT PRIMARY KEY,
name_ VARCHAR(100),
birth_year INT
);
CREATE TABLE books(
id_ INT PRIMARY KEY,
title VARCHAR(100),
author_id INT,
publication_year INT,
genre VARCHAR(100),
FOREIGN KEY (author_id) REFERENCES authors(id_)
);
CREATE TABLE users(
id_ INT PRIMARY KEY, 
name_ VARCHAR(100),
registration_date DATE
);
CREATE TABLE borrowed_books(
user_id INT,
book_id INT,
borrow_date DATE,
return_date DATE,
FOREIGN KEY (book_id) REFERENCES books(id_),
FOREIGN KEY (user_id) REFERENCES users(id_) 
);
