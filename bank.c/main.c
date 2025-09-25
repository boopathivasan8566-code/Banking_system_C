#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Account {
    int accNo;
    char name[50];
    float balance;
    int pin; /* 4-digit PIN */
};

/* Helper to check if account number already exists */
int accountExists(int accNo) {
    struct Account acc;
    FILE *fp = fopen("bank.dat", "rb");
    if (!fp) return 0; /* file doesn't exist yet => no duplicates */
    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accNo == accNo) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Create new account with duplicate-check and PIN set */
void createAccount() {
    struct Account acc;
    FILE *fp = fopen("bank.dat", "ab");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("\nEnter Account Number (integer): ");
    scanf("%d", &acc.accNo);
    if (accountExists(acc.accNo)) {
        printf("An account with number %d already exists! Try another number.\n", acc.accNo);
        fclose(fp);
        return;
    }

    printf("Enter Name (no spaces): ");
    scanf("%49s", acc.name);

    printf("Enter Initial Balance: ");
    scanf("%f", &acc.balance);

    printf("Set a 4-digit PIN for this account: ");
    scanf("%d", &acc.pin);
    if (acc.pin < 0 || acc.pin > 9999) {
        printf("Invalid PIN. It must be a 4-digit number (0000 to 9999). Account not created.\n");
        fclose(fp);
        return;
    }

    fwrite(&acc, sizeof(acc), 1, fp);
    fclose(fp);
    printf("Account created successfully!\n");
}

/* Display all accounts (PINs are not shown) */
void displayAccounts() {
    struct Account acc;
    FILE *fp = fopen("bank.dat", "rb");
    if (fp == NULL) {
        printf("No records found!\n");
        return;
    }
    printf("\n--- Account List (PINs hidden) ---\n");
    while (fread(&acc, sizeof(acc), 1, fp)) {
        printf("Acc No: %d | Name: %s | Balance: %.2f\n", acc.accNo, acc.name, acc.balance);
    }
    fclose(fp);
}

/* Search account by number and show details without PIN */
void searchAccount() {
    struct Account acc;
    int accNo, found = 0;
    FILE *fp = fopen("bank.dat", "rb");
    if (fp == NULL) {
        printf("No records found!\n");
        return;
    }
    printf("\nEnter Account Number to Search: ");
    scanf("%d", &accNo);
    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accNo == accNo) {
            printf("\nAccount Found:\nAcc No: %d\nName  : %s\nBalance: %.2f\n", acc.accNo, acc.name, acc.balance);
            found = 1;
            break;
        }
    }
    if (!found) printf("Account not found!\n");
    fclose(fp);
}

/* Verify PIN for an account; returns 1 if correct, else 0 */
int verifyPin(int accNo, int inputPin) {
    struct Account acc;
    FILE *fp = fopen("bank.dat", "rb");
    if (!fp) return 0;
    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accNo == accNo) {
            fclose(fp);
            return (acc.pin == inputPin);
        }
    }
    fclose(fp);
    return 0;
}

