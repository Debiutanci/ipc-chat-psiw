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

int cq, client, static_size;
void unique_pause()
{
    int x;
    printf("\n\nWpisz dowolną liczbę aby kontynuować: ");
    scanf("%d", &x);
}

/*STURCTS*/
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
typedef struct
{
    int identifier;
    char name[50];
    int active;
} Group;
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
    int is_authenticated;
    char user_name[16];
    int user_identifier;
} CurrentUser;
/*///STURCTS*/

void user_view(char user[16])
{
    printf("Witaj %s!", user);
}

void politesniczat_view()
{
    system("clear");
    printf("===== Politeśniczat2000 =====\n");
}

void menu(CurrentUser cu)
{
    politesniczat_view();
    printf("______________________________");
    if (cu.is_authenticated == 0)
    {
        printf("Jesteś niezalogowany");
    }
    else
    {
        user_view(cu.user_name);
    }
}

int is_correct_request_type(int rt, int min, int max)
{
    if (rt < min || rt > max)
    {
        return 0;
    }
    return 1;
}
int get_request_type(CurrentUser cu)
{
    printf("\n");
    printf("Co chcesz zrobić? \n");
    if (cu.is_authenticated == 0)
    {
        printf("1. Zalogowanie\n");
        int request_type = -1;
        while (is_correct_request_type(request_type, 1, 1) == 0)
        {
            printf("\nPodaj poprawną akcję: ");
            scanf("%d", &request_type);
        }
        return request_type;
    }
    printf("1. Zalogowanie\n2. Wylogowanie\n3. Pokaż aktywnych użytkowników\n4. Napisz wiadomość\n5. Odbierz wiadomość\n6. Wyświetl wszystkie grupy\n7. Wyświetl moje grupy\n8. Zapisz się do grupy\n9. Usuń mnie z grupy\n10. Napisz wiadomość do grupy\n11. Odbierz wiadomości z grupy\n12. Podejrzyj kto jest zapisany do grupy\n");
    int request_type = -1;
    while (is_correct_request_type(request_type, 2, 12) == 0)
    {
        printf("Podaj poprawną akcję: ");
        scanf("%d", &request_type);
    }
    return request_type;
}

void login_request()
{
    politesniczat_view();
    printf("LOGOWANIE:\n");
    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 101;
    request.header = 123;
    request.status_code = -1;

    printf("Podaj email: ");
    scanf("%s", request.user_email);
    printf("Podaj haslo: ");
    scanf("%s", request.user_password);
    msgsnd(cq, &request, static_size, 0);
}

void logout_request(int identifier)
{
    politesniczat_view();

    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 102;
    request.header = 123;
    request.status_code = -1;
    request.user_identifier = identifier;
    msgsnd(cq, &request, static_size, 0);
}

void view_active_users_request()
{
    politesniczat_view();
    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 103;
    request.header = 123;
    request.status_code = -1;
    msgsnd(cq, &request, static_size, 0);
}
int get_target_user(Request response, char display[50])
{
    int tempid;
    for (int i = 0; i < response.active_users_dynamic_size; i++)
    {
        tempid = i + 1;
        printf(
            "%d:  %s %s\n",
            tempid,
            response.response_active_users[i].name,
            response.response_active_users[i].fullname);
    }

    int target = -1;
    while (is_correct_request_type(target, 1, response.active_users_dynamic_size) == 0)
    {
        printf("%s", display);
        scanf("%d", &target);
    }
    return target;
}
void write_message_request(int identifier)
{
    Request r;
    Request temp_response;
    view_active_users_request();
    int state = msgrcv(cq, &temp_response, sizeof(r) - sizeof(r.mtyp), 1, 0);

    politesniczat_view();
    char display[50];
    strcpy(display, "Do kogo chcesz wysłać wiadomość: ");
    int to = get_target_user(temp_response, display);

    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 104;
    request.header = 123;
    request.status_code = -1;
    request.user_identifier = identifier;
    request.to = to;
    printf("Podaj wiadomosc: ");
    scanf("%s", request.msg);
    msgsnd(cq, &request, static_size, 0);
}

