#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define maxname 20
#define maxphone 11
#define maxemail 20
#define maxbirthday 10
#define maxage 3
#define maxjob 20
#define maxcity 15
#define maxpronouns 15
#define maxpreferred 15
#define maxavailability 15
#define MAX_DATA 100
#define MAX_USER_DATABASE 1000


void add();
void users();
void search (char* tokens[], int tokens_length);
char *trim(char *str);
void update(char* tokens[4]);
void types();
void delete(char* tokens[2]);
void help();
void addhelp();
char* field_to_string(int field);
int num_users = 0;

FILE *users_binary;

//user

typedef struct{
    char name[maxname];
    char phone[maxphone];
    char email[maxemail];
    char birthday[maxbirthday];
    char age[maxage];
    char job[maxjob];
    char city[maxcity];// city they're based on
    char pronouns[maxpronouns];
    char preferred[MAX_DATA]; // preferred communication method
    char availability[MAX_DATA]; //person's availability 
} User;

User userDatabase[MAX_USER_DATABASE];

//header structure
typedef struct {
    int num_records;
    int record_size;
    char record_type[50];
} Header;

void serialize_user( User user, FILE *file) {
    fwrite(&user, sizeof(User), 1, file);
}

User deserialize_user(FILE *file) {
    User user;
    fread(&user, sizeof(User), 1, file);
    return user;
}

void write_to_binary_file(User *userDatabase, int num_users) {
    FILE *file = fopen("users.bin", "wb");
    if (file == NULL) {
        printf("Could not open file for writing\n");
        return;
    }

    Header header = {num_users, sizeof(User), "User"};
    fwrite(&header, sizeof(Header), 1, file);

    for(int i = 0; i < num_users; i++) {
        serialize_user(userDatabase[i], file);
    }

    fclose(file);

  
}

