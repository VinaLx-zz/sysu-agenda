from Storage import Storage
from User import User
from Meeting import Meeting
from Date import Date
import MyExcept


def checkEmptyDeletion(func):
    def wrapper(self, *args):
        num = func(self, *args)
        if not num:
            raise MyExcept.EmptyDeletion('No meeting deleted')
        return num
    return wrapper


class AgendaService(object):

    def __init__(self):
        self.startAgenda()

    def startAgenda(self):
        self.storage = Storage.getInstance()

    def quitAgenda(self):
        self.storage.closeStorage()

    def userLogin(self, username, password):
        def predicate(user):
            return (user.getName(), user.getPassword()) == (username, password)
        if not list(self.storage.queryUser(predicate)):
            raise MyExcept.UserNotFound(username)

    def userRegister(self, username, password, email, phone):
        def predicate(user):
            return user.getName() == username
        if list(self.storage.queryUser(predicate)):
            raise MyExcept.UserRepeat('Username was taken')
        self.storage.createUser(User(username, password, email, phone))

    def deleteUser(self, username, password):
        def delUser(user):
            return (user.getName(), user.getPassword()) == (username, password)
        if not self.storage.deleteUser(delUser):
            raise MyExcept.UserNotFound(username)

        def delMeeting(meeting):
            return meeting.isOne(username)
        return self.storage.deleteMeeting(delMeeting)

    def listAllUsers(self):
        return self.storage.queryUser(lambda user: True)

    def queryMeetingTitle(self, username, title):
        def predicate(meeting):
            return meeting.isOne(username) and meeting.getTitle() == title
        return self.storage.queryMeeting(predicate)

    def queryMeetingTime(self, username, startTime, endTime):
        start = Date.stringToDate(startTime)
        end = Date.stringToDate(endTime)
        if end < start:
            raise MyExcept.InvalidDate('EndTime must be later')

        def overlap(meeting):
            return (not(start > meeting.getEndDate() or
                        end < meeting.startDate()) and
                    meeting.isOne(username))
        return self.storage.queryMeeting(overlap)

    def listAllMeetings(self, username):
        def predicate(meeting):
            return meeting.isOne(username)
        return self.storage.queryMeeting(predicate)

    def listAllSponsorMeetings(self, username):
        def predicate(meeting):
            return meeting.getSponsor() == username
        return self.storage.queryMeeting(predicate)

    def listAllParticipateMeetings(self, username):
        def predicate(meeting):
            return meeting.isParticipator(username)
        return self.storage.queryMeeting(predicate)

    @checkEmptyDeletion
    def deleteMeeting(self, username, title):
        def predicate(meeting):
            return meeting.getSponsor() == username and \
                meeting.getTitle() == title
        return self.storage.deleteMeeting(predicate)

    @checkEmptyDeletion
    def deleteAllMeetings(self, username):
        def predicate(meeting):
            return meeting.getSponsor() == username
        return self.storage.deleteMeeting(predicate)

    def createMeeting(self, username, title, startDate, endDate, parts):
        start = Date.stringToDate(startDate)
        end = Date.stringToDate(endDate)
        if (start >= end):
            raise MyExcept.InvalidDate('StartTime must be earlier')

        def exist(username):
            def predicate(user):
                return user.getName() == username
            return predicate

        def overlap(someone):
            def predicate(meeting):
                if meeting.getTitle() == title:
                    raise MyExcept.TitleRepeat(
                        'Meeting sponsored by ' + meeting.getSponsor() +
                        ' has a same title')
                if start >= meeting.getEndTime() or\
                        end <= Meeting.getStartTime():
                    return False
                if meeting.isOne(someone):
                    raise MyExcept.TimeConflict(
                        someone + ' is busy from ' + meeting.getStartTime() +
                        ' to ' + meeting.getEndTime())

        if not list(self.storage.queryUser(exist(username))):
            raise MyExcept.UserNotFound('Sponsor: ' + username)
        for participator in parts:
            if not self.storage.queryUser(exist(participator)):
                raise MyExcept.UserNotFound('Participator:' + username)
            if parts.count(participator) > 1:
                raise MyExcept.UserRepeat(
                    'Participator: ' + username +
                    ' appears more than once in participators')
            self.storage.queryMeeting(overlap(participator))

        if username in parts:
            raise MyExcept.UserRepeat(
                'Sponsor: ' + username + ' found in participators')

        self.storage.queryMeeting(overlap(username))
        self.storage.createMeeting(Meeting(username, title, start, end, parts))
