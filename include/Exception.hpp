#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>

class my_exception {
  private:
    std::string m_message;

  public:
    std::string what() const {
        return m_message;
    }
    virtual std::string type() const {
        return "Exception";
    }
    my_exception(const std::string &p_msg) : m_message(p_msg) {}

    virtual ~my_exception() {}
};

class wrong_arg_num : public my_exception {
  public:
    virtual std::string type() const {
        return "Wrong Argument Number";
    }
    wrong_arg_num(const std::string &p_msg = "") : my_exception(p_msg) {}
};

class invalid_date : public my_exception {
  public:
    virtual std::string type() const {
        return "Invalid Date";
    }
    invalid_date(const std::string &p_msg) : my_exception(p_msg) {}
};

class user_repeat : public my_exception {
  public:
    virtual std::string type() const {
        return "Username Repeat";
    }
    user_repeat(const std::string &p_msg = std::string(""))
        : my_exception(p_msg) {}
};

class user_not_found : public my_exception {
  public:
    virtual std::string type() const {
        return "Username Not Found";
    }
    user_not_found(const std::string &p_msg = std::string(""))
        : my_exception(p_msg) {}
};

class unknown_command : public my_exception {
  public:
    virtual std::string type() const {
        return "Unknown Command";
    }
    unknown_command(const std::string &p_msg = std::string(""))
        : my_exception(p_msg) {}
};

class title_repeat : public my_exception {
  public:
    virtual std::string type() const {
        return "Title Repeat";
    }
    title_repeat(const std::string &p_msg = std::string(""))
        : my_exception(p_msg) {}
};

class empty_deletion : public my_exception {
  public:
    virtual std::string type() const {
        return "Empty Deletion";
    }
    empty_deletion(const std::string &p_msg = "") : my_exception(p_msg) {}
};

class time_conflict : public my_exception {
  public:
    virtual std::string type() const {
        return "Time Confliction";
    }
    time_conflict(const std::string &p_msg = "") : my_exception(p_msg) {}
};

class wrong_format : public my_exception {
  public:
    virtual std::string type() const {
        return "Wrong Format";
    }
    wrong_format(const std::string &p_msg = std::string(""))
        : my_exception(p_msg) {}
};

#endif  // EXCEPTION_HPP