void get_messages_request(int identifier)
{
    Request r;
    Request temp_response;
    view_active_users_request();
    int state = msgrcv(cq, &temp_response, sizeof(r) - sizeof(r.mtyp), 1, 0);

    politesniczat_view();
    char display[50];
    strcpy(display, "Od kogo chcesz odebrać wiadomości: ");
    int from_who = get_target_user(temp_response, display);

    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 105;
    request.header = 123;
    request.status_code = -1;
    request.user_identifier = identifier;
    request.to = from_who;
    msgsnd(cq, &request, static_size, 0);
    sleep(2);
    Request response;
    state = msgrcv(cq, &response, sizeof(response) - sizeof(response.mtyp), 1, 0);
    printf("Liczba nowych wiadomości: %d\n", response.messages_count);
    for (int i = 0; i < response.messages_count; i++)
    {
        printf("Wiadomosc %d: %s\n", i + 1, response.messages[i].msg);
    }
    unique_pause();
    Request fake_request;
    fake_request.mtyp = 1;
    strcpy(fake_request.mtext, "REQUEST");
    fake_request.from = client;
    fake_request.rt = 22; // init communication request type
    msgsnd(cq, &fake_request, sizeof(fake_request) - sizeof(fake_request.mtyp), 0);
}

void get_all_groups_request()
{
    politesniczat_view();
    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 106;
    request.header = 123;
    request.status_code = -1;
    msgsnd(cq, &request, static_size, 0);
}

void get_my_groups_request(int identifier)
{
    politesniczat_view();
    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 107;
    request.header = 123;
    request.status_code = -1;
    request.user_identifier = identifier;
    msgsnd(cq, &request, static_size, 0);
}
int get_target_group()
{
    int g = -1;
    while (is_correct_request_type(g, 1, 3) == 0)
    {
        printf("\nPodaj poprawną groupę: ");
        scanf("%d", &g);
    }
    return g;
}
void assign_me_request(int identifier)
{
    Request r;
    Request temp_response;
    get_all_groups_request();
    int state = msgrcv(cq, &temp_response, sizeof(r) - sizeof(r.mtyp), 1, 0);
    int tempid;
    printf("Grupy: \n");
    for (int i = 0; i < temp_response.active_groups_dynamic_size; i++)
    {
        tempid = i + 1;
        printf(
            "%d:  %s\n",
            tempid,
            temp_response.response_active_groups[i].name);
    }
    printf("Do jakiej grupy chcesz się zpaisać? \n");
    int target_group = get_target_group();

    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 108;
    request.header = 123;
    request.status_code = -1;
    request.user_identifier = identifier;
    request.target_group = target_group;
    msgsnd(cq, &request, static_size, 0);
}
int is_correct_current_group_type(int temp_target, int current[3])
{
    if (temp_target < 1 || temp_target > 3)
    {
        return 0;
    }
    if (current[temp_target - 1] == 1)
    {
        return 1;
    }
    return 0;
}
int get_target_del_group(int current[3])
{
    int g = -1;
    while (is_correct_current_group_type(g, current) == 0)
    {
        printf("\nPodaj poprawną groupę: ");
        scanf("%d", &g);
    }
    return g;
}
void remove_me_request(int identifier)
{
    Request response;
    get_my_groups_request(identifier);
    int state = msgrcv(cq, &response, sizeof(response) - sizeof(response.mtyp), 1, 0);

    int tempid;
    printf("Moje grupy: \n");
    for (int i = 0; i < response.active_groups_dynamic_size; i++)
    {
        if (response.users_groups[i] == 1)
        {
            tempid = i + 1;
            printf(
                "%d:  %s\n",
                tempid,
                response.response_active_groups[i].name);
        }
    }
    printf("Z jakiej grupy chcesz się usunąć? \n");
    int target_group = get_target_del_group(response.users_groups);

    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 109;
    request.header = 123;
    request.status_code = -1;
    request.user_identifier = identifier;
    request.target_group = target_group;
    msgsnd(cq, &request, static_size, 0);
}
void write_to_group_request(int identifier)
{
    Request response;
    get_my_groups_request(identifier);
    int state = msgrcv(cq, &response, sizeof(response) - sizeof(response.mtyp), 1, 0);

    int tempid;
    printf("Moje grupy: \n");
    for (int i = 0; i < response.active_groups_dynamic_size; i++)
    {
        if (response.users_groups[i] == 1)
        {
            tempid = i + 1;
            printf(
                "%d:  %s\n",
                tempid,
                response.response_active_groups[i].name);
        }
    }
    printf("Do jakiej grupy chcesz napisać? \n");
    int target_group = get_target_del_group(response.users_groups);

    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 110;
    request.header = 123;
    request.status_code = -1;
    request.user_identifier = identifier;

    printf("Podaj wiadomosc: ");
    scanf("%s", request.msg);

    request.target_group = target_group;
    msgsnd(cq, &request, static_size, 0);
}
void view_group_msgs_request(int identifier)
{
    Request xresponse;
    get_my_groups_request(identifier);
    int state = msgrcv(cq, &xresponse, sizeof(xresponse) - sizeof(xresponse.mtyp), 1, 0);

    int tempid;
    printf("Moje grupy: \n");
    for (int i = 0; i < xresponse.active_groups_dynamic_size; i++)
    {
        if (xresponse.users_groups[i] == 1)
        {
            tempid = i + 1;
            printf(
                "%d:  %s\n",
                tempid,
                xresponse.response_active_groups[i].name);
        }
    }
    printf("Od jakiej grupy chcesz odebrać wiadomości? \n");
    int target_group = get_target_del_group(xresponse.users_groups);

    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 111;
    request.header = 123;
    request.status_code = -1;
    request.user_identifier = target_group;
    request.to = target_group;
    msgsnd(cq, &request, static_size, 0);
    sleep(2);
    Request response;
    state = msgrcv(cq, &response, sizeof(response) - sizeof(response.mtyp), 1, 0);
    printf("Liczba nowych wiadomości: %d\n", response.messages_count);
    for (int i = 0; i < response.messages_count; i++)
    {
        printf("Wiadomosc %d: %s: %s\n", i + 1, response.authors[i].msg, response.messages[i].msg);
    }
    unique_pause();
    Request fake_request;
    fake_request.mtyp = 1;
    strcpy(fake_request.mtext, "REQUEST");
    fake_request.from = client;
    fake_request.rt = 22; // init communication request type
    msgsnd(cq, &fake_request, sizeof(fake_request) - sizeof(fake_request.mtyp), 0);
}

