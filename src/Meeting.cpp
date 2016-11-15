#include "Meeting.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

using std::find;
using std::string;
using std::vector;
using std::move;

Meeting::Meeting(
    string t_sponsor, vector<string> t_participators, Date t_startTime,
    Date t_endTime, string t_title)
    : m_sponsor(t_sponsor), m_participators(move(t_participators)),
      m_startDate(t_startTime), m_endDate(t_endTime), m_title(t_title) {}

Meeting::Meeting(const Meeting &t_meeting) = default;

string Meeting::getSponsor(void) const {
    return m_sponsor;
}

void Meeting::setSponsor(const string t_sponsor) {
    m_sponsor = t_sponsor;
}

vector<string> Meeting::getParticipator(void) const {
    return move(m_participators);
}

void Meeting::setParticipator(const vector<string> t_participators) {
    m_participators = std::move(t_participators);
}

Date Meeting::getStartDate(void) const {
    return m_startDate;
}

void Meeting::setStartDate(const Date t_startTime) {
    m_startDate = t_startTime;
}

Date Meeting::getEndDate(void) const {
    return m_endDate;
}

void Meeting::setEndDate(const Date t_endTime) {
    m_endDate = t_endTime;
}

string Meeting::getTitle(void) const {
    return m_title;
}

void Meeting::setTitle(const string t_title) {
    m_title = t_title;
}

bool Meeting::isParticipator(const string t_username) const {
    return find(m_participators.begin(), m_participators.end(), t_username) !=
           m_participators.end();
}
