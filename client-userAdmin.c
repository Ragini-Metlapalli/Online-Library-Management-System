#include "client.h"
#include <stdio.h>
#define PORT 8080
int currID;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) { //This converts the IPv4 address string "127.0.0.1" to a binary form suitable for the sin_addr
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected to the server\n");
    authenticate(sock);
    close(sock);
    return 0;
}


//option
void authenticate(int socket_descriptor){
    write(1, "\nWELCOME TO ONLINE LIBRARY MANAGEMENT SYSTEM\n", sizeof("WELCOME TO ONLINE LIBRARY MANAGEMENT SYSTEM\n"));
	while(1){
        
        write(1, "1: Admin Login\n",sizeof("3: Admin Login\n"));
		write(1, "2: User Login\n",sizeof("1: User Login\n"));
		write(1, "3: User Sign up\n",sizeof("2: User Sign up\n"));
		write(1, "4. Exit\n\n", sizeof("4. Exit\n"));
		write(1,"Enter your choice\n",sizeof("Enter your choice\n"));
		
		int choice ; 
		scanf("%d", &choice);

		if(choice == 1){
            printf("\n");
			admin_login(socket_descriptor);
            printf("\n");
			break;
		}
		if (choice == 2){
            printf("\n");
			user_login(socket_descriptor);
            printf("\n");
			break;
		}
		if(choice == 3){
            printf("\n");
			user_signup(socket_descriptor);
            printf("\n");
			break;
		}
        if(choice == 4){
            printf("\n");
            printf("Exiting the library\n");
            exit(EXIT_FAILURE);
        }
        else{
            printf("\n");
            write(1, "Choose a valid option\n", sizeof("Choose a valid option\n"));
        }
	
	}
}

void admin_login(int socket_descriptor){
    int request_type = 11;
    write(socket_descriptor, &request_type, sizeof(int));
	int result;
    Admin ad;
    write(1, "Enter Admin ID : ", sizeof("Enter Admin ID : "));
    scanf("%d", &ad.id);
    currID = ad.id ;   
    write(1, "Password : ", sizeof("Password : "));
    scanf("%s", ad.password);
    write(socket_descriptor, &ad, sizeof(Admin));
    read(socket_descriptor, &result, sizeof(int)); 
    if(!result){
        write(1, "\nInvalid Login. Try again.\n", sizeof("\nInvalid Login. Try again.\n"));
        authenticate(socket_descriptor);
    }
    else{
        write(1, "Successfully logged in\n", sizeof("Successfully logged in\n"));
        admin_menu(socket_descriptor);
    }
}

void user_login(int socket_descriptor){
    int request_type = 12;
    write(socket_descriptor, &request_type, sizeof(int));
    int result;
    User us;
    write(1, "Enter User ID: ", sizeof("Enter User ID: "));
    scanf("%d", &us.id);
    currID = us.id;
    write(1, "Password: ", sizeof("Password: "));
    scanf("%s", us.password);
    write(socket_descriptor, &us, sizeof(User));
    read(socket_descriptor, &result, sizeof(int));
    if(result){
        write(1,"Successfully logged in\n",sizeof("Successfully logged in\n"));   //add the name after getting it from the server
        user_menu(socket_descriptor);
    }
    else{
        write(1, "\nInvalid Login. Try again.\n", sizeof("\nInvalid Login. Try again.\n"));
        authenticate(socket_descriptor);
    }
}

void user_signup(int socket_descriptor){
    int request_type = 13;
    write(socket_descriptor, &request_type, sizeof(int));
    int result;
    User us;
    printf("Enter User ID: ");
    scanf("%d", &us.id);
    //currID = us.id;
    printf("Username: ");
    scanf("%s", us.username);
    printf("Password: ");
    scanf("%s", us.password);
    // Confirm password
    char confirm_password[50];
    printf("Confirm Password: ");
    scanf("%s", confirm_password);
    // Check if passwords match
    if (strcmp(us.password, confirm_password) != 0) {
        printf("Passwords do not match. Sign-up failed.\n");
        authenticate(socket_descriptor);
        //return;
    }
    strcpy(us.type, "user");
    // Send user data to server for sign-up
    write(socket_descriptor, &us, sizeof(User));
    read(socket_descriptor, &result, sizeof(int));
    if(result){
        printf("Successfully signed up. Now please login.\n\n");
        user_login(socket_descriptor); // Proceed to user login after sign-up
    }
    else{
        printf("Sign-up failed, user ID already exists. Please try again.\n");
        authenticate(socket_descriptor);
    }
}


