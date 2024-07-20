#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct definitions
typedef struct {
    int book_id;
    char title[100];
    char author[100];
    char genre[50];
    int quantity;
} Book;

typedef struct {
    int id;
    char username[50];
    char password[50];
    char type[20];
} User;

typedef struct {
    int id;
    char username[50];
    char password[50];
    char type[20];
} Admin;


void populate_data_files() {
    // Populate admins.dat
    FILE *admin_file = fopen("admins.dat", "wb");
    if (!admin_file) {
        perror("Error opening admins.dat");
        exit(EXIT_FAILURE);
    }
    Admin admins[] = {
        {1, "Ragini", "Ragini", "admin"},
    };
    fwrite(admins, sizeof(Admin), sizeof(admins) / sizeof(Admin), admin_file);
    fclose(admin_file);
    printf("admins.dat populated.\n");


    // Populate users.dat
    FILE *user_file = fopen("users.dat", "wb");
    if (!user_file) {
        perror("Error opening users.dat");
        exit(EXIT_FAILURE);
    }
    User users[] = {
        {101, "name1", "pass1", "user"},
        {102, "name2", "pass2", "user"},
        {103, "name3", "pass3", "user"},
        {104, "name4", "pass4", "user"},

    };
    fwrite(users, sizeof(User), sizeof(users) / sizeof(User), user_file);
    fclose(user_file);
    printf("users.dat populated.\n");


    // Populate books.dat
    FILE *book_file = fopen("books.dat", "wb");
    if (!book_file) {
        perror("Error opening books.dat");
        exit(EXIT_FAILURE);
    }
    Book books[] = {
        {1001, "The Great Gatsby", "F. Scott Fitzgerald", "Fiction", 5},
        {1002, "To Kill a Mockingbird", "Harper Lee", "Fiction", 3},
        {1003, "1984", "George Orwell", "Dystopian", 4},
        {1004, "Moby Rick", "Herman Melville", "Adventure", 2},
        {1005, "Harry Potter and the Sorcerer's Stone", "J.K. Rowling", "Fantasy", 5},
        {1006, "The Lord of the Rings", "J.R.R. Tolkien", "Fantasy", 5},
        {1007, "Pride and Prejudice", "Jane Austen", "Romance", 4},
        {1008, "The Catcher in the Rye", "J.D. Salinger", "Literary Fiction", 4},
    };

    fwrite(books, sizeof(Book), sizeof(books) / sizeof(Book), book_file);
    fclose(book_file);
    printf("books.dat populated.\n");


    //creating bororwed_books.dat file
    FILE *borrowed_book_file = fopen("borrowed_books.dat", "wb");
    if (!borrowed_book_file) {
        perror("Error opening books.dat");
        exit(EXIT_FAILURE);
    }
    fclose(borrowed_book_file);
    printf("borrowed_books.dat created.\n");
}


int main() {
    populate_data_files();
    return 0;
}
