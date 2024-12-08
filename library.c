#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#define CLEAR_REGEX "\e[1;1H\e[2J"

#ifdef _WIN32
#include <windows.h>
#define SLEEP(ms) Sleep(ms)
#elif defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#define SLEEP(ms) usleep((ms) * 1000)
#endif

typedef struct
{
    char id[7];
    char title[100];
    char author[100];
    int isBorrowed;
    char description[100];
} Book;

typedef struct
{
    char username[30];
    char password[30];
    int isAdmin;
    Book *books[3];
} User;

User *users[100] = {NULL};
Book *books[100] = {NULL};

void show_logo()
{
    const char *LOGO = "  _      _ _                             \n"
                       " | |    (_) |                            \n"
                       " | |     _| |__   ___  _ __ ___   __ _   \n"
                       " | |    | | '_ \\ / _ \\| '_ ` _ \\ / _` |  \n"
                       " | |____| | | | | (_) | | | | | | (_| |  \n"
                       " |______|_|_| |_|\\___/|_| |_| |_|\\__,_|  \n";

    printf("%s\n", LOGO);
    puts("LIBRARY - A Place to Explore Knowledge\n");
}
void CLEAR_SCREEN()
{
    printf("%s", CLEAR_REGEX);
}
void loadingAnimation()
{
    printf("Loading...\n");
    SLEEP(1000);
};

void getUsers(FILE *fp);
void showUsers(FILE *fp);
void printUserInformation(User *user);
void login(User *user, FILE *fp);
void registerUser(User *user, FILE *fp);
int authentication(User *user, FILE *fp);
void getBooks(FILE *fp);
void showBooks(FILE *fp);
void createBookHeader();
void createBook(FILE *fp);
void deleteBook(FILE *fp);
void editBook(FILE *fp);
void borrowBook(FILE *fpUser, FILE *fpBook, User *user);
void returnBook(FILE *fpUser, FILE *fpBook);
void runLibrary();

int main()
{
    runLibrary();
    return 0;
}

// Utility
void showUsers(FILE *fp)
{
    CLEAR_SCREEN();
    show_logo();
    loadingAnimation();
    printf("Users\n");
    getUsers(fp);
    for (int i = 0; i < 100 && users[i] != NULL; i++)
    {
        printf("1.\nUsername : %s\nPassword : %s\n\n", users[i]->username, users[i]->password);
    }
    printf("Press enter to continue\n");
    getchar();
}

void getUsers(FILE *fp)
{
    for (int i = 0; i < 100; i++)
    {
        if (users[i] != NULL)
        {
            free(users[i]);
        }
        else
        {
            break;
        }
    }
    User *user = (User *)malloc(sizeof(User));
    fseek(fp, 0, SEEK_SET);
    int count = 0;
    while (fread(user, sizeof(User), 1, fp) == 1)
    {
        users[count] = (User *)malloc(sizeof(User)); // Allocate memory for each user
        memcpy(users[count], user, sizeof(User));
        count++;
    }
    free(user);
}

void login(User *user, FILE *fp)
{
    char username[30], password[30];
    int authenticated = 0;
    do
    {
        getUsers(fp);
        CLEAR_SCREEN();
        show_logo();
        printf("Login User\n");
        if (authenticated == 0)
        {
            printf("Please input the right username and password\n\n");
        }
        printf("Username : ");
        scanf("%[^\n]", username);
        getchar();
        printf("Password : ");
        scanf("%[^\n]", password);
        getchar();
        if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0)
        {
            authenticated = 1;
            user->isAdmin = 1;
            strcpy(user->username, "admin");
            strcpy(user->password, "admin123");
            break;
        }
        for (int i = 0; i < 100 && users[i] != NULL; i++)
        {
            if (strcmp(users[i]->username, username) == 0 && strcmp(users[i]->password, password) == 0)
            {
                authenticated = 1;
                memcpy(user, users[i], sizeof(User));
                break;
            }
        }
    } while (authenticated == 0);
}

void printUserInformation(User *user)
{
    printf("User's Profile\n");
    printf("Username : %s\n", user->username);
    printf("Role : %s\n", user->isAdmin == 1 ? "Admin" : "User");
}

