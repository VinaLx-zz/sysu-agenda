from AgendaService import AgendaService
import MyExcept
from sys import stderr
from sys import exit
import re


class AgendaUI(object):

    def __init__(self):
        self.startAgenda()
        self.__username = None
        self.__password = None

    def isLoggedIn(self):
        return self.__username is not None

    def startAgenda(self):
        self.service = AgendaService()

    def quitAgenda(self):
        self.service.quitAgenda()
        print('[BYE]')
        exit(0)

    def getOperation(self):
        if self.isLoggedIn():
            op = input('Agenda@' + self.__username + ':# ')
        else:
            op = input('Agenda:$ ')
        return op.strip()

    def checkLoginState(self, command, required):
        if self.isLoggedIn() != required:
            msg = required and 'in' or 'out'
            raise MyExcept.UnknownCommand(
                command + ' required a log ' + msg + ' state')

    def executeOperation(self, command):
        if command.startswith('//'):
            return
        if command == 'l':
            self.checkLoginState(command, False)
            self.userLogin()
        elif command == 'r':
            self.checkLoginState(command, False)
            self.userRegister()
        elif command == 'q':
            self.quitAgenda()
        elif command == 'o':
            self.checkLoginState(command, True)
            self.userLogOut()
        elif command == 'dc':
            self.checkLoginState(command, True)
            self.deleteUser()
        elif command == 'lu':
            self.checkLoginState(command, True)
            self.listAllUsers()
        elif command == 'cm':
            self.checkLoginState(command, True)
            self.createMeeting()
        elif command == 'la':
            self.checkLoginState(command, True)
            self.listAllMeetings()
        elif command == 'las':
            self.checkLoginState(command, True)
            self.listAllSponsorMeetings()
        elif command == 'lap':
            self.checkLoginState(command, True)
            self.listAllparticipateMeetings()
        elif command == 'qm':
            self.checkLoginState(command, True)
            self.queryMeetingByTitle()
        elif command == 'qt':
            self.checkLoginState(command, True)
            self.queryMeetingByTime()
        elif command == 'dm':
            self.checkLoginState(command, True)
            self.deleteMeetingByTitle()
        elif command == 'da':
            self.checkLoginState(command, True)
            self.deleteAllMeetings()
        elif command == 'h':
            self.printManual()
        else:
            raise MyExcept.UnknownCommand(command)

    def operationLoop(self):
        while True:
            command = self.getOperation()
            try:
                self.executeOperation(command)
            except MyExcept.UnknownCommand as uc:
                print('[%s] %s (press h for help)' %
                      (uc.type(), uc), file=stderr)
            except MyExcept.MyException as e:
                print('[Error] [%s] %s' % (e.type(), e), file=stderr)

    @staticmethod
    def format(*args):
        def trans(s):
            return '[%s]' % s
        return ' '.join(map(trans, args))

    @staticmethod
    def getInput(argNum, prompt):
        argLeft = argNum
        result = []
        while argLeft:
            line = input(prompt + ' ')
            rcvArg = list(filter(None, line.split(' ')))
            if len(rcvArg) > argLeft:
                raise MyExcept.WrongArgNum('%d args left, while receive %d'
                                           % (argLeft, len(rcvArg)))
            result += rcvArg
            argLeft -= len(rcvArg)
        return result

    def userLogin(self):
        prompt = '[LOGIN]'
        print(prompt, self.format('USERNAME', 'PASSWORD'))
        result = self.getInput(2, prompt)
        self.service.userLogin(result[0], result[1])
        self.__username = result[0]
        self.__password = result[1]
        print(prompt, 'Login Success!')

    def userRegister(self):
        prompt = '[REGISTER]'
        print(prompt, self.format('USERNAME', 'PASSWORD', 'EMAIL', 'PHONE'))
        result = self.getInput(4, prompt)
        self.service.userRegister(result[0], result[1], result[2], result[3])
        print(prompt, 'Success!')

    def userLogOut(self):
        self.__username = None
        self.__password = None
        print('[LOGOUT] Success!')

    def deleteUser(self):
        self.service.deleteUser(self.__username, self.__password)
        self.__username = None
        self.__password = None
        print('[DELETE USER] Success!')

    def listAllUsers(self):
        print('[LIST ALL USERS]\n')
        users = list(self.service.listAllUsers())
        usernameLen = 10
        emailLen = 7
        phoneLen = 7
        for user in users:
            if len(user.getName()) > usernameLen:
                usernameLen = len(user.getName())
            if len(user.getEmail()) > emailLen:
                emailLen = len(user.getEmail())
            if len(user.getPhone()) > phoneLen:
                phoneLen = len(user.getPhone())
        result = '{0:<{uw}} {1:<{ew}} {2}\n'.format(
            '[USERNAME]', '[EMAIL]', '[PHONE]', uw=usernameLen, ew=emailLen)
        for user in users:
            result += '{0:<{uw}} {1:<{ew}} {2}\n'.format(
                user.getName(), user.getEmail(), user.getPhone(),
                uw=usernameLen, ew=emailLen)
        print(result)

    def createMeeting(self):
        prompt = '[CREAT MEETING]'
        partNumStr = input(prompt + ' How Many Participators?\n').strip()
        while not re.match('\d+', partNumStr):
            partNumStr = input('INTEGER PLEASE\n')
        partNum = int(partNumStr)
        parts = []
        print('input ONE participator each line')
        for num in range(1, partNum + 1):
            parts.append(input('%s [%d] ' % (prompt, num)).strip())
        print(prompt, self.format(
            'TITLE', 'START(YYYY-MM-DD/HH:MM)', 'END(YYYY-MM-DD/HH:MM)'))
        result = self.getInput(3, prompt)
        self.service.createMeeting(self.__username, result[
                                   0], result[1], result[2], parts)
        print(prompt, 'Success!')

    def listAllMeetings(self):
        print('[LIST ALL MEETINGS]\n')
        self.printMeetings(self.service.listAllMeetings(self.__username))

    def listAllSponsorMeetings(self):
        print('[LIST ALL MEETINGS YOU SPONSORED]\n')
        self.printMeetings(
            self.service.listAllSponsorMeetings(self.__username))

    def listAllparticipateMeetings(self):
        print('[LIST ALL MEETINGS YOU PARTICIPATED]\n')
        self.printMeetings(
            self.service.listAllParticipateMeetings(self.__username))

    def queryMeetingByTitle(self):
        prompt = '[QUERY MEETING]'
        print(prompt, self.format('TITLE'))
        result = self.getInput(1, prompt)
        print(prompt)
        self.printMeetings(self.serviece.meetingQuery(
            self.__username, result[0]))

    def queryMeetingByTime(self):
        prompt = '[QUERY MEETING]'
        print(prompt, self.format(
            'START[YYYY-MM-DD/HH:MM]', 'END(YYYY-MM-DD/HH:MM)'))
        result = self.getInput(2, prompt)
        self.printMeetings(self.service.meetingQuery(
            self.__username, result[0], result[1]))

    def deleteMeetingByTitle(self):
        prompt = 'DELETE MEETING'
        print(prompt, self.format('TITLE'))
        result = self.getInput(1, prompt)
        self.service.deleteMeeting(self.__username, result[0])
        print(prompt, 'Success!')

    def deleteAllMeetings(self):
        self.service.deleteAllMeetings(self.__username)
        print('[DELETE ALL MEETINGS] Success!')

    @staticmethod
    def printMeetings(meetings):
        splitLine = '{:=^40}'.format('')
        meetingList = list(meetings)
        if not meetingList:
            print('NO MEETINGS')
            return
        print(splitLine)
        for meeting in meetingList:
            print('%-15s: %s' % ('TITLE', meeting.getTitle()))
            print('%-15s: %s' % ('SPONSOR', meeting.getSponsor()))
            print('%-15s: %s' % ('START', meeting.getStartTime()))
            print('%-15s: %s' % ('END', meeting.getEndTime()))
            print('%-15s: %s' % ('PARTICIPATORS',
                                 ', '.join(meeting.getParticipators())))
            print(splitLine)
