#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERNAME 100 
#define MAX_PASSWORD 50 
#define PARKING_FEE 10 
#define MAX_FILELINE 1024    
#define USER_FILENAME "User_records.csv"


void StoreRecords(char username[MAX_USERNAME], char password[MAX_PASSWORD], int balance, int hour){ // store the user information into the csv file
    FILE* file = fopen(USER_FILENAME, "a");
    if(file == NULL){ // if there is no such a file the error will show up
        printf("The file doesn't exist. \n");
        exit(1);
    }  
    fprintf(file, "%s, %s, %d, %d \n", username, password, balance, hour); // here puts usrname, password, balance, and parking hour into the file
    fclose(file); 
}

int searchUsers(char username[MAX_USERNAME], char password[MAX_PASSWORD]){ // here is the function that can find user
    int column = 0;
    int sucess_username = 0; // operator that can return a value if the user input was in the file
    int sucess_password = 0;

    FILE* file = fopen(USER_FILENAME, "r"); // file open by using read mode function
        if(file == NULL){
        printf("The file doesn't exist. \n"); 
        exit(1);
    }
    char line[MAX_FILELINE]; // lines of file

    while(fgets(line, sizeof(line), file)){ // read the file line by line
        char* value = strtok(line, ", "); // separate column by ", " using strtok() value return if there is "," in a certain line 
        column = 0;

        while(value){
            if(column == 0){ // column == 0 represents the first column of the line which is username
                if(strcmp(username, value) == 0){ //strcmp() can check to see if these strings match returns value 0 when they both are same
                    sucess_username = 1;
                }  
            }
            if(column == 1){ // column == 1 represents second column which is password
                if(strcmp(password, value) == 0){
                    sucess_password = 1;
                }
            }
            value = strtok(NULL, ", "); // reset the value each end of the line
            column ++; // increment column
        }
    }
    fclose(file); // close the file
    if(sucess_username == 1 && sucess_password == 1){ // if the username and password were corrct
        return 1;
    }
    else{
        return 0;
    }
}
 
 
void update(char current_username[MAX_USERNAME], char current_password[MAX_PASSWORD], int new_balance, int new_hour, int *fail_payment){ // here this function is for user to update their balance and parking hour
    FILE* file = fopen(USER_FILENAME, "r");
    FILE* tempFile = tmpfile();
    if(file == NULL){ // if there is no such a file the error will show up
        printf("The file doesn't exist. \n");
        exit(1);
    }
    char line[MAX_FILELINE]; // lines of file
    while(fgets(line, sizeof(line), file)){ // loops by each line read the file line by line
        char* value = strtok(line, ", "); // separate column by ", " using strtok() value return if there is "," in a certain line 
        char username[MAX_USERNAME];
        char password[MAX_PASSWORD];
        int column = 0;
        int user_found = 0;
        int balance = 0;
        int hour = 0;
        while(value){ // loops by each column
            if(column == 0){ // column == 0 represents the first column of the line which is username
                if(strcmp(current_username, value) == 0){ //strcmp() can check to see if these strings match returns value 0 when they both are same
                    user_found = 1; // if there is a user in the data set the user_found = 1
                }
                else{
                    strcpy(username, value); // if not just set the username as it is, this part is important to store all data without missing any
                }
            }
            if(column == 1){ //password column
                if(!user_found){ //if it couldn't find user then save the data as it is 
                    strcpy(password, value); //copy the password
                }
            }
            if(column == 2){ //balance column
                if(user_found){ // a specific user that's currently active
                    if(new_hour){ // if the parking hour has been updated
                        int preBalance = atoi(value);
                        if(preBalance >= (new_hour*10)){ // if the preBalance is greater equal to the parking hour * 10 
                            new_balance = preBalance - (new_hour*PARKING_FEE); // $10 per hour substract from the balance
                            *fail_payment = 0;
                        }
                        if(preBalance < (new_hour*PARKING_FEE)){
                            new_balance += preBalance; // otherwise the balance will be saved as it is
                            *fail_payment = 1;
                        }
                    }
                    else{ // if the parking hour has not been updated, and balance has been updated, then just increment
                        int preBalance = atoi(value);
                        new_balance += preBalance;
                    }
                }
                else{
                    balance = atoi(value); // the other data would be saved as it is 
                }
            }
            if(column == 3){ // when parking hour has been bought, just increments
                if(user_found){
                    if(*fail_payment == 0){ // if payment didn't fail, then parking hour will be incremneted
                        int preParking = atoi(value);
                        new_hour += preParking; 
                    }
                    if(*fail_payment == 1){ // if it fails to pay
                        new_hour = atoi(value); // save the parking hour as it is
                    }
                }
                else{
                    hour = atoi(value);
                }
            }
            value = strtok(NULL, ", ");
            column++;
        }
        if(user_found){ // for the specific user that made a action for update, save new information here 
            fprintf(tempFile, "%s, %s, %d, %d \n", current_username, current_password, new_balance, new_hour); // save it all in the temporaly file
        }
        else{ // other data will be saved as it is
            fprintf(tempFile, "%s, %s, %d, %d \n", username, password, balance, hour); 
        }
    }
    fclose(file);
    fseek(tempFile, 0, SEEK_SET); //move the file position indicator to the beggining 
    
    file = fopen(USER_FILENAME, "w"); // open the file with w mode
    if (!file) {
        perror("Error opening file for writing");
        fclose(tempFile);
        return;
    }
    while (fgets(line, sizeof(line), tempFile)) { // put all information with new update data back to the original file
        fputs(line, file);
    }
    //close them
    fclose(file);
    fclose(tempFile);
}


