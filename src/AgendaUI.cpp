#include "AgendaUI.hpp"
#include "Exception.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <signal.h>
#include <stdexcept>
#include <vector>

using std::cin;
using std::cout;
using std::cerr;
using std::left;
using std::setw;
using std::list;
using std::move;
using std::vector;
using std::string;
using std::to_string;
using std::logic_error;
using std::domain_error;
using std::invalid_argument;

// aux functions

#define IS_LOG_IN (m_userName != "")
#define NOT_LOG_IN (m_userName == "")

// format print

std::ostream &custom_prompt(const string &name) {
    cout << '[' + name + "] ";
    return cout;
}

void print_format(std::initializer_list<string> format) {
    for (const auto &entry : format) {
        cout << '[' + entry + "] ";
    }
    cout << '\n';
}

void print_manual(bool is_logged_in) {
    if (is_logged_in) {
        cout << "\nAgenda User Manual(loggin version):\n"
             << "o   : log Out agenda\n"
             << "q   : Quit agenda\n"
             << "h   : get command Helper\n"
             << "dc  : Delete agenda Account\n"
             << "lu  : List all agenda User\n"
             << "cm  : Create Meeting\n"
             << "la  : List All meeting\n"
             << "las : List All Sponsored meeting\n"
             << "lap : List All Participated meeting\n"
             << "qm  : Query meeting (by title)\n"
             << "qt  : Query meeting (by Time interval)\n"
             << "dm  : Delete Meeting (by title)\n"
             << "da  : Delete All meeting\n\n";
    } else {
        cout << "\nAgenda User Manual(log out version):\n"
             << "l  : Log in\n"
             << "r  : Register\n"
             << "q  : Quit agenda\n"
             << "h  : get command Helper\n\n";
    }
}

// UI input

string strip(const string &s) {
    string::size_type head(0);
    for (; head < s.size() and isspace(s[head]); ++head)
        ;
    if (head == s.size())
        return "";
    string::size_type tail(s.size());
    for (; tail > 0 and isspace(s[tail - 1]); --tail)
        ;
    return s.substr(head, tail - head);
}

string get_stripped_line() {
    string result;
    getline(cin, result);
    return strip(result);
}

// tokenize a string
int tokenize(const string &line, vector<string> &dest) {
    vector<string> result;
    if (line == "")
        return 0;

    int origin_size(dest.size());

    for (string::size_type start(0);;) {
        string::size_type bound = line.find(' ', start);
        if (bound == string::npos) {
            dest.push_back(line.substr(start));
            break;
        }
        dest.push_back(line.substr(start, bound - start));
        for (start = bound; start < line.size() and line[start] == ' '; ++start)
            ;
        if (start == line.size())
            break;
    }

    return dest.size() - origin_size;
}

vector<string> get_arguments(const int p_arg_num, string prompt) {
    vector<string> result;
    int argLeft = p_arg_num;
    for (string line; argLeft;) {
        custom_prompt(prompt);
        line = get_stripped_line();
        int argRcv = tokenize(line, result);
        if (argRcv > argLeft) {
            throw wrong_arg_num(
                to_string(argLeft) + " args left, while receive " +
                to_string(argRcv));
        } else {
            argLeft -= argRcv;
        }
    }
    return result;
}

// some checker

void log_x_first(const string &command, const string in_or_out) {
    throw unknown_command(
        "Command (" + command + ") require a log " + in_or_out + " state.");
}

void check_login_state(
    bool required_login, bool actual_login, const string &command) {
    if (required_login != actual_login) {
        if (not required_login) {
            log_x_first(command, "out");
        } else {
            log_x_first(command, "in");
        }
    }
}

// check argument numbers, throw if fail
void check_input_number(int expect, int actual) {
    if (expect != actual) {
        throw wrong_arg_num(
            "Expect " + to_string(expect) + " argument(s)." +
            " While actually have " + to_string(actual));
    }
}

// format print

AgendaUI::AgendaUI() {
    startAgenda();
}

void AgendaUI::OperationLoop(void) {
    for (;;) {
        string operation = getOperation();
        try {
            executeOperation(operation);
        } catch (const unknown_command &uc) {
            cerr << '[' << uc.type() << "] " << uc.what()
                 << " (press h for command help)\n";
        } catch (const my_exception &e) {
            m_logger->log_error(e);
            cerr << "[Error] " << '[' << e.type() << "] " << e.what() << '\n';
        }
    }
}

void AgendaUI::startAgenda(void) {
    m_agendaService.startAgenda();
    m_logger = Logger::get_instance();
    m_logger->log_start();
}

string AgendaUI::getOperation() {
    // prompt
    cout << "Agenda";
    if (IS_LOG_IN) {
        cout << "@" << m_userName << ":# ";
    } else {
        cout << ":$ ";
    }

    return get_stripped_line();
}

