#include <stdio.h>
#include "server.h"
#define PORT 8080


int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully.\n");
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);
    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Bind to port %d successful.\n", PORT);
    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d\n", PORT);
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        pthread_t thread_id;
        int *client_socket = malloc(sizeof(int));
        *client_socket = new_socket;
        if (pthread_create(&thread_id, NULL, handleClient, client_socket) != 0) {
            perror("pthread_create failed");
            close(new_socket);
            free(client_socket);
        } else {
            pthread_detach(thread_id); // Detach the thread to clean up automatically
        }
    }
    close(server_fd);
    return 0;
}



void *handleClient(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);
    int request_type;
    while (1) {
        if (read(client_socket, &request_type, sizeof(int)) <= 0) {
            printf("Client disconnected or error reading request type\n");
            break;
        }
    if(request_arr[request_type - 1] )printf("Received request: %s\n", request_arr[request_type - 1]);
    // Handle the request based on type
    switch (request_type) {
        case 1:
            handle_search_books(client_socket);
            break;
        case 2:
            handle_view_borrowed_books(client_socket);
            break;
        case 3:
            borrow_book(client_socket);
            break;
        case 4:
            return_book(client_socket);
            break;
        case 5:
            changePassword(client_socket);
            break;
        case 6:
            viewProfile(client_socket);
            break;
        case 7:
            addBook(client_socket);
            break;
        case 8:
            deleteBook(client_socket);
            break;
        case 9:
            viewBorrowedUsers(client_socket);
            break;
        case 10:
            handleViewUserDetails(client_socket);
            break;
        case 11:
            handleAdminLogin(client_socket);
            break;
        case 12:
            handleUserLogin(client_socket);
            break;
        case 13:
            handleUserSignup(client_socket);
            break;
        case 14:
            send_books(client_socket);
            break;
        case 15:
            modify_book(client_socket);
            break;
        default:
            printf("\n");
    }
    }
    close(client_socket);
    return NULL;
}



void ReadlockFile(int fd) {
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // Lock entire file

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        exit(EXIT_FAILURE);
    }
}

void WritelockFile(int fd) {
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // Lock entire file

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        exit(EXIT_FAILURE);
    }
}

void unlockFile(int fd) {
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // Unlock entire file

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error unlocking file");
        exit(EXIT_FAILURE);
    }
}

int getFileDescriptor(FILE *file) {
    int fd = fileno(file);
    if (fd == -1) {
        perror("Error getting file descriptor");
        exit(EXIT_FAILURE);  // Or handle the error according to your application's logic
    }
    return fd;
}


void handleAdminLogin(int socket_descriptor) {
    Admin receivedAdmin;
    int loginResult = 0; // 0 = failure, 1 = success
    // Read the admin data sent by the client
    read(socket_descriptor, &receivedAdmin, sizeof(Admin));
    // Open the admins.dat file to verify credentials
    FILE *file = fopen("admins.dat", "rb");
    if (file == NULL) {
        perror("Error opening admins.dat");
        write(socket_descriptor, &loginResult, sizeof(int));
        return;
    }
    int fd = getFileDescriptor(file);
    ReadlockFile(fd);
    Admin admin;
    while (fread(&admin, sizeof(Admin), 1, file)) {
        if (admin.id == receivedAdmin.id && strcmp(admin.password, receivedAdmin.password) == 0) {
            loginResult = 1; // Successful login
            break;
        }
    }
    unlockFile(fd);
    fclose(file);
    // Send the login result back to the client
    write(socket_descriptor, &loginResult, sizeof(int));
}



void handleUserLogin(int socket_descriptor) {
    User receivedUser;
    int loginResult = 0; // 0 = failure, 1 = success
    // Read the user data sent by the client
    read(socket_descriptor, &receivedUser, sizeof(User));
    // Open the users.dat file to verify credentials
    FILE *file = fopen("users.dat", "rb");
    if (file == NULL) {
        perror("Error opening users.dat");
        write(socket_descriptor, &loginResult, sizeof(int));
        return;
    }
    int fd = getFileDescriptor(file);
    ReadlockFile(fd);
    User user;
    while (fread(&user, sizeof(User), 1, file)) {
        if (user.id == receivedUser.id && strcmp(user.password, receivedUser.password) == 0) {
            loginResult = 1; // Successful login
            break;
        }
    }
    unlockFile(fd);
    fclose(file);
    // Send the login result back to the client
    write(socket_descriptor, &loginResult, sizeof(int));
}


