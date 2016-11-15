#ifndef LOGGER_H
#define LOGGER_H

#include "Exception.hpp"
#include "Path.hpp"
#include <cassert>
#include <ctime>
#include <ctime>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class Logger {
  public:
    enum State {
        none,
        login,
        createUser,
        createMeeting,
        deleteUser,
        deleteMeeting,
        listUser,
        listMeeting,
        listByPart,
        listBySpon,
        queryMeeting,
    };

  private:
    using string = std::string;

    State m_state;
    std::ofstream m_dest;
    string m_username;

    // for singleton
  private:
    static std::shared_ptr<Logger> m_logger;
    Logger() : m_state(none), m_dest(Path::logPath, std::ofstream::app) {}

  public:
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    static std::shared_ptr<Logger> get_instance();

    // log functions
  public:
    void log_out();

    void log_create(
        const string &p_start_time, const string &p_end_time,
        const string &p_title, const std::vector<string> &p_parts);

    void log_start(const State p_state);
    void log_start(const State p_state, const string &arg1);
    void log_start(const State p_state, const string &arg1, const string &arg2);

    void log_success();
    void log_success(int result);
    void log_error(const my_exception &p_error);
    void log_exit(int p_error);
    void log_start();

  private:
    string time_tag();

    void log_line(const string &p_message);
    void log_line();

    template <typename... Args>
    void log_operation(const std::string &p_operation, Args &&... args);

    template <typename... Args>
    void print_arguments(
        const string &p_argument, const string &p_value, Args &&... args);

    void print_arguments();
};

#endif  // LOGGER_H