/* Deposit after PIN verification */
void depositMoney() {
    struct Account acc;
    FILE *fp = fopen("bank.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    int accNo, pin, found = 0;
    float amt;

    if (fp == NULL || temp == NULL) {
        printf("Error opening file!\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    printf("\nEnter Account Number to Deposit: ");
    scanf("%d", &accNo);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accNo == accNo) {
            if (acc.pin == pin) {
                printf("Enter Amount to Deposit: ");
                scanf("%f", &amt);
                if (amt <= 0) {
                    printf("Enter a positive amount. Transaction cancelled.\n");
                } else {
                    acc.balance += amt;
                    printf("Deposit Successful! New Balance: %.2f\n", acc.balance);
                }
                found = 1;
            } else {
                printf("Incorrect PIN. Transaction cancelled.\n");
            }
        }
        fwrite(&acc, sizeof(acc), 1, temp);
    }

    fclose(fp);
    fclose(temp);
    remove("bank.dat");
    rename("temp.dat", "bank.dat");

    if (!found) printf("Account not found!\n");
}

/* Withdraw after PIN verification */
void withdrawMoney() {
    struct Account acc;
    FILE *fp = fopen("bank.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    int accNo, pin, found = 0;
    float amt;

    if (fp == NULL || temp == NULL) {
        printf("Error opening file!\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    printf("\nEnter Account Number to Withdraw: ");
    scanf("%d", &accNo);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accNo == accNo) {
            if (acc.pin == pin) {
                printf("Enter Amount to Withdraw: ");
                scanf("%f", &amt);
                if (amt <= 0) {
                    printf("Enter a positive amount. Transaction cancelled.\n");
                } else if (amt > acc.balance) {
                    printf("Insufficient Balance! Current Balance: %.2f\n", acc.balance);
                } else {
                    acc.balance -= amt;
                    printf("Withdraw Successful! New Balance: %.2f\n", acc.balance);
                }
                found = 1;
            } else {
                printf("Incorrect PIN. Transaction cancelled.\n");
            }
        }
        fwrite(&acc, sizeof(acc), 1, temp);
    }

    fclose(fp);
    fclose(temp);
    remove("bank.dat");
    rename("temp.dat", "bank.dat");

    if (!found) printf("Account not found!\n");
}

/* Update account holder name (requires PIN) */
void updateAccountName() {
    struct Account acc;
    FILE *fp = fopen("bank.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    int accNo, pin, found = 0;

    if (fp == NULL || temp == NULL) {
        printf("Error opening file!\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    printf("\nEnter Account Number to Update Name: ");
    scanf("%d", &accNo);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accNo == accNo) {
            if (acc.pin == pin) {
                printf("Enter New Name (no spaces): ");
                scanf("%49s", acc.name);
                printf("Name updated successfully!\n");
                found = 1;
            } else {
                printf("Incorrect PIN. Update cancelled.\n");
            }
        }
        fwrite(&acc, sizeof(acc), 1, temp);
    }

    fclose(fp);
    fclose(temp);
    remove("bank.dat");
    rename("temp.dat", "bank.dat");

    if (!found) printf("Account not found or PIN incorrect!\n");
}

/* Change PIN (requires old PIN) */
void changePin() {
    struct Account acc;
    FILE *fp = fopen("bank.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    int accNo, oldPin, newPin, found = 0;

    if (fp == NULL || temp == NULL) {
        printf("Error opening file!\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    printf("\nEnter Account Number to Change PIN: ");
    scanf("%d", &accNo);
    printf("Enter Old PIN: ");
    scanf("%d", &oldPin);

    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accNo == accNo) {
            if (acc.pin == oldPin) {
                printf("Enter New 4-digit PIN: ");
                scanf("%d", &newPin);
                if (newPin < 0 || newPin > 9999) {
                    printf("Invalid PIN. PIN must be 0000 to 9999. Change cancelled.\n");
                } else {
                    acc.pin = newPin;
                    printf("PIN changed successfully!\n");
                    found = 1;
                }
            } else {
                printf("Incorrect old PIN. Change cancelled.\n");
            }
        }
        fwrite(&acc, sizeof(acc), 1, temp);
    }

    fclose(fp);
    fclose(temp);
    remove("bank.dat");
    rename("temp.dat", "bank.dat");

    if (!found) printf("Account not found or old PIN incorrect!\n");
}

/* Delete account (requires PIN) */
void deleteAccount() {
    struct Account acc;
    FILE *fp = fopen("bank.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    int accNo, pin, found = 0;

    if (fp == NULL || temp == NULL) {
        printf("Error opening file!\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    printf("\nEnter Account Number to Delete: ");
    scanf("%d", &accNo);
    printf("Enter PIN: ");
    scanf("%d", &pin);

    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accNo == accNo) {
            if (acc.pin == pin) {
                printf("Account %d deleted successfully!\n", accNo);
                found = 1;
                /* skip writing this record to temp -> deletes it */
                continue;
            } else {
                printf("Incorrect PIN. Deletion cancelled for this account.\n");
            }
        }
        fwrite(&acc, sizeof(acc), 1, temp);
    }

    fclose(fp);
    fclose(temp);
    remove("bank.dat");
    rename("temp.dat", "bank.dat");

    if (!found) printf("Account not found or PIN incorrect!\n");
}

/* Sample run demonstration printed to console */
//void sampleRun() {
//    printf("\n--- Sample Run ---\n");
//    printf("1) Create account: AccNo=123 Name=Bhoopathy Balance=0 PIN=4321\n");
//    printf("2) Deposit 500 to AccNo=123 (enter PIN 4321) -> Balance becomes 500.00\n");
//    printf("3) Exit and restart program. Display Accounts -> shows AccNo=123 with Balance=500.00\n");
//    printf("4) Change name: Update AccNo=123 (enter PIN 4321) -> New Name set\n");
//    printf("5) Change PIN: Change PIN for AccNo=123 (enter old PIN 4321) -> New PIN set\n");
//    printf("6) Delete account: Delete AccNo=123 (enter PIN) -> account removed\n");
//    printf("--- End of Sample ---\n\n");
//}

int main() {
    int choice;
    while (1) {
        printf("\n\t\t====== BANKING SYSTEM ======\n");
        printf("\n1. Create Account\n");
        printf("\n2. Display All Accounts\n");
        printf("\n3. Search Account by Number\n");
        printf("\n4. Deposit Money\n");
        printf("\n5. Withdraw Money\n");
        printf("\n6. Update Account Name\n");
        printf("\n7. Change PIN\n");
        printf("\n8. Delete Account\n");
        printf("\n9. Exit\n");
        printf("\n\t Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: createAccount(); break;
            case 2: displayAccounts(); break;
            case 3: searchAccount(); break;
            case 4: depositMoney(); break;
            case 5: withdrawMoney(); break;
            case 6: updateAccountName(); break;
            case 7: changePin(); break;
            case 8: deleteAccount(); break;
            case 9: printf("Exiting...\n"); exit(0);
            default: printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}

