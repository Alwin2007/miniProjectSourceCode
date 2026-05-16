// Bank-account program - Version 2.0
// Enhanced with search, view all, deposit/withdraw, and balance check
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clientData structure definition
struct clientData {
    unsigned int acctNum; // account number
    char lastName[15]; // account last name
    char firstName[10]; // account first name
    double balance; // account balance
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void viewAllRecords(FILE *fPtr); // NEW
void searchRecord(FILE *fPtr); // NEW
void checkBalance(FILE *fPtr); // NEW
void depositWithdraw(FILE *fPtr); // NEW

int main(int argc, char *argv[]) {
    FILE *cfPtr;
    unsigned int choice;
    
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(-1);
    }
    
    printf("\n====================================\n");
    printf(" BANK ACCOUNT SYSTEM v2.0\n");
    printf("====================================\n");
    
    while ((choice = enterChoice()) != 9) {
        switch (choice) {
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
                viewAllRecords(cfPtr);
                break;
            case 6:
                searchRecord(cfPtr);
                break;
            case 7:
                checkBalance(cfPtr);
                break;
            case 8:
                depositWithdraw(cfPtr);
                break;
            default:
                puts("Incorrect choice");
                break;
        }
    }
    
    printf("\nThank you for using Bank Account\n");
    printf("System v2.0!\n");
    fclose(cfPtr);
    return 0;
}

// create formatted text file for printing
void textFile(FILE *readPtr) {
    FILE *writePtr;
    int result;
    struct clientData client = {0, "", "", 0.0};
    
    if ((writePtr = fopen("accounts.txt", "w")) == NULL) {
        puts("File could not be opened.");
    } else {
        rewind(readPtr);
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
            "Acct", "Last Name", "First Name", "Balance");
        fprintf(writePtr, "-------------------------\n");
        
        while (!feof(readPtr)) {
            result = fread(&client, sizeof(struct clientData), 
                1, readPtr);
            if (result != 0 && client.acctNum != 0) {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n",
                    client.acctNum, client.lastName,
                    client.firstName, client.balance);
            }
        }
        fclose(writePtr);
        puts("accounts.txt has been created!");
    }
}

// update balance in record
void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};
    
    printf("%s", "Enter account to update (1-100): ");
    scanf("%d", &account);
    
    fseek(fPtr, (account-1)*sizeof(struct clientData),
        SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) {
        printf("Account #%d has no information.\n",
            account);
    } else {
        printf("\n%-6s%-16s%-11s%10s\n", "Acct",
            "Last Name", "First Name", "Balance");
        printf("%-6d%-16s%-11s%10.2f\n\n",
            client.acctNum, client.lastName,
            client.firstName, client.balance);
        printf("%s", "Enter charge(+) or payment(-): ");
        scanf("%lf", &transaction);
        
        client.balance += transaction;
        printf("\nUpdated Record:\n");
        printf("%-6d%-16s%-11s%10.2f\n",
            client.acctNum, client.lastName,
            client.firstName, client.balance);
        
        fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        puts("Record updated successfully!");
    }
}

// delete an existing record
void deleteRecord(FILE *fPtr) {
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0};
    unsigned int accountNum;
    char confirm;
    
    printf("%s", "Enter account to delete (1-100): ");
    scanf("%d", &accountNum);
    
    fseek(fPtr, (accountNum-1)*sizeof(struct clientData),
        SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) {
        printf("Account %d does not exist.\n", accountNum);
    } else {
        printf("\nAccount to delete:\n");
        printf("%-6d%-16s%-11s%10.2f\n",
            client.acctNum, client.lastName,
            client.firstName, client.balance);
        
        printf("\nAre you sure? (y/n): ");
        scanf(" %c", &confirm);
        
        if (confirm == 'y' || confirm == 'Y') {
            fseek(fPtr,
                (accountNum-1)*sizeof(struct clientData),
                SEEK_SET);
            fwrite(&blankClient,
                sizeof(struct clientData), 1, fPtr);
            puts("Account deleted successfully!");
        } else {
            puts("Delete cancelled.");
        }
    }
}

