/*====================IMPORTS=======================*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

int cq, server;
void specyfic_log(char *str)
{
    printf("%s ", str);
    // for(int i = 0 ; i < 3 ; i++) {
    //     printf(".");
    //     fflush(stdout);
    //     sleep(1);
    // }
    printf("\n");
}

/*STURCTS*/
typedef struct
{
    int identifier;
    char name[50];
    int active;
} Group;
typedef struct
{
    int identifier;
    char email[50];
    char name[50];
    char fullname[50];
    char password[50];
    int active;
} User;
typedef struct
{
    char msg[50];
} Message;
typedef struct Request
{
    long mtyp;
    char mtext[16];
    int rt;
    int from;
    int header;
    int status_code;
    char user_email[16];
    char user_password[16];

    int user_identifier;
    char response_user_name[16];
    User response_active_users[9];
    int active_users_dynamic_size;

    int to;
    char msg[50];

    Message messages[15];
    Message authors[15];
    int messages_count;
    Group response_active_groups[3];
    int active_groups_dynamic_size;

    int users_groups[3];
    int target_group;

} Request;
typedef struct
{
    int status;
    User user;
} UserGetter;
typedef struct
{
    int active;
    int identifier;
    int from;
    char msg[50];
} UsersMessage;
typedef struct
{
    int user;
    int groups[3];
} UsersToGroups;
/*///STURCTS*/

/*GROUPS*/
struct Group *read_all_groups()
{
    FILE *file;
    file = fopen("db/groups.txt", "r");
    if (file == NULL)
    {
        printf("FILE ERROR 1\n");
        exit(0);
    }
    Group *groups = malloc(sizeof(Group) * 3);
    int r = 0;
    int count = 0;
    do
    {
        r = fscanf(
            file,
            "%49[^,],%d\n",
            groups[count].name,
            &groups[count].identifier,
            1);
        if (r == 2)
            count++;
        if (r != 2 && !feof(file))
        {
            printf("DB ERROR 1\n");
            exit(0);
        }
        if (ferror(file))
        {
            printf("EB ERROR 2\n");
            exit(0);
        }
    } while (!feof(file));
    fclose(file);
    return groups;
}

/*READ USERS*/
struct User *read_all_users()
{
    FILE *file;
    file = fopen("db/users.txt", "r");
    if (file == NULL)
    {
        printf("FILE ERROR 1\n");
        exit(0);
    }
    User *users = malloc(sizeof(User) * 100);
    int r = 0;
    int count = 0;
    do
    {
        r = fscanf(
            file,
            "%49[^,],%49[^,],%49[^,],%49[^,],%d\n",
            users[count].email,
            users[count].name,
            users[count].fullname,
            users[count].password,
            &users[count].identifier,
            0);
        if (r == 5)
            count++;
        if (r != 5 && !feof(file))
        {
            printf("DB ERROR 1\n");
            exit(0);
        }
        if (ferror(file))
        {
            printf("EB ERROR 2\n");
            exit(0);
        }
    } while (!feof(file));
    fclose(file);
    return users;
}
UserGetter get_user_by_identifier(int identifier, User users[100])
{
    UserGetter ug;
    ug.status = 0;
    int i = 0;
    while (&users[i] != NULL)
    {
        if (i == 10)
        {
            return ug;
        }
        if (users[i].identifier == identifier)
        {
            ug.status = 1;
            ug.user = users[i];
            return ug;
        }
        i++;
    }
    return ug;
}
UserGetter get_user_by_email(char email[100], User users[100])
{
    UserGetter ug;
    ug.status = 0;
    int i = 0;
    while (&users[i] != NULL)
    {
        if (i == 10)
        {
            return ug;
        }
        if (strcmp(users[i].email, email) == 0)
        {
            ug.status = 1;
            ug.user = users[i];
            return ug;
        }
        i++;
    }
    return ug;
}
int validate_password(char password[100], User dbuser)
{
    if (strcmp(password, dbuser.password) == 0)
    {
        return 1;
    }
    return 0;
}
void activate_user(int identifier, User users[100])
{
    for (int i = 0; i < 100; i++)
    {
        if (users[i].identifier == identifier)
        {
            users[i].active = 1;
        }
    }
}
void deactivate_user(int identifier, User users[100])
{
    for (int i = 0; i < 100; i++)
    {
        if (users[i].identifier == identifier)
        {
            users[i].active = 0;
        }
    }
}
void log_users(User users[100])
{
    printf("Log users:");
    for (int i = 0; i < 2; i++)
    {
        printf(
            "%d %s %s %s %s\n",
            users[i].identifier,
            users[i].email,
            users[i].name,
            users[i].fullname,
            users[i].password);
    }
}
/*///READ USERS*/