void user_menu(int socket_descriptor) {
    int choice;
    while (1) {
        printf("\n");
        write(1, "1. Search Books\n", sizeof("1. Search Books\n"));
        write(1, "2. View Borrowed Books\n", sizeof("2. View Borrowed Books\n"));
        write(1, "3. Borrow Book\n", sizeof("3. Borrow Book\n"));
        write(1, "4. Return Book\n", sizeof("4. Return Book\n"));
        write(1, "5. Change Password\n", sizeof("5. Change Password\n"));
        write(1, "6. View Profile\n", sizeof("6. View Profile\n"));
        write(1, "7. Logout\n", sizeof("7. Logout\n"));
        write(1, "\nEnter your choice: ", sizeof("Enter your choice: "));
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\n");
                search_books(socket_descriptor);
                printf("\n");
                break;
            case 2:
                printf("\n");
                view_borrowed_books(socket_descriptor);
                printf("\n");
                break;
            case 3:
                printf("\n");
                borrow_book(socket_descriptor);
                printf("\n");
                break;
            case 4:
                printf("\n");
                return_book(socket_descriptor);
                printf("\n");
                break;
            case 5:
                printf("\n");
                changePassword(socket_descriptor);
                printf("\n");
                break;
            case 6:
                printf("\n");
                viewProfile(socket_descriptor);
                printf("\n");
                break;
            case 7:
                printf("\n");
                write(1, "Logged out successfully.\n", sizeof("Logged out successfully.\n"));
                authenticate(socket_descriptor);
                break;
            default:
                printf("\n");
                write(1, "Invalid option. Please try again.\n", sizeof("Invalid option. Please try again.\n"));
                break;
        }
    }
}

void admin_menu(int socket_descriptor) {
    int choice;
    while (1) {
        printf("\n");
        write(1, "1. Add a new Book\n", sizeof("1. Add a new Book\n"));
        write(1, "2. Remove a Book from the library\n", sizeof("2. Remove a Book from the library\n"));
        write(1, "3. View Availbale Books\n", sizeof("3. View Availbale Books\n"));
        write(1, "4. View User Details\n", sizeof("4. View User Details\n"));
        write(1, "5. View Users who borrowed books\n", sizeof("5. View Users who borrowed books\n"));
        write(1, "6. Update book quantity\n", sizeof("6. Update book quantity\n"));
        write(1, "7. Logout\n", sizeof("7. Logout\n"));
        write(1, "\nEnter your choice: ", sizeof("Enter your choice: "));
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\n");
                addBook(socket_descriptor);
                printf("\n");
                break;
            case 2:
                printf("\n");
                deleteBook(socket_descriptor);
                printf("\n");
                break;
            case 3:
                printf("\n");
                show_books(socket_descriptor);
                printf("\n");
                break;
            case 4:
                printf("\n");
                viewUserDetails(socket_descriptor);
                printf("\n");
                break;
            case 5:
                printf("\n");
                viewBorrowedUsers(socket_descriptor);
                printf("\n");
                break;
            case 6:
                printf("\n");
                modifyBook(socket_descriptor);
                printf("\n");
                break;
            case 7:
                printf("\n");
                write(1, "Logged out successfully.\n", sizeof("Logged out successfully.\n"));
                authenticate(socket_descriptor);
                break;
            default:
                printf("\n");
                write(1, "Invalid option. Please try again.\n", sizeof("Invalid option. Please try again.\n"));
                break;
        }
    }
}


void search_books(int socket_descriptor) {
    int request_type = 1; // Request type for advanced search
    write(socket_descriptor, &request_type, sizeof(int));
    int filter_choice;
    char criteria[256], search_term[256];
    while (1) {
        // Display menu options
        printf("Choose search filter:\n");
        printf("1. Title\n");
        printf("2. Author\n");
        printf("3. Genre\n");
        
        // Read user input
        scanf("%d", &filter_choice);

        // Validate user input
        if (filter_choice >= 1 && filter_choice <= 3) {
            break;  // Exit loop if input is valid
        } else {
            // Invalid input, display error message
            printf("Invalid choice. Please enter a number between 1 and 3.\n");
        }
    }
    printf("Enter search term: ");
    scanf(" %[^\n]", search_term);

    //write(socket_descriptor, criteria, sizeof(criteria));
    write(socket_descriptor, &filter_choice, sizeof(int));
    write(socket_descriptor, search_term, sizeof(search_term));

    char buffer[4096];
    read(socket_descriptor, buffer, sizeof(buffer));
    printf("Available books search Results:\n%s\n", buffer);
    user_menu(socket_descriptor);
}


