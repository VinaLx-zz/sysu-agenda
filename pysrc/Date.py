import MyExcept
import re


class Date(object):

    def __init__(self, year, month, day, hour, minute):
        self.__year = year
        self.__month = month
        self.__day = day
        self.__hour = hour
        self.__minute = minute

    @classmethod
    def stringToDate(cls, dateString):
        match = re.match(
            '^(\d{4})\-(\d{2})\-(\d{2})/(\d{2}):(\d{2})', dateString)
        if match is None:
            raise MyExcept.WrongFormat('Wrong Date Format')
        ret = cls(*map(int, match.groups()))
        if not cls.isValid(ret):
            raise MyExcept.InvalidDate(ret)
        return ret

    @classmethod
    def isValid(cls, date):
        if date.__hour < 0 or date.__hour >= 24:
            return False

        if date.__minute < 0 or date.__minute >= 60:
            return False

        if date.__year < 1000 or date.__year >= 10000:
            return False

        if date.__month <= 0 or date.__month > 12:
            return False

        return cls.checkDay(date.__year, date.__month, date.__day)

    @staticmethod
    def isLeap(year):
        if year % 400 == 0:
            return True
        if year % 100 == 0:
            return False
        if year % 4 == 0:
            return True
        return False

    @classmethod
    def checkDay(cls, year, month, day):
        if day <= 0:
            return False
        if cls.isLeap(year) and month == 2 and day == 29:
            return True

        dayOfMonth = (31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31)
        if (day > dayOfMonth[month - 1]):
            return False
        return True

    def __str__(self):
        return '%d-%02d-%02d/%02d:%02d' % (
            self.__year, self.__month, self.__day, self.__month, self.__minute)

    def __eq__(self, that):
        return self.__year == that.__year and self.__month == that.__month and\
            self.__day == that.__day and self.__hour == that.__hour and\
            self.__minute == that.__minute

    def __gt__(self, that):
        return (self.__year, self.__month, self.__day, self.__hour, self.__minute) \
            > (that.__year, that.__month, that.__day, that.__hour, that.__minute)

    def __lt__(self, that):
        return not self >= that

    def __ge__(self, that):
        return self > that or self == that

    def __le__(self, that):
        return not self > that