void handleUserSignup(int socket_descriptor) {
    User newUser;
    int signupResult = 0; // 0 = failure, 1 = success
    // Read the user data sent by the client
    read(socket_descriptor, &newUser, sizeof(User));
    // Open the users.dat file to check for existing user ID
    FILE *file = fopen("users.dat", "rb");
    if (file == NULL) {
        perror("Error opening users.dat");
        write(socket_descriptor, &signupResult, sizeof(int));
        return;
    }
    int fd = getFileDescriptor(file);
    ReadlockFile(fd);
    User user;
    while (fread(&user, sizeof(User), 1, file)) {
        if (user.id == newUser.id) {
            // User ID already exists
            fclose(file);
            write(socket_descriptor, &signupResult, sizeof(int));
            return;
        }
    }
    unlockFile(fd);
    fclose(file);
    // Open the users.dat file in append mode to add new user
    file = fopen("users.dat", "ab");
    if (file == NULL) {
        perror("Error opening users.dat");
        write(socket_descriptor, &signupResult, sizeof(int));
        return;
    }
    int fd2 = getFileDescriptor(file);
    WritelockFile(fd2);
    fwrite(&newUser, sizeof(User), 1, file);
    unlockFile(fd2);
    fclose(file);
    signupResult = 1; // Successful signup
    write(socket_descriptor, &signupResult, sizeof(int));
}




void handle_search_books(int client_socket) {
    char criteria[256], search_term[256];
    int choice;
    //read(client_socket, criteria, sizeof(criteria));
    read(client_socket, &choice, sizeof(int));
    read(client_socket, search_term, sizeof(search_term));
    int found = 0;
    FILE *file = fopen("books.dat", "rb");
    if (file == NULL) {
        perror("Failed to open books.dat");
        return;
    }
    int fd = getFileDescriptor(file);
    ReadlockFile(fd);
    Book book;
    char buffer[4096] = {0};

    while (fread(&book, sizeof(Book), 1, file)) {
        // if ((strcmp(criteria, "title") == 0 && strstr(book.title, search_term)) ||
        //     (strcmp(criteria, "author") == 0 && strstr(book.author, search_term)) ||
        //     (strcmp(criteria, "genre") == 0 && strstr(book.genre, search_term))) {
        if ((choice == 1 && strstr(book.title, search_term)) ||
            (choice == 2 && strstr(book.author, search_term)) ||
            (choice == 3 && strstr(book.genre, search_term))) {
            char book_info[1024];
            found = 1;
            snprintf(book_info, sizeof(book_info), 
                     "ID: %d, Title: %s, Author: %s, Genre: %s, Quantity: %d\n", 
                     book.book_id, book.title, book.author, book.genre, book.quantity);
            strncat(buffer, book_info, sizeof(buffer) - strlen(buffer) - 1);
        }
    }
    if (!found) {
        char not_found_msg[] = "No books found with the specified requirement.";
        write(client_socket, not_found_msg, sizeof(not_found_msg)); // Send not found message
        unlockFile(fd);
        fclose(file);
        return;
    }
    unlockFile(fd);
    fclose(file);
    write(client_socket, buffer, sizeof(buffer));
}