void view_borrowed_books(int socket_descriptor) {
    int request_type = 2;
    write(socket_descriptor, &request_type, sizeof(int));
    BorrowedBook borrowed_book;
    int found = 0;
    int user_id = currID;
    // Send user ID to server
    write(socket_descriptor, &user_id, sizeof(int));
    // Receive borrowed book information from server
    char buffer[4096] = {0};
    read(socket_descriptor, buffer, sizeof(buffer));
    printf("%s\n", buffer);
    user_menu(socket_descriptor);
}


void display_books(int socket_descriptor) {
    char buffer[1024] = {0};
    read(socket_descriptor, buffer, sizeof(buffer));
    printf("Available Books:\n%s\n", buffer);
}

void show_books(int socket_descriptor) {
    int request_type = 14;
    write(socket_descriptor, &request_type, sizeof(int));
    char buffer[1024] = {0};
    read(socket_descriptor, buffer, sizeof(buffer));
    printf("Available Books:\n%s\n", buffer);
    admin_menu(socket_descriptor);
}


void borrow_book(int socket_descriptor) {
    int request_type = 3;
    int user_id = currID, book_id;
    char buffer[1024] = {0};
    char continue_borrow;
    do {
        write(socket_descriptor, &request_type, sizeof(int));
        write(socket_descriptor, &user_id, sizeof(int));
        display_books(socket_descriptor);
        printf("Select a book you wish to borrow by entering the Book ID: ");
        scanf("%d", &book_id);
        // Send book ID to server for borrowing
        write(socket_descriptor, &book_id, sizeof(int));
        read(socket_descriptor, buffer, sizeof(buffer));
        printf("\n%s\n", buffer);
        // Asking the user if they want to borrow another book
        printf("\nDo you want to borrow another book? (y/n): ");
        scanf(" %c", &continue_borrow);
    } while (continue_borrow == 'y' || continue_borrow == 'Y');

    user_menu(socket_descriptor);
}




void return_book(int socket_descriptor) {
    int request_type = 4;
    int user_id = currID;
    int book_id_to_return, dummy;
    int found, result;
    char continue_return;
    char buffer[1024] = {0};
    do {
        write(socket_descriptor, &request_type, sizeof(int));
        write(socket_descriptor, &user_id, sizeof(int));
        // Receive borrowed books list from server
        read(socket_descriptor, buffer, sizeof(buffer));
        read(socket_descriptor, &found, sizeof(int));
        write(socket_descriptor, &found, sizeof(int));
        if(found == 1){
            printf("%s\n", buffer);
            // Prompt user to enter the book ID to return
            printf("Enter the Book ID to return: ");
            scanf("%d", &book_id_to_return);
            // Send the chosen book ID to the server for returning
            write(socket_descriptor, &book_id_to_return, sizeof(int));
            read(socket_descriptor, &result, sizeof(int));
            if(result){
                printf("\nThe book has been returned succesfully.\n");
            }
            else{
                printf("\nInvalid Book ID. Failed to return the book.\n");
            }
        }
        else {
            printf("%s\n", buffer);
        }
        printf("\nDo you want to return another book? (y/n): ");
        scanf(" %c", &continue_return);
    } while (continue_return == 'y' || continue_return == 'Y');
    
    user_menu(socket_descriptor);
}


void changePassword(int socket_descriptor) {
    int request_type = 5;
    write(socket_descriptor, &request_type, sizeof(int));
    User user;
    char buffer[1024] = {0};
    int user_id = currID;
    user.id = user_id;
    printf("Enter new password: ");
    scanf("%s", user.password);
    write(socket_descriptor, &user, sizeof(User));
    printf("Password change request sent to the server.\n");
    read(socket_descriptor, buffer, sizeof(buffer));
    printf("%s\n", buffer);
    user_menu(socket_descriptor);
}