void registerUser(User *user, FILE *fp)
{
    int validated = 0;
    char temp[30];
    do
    {
        int usernameExists = 0, usernameContainsSpace = 0, usernameContainsAdmin = 0;
        do
        {
            CLEAR_SCREEN();
            show_logo();
            validated = 1;
            printf("Register User\n");
            printf("Username must be 5-20 characters long\n\n");
            if (usernameExists == 1)
            {
                printf("Username already exists\n");
                usernameExists = 0;
            }
            if (usernameContainsSpace == 1)
            {
                printf("Username cannot contain space\n");
                usernameContainsSpace = 0;
            }
            if (usernameContainsAdmin == 1)
            {
                printf("Username cannot be admin\n");
                usernameContainsAdmin = 0;
            }
            printf("username : ");
            scanf("%[^\n]", temp);
            getchar();
            if (strlen(temp) < 5 || strlen(temp) > 20)
            {
                validated = 0;
                continue;
            }
            if (strcmp(temp, "admin") == 0)
            {
                validated = 0;
                usernameContainsAdmin = 1;
                continue;
            }
            for (int i = 0; i < strlen(temp); i++)
            {
                if (temp[i] != ' ')
                {
                    temp[i] = tolower(temp[i]);
                }
                else
                {
                    usernameContainsSpace = 1;
                    validated = 0;
                    break;
                }
            }

            for (int i = 0; users[i] != NULL; i++)
            {
                if (strcmp(temp, users[i]->username) == 0)
                {
                    usernameExists = 1;
                    validated = 0;
                    break;
                }
            }

            if (validated == 1)
            {
                strcpy(user->username, temp);
            }
        } while (validated == 0);

        do
        {
            CLEAR_SCREEN();
            show_logo();
            validated = 1;
            printf("Register User\n");
            printf("Password must be 8-20 characters long\n");
            printf("Insert password : ");
            scanf("%[^\n]", temp);
            getchar();
            if (strlen(temp) < 8 || strlen(temp) > 20)
            {
                validated = 0;
                continue;
            }
            for (int i = 0; i < strlen(temp); i++)
            {
                temp[i] = tolower(temp[i]);
            }

            strcpy(user->password, temp);
        } while (validated == 0);
    } while (validated == 0);

    for (int i = 0; i < 3; i++)
    {
        user->books[i] = NULL;
    }

    fwrite(user, sizeof(User), 1, fp);
    fflush(fp);
    loadingAnimation();
    printf("Successfully added new user to the system\n");
    printf("username : %s\n", user->username);

    printf("Press enter to continue\n");
    getchar();
}

int authentication(User *user, FILE *fp)
{
    int menu = 0;
    do
    {
        CLEAR_SCREEN();
        show_logo();
        printf("1. Login\n2. Register\n3. Exit\n");
        scanf("%d", &menu);
        getchar();
        if (menu == 1)
        {
            login(user, fp);
            break;
        }
        else if (menu == 2)
        {
            registerUser(user, fp);
            break;
        }
    } while (menu != 3);
    if (menu == 3)
    {
        return 1;
    }
    return 0;
}

// Admin section

void resetFile(const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror("Error opening file");
        return;
    }
    fclose(fp);
}

void showBooks(FILE *fp)
{
    CLEAR_SCREEN();
    show_logo();
    loadingAnimation();
    printf("Books\n");
    getBooks(fp);
    for (int i = 0; i < 100 && books[i] != NULL; i++)
    {
        printf("%d.\nId : %s\nTitle : %s\nAuthor : %s\nDescription : %s\nIs Borrowed : %d\n\n", i + 1, books[i]->id, books[i]->title, books[i]->author, books[i]->description, books[i]->isBorrowed);
    }
    printf("Press enter to continue\n");
    getchar();
}

void getBooks(FILE *fp)
{
    for (int i = 0; i < 100; i++)
    {
        if (books[i] != NULL)
        {
            free(books[i]);
            books[i] = NULL;
        }
        else
        {
            break;
        }
    }
    Book *book = (Book *)malloc(sizeof(Book));
    fseek(fp, 0, SEEK_SET);
    int count = 0;
    while (fread(book, sizeof(Book), 1, fp) == 1)
    {
        books[count] = (Book *)malloc(sizeof(Book));
        memcpy(books[count], book, sizeof(Book));
        count++;
    }
    free(book);
}