// create and insert record
void newRecord(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;
    
    printf("%s", "Enter new account (1-100): ");
    scanf("%d", &accountNum);
    
    fseek(fPtr, (accountNum-1)*sizeof(struct clientData),
        SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum != 0) {
        printf("Account #%d already exists.\n",
            client.acctNum);
    } else {
        printf("%s", "Enter lastname, firstname, balance\n? ");
        scanf("%14s%9s%lf", client.lastName,
            client.firstName, &client.balance);
        
        client.acctNum = accountNum;
        fseek(fPtr,
            (client.acctNum-1)*sizeof(struct clientData),
            SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Account #%d created!\n", accountNum);
    }
}

// NEW FUNCTION: view all records with summary
void viewAllRecords(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0};
    int result;
    int count = 0;
    double total = 0.0;
    
    rewind(fPtr);
    printf("\n%-6s%-16s%-11s%10s\n", "Acct",
        "Last Name", "First Name", "Balance");
    printf("---------------------------\n");
    
    while (!feof(fPtr)) {
        result = fread(&client,
            sizeof(struct clientData), 1, fPtr);
        if (result != 0 && client.acctNum != 0) {
            printf("%-6d%-16s%-11s%10.2f\n",
                client.acctNum, client.lastName,
                client.firstName, client.balance);
            count++;
            total += client.balance;
        }
    }
    
    printf("---------------------------\n");
    printf("Total: %d accounts, Balance: $%.2f\n",
        count, total);
}

// NEW FUNCTION: search by last name
void searchRecord(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0};
    char searchName[15];
    int result;
    int found = 0;
    
    printf("Enter last name to search: ");
    scanf("%14s", searchName);
    
    rewind(fPtr);
    printf("\n%-6s%-16s%-11s%10s\n", "Acct",
        "Last Name", "First Name", "Balance");
    printf("---------------------------\n");
    
    while (!feof(fPtr)) {
        result = fread(&client,
            sizeof(struct clientData), 1, fPtr);
        if (result != 0 && client.acctNum != 0) {
            if (strcmp(client.lastName, searchName) == 0) {
                printf("%-6d%-16s%-11s%10.2f\n",
                    client.acctNum, client.lastName,
                    client.firstName, client.balance);
                found++;
            }
        }
    }
    
    if (found == 0)
        printf("No accounts found for '%s'.\n",
            searchName);
    else
        printf("---------------------------\n");
    printf("%d account(s) found.\n", found);
}

// NEW FUNCTION: check account balance and details
void checkBalance(FILE *fPtr) {
    unsigned int account;
    struct clientData client = {0, "", "", 0.0};
    
    printf("Enter account number (1-100): ");
    scanf("%d", &account);
    
    fseek(fPtr, (account-1)*sizeof(struct clientData),
        SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) {
        printf("Account #%d does not exist.\n", account);
    } else {
        printf("\n--- Account Details ---\n");
        printf("Account # : %d\n", client.acctNum);
        printf("Name : %s %s\n", client.firstName,
            client.lastName);
        printf("Balance : $%.2f\n", client.balance);
        
        if (client.balance < 0)
            printf("Status: OVERDRAWN!\n");
        else if (client.balance == 0)
            printf("Status: Zero balance\n");
        else
            printf("Status: Active\n");
    }
}

// NEW FUNCTION: deposit or withdraw money
void depositWithdraw(FILE *fPtr) {
    unsigned int account;
    double amount;
    char type;
    struct clientData client = {0, "", "", 0.0};
    
    printf("Enter account (1-100): ");
    scanf("%d", &account);
    
    fseek(fPtr, (account-1)*sizeof(struct clientData),
        SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    
    if (client.acctNum == 0) {
        printf("Account #%d does not exist.\n", account);
        return;
    }
    
    printf("Current Balance: $%.2f\n", client.balance);
    printf("(D)eposit or (W)ithdraw? : ");
    scanf(" %c", &type);
    printf("Enter amount: $");
    scanf("%lf", &amount);
    
    if (amount <= 0) {
        puts("Invalid amount!");
        return;
    }
    
    if (type == 'D' || type == 'd') {
        client.balance += amount;
        printf("Deposited $%.2f | New: $%.2f\n",
            amount, client.balance);
    } else if (type == 'W' || type == 'w') {
        if (amount > client.balance)
            printf("Warning: Low funds!\n");
        client.balance -= amount;
        printf("Withdrawn $%.2f | New: $%.2f\n",
            amount, client.balance);
    } else {
        puts("Invalid option!");
        return;
    }
    
    fseek(fPtr, (account-1)*sizeof(struct clientData),
        SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    puts("Transaction successful!");
}

// enable user to input menu choice
unsigned int enterChoice(void) {
    unsigned int menuChoice;
    
    printf("\n====================================\n");
    printf(" MAIN MENU\n");
    printf("====================================\n");
    printf("1 - Save accounts to accounts.txt\n");
    printf("2 - Update an account\n");
    printf("3 - Add a new account\n");
    printf("4 - Delete an account\n");
    printf("5 - View ALL accounts\n");
    printf("6 - Search by last name\n");
    printf("7 - Check account balance\n");
    printf("8 - Deposit / Withdraw\n");
    printf("9 - Exit\n");
    printf("====================================\n");
    printf("? ");
    scanf("%u", &menuChoice);
    
    return menuChoice;
}