void viewProfile(int socket_descriptor) {
    int request_type = 6;
    write(socket_descriptor, &request_type, sizeof(int));
    int userID = currID;
    write(socket_descriptor, &userID, sizeof(int));
    User user;
    read(socket_descriptor, &user, sizeof(User));
    printf("\nUser ID: %d\nUsername: %s\nPassword: %s\nType: %s\n", user.id, user.username, user.password, user.type);
    user_menu(socket_descriptor);
}



void addBook(int socket_descriptor){
    int request_type = 7;
    write(socket_descriptor, &request_type, sizeof(int));
    Book newBook;
    char buffer[100];
    // Get book details from admin
    printf("Enter the details of the book that you would like to add: \n");
    printf("Enter Book ID: ");
    scanf("%d", &newBook.book_id);
    printf("Enter Title: ");
    scanf(" %[^\n]", newBook.title);
    printf("Enter Author: ");
    scanf(" %[^\n]", newBook.author);
    printf("Enter Quantity: ");
    scanf("%d", &newBook.quantity);
    printf("Enter Genre: ");
    scanf(" %[^\n]", newBook.genre);
    // Send book details to server
    write(socket_descriptor, &newBook, sizeof(Book));
    printf("Book details sent to server\n");
    read(socket_descriptor, buffer, sizeof(buffer));
    printf("\n%s\n", buffer);
    admin_menu(socket_descriptor);
}



void deleteBook(int socket_descriptor){
    int request_type = 8;
    write(socket_descriptor, &request_type, sizeof(int));
    int bookID;
    char response[100];
    display_books(socket_descriptor);
    printf("Enter Book ID to delete: ");
    scanf("%d", &bookID);
    // Send book ID to server
    write(socket_descriptor, &bookID, sizeof(int));
    printf("Book ID sent to server for deletion.\n");
    read(socket_descriptor, response, sizeof(response));
    printf("\n%s\n", response);
    admin_menu(socket_descriptor);
}



void viewBorrowedUsers(int socket_descriptor) {
    int request_type = 9;
    write(socket_descriptor, &request_type, sizeof(int));
    char buffer[1024]; 
    memset(buffer, 0, sizeof(buffer));
    read(socket_descriptor, buffer, sizeof(buffer));
    // Print the received data (user details or message)
    printf("\nDetails of users who borrowed books:\n%s\n", buffer);
    admin_menu(socket_descriptor);
}



void viewUserDetails(int socket_descriptor) {
    int request_type = 10;
    write(socket_descriptor, &request_type, sizeof(int));
    char buffer[1024] = {0}, book_buff[1024] = {0}; 
    int user_id, found, dummy;
    // memset(buffer, 0, sizeof(buffer));
    printf("Enter the user ID you wish to know the details of: ");
    scanf("%d", &user_id);
    write(socket_descriptor,&user_id, sizeof(int));
    // Read data from server
    read(socket_descriptor, &found, sizeof(int));
    if(found){
        read(socket_descriptor, buffer, sizeof(buffer));
        // Print the received data (user details or message)
        printf("%s", buffer);
        read(socket_descriptor, book_buff, sizeof(book_buff));
        printf("Borrowed Books: \n%s\n", book_buff);
        read(socket_descriptor, &dummy, sizeof(int));
        admin_menu(socket_descriptor);
        return;
    }
    else{
        read(socket_descriptor, buffer, sizeof(buffer));
        printf("\n%s\n", buffer);
        admin_menu(socket_descriptor);
        return;
    }
    admin_menu(socket_descriptor);
}

void modifyBook(int socket_descriptor) {
    int request_type = 15; 
    write(socket_descriptor, &request_type, sizeof(int));
    int book_id, new_quantity;
    display_books(socket_descriptor);
    printf("Enter the Book ID to modify: ");
    scanf("%d", &book_id);
    printf("Enter the new quantity: ");
    scanf("%d", &new_quantity);
    write(socket_descriptor, &book_id, sizeof(int));
    write(socket_descriptor, &new_quantity, sizeof(int));
    char buffer[1024] = {0};
    read(socket_descriptor, buffer, sizeof(buffer));
    printf("\n%s\n", buffer);
    admin_menu(socket_descriptor);
}