bool AgendaUI::executeOperation(string t_operation) {
    if (t_operation == "" or t_operation.substr(0, 2) == "//") {
        return true;
    }
    if (t_operation == "l") {
        check_login_state(false, IS_LOG_IN, t_operation);
        userLogIn();
    } else if (t_operation == "r") {
        check_login_state(false, IS_LOG_IN, t_operation);
        userRegister();
    } else if (t_operation == "q") {
        // check_login_state(false, IS_LOG_IN, t_operation);
        quitAgenda();
    } else if (t_operation == "o") {
        check_login_state(true, IS_LOG_IN, t_operation);
        userLogOut();
    } else if (t_operation == "dc") {
        check_login_state(true, IS_LOG_IN, t_operation);
        deleteUser();
    } else if (t_operation == "lu") {
        check_login_state(true, IS_LOG_IN, t_operation);
        listAllUsers();
    } else if (t_operation == "cm") {
        check_login_state(true, IS_LOG_IN, t_operation);
        createMeeting();
    } else if (t_operation == "la") {
        check_login_state(true, IS_LOG_IN, t_operation);
        listAllMeetings();
    } else if (t_operation == "las") {
        check_login_state(true, IS_LOG_IN, t_operation);
        listAllSponsorMeetings();
    } else if (t_operation == "lap") {
        check_login_state(true, IS_LOG_IN, t_operation);
        listAllParticipateMeetings();
    } else if (t_operation == "qm") {
        check_login_state(true, IS_LOG_IN, t_operation);
        queryMeetingByTitle();
    } else if (t_operation == "qt") {
        check_login_state(true, IS_LOG_IN, t_operation);
        queryMeetingByTimeInterval();
    } else if (t_operation == "dm") {
        check_login_state(true, IS_LOG_IN, t_operation);
        deleteMeetingByTitle();
    } else if (t_operation == "da") {
        check_login_state(true, IS_LOG_IN, t_operation);
        deleteAllMeetings();
    } else if (t_operation == "h") {
        print_manual(IS_LOG_IN);
    } else {
        throw unknown_command(t_operation);
    }
    return true;
}

void AgendaUI::userLogIn(void) {
    string promptMsg = "LOGIN";
    custom_prompt(promptMsg);
    print_format({"USERNAME", "PASSWORD"});

    vector<string> result = get_arguments(2, promptMsg);

    m_logger->log_start(Logger::login, result[0]);

    m_agendaService.userLogIn(result[0], result[1]);

    m_userName = result[0];
    m_userPassword = result[1];

    m_logger->log_success();
    custom_prompt(promptMsg) << "Login Success!\n";
}

void AgendaUI::userRegister(void) {
    string promptMsg("REGISTER");
    custom_prompt(promptMsg);
    print_format({"USERNAME", "PASSWORD", "EMAIL", "PHONE"});

    vector<string> result = get_arguments(4, promptMsg);

    m_logger->log_start(Logger::createUser, result[0]);
    m_agendaService.userRegister(result[0], result[1], result[2], result[3]);
    m_logger->log_success();
    custom_prompt(promptMsg) << "Success!\n";
}

void AgendaUI::quitAgenda(int e) {
    // m_agendaService.quitAgenda();
    if (IS_LOG_IN) {
        m_logger->log_out();
    }
    m_logger->log_exit(e);
    cout << "[BYE]\n";
    exit(0);
}

void AgendaUI::userLogOut(void) {
    m_userName = "";
    m_userPassword = "";
    m_logger->log_out();
    custom_prompt("LOGOUT") << "Logged out\n";
}

void AgendaUI::deleteUser(void) {
    m_logger->log_start(Logger::deleteUser);
    int result = m_agendaService.deleteUser(m_userName, m_userPassword);
    m_logger->log_success(result);
    m_userName = "";
    m_userPassword = "";
    custom_prompt("DELETE USER") << "Success!\n";
}

void AgendaUI::listAllUsers(void) {
    custom_prompt("LIST ALL USERS") << "\n\n";

    m_logger->log_start(Logger::listUser);
    list<User> userList = m_agendaService.listAllUsers();

    int maxUnLen(10), maxEmLen(7), maxPhLen(7);
    for (const auto &user : userList) {
        int unLen = user.getName().size(), emLen = user.getEmail().size(),
            phLen = user.getPhone().size();
        if (unLen > maxUnLen)
            maxUnLen = unLen;
        if (emLen > maxEmLen)
            maxEmLen = emLen;
        if (phLen > maxPhLen)
            maxPhLen = phLen;
    }
    ++maxUnLen;
    ++maxPhLen;
    ++maxEmLen;
    cout << left << setw(maxUnLen) << "[Username] " << setw(maxEmLen)
         << "[Email] "
         << "[Phone]\n";

    for (const auto &user : userList) {
        cout << setw(maxUnLen) << user.getName() << setw(maxEmLen)
             << user.getEmail() << setw(maxPhLen) << user.getPhone() << '\n';
    }

    m_logger->log_success(userList.size());
    // cout << '\n';
}