void handle_view_borrowed_books(int socket_descriptor) {
    BorrowedBook borrowed_book;
    int user_id;
    int found = 0;
    char buffer[4096] = {0};  // Increased buffer size to handle multiple book entries
    read(socket_descriptor, &user_id, sizeof(int));
    // Open the file containing borrowed book information
    FILE* borrowed_books_file = fopen("borrowed_books.dat", "rb");
    if (borrowed_books_file == NULL) {
        perror("Error opening file");
        return;
    }
    int fd = getFileDescriptor(borrowed_books_file);
    ReadlockFile(fd);
    // Map to store book IDs and their corresponding quantities
    struct BorrowedInfo {
        int book_id;
        int quantity;
    } borrowed_info[100];
    int borrowed_count = 0;
    // Read borrowed books and collect information for the specified user ID
    while (fread(&borrowed_book, sizeof(BorrowedBook), 1, borrowed_books_file)) {
        if (borrowed_book.user_id == user_id) {
            int found_in_list = 0;
            for (int j = 0; j < borrowed_count; j++) {
                if (borrowed_info[j].book_id == borrowed_book.book_id) {
                    borrowed_info[j].quantity += borrowed_book.quantity;
                    found_in_list = 1;
                    break;
                }
            }
            if (!found_in_list) {
                borrowed_info[borrowed_count].book_id = borrowed_book.book_id;
                borrowed_info[borrowed_count].quantity = borrowed_book.quantity;
                borrowed_count++;
            }
            found = 1;
        }
    }
    unlockFile(fd);
    fclose(borrowed_books_file);
    // Open the file containing book details
    FILE* book_file = fopen("books.dat", "rb");
    if (!book_file) {
        perror("Error opening books.dat");
        exit(EXIT_FAILURE);
    }
    int fd2 = getFileDescriptor(book_file);
    ReadlockFile(fd2);
    // Read book details and match with borrowed books
    Book book;
    while (fread(&book, sizeof(Book), 1, book_file)) {
        for (int j = 0; j < borrowed_count; j++) {
            if (borrowed_info[j].book_id == book.book_id) {
                char book_info[1024];
                snprintf(book_info, sizeof(book_info), "ID: %d, Title: %s, Author: %s, Genre: %s, Number of Copies borrowed: %d\n",
                         book.book_id, book.title, book.author, book.genre, borrowed_info[j].quantity);
                strcat(buffer, book_info);
            }
        }
    }
    unlockFile(fd2);
    fclose(book_file);
    if (found) {
        write(socket_descriptor, buffer, sizeof(buffer));
    } else {
        char not_found_msg[] = "No books borrowed.";
        write(socket_descriptor, not_found_msg, sizeof(not_found_msg)); // Send not found message
    }
}


void send_books(int socket_descriptor) {
    FILE *book_file = fopen("books.dat", "rb");
    if (!book_file) {
        perror("Error opening books.dat");
        exit(EXIT_FAILURE);
    }
    int fd = getFileDescriptor(book_file);
    ReadlockFile(fd);
    Book book;
    char buffer[1024] = {0};
    while (fread(&book, sizeof(Book), 1, book_file)) {
        char book_info[256];
        snprintf(book_info, sizeof(book_info), "ID: %d, Title: %s, Author: %s, Quantity: %d\n", book.book_id, book.title, book.author, book.quantity);
        strcat(buffer, book_info);
    }
    unlockFile(fd);
    fclose(book_file);
    // Send available books information to client
    write(socket_descriptor, buffer, sizeof(buffer));
}


void borrow_book(int socket_descriptor) {
    int user_id;
    read(socket_descriptor, &user_id, sizeof(int));
    send_books(socket_descriptor);
    int book_id;
    Book book;
    int found = 0;
    read(socket_descriptor, &book_id, sizeof(int));
    FILE* books_file = fopen("books.dat", "rb");
    if (!books_file) {
        perror("Error opening books.dat");
        exit(EXIT_FAILURE);
    }
    int fd = getFileDescriptor(books_file);
    ReadlockFile(fd);
    while (fread(&book, sizeof(Book), 1, books_file)) {
        if (book_id == book.book_id && book.quantity > 0) {
            found = 1;
            break;
        }
    }
    unlockFile(fd);
    fclose(books_file);
    if (found) {
        FILE *borrowed_books_file = fopen("borrowed_books.dat", "rb+");
        if (!borrowed_books_file) {
            perror("Error opening borrowed_books.dat");
            exit(EXIT_FAILURE);
        }
        int fd2 = getFileDescriptor(borrowed_books_file);
        WritelockFile(fd2);
        BorrowedBook borrowed_book;
        int entry_found = 0;
        while (fread(&borrowed_book, sizeof(BorrowedBook), 1, borrowed_books_file)) {
            if (borrowed_book.user_id == user_id && borrowed_book.book_id == book_id) {
                borrowed_book.quantity++;
                fseek(borrowed_books_file, -sizeof(BorrowedBook), SEEK_CUR);
                fwrite(&borrowed_book, sizeof(BorrowedBook), 1, borrowed_books_file);
                entry_found = 1;
                break;
            }
        }
        if (!entry_found) {
            borrowed_book.user_id = user_id;
            borrowed_book.book_id = book_id;
            borrowed_book.quantity = 1;
            fwrite(&borrowed_book, sizeof(BorrowedBook), 1, borrowed_books_file);
        }
        unlockFile(fd2);
        fclose(borrowed_books_file);
        // Send confirmation message to client
        char confirmation_message[] = "Book borrowed successfully!";
        write(socket_descriptor, confirmation_message, sizeof(confirmation_message));
        // Update book quantity in the books.dat file
        update_book_quantity(book_id, -1); // Decrease quantity by 1
    } 
    else {
        char error_message[] = "Book requested for borrowing not found or out of stock";
        write(socket_descriptor, error_message, sizeof(error_message));
    }
}