void view_members(int identifier)
{
    Request xresponse;
    get_my_groups_request(identifier);
    int state = msgrcv(cq, &xresponse, sizeof(xresponse) - sizeof(xresponse.mtyp), 1, 0);

    int tempid;
    printf("Moje grupy: \n");
    for (int i = 0; i < xresponse.active_groups_dynamic_size; i++)
    {
        if (xresponse.users_groups[i] == 1)
        {
            tempid = i + 1;
            printf(
                "%d:  %s\n",
                tempid,
                xresponse.response_active_groups[i].name);
        }
    }
    printf("Z jakiej grupy chcesz wyświetlić użytkowników? \n");
    int target_group = get_target_del_group(xresponse.users_groups);

    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 112;
    request.header = 123;
    request.status_code = -1;
    request.user_identifier = target_group;
    request.to = target_group;
    msgsnd(cq, &request, static_size, 0);
    sleep(2);
    Request response;
    state = msgrcv(cq, &response, sizeof(response) - sizeof(response.mtyp), 1, 0);
    printf("Liczba przypisanych użytkowników: %d\n", response.active_users_dynamic_size);
    for (int i = 0; i < response.active_users_dynamic_size; i++)
    {
        printf("User %d: %s: \n", i + 1, response.response_active_users[i].name);
    }
    unique_pause();
    Request fake_request;
    fake_request.mtyp = 1;
    strcpy(fake_request.mtext, "REQUEST");
    fake_request.from = client;
    fake_request.rt = 22; // init communication request type
    msgsnd(cq, &fake_request, sizeof(fake_request) - sizeof(fake_request.mtyp), 0);
}