void log_r(Request request)
{
    // printf("MTyp %d\n", request.mtyp);
    printf("Recieving request ...\n");
    printf("M-text %s\n", request.mtext);
    printf("FROM %d\n", request.from);
    printf("RT %d\n", request.rt);
    printf("HEADER %d\n", request.header);
    printf("status_code %d\n", request.status_code);
    printf("user - email %s\n", request.user_email);
    printf("user - password %s\n", request.user_password);
}

/*MESSAGES*/
void push_msg(UsersMessage um[150], int identifier, int from, char msg[50])
{
    int i = 0;
    while (um[i].active != 0)
    {
        i++;
    }
    um[i].active = 1;
    um[i].identifier = identifier;
    um[i].from = from;
    strcpy(um[i].msg, msg);
}

/*///MESSAGES*/

void non_content_response(Request request)
{
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = -1;
    response.header = request.from;
    response.status_code = 204;
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
}

/*COVERS*/
void cover_22(Request request)
{
    non_content_response(request);
}
int cover_101(Request request, User users[100])
{
    int identifier = -1;
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = 601;
    response.header = request.from;
    UserGetter ug = get_user_by_email(request.user_email, users);
    if (ug.status == 0)
    {
        printf("[log] request returns 404\n");
        response.status_code = 404;
    }
    else
    {
        if (validate_password(request.user_password, ug.user) == 0)
        {
            printf("[log] request returns 400\n");
            response.status_code = 400;
        }
        else
        {
            printf("[log] request returns 200\n");
            response.status_code = 200;
            response.user_identifier = ug.user.identifier;
            strcpy(response.response_user_name, ug.user.name);
            identifier = ug.user.identifier;
        }
    }
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
    return identifier;
}
void cover_103(Request request, User users[100])
{
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = 603;
    response.header = request.from;
    response.status_code = 200;

    int ci = 0;
    for (int i = 0; i < 100; i++)
    {
        if (users[i].active == 1)
        {
            response.response_active_users[ci] = users[i];
            ci++;
        }
    }
    response.active_users_dynamic_size = ci;
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
}

void cover_104(Request request, UsersMessage um[150])
{
    push_msg(um, request.to, request.user_identifier, request.msg);
}

