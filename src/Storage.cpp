#include "Storage.hpp"
#include "Exception.hpp"
#include "Path.hpp"
#include <fstream>
#include <iostream>
#include <regex>
#include <utility>

using std::list;
using std::string;
using std::function;
using std::shared_ptr;
using std::ofstream;
using std::ifstream;
using std::getline;

shared_ptr<Storage> Storage::m_instance = nullptr;

Storage::Storage() : m_dirty(false) {
    readFromFile();
}

bool Storage::readFromFile(void) {
    ifstream userStream(Path::userPath, ifstream::in),
        meetingStream(Path::meetingPath, ifstream::in);

    if (userStream.fail() or meetingStream.fail())
        return false;

    std::regex userPattern("^\"(.+)\",\"(.+)\",\"(.+)\",\"(.+)\"$");
    std::smatch result;
    for (string line; getline(userStream, line);) {
        if (not std::regex_match(line, result, userPattern)) {
            throw wrong_format("wrong file format");
        }
        m_userList.push_back(User(result[1], result[2], result[3], result[4]));
    }
    userStream.close();

    std::regex meetingPattern("^\"(.+)\",\"(.+)\",\"(.+)\",\"(.+)\",\"(.+)\"$");
    for (string line; getline(meetingStream, line);) {
        if (not std::regex_match(line, result, meetingPattern)) {
            throw wrong_format("wrong file format");
        }
        Meeting meeting;
        meeting.setSponsor(result[1]);

        string paticipators = result[2];
        std::vector<string> temp;
        for (string::size_type amper(0), start(0);;) {
            amper = paticipators.find('&', start);
            if (amper != string::npos) {
                temp.push_back(paticipators.substr(start, amper - start));
                start = amper + 1;
            } else {
                temp.push_back(paticipators.substr(start));
                break;
            }
        }
        meeting.setParticipator(temp);

        string startDate(result[3]), endDate(result[4]);

        meeting.setStartDate(Date::stringToDate(startDate));
        meeting.setEndDate(Date::stringToDate(endDate));

        meeting.setTitle(result[5]);
        m_meetingList.push_back(meeting);
    }
    meetingStream.close();
    return true;
}

bool Storage::writeToFile(void) {
    ofstream userStream(Path::userPath, ofstream::out | ofstream::trunc),
        meetingStream(Path::meetingPath, ofstream::out | ofstream::trunc);

    if (userStream.fail() or meetingStream.fail())
        return false;

    for (const auto &user : m_userList) {
        userStream << '\"' << user.getName() << "\",\"" << user.getPassword()
                   << "\",\"" << user.getEmail() << "\",\"" << user.getPhone()
                   << "\"\n";
    }
    userStream.close();
    for (const auto &meeting : m_meetingList) {
        meetingStream << '\"' << meeting.getSponsor() << "\",\"";
        auto participators(meeting.getParticipator());
        for (auto pIter = participators.begin();;) {
            meetingStream << *pIter++;
            if (pIter == participators.end()) {
                meetingStream << "\",\"";
                break;
            } else {
                meetingStream << '&';
            }
        }
        meetingStream << Date::dateToString(meeting.getStartDate()) << "\",\""
                      << Date::dateToString(meeting.getEndDate()) << "\",\""
                      << meeting.getTitle() << "\"\n";
    }
    meetingStream.close();
    return true;
}

shared_ptr<Storage> Storage::getInstance(void) {
    if (m_instance == nullptr)
        m_instance = shared_ptr<Storage>(new Storage);
    return m_instance;
}

Storage::~Storage() {
    sync();
}

void Storage::createUser(const User &t_user) {
    m_userList.push_back(t_user);
    m_dirty = true;
}

list<User> Storage::queryUser(function<bool(const User &)> filter) const {
    list<User> result;
    for (const auto &user : m_userList) {
        if (filter(user))
            result.push_back(user);
    }
    return result;
}

int Storage::updateUser(
    function<bool(const User &)> filter, function<void(User &)> switcher) {
    int count = 0;
    for (auto &user : m_userList) {
        if (filter(user)) {
            switcher(user);
            ++count;
        }
    }
    if (count)
        m_dirty = true;
    return count;
}

int Storage::deleteUser(function<bool(const User &)> filter) {
    int size = m_userList.size();
    m_userList.remove_if(filter);

    int removed = size - m_userList.size();
    if (removed)
        m_dirty = true;
    return removed;
}

void Storage::createMeeting(const Meeting &t_meeting) {
    m_meetingList.push_back(t_meeting);
    m_dirty = true;
}

list<Meeting> Storage::queryMeeting(
    function<bool(const Meeting &)> filter) const {
    list<Meeting> result;
    for (const auto &meeting : m_meetingList) {
        if (filter(meeting))
            result.push_back(meeting);
    }
    return result;
}

int Storage::updateMeeting(
    function<bool(const Meeting &)> filter,
    function<void(Meeting &)> switcher) {
    int count = 0;
    for (auto &meeting : m_meetingList) {
        if (filter(meeting)) {
            switcher(meeting);
            ++count;
        }
    }
    if (count)
        m_dirty = true;
    return count;
}

int Storage::deleteMeeting(function<bool(const Meeting &)> filter) {
    int size = m_meetingList.size();
    m_meetingList.remove_if(filter);
    int removed = size - m_meetingList.size();

    if (removed)
        m_dirty = true;
    return removed;
}

bool Storage::sync(void) {
    if (m_dirty and not writeToFile())
        return false;
    m_dirty = false;
    return true;
}
