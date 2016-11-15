class MyException(Exception):

    def __init__(self, message):
        super(MyException, self).__init__(message)

    @staticmethod
    def type():
        return 'MyException'


class WrongArgNum(MyException):

    def __init__(self, message):
        super(WrongArgNum, self).__init__(message)

    @staticmethod
    def type():
        return 'Wrong Argument Number'


class InvalidDate(MyException):

    def __init__(self, message):
        super(InvalidDate, self).__init__(message)

    @staticmethod
    def type():
        return 'Invalid Date'


class UserRepeat(MyException):

    def __init__(self, message):
        super(UserRepeat, self).__init__(message)

    @staticmethod
    def type():
        return 'Username Repeat'


class UserNotFound(MyException):

    def __init__(self, message):
        super(UserNotFound, self).__init__(message)

    @staticmethod
    def type():
        return 'Username Not Found'


class TitleRepeat(MyException):

    def __init__(self, message):
        super(TitleRepeat, self).__init__(message)

    @staticmethod
    def type():
        return 'Title Repeat'


class EmptyDeletion(MyException):

    def __init__(self, message):
        super(EmptyDeletion, self).__init__(message)

    @staticmethod
    def type():
        return 'Empty Deletion'


class TimeConflict(MyException):

    def __init__(self, message):
        super(TimeConflict, self).__init__(message)

    @staticmethod
    def type():
        return 'Time Confliction'


class WrongFormat(MyException):

    def __init__(self, message):
        super(WrongFormat, self).__init__(message)

    @staticmethod
    def type():
        return 'Wrong Format'


class UnknownCommand(MyException):

    def __init__(self, message):
        super(UnknownCommand, self).__init__(message)

    @staticmethod
    def type():
        return 'Unknown Command'