void update_book_quantity(int book_id, int change) {
    FILE *book_file = fopen("books.dat", "rb+");
    if (!book_file) {
        perror("Error opening books file");
        return;
    }
    int fd = getFileDescriptor(book_file);
    WritelockFile(fd);
    Book book;
    int found = 0;
    while (fread(&book, sizeof(Book), 1, book_file)) {
        if (book.book_id == book_id) {
            // Move file pointer to the correct position
            fseek(book_file, -sizeof(Book), SEEK_CUR);
            // Update quantity
            book.quantity = book.quantity + change;
            // Write updated book information back to file
            fwrite(&book, sizeof(Book), 1, book_file);
            found = 1;
            break;
        }
    }
    unlockFile(fd);
    fclose(book_file);
    if (!found) {
        printf("Book not found\n");
    }
}


void send_borrowed_books(int user_id, int socket_descriptor){
    FILE *borrowed_file = fopen("borrowed_books.dat", "rb");
    if (!borrowed_file) {
        perror("Error opening borrowed_books.dat");
        exit(EXIT_FAILURE);
    }
    int fd = getFileDescriptor(borrowed_file);
    ReadlockFile(fd);
    BorrowedBook borrowed_book;
    char buffer[1024] = {0};
    int found = 0;
    while (fread(&borrowed_book, sizeof(BorrowedBook), 1, borrowed_file)) {
        if (borrowed_book.user_id == user_id) {
            char book_info[256];
            snprintf(book_info, sizeof(book_info), "Book ID: %d\nNumber of copies borrowed: %d\n", borrowed_book.book_id, borrowed_book.quantity);
            strncat(buffer, book_info, sizeof(buffer) - strlen(buffer) - 1);
            found = 1;
        }
    }
    if(found == 1){
        write(socket_descriptor, buffer, sizeof(buffer));
        write(socket_descriptor, &found, sizeof(int));
    }
    else{
        char response[1024] = {0};
        snprintf(response, sizeof(response), "No borrowed books found for user ID %d\n", user_id);
        write(socket_descriptor, response, sizeof(response));
        write(socket_descriptor, &found, sizeof(int));
    }
    unlockFile(fd);
    fclose(borrowed_file);
}


void return_book(int client_socket) {
    int user_id, res;
    read(client_socket, &user_id, sizeof(int));
    // Send borrowed books list to the client
    send_borrowed_books(user_id, client_socket);
    int book_id_to_return;
    read(client_socket, &res, sizeof(int));
    if(res){
        read(client_socket, &book_id_to_return, sizeof(int));
        FILE *borrowed_file = fopen("borrowed_books.dat", "rb+");
        int fd = getFileDescriptor(borrowed_file);
        WritelockFile(fd);
        if (!borrowed_file) {
            perror("Error opening borrowed_books.dat");
            int error_code = 0;
            write(client_socket, &error_code, sizeof(int));
            return;
        }
        BorrowedBook borrowed_book;
        int found = 0;
        FILE *temp_file = fopen("temp_borrowed_books.dat", "wb");
        int fd2 = getFileDescriptor(temp_file);
        WritelockFile(fd2);
        while (fread(&borrowed_book, sizeof(BorrowedBook), 1, borrowed_file)) {
            if (borrowed_book.user_id == user_id && borrowed_book.book_id == book_id_to_return) {
                found = 1;
                if (borrowed_book.quantity > 1) {
                    borrowed_book.quantity -= 1;
                    fwrite(&borrowed_book, sizeof(BorrowedBook), 1, temp_file);
                //     found = 2;
                // } else {
                //     found = 1;
                }
            } else {
                fwrite(&borrowed_book, sizeof(BorrowedBook), 1, temp_file);
            }
        }
        unlockFile(fd);
        fclose(borrowed_file);
        unlockFile(fd2);
        fclose(temp_file);
        if (found == 1) {
            remove("borrowed_books.dat");
            rename("temp_borrowed_books.dat", "borrowed_books.dat");
            update_book_quantity(book_id_to_return, 1);
            int success_code = 1;
            write(client_socket, &success_code, sizeof(int));
        } else {
            remove("temp_borrowed_books.dat");
            int error_code = 0;
            write(client_socket, &error_code, sizeof(int));
        }
    }
}