void read_from_binary_file(User *userDatabase, int *num_users) {
    FILE *file = fopen("users.bin", "rb");
    if (file == NULL) {
        printf("Could not open file for reading\n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, file);
    *num_users = header.num_records;

    for(int i = 0; i < header.num_records; i++) {
        userDatabase[i] = deserialize_user(file);
    }

    fclose(file);
}



void add(char* user_info){
    if(num_users >= MAX_USER_DATABASE){
        printf("Database is full\n");
        return;
    }   

    User new_user={0}; //initialize all fields to 0
    memset(&new_user, 0, sizeof(User)); // clear the struct
    new_user.age[0] = '\0'; // set age to an empty string
    char* token;
    char* rest=user_info;

    int field = 0;
    for(field = 0; field < 10; field++) {
        char formatted[MAX_DATA];
        int max_size;
        token = strsep(&rest, ",");
        if (token == NULL) {
            token = "";
        }
        switch (field) {
            case 0: max_size = maxname; strncpy(new_user.name, token, maxname); break;
            case 1: max_size = maxphone; strncpy(new_user.phone, token, maxphone); break;
            case 2: max_size = maxbirthday; strncpy(new_user.birthday, token, maxbirthday); break;
            case 3: max_size = maxemail; strncpy(new_user.email, token, maxemail); break;
            case 4: 
                max_size = maxage; 
                int age = atoi(token);
                snprintf(new_user.age, sizeof(new_user.age), "%d", age);
                break;
            case 5: max_size = maxjob; strncpy(new_user.job, token, maxjob); break;
            case 6: max_size = maxcity; strncpy(new_user.city, token, maxcity); break;
            case 7: max_size = maxpronouns; strncpy(new_user.pronouns, token, maxpronouns); break;
            case 8: max_size = maxpreferred; strncpy(new_user.preferred, token, maxpreferred); break;
            case 9: max_size = maxavailability; strncpy(new_user.availability, token, maxavailability); break;
            default: max_size = MAX_DATA; break;
        }
    }
    userDatabase[num_users++] = new_user;
    write_to_binary_file(userDatabase, num_users);
}   
void update(char* tokens[]) {
    char* phone_numb = tokens[1];
    char* type = tokens[2];
    char* change = tokens[3];

    User userDatabase[MAX_USER_DATABASE];
    int num_users = 0;

    read_from_binary_file(userDatabase, &num_users);

    int found = 0;

    for(int i = 0; i < num_users; i++) {
        if(strcmp(userDatabase[i].phone, phone_numb) == 0) {
            found = 1;
            printf("Matched phone number\n");

            if(strcmp(type, "name") == 0) {
                strncpy(userDatabase[i].name, change, maxname);
            } else if(strcmp(type, "phone") == 0) {
                strncpy(userDatabase[i].phone, change, maxphone);
            } else if(strcmp(type, "birthday") == 0) {
                strncpy(userDatabase[i].birthday, change, maxbirthday);
            } else if(strcmp(type, "email") == 0) {
                strncpy(userDatabase[i].email, change, maxemail);
            } else if(strcmp(type, "age") == 0) {
                int age = atoi(change);
                snprintf(userDatabase[i].age, sizeof(userDatabase[i].age), "%d", age);
            } else if(strcmp(type, "job") == 0) {
                strncpy(userDatabase[i].job, change, maxjob);
            } else if(strcmp(type, "city") == 0) {
                strncpy(userDatabase[i].city, change, maxcity);
            } else if(strcmp(type, "pronouns") == 0) {
                strncpy(userDatabase[i].pronouns, change, maxpronouns);
            } else if(strcmp(type, "preferred") == 0) {
                strncpy(userDatabase[i].preferred, change, maxpreferred);
            } else if(strcmp(type, "availability") == 0) {
                strncpy(userDatabase[i].availability, change, maxavailability);
            }

            printf("Updated field %s\n", type);
            break;
        }
    }

    if (!found) {
        printf("Phone number not found\n");
        return;
    }

   
    write_to_binary_file(userDatabase, num_users);
}
void main() {
    printf("Welcome to the networking system!\n");

    read_from_binary_file(userDatabase, &num_users);

    char *cmds = 
    "\nUsage:\n"
    "  execute <command> [options]\n\n"
    "Commands:\n"
    "  add                                   Add a new user.\n"
    "  addhelp                               Show the format to add a new user.\n"
    "  search <phone number>                 Search their name by entering the phone number.\n"
    "  update <phone number> <type> <change> Update <type> by phone number. Enter type to see types you can change.\n"
    "  delete <phone number>                 Delete user info by phone number.\n"
    "  users                                 List all users.\n"
    "  types                                 List types used in update.\n"
    "  help                                  Show this list of commands.\n"
    "  exit                                  Close this program and save updates.\n\n";
    printf("%s", cmds);
    
    char command[100];
    char *tokens[100];
    int tokens_length;

    while (1) {
        printf("Enter a command: ");
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = 0; // remove newline character

        char *token = strtok(command, " ");
        tokens_length = 0;
        while (token != NULL) {
            tokens[tokens_length++] = token;
            token = strtok(NULL, " ");
        }
        if(tokens_length == 0){
            continue;
        }
        char *cmd = tokens[0];

        if (strcmp(cmd, "add") == 0) {
            if(tokens_length < 2){
                printf("Invalid number of arguments\n");
                printf("Usage: add <user info>\n");
                continue;
            }
            add(tokens[1]);
        } 
            else if(strcmp(cmd, "addhelp") == 0){
            addhelp();
        }
        else if(strcmp(cmd, "search")== 0){
            search(tokens, tokens_length);
        }
        else if (strcmp(cmd, "users")==0){
            users();
        }

        else if(strcmp(cmd,"types")==0){
            types();
        }

        else if(strcmp(cmd,"update")==0){
            update(tokens);
        }

        else if(strcmp(cmd,"delete")==0){
            delete(tokens);
        }

        else if(strcmp(cmd,"help")==0){
            printf("%s", cmds);
        }

        else if(strcmp(cmd,"exit")==0){
            break;
        }

        else{
            printf("Invalid command\n");
        }

        printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }

    printf("Changes saved, thank you for using the network system!\n");
}


void addhelp(){
    printf("Enter user information in the following format:\n\n");
    printf("name,phone,birthday,email,age,job,city,pronouns,preferred(communication method),availability\n\n");
    printf("To skip information, enter two consecutive commas\n");
    printf("Example: Alex,1234567890,01/01/2000,,20,Engineer,New_York,he/him,phone,weekdays\n\n");

    printf("If you want to enter lastname enter it in this format: Alex-Smith\n");
    printf("DO NOT enter spaces when entering data\n");
}

char *trim(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0) 
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';

    return str;
}


char* field_to_string(int field) {
    switch(field) {
        case 0: return "name";
        case 1: return "phone";
        case 2: return "birthday";
        case 3: return "email";
        case 4: return "age";
        case 5: return "job";
        case 6: return "city";
        case 7: return "pronouns";
        case 8: return "preferred";
        case 9: return "availability";
        default: return "";
    }
}

void search (char* tokens[], int tokens_length){
    char* phone_numb = tokens[1];

    FILE *file = fopen("users.bin", "rb");
    if(file == NULL){
        printf("Could not open file for reading-search\n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, file);

    User user;
    int match = 0;

    for(int i = 0; i < header.num_records; i++) {
        user = deserialize_user(file);
        char* trimmed_phone = trim(user.phone); // trim the phone number
        if(strcmp(trimmed_phone, phone_numb) == 0){
            match = 1;
            printf("Name: %s\n", user.name);
            printf("Phone: %s\n", user.phone);
            printf("Email: %s\n", user.email);
            printf("Birthday: %s\n", user.birthday);
            printf("Age: %s\n", user.age);
            printf("Job: %s\n", user.job);
            printf("City: %s\n", user.city);
            printf("Pronouns: %s\n", user.pronouns);
            printf("Preferred: %s\n", user.preferred);
            printf("Availability: %s\n", user.availability);
            break;
        }
    }

    if(!match){
        printf("No user found with phone number %s\n", phone_numb);
    }

    fclose(file);
}

void users() {
    User userDatabase[MAX_USER_DATABASE];
    int num_users = 0;
    read_from_binary_file(userDatabase, &num_users);
    for(int i = 0; i < num_users; i++) {
        printf("%s, %s \n", userDatabase[i].name, userDatabase[i].phone);
    }
}

void types(){
    
     printf("name\nemail\nbirthday\nage\njob\ncity\npronouns\npreferred\navailability\n");

}

void delete(char* tokens[2]) {
    char* phone_numb = tokens[1];
    User userDatabase[MAX_USER_DATABASE];
    int num_users = 0;

    read_from_binary_file(userDatabase, &num_users);

    int found = 0;

    for(int i = 0; i < num_users; i++) {
        if(strcmp(userDatabase[i].phone, phone_numb) == 0) {
            found = 1;
            printf("Matched phone number, deleting user\n");

          
            for(int j = i; j < num_users - 1; j++) {
                userDatabase[j] = userDatabase[j + 1];
            }

            num_users--;
            break;
        }
    }

    if (!found) {
        printf("Phone number not found\n");
        return;
    }

    write_to_binary_file(userDatabase, num_users);
}