void send_request(int request_type, CurrentUser cu)
{
    request_type += 100;
    Request request;
    request.rt = request_type;

    switch (request_type)
    {
    case 101:
        login_request();
        break;
    case 102:
        logout_request(cu.user_identifier);
        break;
    case 103:
        view_active_users_request();
        break;
    case 104:
        write_message_request(cu.user_identifier);
        break;
    case 105:
        get_messages_request(cu.user_identifier);
        break;
    case 106:
        get_all_groups_request();
        break;
    case 107:
        get_my_groups_request(cu.user_identifier);
        break;
    case 108:
        assign_me_request(cu.user_identifier);
        break;
    case 109:
        remove_me_request(cu.user_identifier);
        break;
    case 110:
        write_to_group_request(cu.user_identifier);
        break;
    case 111:
        view_group_msgs_request(cu.user_identifier);
        break;
    case 112:
        view_members(cu.user_identifier);
        break;
    default:
        break;
    }
}

void cover_users_listing_603(Request response)
{
    int tempid;
    for (int i = 0; i < response.active_users_dynamic_size; i++)
    {
        tempid = i + 1;
        printf(
            "%d:  %s %s\n",
            tempid,
            response.response_active_users[i].name,
            response.response_active_users[i].fullname);
    }
    unique_pause();
}
void cover_606(Request response)
{
    int tempid;
    for (int i = 0; i < response.active_groups_dynamic_size; i++)
    {
        tempid = i + 1;
        printf(
            "%d:  %s\n",
            tempid,
            response.response_active_groups[i].name);
    }
    unique_pause();
}
void cover_607(Request response)
{
    int tempid;
    printf("Moje grupy: \n");
    for (int i = 0; i < response.active_groups_dynamic_size; i++)
    {
        if (response.users_groups[i] == 1)
        {
            tempid = i + 1;
            printf(
                "%d:  %s\n",
                tempid,
                response.response_active_groups[i].name);
        }
    }
    unique_pause();
}

void safe_check()
{
    Request request;
    request.mtyp = 1;
    strcpy(request.mtext, "REQUEST");
    request.from = client;
    request.rt = 22; // init communication request type
    request.header = 123;
    request.status_code = -1;
    strcpy(request.user_email, "-1");
    strcpy(request.user_password, "-1");
    msgsnd(cq, &request, static_size, 0);
    Request response;
    int state = msgrcv(cq, &response, static_size, 1, 0);
    log_r(response);
    if ((int)response.from == client || (int)response.status_code != 204)
    {
        printf("ERROR\n");
        exit(0);
    }
    printf("OK\n");
}

void log_r(Request request)
{
    // printf("MTyp %d\n", request.mtyp);
    printf("Recieving request ...\n");
    printf("M-text %s\n", request.mtext);
    printf("FROM %d\n", request.from);
    printf("RT %d\n", request.rt);
    printf("HEADER %d\n", request.header);
    printf("status_code %d\n", request.status_code);
}

int main(int argc, char *argv[])
{
    int debug = 0;
    client = 2137;
    cq = msgget(0x123456, 0644 | IPC_CREAT);
    Request request;
    static_size = sizeof(request) - sizeof(request.mtyp);

    // init communication with server
    safe_check(static_size);

    // init current user
    CurrentUser cu;
    cu.is_authenticated = 0;

    int ttt;

    // client proccess
    int current_request_type;
    while (1)
    {
        menu(cu);

        // creating request
        current_request_type = get_request_type(cu);
        if (current_request_type == 2)
        {
            cu.is_authenticated = 0;
        }
        send_request(current_request_type, cu);

        // listening
        Request response;
        int state = msgrcv(cq, &response, sizeof(request) - sizeof(request.mtyp), 1, 0);
        if (debug == 1)
        {
            log_r(response);
            scanf("%d", &ttt);
        }
        switch (response.rt)
        {
        case 601:
            if (response.status_code == 200)
            {
                printf("Zalogowano :) \n");
                cu.is_authenticated = 1;
                strcpy(cu.user_name, response.response_user_name);
                cu.user_identifier = response.user_identifier;
            }
            else
            {
                printf("Niepoprawne dane logowania :/ \n");
            }
            break;
        case 603:
            cover_users_listing_603(response);
            break;
        case 604:
            break;
        case 605:
            break;
        case 606:
            cover_606(response);
            break;
        case 607:
            cover_607(response);
            break;
        case 608:
            break;
        case 609:
            break;
        case 610:
            break;
        case 611:
            break;
        case 612:
            break;
        default:
            break;
        }
    }
    return 0;
}

/*
RT:
22 - init
101 - login
102 - logout

*/