void cover_105(Request request, UsersMessage um[150])
{
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = 605;
    response.header = request.from;
    response.status_code = 200;

    int count_of_new_messages = 0;
    for (int i = 0; i < 150; i++)
    {
        if (um[i].active == 1 && um[i].identifier == request.user_identifier && um[i].from == request.to)
        {
            strcpy(response.messages[count_of_new_messages].msg, um[i].msg);
            um[i].active = 0;
            count_of_new_messages++;
        }
    }
    response.messages_count = count_of_new_messages;
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
    sleep(1);
}
void cover_106(Request request, Group groups[3])
{
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = 606;
    response.header = request.from;
    response.status_code = 200;
    for (int i = 0; i < 3; i++)
    {
        response.response_active_groups[i] = groups[i];
    }
    response.active_groups_dynamic_size = 3;
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
}
void cover_107(Request request, UsersToGroups utg[10], Group groups[3])
{
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = 607;
    response.header = request.from;
    response.status_code = 200;
    for (int i = 0; i < 3; i++)
    {
        response.response_active_groups[i] = groups[i];
    }
    response.active_groups_dynamic_size = 3;
    for (int i = 0; i < 10; i++)
    {
        if (utg[i].user == request.user_identifier)
        {
            for (int j = 0; j < 3; j++)
            {
                response.users_groups[j] = utg[i].groups[j];
            }
        }
    }
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
    sleep(1);
}
void cover_108(Request request, UsersToGroups utg[10])
{
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = 608;
    response.header = request.from;
    response.status_code = 200;
    for (int i = 0; i < 10; i++)
    {
        if (utg[i].user == request.user_identifier)
        {
            utg[i].groups[request.target_group - 1] = 1;
        }
    }
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
    sleep(1);
}
void cover_109(Request request, UsersToGroups utg[10])
{
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = 609;
    response.header = request.from;
    response.status_code = 200;
    for (int i = 0; i < 10; i++)
    {
        if (utg[i].user == request.user_identifier)
        {
            utg[i].groups[request.target_group - 1] = 0;
        }
    }
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
    sleep(1);
}
void cover_110(Request request, UsersMessage ug[150])
{
    push_msg(ug, request.target_group, request.user_identifier, request.msg);
}
void cover_111(Request request, UsersMessage um[150], User users[100])
{
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = 611;
    response.header = request.from;
    response.status_code = 200;

    int count_of_new_messages = 0;
    for (int i = 0; i < 150; i++)
    {
        if (um[i].active == 1 && um[i].identifier == request.user_identifier)
        {
            strcpy(response.messages[count_of_new_messages].msg, um[i].msg);
            UserGetter ug = get_user_by_identifier(um[i].from, users);
            strcpy(response.authors[count_of_new_messages].msg, ug.user.name);
            count_of_new_messages++;
        }
    }
    response.messages_count = count_of_new_messages;
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
    sleep(1);
}
void cover_112(Request request, UsersToGroups utg[10], User users[100])
{
    Request response;
    response.mtyp = 1;
    strcpy(response.mtext, "RESPONSE");
    response.from = server;
    response.rt = 603;
    response.header = request.from;
    response.status_code = 200;

    int ci = 0;
    for (int i = 0; i < 100; i++)
    {
        if (utg[users[i].identifier - 1].groups[request.to - 1] == 1)
        {
            response.response_active_users[ci] = users[i];
            ci++;
        }
    }
    response.active_users_dynamic_size = ci;
    msgsnd(cq, &response, sizeof(response) - sizeof(response.mtyp), 0);
    sleep(1);
}

/*///COVERS*/

int main(int argc, char *argv[])
{
    // specyfic_log("Server process initializing");
    // SERVER SETTINGS =======================
    int debug = 0;

    // USERS =================================
    specyfic_log("Reading users");
    User *users = read_all_users();
    Group *groups = read_all_groups();
    UsersToGroups *utg = malloc(sizeof(UsersToGroups) * 10);
    for (int i = 0; i < 10; i++)
    {
        utg[i].user = i + 1;
        for (int j = 0; j < 3; j++)
        {
            utg[i].groups[j] = 0;
        }
    }

    UsersMessage *users_messages = malloc(sizeof(UsersMessage) * 150);
    UsersMessage *groups_messages = malloc(sizeof(UsersMessage) * 150);

    if (debug == 1)
    {
        log_users(users);
    }
    server = 420;
    cq = msgget(0x123456, 0644 | IPC_CREAT);
    Request request;
    while (1)
    {
        printf("listening ...\n");
        int state = msgrcv(cq, &request, sizeof(request) - sizeof(request.mtyp), 1, 0);
        if (debug == 1)
        {
            log_r(request);
        }
        if (request.from != server)
        {
            switch (request.rt)
            {

            // default init case
            case 22:
                cover_22(request);
                break;
            case 101:
                int identifier = cover_101(request, users);
                if (identifier != -1)
                {
                    activate_user(identifier, users);
                }
                break;
            case 102:
                deactivate_user(request.user_identifier, users);
                cover_22(request);
                break;
            case 103:
                cover_103(request, users);
                break;
            case 104:
                cover_104(request, users_messages);
                cover_22(request);
                break;
            case 105:
                cover_105(request, users_messages);
                break;
            case 106:
                cover_106(request, groups);
                break;
            case 107:
                cover_107(request, utg, groups);
                break;
            case 108:
                cover_108(request, utg);
                break;
            case 109:
                cover_109(request, utg);
                break;
            case 110:
                cover_110(request, groups_messages);
                cover_22(request);
                break;
            case 111:
                cover_111(request, groups_messages, users);
                break;
            case 112:
                cover_112(request, utg, users);
                break;
            default:
                break;
            }
        }
        printf("Sending response ...\n");
        sleep(1);
    }
    return 0;
}