void changePassword(int socket_descriptor) {
    User user;
    read(socket_descriptor, &user, sizeof(User));
    FILE *user_file = fopen("users.dat", "rb+");
    if (!user_file) {
        perror("Error opening users.dat");
        exit(EXIT_FAILURE);
    }
    int fd = getFileDescriptor(user_file);
    WritelockFile(fd);
    User temp_user;
    int found = 0;
    // Read each user record and update the password for the matching user ID
    while (fread(&temp_user, sizeof(User), 1, user_file)) {
        if (temp_user.id == user.id) {
            strcpy(temp_user.password, user.password);
            fseek(user_file, -sizeof(User), SEEK_CUR); // Move file pointer back to overwrite the correct record
            fwrite(&temp_user, sizeof(User), 1, user_file);
            found = 1;
            break;
        }
    }
    unlockFile(fd);
    fclose(user_file);
    char response[1024] = {0};
    if (found) {
        snprintf(response, sizeof(response), "Password for user ID %d changed successfully.\n", user.id);
    } else {
        snprintf(response, sizeof(response),"User ID %d not found.\n", user.id);
    }
    write(socket_descriptor, response, sizeof(response));
}


void viewProfile(int socket_descriptor) {
    int userID;
    read(socket_descriptor, &userID, sizeof(int));
    FILE *file = fopen("users.dat", "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    int fd = getFileDescriptor(file);
    ReadlockFile(fd);
    User user;
    while (fread(&user, sizeof(User), 1, file)) {
        if (user.id == userID) {
            write(socket_descriptor, &user, sizeof(User));
            unlockFile(fd);
            fclose(file);
            return;
        }
    }
    unlockFile(fd);
    fclose(file);
    printf("User with ID %d not found.\n", userID);
}


//admin
void addBook(int client_socket) { //check if book already exists and increment the quantity or do something
    Book newBook;
    read(client_socket, &newBook, sizeof(Book));
    FILE *file = fopen("books.dat", "rb+");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    int fd = getFileDescriptor(file);
    WritelockFile(fd);
    int found = 0;
    Book book;
    while (fread(&book, sizeof(Book), 1, file)) {
        if (book.book_id == newBook.book_id) {
            write(client_socket, "Book ID already exists. Please add correct book details.\n", sizeof("Book ID already exists. Please add correct book details.\n"));
            unlockFile(fd);
            fclose(file);
            return;
        }
    }
    fwrite(&newBook, sizeof(Book), 1, file);
    unlockFile(fd);
    fclose(file);
    write(client_socket, "Book added successfully.\n", sizeof("Book added successfully.\n"));
}

void deleteBook(int socket_descriptor){
    send_books(socket_descriptor);
    int receivedBookID;
    read(socket_descriptor, &receivedBookID, sizeof(int));
    //printf("Admin requested to delete Book ID: %d\n", receivedBookID);
    FILE *file = fopen("books.dat", "rb+");
    if (file == NULL) {
        printf("Error opening books file.\n");
        return;
    }
    int fd = getFileDescriptor(file);
    WritelockFile(fd);
    Book book;
    FILE *tempFile = fopen("temp.dat", "wb+");
    int fd2 = getFileDescriptor(tempFile);
    WritelockFile(fd2);
    if (tempFile == NULL) {
        printf("Error opening temp file.\n");
        unlockFile(fd);
        fclose(file);
        return;
    }
    int found = 0;
    while (fread(&book, sizeof(Book), 1, file)) {
        if (book.book_id != receivedBookID) {
            fwrite(&book, sizeof(Book), 1, tempFile);
        } else {
            found = 1;
        }
    }
    unlockFile(fd);
    fclose(file);
    unlockFile(fd2);
    fclose(tempFile);
    if (!found) {
        char response[100];
        snprintf(response, sizeof(response), "Book with ID %d not found.\n", receivedBookID);
        write(socket_descriptor, response, sizeof(response));
        remove("temp.dat");
        return;
    }
    remove("books.dat");
    rename("temp.dat", "books.dat");
    char response[100];
    snprintf(response, sizeof(response), "Book with ID %d deleted successfully.\n", receivedBookID);
    write(socket_descriptor, response, sizeof(response));
}


void viewBorrowedUsers(int socket_descriptor) {
    FILE* file = fopen("borrowed_books.dat", "rb");
    if (file == NULL) {
        printf("Error opening borrowed_books file.\n");
        return;
    }
    int fd = getFileDescriptor(file);
    ReadlockFile(fd);
    BorrowedBook borrowedBook;
    User user;
    int count = 0;
    char buffer[1024] = {0};
    int unique_user_ids[100] = {0};
    int unique_user_count = 0;
    int is_unique;
    while (fread(&borrowedBook, sizeof(BorrowedBook), 1, file)) {
        is_unique = 1;
        for (int i = 0; i < unique_user_count; ++i) {
            if (borrowedBook.user_id == unique_user_ids[i]) {
                is_unique = 0;
                break;
            }
        }
        if (is_unique) {
            unique_user_ids[unique_user_count++] = borrowedBook.user_id;
            FILE* userFile = fopen("users.dat", "rb");
            if (userFile == NULL) {
                printf("Error opening users file.\n");
                unlockFile(fd);
                fclose(file);
                return;
            }
            int fd2 = getFileDescriptor(userFile);
            ReadlockFile(fd2);
            while (fread(&user, sizeof(User), 1, userFile)) {
                if (user.id == borrowedBook.user_id) {
                    sprintf(buffer + strlen(buffer), "User ID: %d\nUsername: %s\n\n", user.id, user.username);
                    count++;
                    break;
                }
            }
            unlockFile(fd2);
            fclose(userFile);
        }
    }
    unlockFile(fd);
    fclose(file);
    if (count == 0) {
        strcpy(buffer, "No users have borrowed books.\n");
    }
    write(socket_descriptor, buffer, sizeof(buffer));
}



void handleViewUserDetails(int socket_descriptor) {
    int userId;
    read(socket_descriptor, &userId, sizeof(int));
    FILE* file = fopen("users.dat", "rb");
    if (file == NULL) {
        char error_message[] = "Error opening user details file.\n";
        write(socket_descriptor, error_message, sizeof(error_message) - 1);
        return;
    }
    int fd = getFileDescriptor(file);
    ReadlockFile(fd);
    User user;
    int user_found = 0; // Flag to indicate if user is found
    while (fread(&user, sizeof(User), 1, file)) {
        if (user.id == userId) {
            user_found = 1;
            char response[1024];
            write(socket_descriptor, &user_found, sizeof(int));
            snprintf(response, sizeof(response), "User ID: %d\nUsername: %s\nPassword: %s\nType: %s\n", user.id, user.username, user.password, user.type);
            write(socket_descriptor, response, sizeof(response));
            send_borrowed_books(userId, socket_descriptor);
            break;
        }
    }
    unlockFile(fd);
    fclose(file);
    if (!user_found){
        write(socket_descriptor, &user_found, sizeof(int));
        char response[1024] = {0};
        snprintf(response, sizeof(response),"User not found.\n");
        write(socket_descriptor, response, sizeof(response));
    }
}

void modify_book(int socket_descriptor) {
    int book_id, new_quantity;
    send_books(socket_descriptor);
    read(socket_descriptor, &book_id, sizeof(int));
    read(socket_descriptor, &new_quantity, sizeof(int));
    FILE *file = fopen("books.dat", "rb+");
    char buffer[1024] = {0};
    if (file == NULL) {
        strcpy(buffer, "Could not open books file.\n");
        write(socket_descriptor, buffer, sizeof(buffer));
        return;
    }
    int fd = getFileDescriptor(file);
    WritelockFile(fd);
    Book book;
    int found = 0;
    while (fread(&book, sizeof(Book), 1, file)) {
        if (book.book_id == book_id) {
            book.quantity = new_quantity;
            fseek(file, -sizeof(Book), SEEK_CUR);
            fwrite(&book, sizeof(Book), 1, file);
            strcpy(buffer, "Book details modified successfully.\n");
            found = 1;
            break;
        }
    }
    unlockFile(fd);
    fclose(file);
    if (!found) {
        strcpy(buffer, "Book ID not found.\n");
    }
    write(socket_descriptor, buffer, sizeof(buffer));
}
