// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
    char address[30];     // account address
    char phone[15];       // account phone
}; // end structure clientData

// prototypes
unsigned int enterChoice(void);
unsigned int getAccountNumber(void);
void textFile(FILE *readPtr);
void listRecords(FILE *fPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchRecord(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL)
        {
            printf("%s: File could not be opened.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            searchRecord(cfPtr);
            break;
        case 6:
            listRecords(cfPtr);
            break;
        default:
            puts("Incorrect choice");
            break;
        }
    }

    fclose(cfPtr);
    return 0;
} // end main

unsigned int getAccountNumber(void)
{
    unsigned int account;

    if (scanf("%u", &account) != 1)
    {
        while (getchar() != '\n' && !feof(stdin))
            ;
        return 0;
    }

    if (account < 1 || account > 100)
    {
        puts("Account number must be between 1 and 100.");
        return 0;
    }

    return account;
}

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    struct clientData client = {0, "", "", 0.0, "", ""};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
        return;
    }

    rewind(readPtr); // sets pointer to beginning of file
    fprintf(writePtr, "%-6s%-16s%-11s%10s%-30s%-15s\n", "Acct", "Last Name", "First Name", "Balance", "Address", "Phone");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-6d%-16s%-11s%10.2f%-30s%-15s\n", client.acctNum, client.lastName, client.firstName,
                    client.balance, client.address, client.phone);
        }
    }

    fclose(writePtr); // fclose closes the file
} // end function textFile

void listRecords(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, "", ""};
    unsigned int count = 0;

    rewind(fPtr);
    puts("\nExisting accounts:");
    printf("%-6s%-16s%-11s%10s%-30s%-15s\n", "Acct", "Last Name", "First Name", "Balance", "Address", "Phone");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6d%-16s%-11s%10.2f%-30s%-15s\n", client.acctNum, client.lastName, client.firstName,
                   client.balance, client.address, client.phone);
            count++;
        }
    }

    if (count == 0)
    {
        puts("No account records found.");
    }
} // end function listRecords

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    struct clientData client = {0, "", "", 0.0, "", ""};

    printf("%s", "Enter account to update ( 1 - 100 ): ");
    account = getAccountNumber();
    if (account == 0)
    {
        return;
    }

    if (fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET) != 0)
    {
        puts("Error seeking file.");
        return;
    }

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Unable to read account #%u.\n", account);
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printf("Account #%u - %s %s\n", client.acctNum, client.firstName, client.lastName);
    printf("Current balance: %.2f\n", client.balance);
    printf("Address: %s\n", client.address);
    printf("Phone: %s\n\n", client.phone);
    printf("%s", "Enter charge ( + ) or payment ( - ): ");

    if (scanf("%lf", &transaction) != 1)
    {
        puts("Invalid transaction amount.");
        while (getchar() != '\n' && !feof(stdin))
            ;
        return;
    }

    if (client.balance + transaction < 0)
    {
        puts("Insufficient balance.");
        return;
    }

    client.balance += transaction; // update record balance
    printf("\nUpdated Balance:\n");
    printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

    if (fseek(fPtr, -((long) sizeof(struct clientData)), SEEK_CUR) != 0)
    {
        puts("Error seeking file.");
        return;
    }

    fwrite(&client, sizeof(struct clientData), 1, fPtr);
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                        // stores record read from file
    struct clientData blankClient = {0, "", "", 0.0, "", ""}; // blank client
    unsigned int accountNum;                         // account number

    printf("%s", "Enter account number to delete ( 1 - 100 ): ");
    accountNum = getAccountNumber();
    if (accountNum == 0)
    {
        return;
    }

    if (fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET) != 0)
    {
        puts("Error seeking file.");
        return;
    }

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Unable to read account #%u.\n", accountNum);
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
        return;
    }

    if (fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET) != 0)
    {
        puts("Error seeking file.");
        return;
    }

    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    puts("Account deleted.");
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, "", ""};
    unsigned int accountNum; // account number

    printf("%s", "Enter new account number ( 1 - 100 ): ");
    accountNum = getAccountNumber();
    if (accountNum == 0)
    {
        return;
    }

    if (fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET) != 0)
    {
        puts("Error seeking file.");
        return;
    }

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        client.acctNum = 0;
    }

    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
        return;
    }

    printf("Enter lastname: ");
    if (scanf("%14s", client.lastName) != 1)
    {
        puts("Invalid last name.");
        return;
    }

    printf("Enter firstname: ");
    if (scanf("%9s", client.firstName) != 1)
    {
        puts("Invalid first name.");
        return;
    }

    printf("Enter balance: ");
    if (scanf("%lf", &client.balance) != 1)
    {
        puts("Invalid balance.");
        while (getchar() != '\n' && !feof(stdin))
            ;
        return;
    }

    while (getchar() != '\n' && !feof(stdin))
        ;

    printf("Enter address: ");
    if (fgets(client.address, sizeof(client.address), stdin) == NULL)
    {
        puts("Invalid address.");
        return;
    }
    client.address[strcspn(client.address, "\n")] = '\0';

    printf("Enter phone: ");
    if (scanf("%14s", client.phone) != 1)
    {
        puts("Invalid phone number.");
        return;
    }

    client.acctNum = accountNum;

    if (fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET) != 0)
    {
        puts("Error seeking file.");
        return;
    }

    fwrite(&client, sizeof(struct clientData), 1, fPtr);
} // end function newRecord

// search account
void searchRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, "", ""};
    unsigned int account;

    printf("Enter account number to search: ");
    account = getAccountNumber();
    if (account == 0)
    {
        return;
    }

    if (fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET) != 0)
    {
        puts("Error seeking file.");
        return;
    }

    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Account not found.\n");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\nAccount Details\n");
        printf("Account Number : %u\n", client.acctNum);
        printf("Name           : %s %s\n", client.firstName, client.lastName);
        printf("Balance        : %.2f\n", client.balance);
        printf("Address        : %s\n", client.address);
        printf("Phone          : %s\n", client.phone);
    }
}

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; // variable to store user's choice

    printf("%s", "\nEnter your choice\n"
                 "1 - store a formatted text file of accounts called\n"
                 "    \"accounts.txt\" for printing\n"
                 "2 - update an account\n"
                 "3 - add a new account\n"
                 "4 - delete an account\n"
                 "5 - search account\n"
                 "6 - list all accounts\n"
                 "7 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1)
    {
        while (getchar() != '\n' && !feof(stdin))
            ;
        return 0;
    }

    return menuChoice;
} // end function enterChoice