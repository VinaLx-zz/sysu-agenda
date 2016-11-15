#include "AgendaService.hpp"
#include "Exception.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

using std::string;
using std::list;
using std::vector;
using std::find;
using std::invalid_argument;
using std::out_of_range;

AgendaService::AgendaService() {}

AgendaService::~AgendaService() {
    if (m_storage != nullptr)
        quitAgenda();
}

void AgendaService::userLogIn(const string &userName, const string &password) {
    auto filter = [&](const User &user) -> bool {
        return (user.getName() == userName and user.getPassword() == password);
    };
    if (m_storage->queryUser(filter).empty())
        throw user_not_found("Username doesn't match password");
}

void AgendaService::userRegister(
    const string &userName, const string &password, const string &email,
    const string &phone) {
    auto filter = [&](const User &user) -> bool {
        return user.getName() == userName;
    };
    if (not m_storage->queryUser(filter).empty())
        throw user_repeat("Username taken by others");

    m_storage->createUser(User(userName, password, email, phone));
}

int AgendaService::deleteUser(const string &userName, const string &password) {
    auto filter = [&](const User &user) -> bool {
        return (user.getName() == userName and user.getPassword() == password);
    };
    // check if the user exist
    if (not m_storage->deleteUser(filter))
        throw user_not_found("User not exist");

    auto removeUser = [&](const Meeting &meeting) {
        return meeting.getSponsor() == userName or
               meeting.isParticipator(userName);
    };
    return m_storage->deleteMeeting(removeUser);
}

list<User> AgendaService::listAllUsers(void) const {
    auto all = [](const User &) { return true; };
    return (m_storage->queryUser(all));
}

void AgendaService::createMeeting(
    const string &userName, const string &title, const string &startDate,
    const string &endDate, const vector<string> participator) {
    auto sDate = Date::stringToDate(startDate),
         eDate = Date::stringToDate(endDate);
    // check if user exist
    auto exist = [&](const User &user) { return user.getName() == userName; };
    if (m_storage->queryUser(exist).empty())
        throw user_not_found("Sponsor: " + userName);

    for (const auto &part : participator) {
        if (m_storage
                ->queryUser(
                    [&](const User &user) { return user.getName() == part; })
                .empty()) {
            throw user_not_found("Participator: " + part);
        }
    }

    // check if dates valid
    if (not Date::isValid(sDate)) {
        throw invalid_date(startDate);
    }
    if (not Date::isValid(eDate)) {
        throw out_of_range(endDate);
    }

    if (sDate >= eDate) {
        throw invalid_date("Start not earlier than End");
    }

    // check if sponsor is one of participators
    if (std::find(participator.begin(), participator.end(), userName) !=
        participator.end()) {
        throw user_repeat("Sponsor: " + userName + " found in participators");
    }
    // check if participator repeat
    for (auto iter = participator.begin(); iter != participator.end(); ++iter) {
        if (std::find(participator.begin(), iter, *iter) != iter) {
            throw user_repeat(
                "Participator: " + *iter + " appears more than once");
        }
    }

    // check spare time of everyone
    auto overlap = [&](const Meeting &meeting) -> bool {
        // if title is repeated
        if (meeting.getTitle() == title) {
            throw title_repeat(
                "Meeting sponsored by " + meeting.getSponsor() +
                " has a same title");
        }

        // if not overlap return false;
        if (sDate >= meeting.getEndDate() or eDate <= meeting.getStartDate())
            return false;

        // if sponsor is busy
        if (userName == meeting.getSponsor() or
            meeting.isParticipator(userName))
            throw time_conflict(
                "Sponsor: " + userName + "\" is busy from " +
                Date::dateToString(meeting.getStartDate()) + " to " +
                Date::dateToString(meeting.getEndDate()));

        // if any participator is busy
        for (auto &part : participator) {
            if (part == meeting.getSponsor() or meeting.isParticipator(part))
                throw out_of_range(
                    "Participator: " + part + " is busy from " +
                    Date::dateToString(meeting.getStartDate()) + " to " +
                    Date::dateToString(meeting.getEndDate()));
        }

        return false;
    };
    m_storage->queryMeeting(overlap);

    m_storage->createMeeting(
        Meeting(userName, (participator), startDate, endDate, title));
}

list<Meeting>
AgendaService::meetingQuery(const string &userName, const string &title) const {
    auto filter = [&](const Meeting &meeting) -> bool {
        return (meeting.isParticipator(userName) or
                meeting.getSponsor() == userName) and
               meeting.getTitle() == title;
    };
    return (m_storage->queryMeeting(filter));
}

list<Meeting> AgendaService::meetingQuery(
    const string &userName, const string &startDate,
    const string &endDate) const {
    auto sDate = Date::stringToDate(startDate),
         eDate = Date::stringToDate(endDate);
    if (not Date::isValid(sDate)) {
        throw out_of_range("Start date is invalid");
    }
    if (not Date::isValid(eDate)) {
        throw out_of_range("End date is invalid");
    }

    if (sDate > eDate) {
        throw invalid_date("End date must be later than start date");
    }

    auto overlap = [&](const Meeting &meeting) -> bool {
        return not(eDate < meeting.getStartDate() or
                   sDate > meeting.getEndDate()) and
               (meeting.getSponsor() == userName or
                meeting.isParticipator(userName));
    };
    return (m_storage->queryMeeting(overlap));
}

list<Meeting> AgendaService::listAllMeetings(const string &userName) const {
    auto all = [&](const Meeting &meeting) {
        return meeting.getSponsor() == userName or
               meeting.isParticipator(userName);
    };
    return (m_storage->queryMeeting(all));
}

list<Meeting>
AgendaService::listAllSponsorMeetings(const string &userName) const {
    auto filter = [&](const Meeting &meeting) -> bool {
        return meeting.getSponsor() == userName;
    };
    return (m_storage->queryMeeting(filter));
}

list<Meeting>
AgendaService::listAllParticipateMeetings(const string &userName) const {
    auto filter = [&](const Meeting &meeting) -> bool {
        return meeting.isParticipator(userName);
    };
    return (m_storage->queryMeeting(filter));
}

int AgendaService::deleteMeeting(const string &userName, const string &title) {
    auto filter = [&](const Meeting &meeting) -> bool {
        return meeting.getSponsor() == userName and meeting.getTitle() == title;
    };
    int deleteNum = m_storage->deleteMeeting(filter);

    if (not deleteNum)
        throw empty_deletion(
            "No meeting found. Title: " + title + ". Sponsor: " + userName);
    return deleteNum;
}

int AgendaService::deleteAllMeetings(const string &userName) {
    auto filter = [&](const Meeting &meeting) {
        return meeting.getSponsor() == userName;
    };
    int deleteNum = m_storage->deleteMeeting(filter);

    if (not deleteNum)
        throw empty_deletion("No meeting found. Sponsor: " + userName);
    return deleteNum;
}

void AgendaService::startAgenda(void) {
    m_storage = Storage::getInstance();
}

void AgendaService::quitAgenda(void) {
    m_storage->sync();
    m_storage = nullptr;
}
