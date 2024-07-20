#ifndef CLIENT_H
#define CLIENT_H

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <string.h>
#include <stdbool.h>

//gloabal variable
extern int currID;

// Struct definitions
typedef struct {
    int book_id;
    char title[100];
    char author[100];
    char genre[50];
    int quantity;
} Book;

typedef struct {
    int user_id;
    int book_id;
    int quantity;
} BorrowedBook;

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


// Function declarations
void authenticate(int socket_descriptor);
void admin_login(int socket_descriptor);
void user_login(int socket_descriptor);
void user_signup(int socket_descriptor);
void user_menu(int socket_descriptor);
void admin_menu(int socket_descriptor);

// User functionalities
void search_books(int socket_descriptor);
void view_borrowed_books(int socket_descriptor);
void borrow_book(int socket_descriptor);
void return_book(int socket_descriptor);
void changePassword(int socket_descriptor);
void viewProfile(int socket_descriptor);

// Admin functionalities
void addBook(int socket_descriptor);
void deleteBook(int socket_descriptor);
void display_books(int socket_descriptor);
void viewUserDetails(int socket_descriptor);
void viewBorrowedUsers(int socket_descriptor);
void show_books(int socket_descriptor);
void modifyBook(int socket_descriptor);

#endif // CLIENT_H