// Login and Register program
int main(){
    int opt_1; // for user option
    int num = 1; // value for while
    char username[MAX_PASSWORD]; // username that will be registered 
    char password[MAX_PASSWORD]; // password that will be registered 
    int login_success = 0; // check by using searchUser() and the user input is correct
    int balance = 0;
    int hour = 0;
    
    while(num){ // this loops as long as the user succeeded to login or enter 3
        printf("--------------Parking Lot System-------------- \n"); 
        printf("1. Register \n");
        printf("2. Login \n");
        printf("3. Exit \n");
        printf("----------------------------------------------- \n");
        scanf(" %d", &opt_1); // get the user input for these options
        // it only takes integer 1, 2, 3
        switch (opt_1){
        case 1: // when users chose 1 to register 

            printf("Create your username: "); // ask users to input their user name 
            scanf("%s", username);
    
            printf("Create your password: "); // ask users to input their password
            scanf("%s", password);

            StoreRecords(username, password, balance, hour);
            printf("You successfully created an account! \n");

        break;

        case 2: // when users chose 2 to login

            printf("Enter your username: ");
            scanf("%s", username);
            printf("Enter your password: ");
            scanf("%s", password);

            login_success = searchUsers(username, password); // this function returns 1 when loging was sucessful

            if(login_success != 0){ // if the login_sucesss is not 0 then it's sucessful
                printf("You successfully login! \n");
                while(login_success){
                    int opt_2;
                    int fail_payment;
                    int *fail_payment_ptr = &fail_payment;
                    balance = 0;
                    hour = 0;
                    printf("--------------User options--------------\n");
                    printf("1. Deposite Money in Your Balance \n");
                    printf("2. Buy Parking Hours ($10/hour) \n");
                    printf("3. Logout \n");
                    printf("----------------------------------------------- \n");
                    scanf("%d", &opt_2);
                    switch (opt_2){
                        case 1:
                            printf("Enter the amount of money that you want to deposite: ");
                            scanf("%d", &balance);
                            update(username, password, balance, hour, fail_payment_ptr);
                            printf("Data has been updated \n");
                        break;

                        case 2:
                            printf("Enter the parking hours: ");
                            scanf("%d", &hour);
                            update(username, password, balance, hour, fail_payment_ptr);
                            if(fail_payment){
                                printf("You couldn't buy parking hours because there is not enought money in your account \n");
                                break;
                            }
                            printf("Data has been updated \n");
                        break;
                        case 3:
                            login_success = 0;
                        break;
                        default:
                        break;
                    }
                }

            }
            else{
                printf("You couldn't login! \n");
            }
        break;

        case 3: // if the user choose 3 to exit, loop stops
            num = 0;
        break;
        default:
        break;
        }
    }
    printf("Thank you for using our service. \n");
    return 0;
}