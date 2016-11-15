#include "Logger.hpp"
#include "Exception.hpp"

std::shared_ptr<Logger> Logger::get_instance() {
    if (m_logger == nullptr) {
        m_logger = std::shared_ptr<Logger>(new Logger);
    }
    return m_logger;
}

void Logger::log_out() {
    assert(m_username != "");
    log_line(m_username + " log out");
    log_line();
}

void Logger::log_create(
    const string &p_start_time, const string &p_end_time, const string &p_title,
    const std::vector<string> &p_parts) {
    m_state = createMeeting;

    std::ostringstream parts;
    for (auto pIter = begin(p_parts);;) {
        parts << *pIter++;
        if (pIter != end(p_parts))
            parts << '&';
        else
            break;
    }

    log_operation(
        "Create Meeting", "Start Time", p_start_time, "End Time", p_end_time,
        "Title", p_title, "Participators", parts.str());
}

void Logger::log_start(const State p_state) {
    string operationType;

    if (p_state == deleteUser) {
        operationType = "Delete Account";
    } else if (p_state == deleteMeeting) {
        operationType = "Delete Sponsored Meeting";
    } else if (p_state == listUser) {
        operationType = "List All Users";
    } else if (p_state == listMeeting) {
        operationType = "List All Relative Meetings";
    } else if (p_state == listBySpon) {
        operationType = "List Sponsored Meetings";
    } else if (p_state == listByPart) {
        operationType = "List Participated Meetings";
    } else {
        // throw unknown_state("No match operation");
    }

    m_state = p_state;
    log_operation(operationType);
}
void Logger::log_start(const State p_state, const string &arg1) {
    string operationType;
    string paramType;

    if (p_state == deleteMeeting) {
        operationType = "Delete Meeting by Title";
        paramType = "Title";
    } else if (p_state == login) {
        operationType = "Login";
        paramType = "Username";
        m_username = arg1;
    } else if (p_state == queryMeeting) {
        operationType = "Query Meeting by Title";
        paramType = "Title";
    } else if (p_state == createUser) {
        operationType = "User Register";
        paramType = "Username";
    } else {
        // throw unknown_state("No match operation");
    }

    m_state = p_state;
    log_operation(operationType, paramType, arg1);
}
void Logger::log_start(
    const State p_state, const string &arg1, const string &arg2) {
    string operationType;
    string param1Type;
    string param2Type;

    if (p_state == queryMeeting) {
        operationType = "Query Meeting by Time";
        param1Type = "Start Time";
        param2Type = "End Time";
    } else {
        // throw unknown_state("No match operation");
    }

    m_state = p_state;
    log_operation(operationType, param1Type, arg1, param2Type, arg2);
}

void Logger::log_success() {
    m_dest << "[Result] Success\n" << std::endl;
    m_state = none;
}
void Logger::log_success(int result) {
    string remark;

    if (m_state == deleteUser) {
        remark = std::to_string(result) + " relative meeting(s) deleted";
    } else {
        remark = std::to_string(result) + " entries found";
    }
    m_dest << "[Result] Success\n[Remark] " << remark << '\n' << std::endl;
}

void Logger::log_error(const my_exception &p_error) {
    m_dest << "[Result] Fail\n[Error Type] " << p_error.type()
           << "\n[Description] " << p_error.what() << '\n';
    log_line();
    if (m_state == login) {
        m_username = "";
    }
    m_state = none;
}

std::string Logger::time_tag() {
    char tag[20];
    auto now = time(nullptr);
    strftime(tag, 20, "[%F %R] ", localtime(&now));
    return tag;
}

void Logger::log_line(const string &p_message) {
    m_dest << time_tag() << p_message << std::endl;
}
void Logger::log_line() {
    m_dest << std::endl;
}

template <typename... Args>
void Logger::log_operation(const std::string &p_operation, Args &&... args) {
    m_dest << time_tag() << '\n' << "[Operation] " << p_operation << '\n';
    if (not(m_state == login or m_state == createUser))
        m_dest << "[User] " << m_username << '\n';

    if (sizeof...(args)) {
        m_dest << "[Parameters]\n";
    }
    print_arguments(std::forward<Args>(args)...);
}

template <typename... Args>
void Logger::print_arguments(
    const string &p_argument, const string &p_value, Args &&... args) {
    m_dest << "    [" << p_argument << "] " << p_value << std::endl;
    print_arguments(std::forward<Args>(args)...);
}

void Logger::print_arguments() {}

void Logger::log_exit(int p_error) {
    if (not p_error) {
        log_line("System Exit Normally\n");
    } else {
        log_line(
            "Unexpected Exit (Error Code " + std::to_string(p_error) + ")\n");
    }
}

void Logger::log_start() {
    log_line("System Start\n");
}
std::shared_ptr<Logger> Logger::m_logger = nullptr;