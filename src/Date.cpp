#include "Date.hpp"
#include "Exception.hpp"
#include <cassert>
#include <cctype>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

using std::to_string;
using std::stoi;
using std::vector;
using std::string;

const int Date::day_of_month[] = {31, 28, 31, 30, 31, 30,
                                  31, 31, 30, 31, 30, 31};

// auxillary static functions

bool Date::isLeap(int t_year) {
    if (t_year % 400 == 0)
        return true;
    if (t_year % 100 == 0)
        return false;
    if (t_year % 4 == 0)
        return true;
    return false;
}

bool Date::checkDay(const int t_year, const int t_month, const int t_day) {
    if (t_day <= 0)
        return false;
    if (isLeap(t_year) and t_month == 2 and t_day == 29)
        return true;
    if (t_day > day_of_month[t_month - 1])
        return false;
    return true;
}

string two_digits_string(const int t_num) {
    assert(t_num >= 0 and t_num < 100);

    string result = to_string(t_num);
    if (t_num < 10)
        result = '0' + result;

    return result;
}

// static functions

bool Date::isValid(const Date t_date) {
    // check hour
    if (t_date.m_hour < 0 or t_date.m_hour >= 24)
        return false;
    // check minute
    if (t_date.m_minute < 0 or t_date.m_minute >= 60)
        return false;
    // check year
    if (t_date.m_year < 1000 or t_date.m_year >= 10000)
        return false;
    // check month
    if (t_date.m_month <= 0 or t_date.m_month > 12)
        return false;
    // check day
    return checkDay(t_date.m_year, t_date.m_month, t_date.m_day);
    // check pass
}

Date Date::stringToDate(const string t_dateString) {
    std::regex pattern("^(\\d{4})\\-(\\d{2})\\-(\\d{2})/(\\d{2}):(\\d{2})$");
    std::smatch result;
    if (not std::regex_match(t_dateString, result, pattern))
        throw wrong_format("Wrong Date Format: " + t_dateString);
    int year = stoi(result[1]), month = stoi(result[2]), day = stoi(result[3]),
        hour = stoi(result[4]), minute = stoi(result[5]);

    Date ret(year, month, day, hour, minute);
    if (not isValid(ret)) {
        throw invalid_date(t_dateString);
    }
    return ret;
}

string Date::dateToString(Date t_date) {
    string result = to_string(t_date.m_year) + '-' +
                    two_digits_string(t_date.m_month) + '-' +
                    two_digits_string(t_date.m_day) + '/' +
                    two_digits_string(t_date.m_hour) + ':' +
                    two_digits_string(t_date.m_minute);

    if (not isValid(t_date)) {
        throw invalid_date(result);
    }
    return result;
}

// constructors

Date::Date() : m_year(0), m_month(0), m_day(0), m_hour(0), m_minute(0) {}

Date::Date(int t_year, int t_month, int t_day, int t_hour, int t_minute)
    : m_year(t_year), m_month(t_month), m_day(t_day), m_hour(t_hour),
      m_minute(t_minute) {}

Date::Date(string dateString) : Date() {
    *this = stringToDate(dateString);
}

Date &Date::operator=(const Date &t_date) {
    m_year = t_date.m_year;
    m_month = t_date.m_month;
    m_day = t_date.m_day;
    m_hour = t_date.m_hour;
    m_minute = t_date.m_minute;
    return *this;
}

// getters

int Date::getYear(void) const {
    return m_year;
}

int Date::getMonth(void) const {
    return m_month;
}

int Date::getDay(void) const {
    return m_day;
}

int Date::getHour(void) const {
    return m_hour;
}

int Date::getMinute(void) const {
    return m_minute;
}

// setters with boundary check

void Date::setYear(const int t_year) {
    m_year = t_year;
}

void Date::setMonth(const int t_month) {
    m_month = t_month;
}

void Date::setDay(const int t_day) {
    m_day = t_day;
}

void Date::setHour(const int t_hour) {
    m_hour = t_hour;
}

void Date::setMinute(const int t_minute) {
    m_minute = t_minute;
}

// operator overloads
bool Date::operator==(const Date &t_date) const {
    return m_year == t_date.m_year and m_month == t_date.m_month and
           m_day == t_date.m_day and m_hour == t_date.m_hour and
           m_minute == t_date.m_minute;
}

bool Date::operator>(const Date &t_date) const {
    return vector<int>{m_year, m_month, m_day, m_hour, m_minute} >
           vector<int>{t_date.m_year, t_date.m_month, t_date.m_day,
                       t_date.m_hour, t_date.m_minute};
}

bool Date::operator<(const Date &t_date) const {
    return not(*this >= t_date);
}

bool Date::operator>=(const Date &t_date) const {
    return (*this > t_date or *this == t_date);
}

bool Date::operator<=(const Date &t_date) const {
    return not(*this > t_date);
}
