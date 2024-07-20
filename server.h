#ifndef SERVER_H
#define SERVER_H
#include<netinet/in.h>
#include<stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fcntl.h>
#include <pthread.h>

// Constants
#define PORT 8080

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


char* request_arr[] = {"Search for a book", "view books that have been borrowed", "Borrow a book",
                        "Return a book", "Change Password", "View my profile", "Add a book", "Remove a book", 
                        "View details of users who have borrowed books", "View details of a User",
                        "Login of an admin", "Login of a user", "Sign up of a user", "View books in library"};

// Function declarations
void *handleClient(void *arg);
void handleAdminLogin(int socket_descriptor);
void handleUserLogin(int socket_descriptor);
void handleUserSignup(int socket_descriptor);
void handle_search_books(int socket_descriptor);
void handle_view_borrowed_books(int socket_descriptor);
void send_books(int socket_descriptor);
void borrow_book(int socket_descriptor);
void update_book_quantity(int book_id, int change);
void send_borrowed_books(int user_id, int socket_descriptor);
void return_book(int socket_descriptor);
void changePassword(int socket_descriptor);
void viewProfile(int socket_descriptor);
void addBook(int client_socket);
void deleteBook(int client_socket);
void viewBorrowedUsers(int socket_descriptor);
void handleViewUserDetails(int socket_descriptor);
void modify_book(int socket_descriptor);
void ReadlockFile(int fd);
void WritelockFile(int fd);
void unlockFile(int fd);
int getFileDescriptor(FILE *file);

#endif // SERVER_H