void AgendaUI::createMeeting(void) {
    string promptMsg("CREATE MEETING");

    custom_prompt(promptMsg) << "How Many Participators?\n";

    string partStr = get_stripped_line();

    for (; partStr == "" or
           not std::all_of(partStr.begin(), partStr.end(), isdigit);) {
        custom_prompt(promptMsg) << "Input an INTEGER plz\n";
        cin.clear();
        partStr = get_stripped_line();
    }

    int partNum = std::stoi(partStr);

    custom_prompt(promptMsg) << "Now input ONE participator each line\n";

    vector<string> participators;
    for (int i = 1; i <= partNum;) {
        custom_prompt(promptMsg) << "[" << i << "] ";
        string participator = get_stripped_line();
        if (participator == "")
            continue;
        participators.push_back(participator);
        ++i;
    }

    custom_prompt(promptMsg);
    print_format({"TITLE", "START(YYYY-MM-DD/HH:MM)", "END(YYYY-MM-DD/HH:MM)"});

    vector<string> result = get_arguments(3, promptMsg);

    m_logger->log_create(result[1], result[2], result[0], participators);
    m_agendaService.createMeeting(
        m_userName, result[0], result[1], result[2], participators);
    m_logger->log_success();
    custom_prompt(promptMsg) << "Success!\n";
}

void AgendaUI::listAllMeetings(void) {
    custom_prompt("LIST ALL MEETINGS") << '\n';
    m_logger->log_start(Logger::listMeeting);
    auto list = m_agendaService.listAllMeetings(m_userName);
    printMeetings(list);
    m_logger->log_success(list.size());
}

void AgendaUI::listAllSponsorMeetings(void) {
    custom_prompt("LIST ALL MEETINGS YOU SPONSORED") << '\n';
    m_logger->log_start(Logger::listBySpon);
    auto list = m_agendaService.listAllSponsorMeetings(m_userName);
    printMeetings(list);
    m_logger->log_success(list.size());
}

void AgendaUI::listAllParticipateMeetings(void) {
    custom_prompt("LIST ALL MEETINGS YOU PARTICIPATED") << '\n';
    m_logger->log_start(Logger::listByPart);
    auto list = m_agendaService.listAllParticipateMeetings(m_userName);
    printMeetings(list);
    m_logger->log_success(list.size());
}

void AgendaUI::queryMeetingByTitle(void) {
    string promptMsg("QUERY MEETING");
    custom_prompt(promptMsg);
    print_format({"TITLE"});

    vector<string> result = get_arguments(1, promptMsg);

    custom_prompt(promptMsg) << '\n';
    m_logger->log_start(Logger::queryMeeting, result[0]);
    auto list = m_agendaService.meetingQuery(m_userName, result[0]);
    printMeetings(list);
    m_logger->log_success(list.size());
}

void AgendaUI::queryMeetingByTimeInterval(void) {
    string promptMsg("QUERY MEETING");
    custom_prompt(promptMsg);
    print_format({"START(YYYY-MM-DD/HH:MM)", "END(YYYY-MM-DD/HH:MM)"});

    vector<string> result = get_arguments(2, promptMsg);

    custom_prompt(promptMsg) << '\n';
    m_logger->log_start(Logger::queryMeeting, result[0], result[1]);
    auto list = m_agendaService.meetingQuery(m_userName, result[0], result[1]);
    printMeetings(list);
    m_logger->log_success(list.size());
}

void AgendaUI::deleteMeetingByTitle(void) {
    string promptMsg("DELETE MEETING");
    custom_prompt(promptMsg);
    print_format({"TITLE"});

    vector<string> result = get_arguments(1, promptMsg);

    m_logger->log_start(Logger::deleteMeeting, result[0]);
    int deleteNum = m_agendaService.deleteMeeting(m_userName, result[0]);
    m_logger->log_success(deleteNum);

    custom_prompt(promptMsg) << "Delete Success!\n";
}

void AgendaUI::deleteAllMeetings(void) {
    m_logger->log_start(Logger::deleteMeeting);
    int deleteNum = m_agendaService.deleteAllMeetings(m_userName);
    m_logger->log_success(deleteNum);
    custom_prompt("DELETE ALL MEETINGS") << "Success!\n";
}

void AgendaUI::printMeetings(const list<Meeting> &t_meetings) const {
    string splitLine(40, '=');
    if (t_meetings.empty()) {
        cout << "NO MEETINGS\n";
        return;
    }
    cout << splitLine << '\n';
    for (const auto &meeting : t_meetings) {
        cout << left << setw(15) << "Title: " << meeting.getTitle() << '\n'
             << setw(15) << "Sponsor: " << meeting.getSponsor() << '\n'
             << setw(15)
             << "Start Time: " << Date::dateToString(meeting.getStartDate())
             << '\n'
             << setw(15)
             << "End Time: " << Date::dateToString(meeting.getEndDate()) << '\n'
             << setw(15) << "Participators: ";
        for (const auto &participator : meeting.getParticipator()) {
            cout << participator << ' ';
        }
        cout << "\n" << splitLine << "\n";
    }
}
