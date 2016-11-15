from Meeting import Meeting
from User import User
from Date import Date
import Path
import re
import MyExcept


def count(aSet):
    def dec(func):
        def wrapper(self, *args):
            before = len(aSet)
            func(self, *args)
            after = len(aSet)
            if before - after:
                self.dirty = True
            return after - before
        return wrapper
    return dec


class Storage(object):

    class __Storage:

        def __init__(self):
            self.users = set()
            self.meetings = set()
            self.dirty = False
            self.readUsers()
            self.readMeetings()

        def createUser(self, user):
            self.users.add(user)
            self.dirty = True

        def queryUser(self, pred):
            return filter(pred, self.users)

        @count
        def deleteUser(self, pred):
            for user in self.users:
                if pred(user):
                    self.users.remove(user)

        def createMeeting(self, meeting):
            self.meetings.add(meeting)
            self.dirty = True

        def queryMeeting(self, pred):
            return filter(pred, self.meetings)

        @count
        def deleteMeeting(self, pred):
            for meeting in self.meetings:
                if pred(meeting):
                    self.meeting.remove(meeting)

        def readUsers(self):
            userfile = open(Path.userPath, 'r')
            for line in userfile:
                items = re.findall('"(.+?)"', line)
                if len(items) != 4:
                    raise MyExcept.WrongFormat('Wrong File Format of ' + line)
                self.users.add(
                    User(items[0], items[1], items[2], items[3]))
            return True

        def readMeetings(self):
            meetingfile = open(Path.meetingPath, 'r')
            for line in meetingfile:
                items = re.findall('"(.+?)"', line)
                if len(items) != 5:
                    raise MyExcept.WrongFormat('Wrong File Format of ' + line)
                participators = items[1].split('&')
                self.meetings.add(
                    Meeting(items[0], items[4], Date.stringToDate(items[2]),
                            Date.stringToDate(items[3]), participators))

        def writeUser(self):
            userfile = open(Path.userPath, 'w')
            for user in self.users:
                userfile.write('"%s","%s","%s","%s"\n' % (
                    user.getName(), user.getPassword(),
                    user.getEmail(), user.getPhone()))

        def writeMeeting(self):
            meetingfile = open(Path.meetingPath, 'w')
            for meeting in self.meetings:
                meetingfile.write('"%s","%s","%s","%s","%s"' % (
                                  meeting.getSponsor(),
                                  '&'.join(meeting.getParticipators()),
                                  meeting.getStartTime(),
                                  meeting.getEndTime(), meeting.getTitle()))

        def closeStorage(self):
            if self.dirty:
                self.writeMeeting()
                self.writeUser()

    instance = None

    @classmethod
    def getInstance(cls):
        if Storage.instance is None:
            Storage.instance = Storage.__Storage()
        return Storage.instance

    def __getattr__(self, name):
        if name == 'getInstance':
            return self.__dict__['name']
        return getattr(self.instance, name)