// Create Book section

void createIdBook(FILE *fp, char *id)
{
    char newId[6];
    int validated = 0;
    do
    {
        getBooks(fp);

        char randomize[5];
        int exists = 0;

        do
        {
            char id[6] = "B";
            for (int i = 0; i < 4; i++)
            {
                int randomDigit = rand() % 10;
                randomize[i] = '0' + randomDigit;
            }
            randomize[4] = '\0';
            strcat(id, randomize);

            exists = 0;
            for (int i = 0; i < 100 && books[i] != NULL; i++)
            {
                if (strcmp(id, books[i]->id) == 0)
                {
                    exists = 1;
                    break;
                }
            }
        } while (exists == 1);

        strcpy(newId, "B");
        strcat(newId, randomize);

        validated = 1;
    } while (validated == 0);

    strcpy(id, newId);
}

void createBookHeader()
{
    CLEAR_SCREEN();
    show_logo();
    printf("Create Book\n");
}

void createBook(FILE *fp)
{
    Book *book = (Book *)malloc(sizeof(Book));
    if (book == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    char temp[100];
    int validated = 0;

    do
    {
        createBookHeader();
        validated = 1;
        printf("Title must be 5-50 characters long\n\n");
        printf("Title : ");
        scanf("%[^\n]", temp);
        getchar(); // Clear newline
        if (strlen(temp) < 5 || strlen(temp) > 100)
        {
            validated = 0;
            printf("Invalid title length. Try again.\n");
            SLEEP(1000);
        }
        else
        {
            strcpy(book->title, temp);
        }
    } while (validated == 0);
    do
    {
        createBookHeader();
        validated = 1;
        printf("Description must be 5-50 characters long\n\n");
        printf("Description : ");
        scanf("%[^\n]", temp);
        getchar();
        if (strlen(temp) < 5 || strlen(temp) > 99)
        {
            validated = 0;
            printf("Invalid description length. Try again.\n");
            SLEEP(1000);
        }
        else
        {
            strcpy(book->description, temp);
        }
    } while (validated == 0);

    do
    {
        createBookHeader();
        validated = 1;
        printf("Author name must be 5-30 characters long\n\n");
        printf("Author : ");
        scanf("%[^\n]", temp);
        getchar();
        if (strlen(temp) < 5 || strlen(temp) > 30)
        {
            validated = 0;
            printf("Invalid author name length. Try again.\n");
            SLEEP(1000);
        }
        else
        {
            strcpy(book->author, temp);
        }
    } while (validated == 0);

    createIdBook(fp, book->id);
    book->isBorrowed = 0;

    for (int i = 0; i < 100; i++)
    {
        if (books[i] == NULL)
        {
            books[i] = book;
            break;
        }
    }
    // Save the book in the file
    fwrite(book, sizeof(Book), 1, fp);
    fflush(fp);
    createBookHeader();
    loadingAnimation();
    printf("Successfully added new book to the system\n");
    printf("Id : %s\nTitle : %s\nAuthor : %s\nDescription : %s\n\n",
           book->id, book->title, book->author, book->description);
    printf("Press enter to continue\n");
    getchar();
}

// Delete book section

void deleteBookHeader()
{
    CLEAR_SCREEN();
    show_logo();
    printf("Delete Book\n");
}

void deleteBook(FILE *fp)
{
    char temp[7];
    int exists = 0;
    do
    {
        deleteBookHeader();
        getBooks(fp);
        printf("Insert book id (Enter 0 to cancel): ");
        scanf("%[^\n]", temp);
        getchar();
        if (strcmp(temp, "0") != 0)
        {
            for (int i = 0; i < 100 && books[i] != NULL; i++)
            {
                if (strcmp(temp, books[i]->id) == 0)
                {
                    exists = 1;
                    break;
                }
            }
        }
        else
        {
            break;
        }
        if (exists == 0)
        {
            printf("Failed to find book by id, please try again\n\n");
            SLEEP(2000);
        }
    } while (exists == 0);

    if (exists == 1)
    {
        Book *newBooks[100] = {NULL};
        int count = 0;
        resetFile("book.dat");
        for (int i = 0; i < 100 && books[i] != NULL; i++)
        {
            if (books[i] != NULL && strcmp(temp, books[i]->id) != 0)
            {
                fwrite(books[i], sizeof(Book), 1, fp);
                newBooks[count++] = books[i];
            }
        }

        memcpy(books, newBooks, sizeof(newBooks));
        printf("Book with ID %s deleted successfully.\n", temp);
    }
}

// End of delete book section

// Edit book section

void editBookHeader()
{
    CLEAR_SCREEN();
    show_logo();
    printf("Edit Book\n");
}

void editBook(FILE *fp)
{
    char temp[7];
    int exists = 0;
    int book_idx = 0;
    do
    {
        editBookHeader();
        getBooks(fp);
        printf("Insert book id (Enter 0 to cancel): ");
        scanf("%[^\n]", temp);
        getchar();
        if (strcmp(temp, "0") != 0)
        {
            for (int i = 0; i < 100 && books[i] != NULL; i++)
            {
                if (strcmp(temp, books[i]->id) == 0)
                {
                    book_idx = i;
                    exists = 1;
                    break;
                }
            }
        }
        else
        {
            break;
        }
        if (exists == 0)
        {
            printf("Failed to find book by id, please try again\n\n");
            SLEEP(1000);
        }
    } while (exists == 0);

    if (exists == 1)
    {
        int menu = 0;
        do
        {
            editBookHeader();
            printf("Choose what you want to edit:\n");
            printf("1. Title\n2. Description\n3. Author\n4. Is Borrowed\n5. Exit\n");
            scanf("%d", &menu);
            getchar();
            puts("");
            char temp[100];
            switch (menu)
            {
            case 1:
                printf("Insert new title: ");
                scanf("%[^\n]", temp);
                if (strlen(temp) > 100 || strlen(temp) < 5)
                {
                    printf("Invalid title length. Try again.\n");
                    SLEEP(1000);
                }
                else
                {
                    strcpy(books[book_idx]->title, temp);
                }
                break;
            case 2:
                printf("Insert new description: ");
                scanf("%[^\n]", temp);
                if (strlen(temp) > 100 || strlen(temp) < 5)
                {
                    printf("Invalid description length. Try again.\n");
                    SLEEP(1000);
                }
                else
                {
                    strcpy(books[book_idx]->description, temp);
                }
                break;
            case 3:
                printf("Insert new author: ");
                scanf("%[^\n]", temp);
                if (strlen(temp) > 100 || strlen(temp) < 5)
                {
                    printf("Invalid author length. Try again.\n");
                    SLEEP(1000);
                }
                else
                {
                    strcpy(books[book_idx]->author, temp);
                }
                break;
            case 4:
                printf("Is borrowed : %d\n", books[book_idx]->isBorrowed);
                printf("Change to (1 for yes, 0 for no): ");
                scanf("%d", &books[book_idx]->isBorrowed);

                break;
            default:
                printf("Invalid input. Try again.\n");
                SLEEP(1000);
                break;
            }
        } while (menu != 4);
        resetFile("book.dat");
        for (int i = 0; i < 100 && books[i] != NULL; i++)
        {
            fwrite(books[i], sizeof(Book), 1, fp);
        }
        printf("Book with ID %s edited successfully.\n", temp);
        SLEEP(1000);
    }
}

// End of edit book section

// End of admin section

// User section

void userHeader()
{
    loadingAnimation();
    CLEAR_SCREEN();
    show_logo();
}

void showAvailBooks(FILE *fpBook)
{
    userHeader();
    printf("\nShow Available Books\n");
    getBooks(fpBook);
    for (int i = 0, count = 1; i < 100 && books[i] != NULL; i++)
    {
        if (books[i]->isBorrowed == 0)
        {
            printf("%d.\nId : %s\nTitle : %s\nAuthor : %s\nDescription : %s\n\n", count, books[i]->id, books[i]->title, books[i]->author, books[i]->description);
            count++;
        }
    }
    printf("Press enter to continue\n");
    getchar();
}

void showBorrowedBooks(User *user)
{
    // Check if user pointer is valid
    if (user == NULL)
    {
        printf("Invalid user.\n");
        return;
    }

    // Check if books array is valid
    if (user->books == NULL)
    {
        printf("No books array found.\n");
        return;
    }

    userHeader();
    printf("\nShow Borrowed Books\n");

    // Loop through the books array
    for (int i = 0; i < 3; i++)
    {
        // Check if the current book pointer is valid
        if (user->books[i] != NULL)
        {
            printf("%d.\nId : %s\nTitle : %s\nAuthor : %s\nDescription : %s\n\n",
                   i + 1,
                   user->books[i]->id,
                   user->books[i]->title,
                   user->books[i]->author,
                   user->books[i]->description);
        }
        else
        {
            printf("%d. [No Book Borrowed]\n\n", i + 1);
        }
    }
    printf("Press enter to continue\n");
    getchar();
}

void borrowBook(FILE *fpUser, FILE *fpBook, User *user) {
    char bookId[7];
    int bookFound = 0;

    // Display available books
    showAvailBooks(fpBook);

    // Prompt user for book ID to borrow
    userHeader();
    printf("Enter the Book ID to borrow (or 0 to cancel): ");
    scanf("%[^\n]", bookId);
    getchar();  // To handle newline character after input

    // If user enters "0", cancel the borrowing process
    if (strcmp(bookId, "0") == 0) {
        return;
    }

    // Search for the book with the entered ID
    for (int i = 0; i < 100 && books[i] != NULL; i++) {
        if (strcmp(books[i]->id, bookId) == 0 && books[i]->isBorrowed == 0) {
            // Book found and not borrowed yet
            bookFound = 1;

            // Mark the book as borrowed
            books[i]->isBorrowed = 1;

            // Find the first available slot in the user's borrowed books array
            for (int j = 0; j < 3; j++) {
                if (user->books[j] == NULL) {
                    user->books[j] = books[i];  // Assign the book to the user's borrowed books
                    break;
                }
            }

            // Update the file with the new state of the book (borrowed)
            rewind(fpBook);
            for (int i = 0; i < 100 && books[i] != NULL; i++) {
                fwrite(books[i], sizeof(Book), 1, fpBook);
            }

            // Update the user's information in the file
            rewind(fpUser);
            fseek(fpUser, 0, SEEK_SET);
            for (int i = 0; i < 100 && users[i] != NULL; i++) {
                if (strcmp(users[i]->username, user->username) == 0) {
                    fwrite(user, sizeof(User), 1, fpUser);
                    break;
                }
            }

            printf("Successfully borrowed the book with ID %s.\n", bookId);
            break;
        }
    }

    if (!bookFound) {
        printf("The book is either unavailable or does not exist.\n");
    }

    printf("Press Enter to continue.\n");
    getchar();
}



void returnBook(FILE *fpUser, FILE *fpBook)
{
}

// End of user section

void runLibrary()
{
    FILE *fp = fopen("user.dat", "ab+");
    FILE *fpBook = fopen("book.dat", "ab+");
    getUsers(fp);
    User *user = (User *)malloc(sizeof(User));
    int exit = authentication(user, fp);

    if (exit == 1)
    {
        free(user);
        return;
    }
    int menu = 0;
    if (user->isAdmin == 1)
    {
        do
        {
            CLEAR_SCREEN();
            show_logo();
            printUserInformation(user);
            printf("1. Create Book\n2. Show Books\n3. Delete Book\n4. Edit Book\n5. Show Users\n6. Exit\n");
            scanf("%d", &menu);
            getchar();
            switch (menu)
            {
            case 1:
                createBook(fpBook);
                break;
            case 2:
                showBooks(fpBook);
                break;
            case 3:
                deleteBook(fpBook);
                break;
            case 4:
                editBook(fpBook);
                break;
            case 5:
                showUsers(fp);
                break;
            case 6:
                break;
            }
        } while (menu != 6);
    }
    else
    {
        do
        {
            CLEAR_SCREEN();
            show_logo();
            printUserInformation(user);
            printf("1. Borrow Book\n2. Show Available Book\n3. Show Borrowed Book\n4. Return Book\n5. Exit\n");
            scanf("%d", &menu);
            getchar();
            switch (menu)
            {
            case 1:
                borrowBook(fp, fpBook, user);
                break;
            case 2:
                showAvailBooks(fpBook);
                break;
            case 3:
                showBorrowedBooks(user);
                break;
            case 4:
                returnBook(fp, fpBook);
                break;
            case 5:
                break;
            default:
                break;
            }
        } while (menu != 5);
    }
    free(user);
}
