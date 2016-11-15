import MyExcept


class Meeting(object):

    def __init__(self, sponsor, title, startTime, endTime, participators):
        if startTime >= endTime:
            raise MyExcept.InvalidDate('Start later than End')
        self.__sponsor = sponsor
        self.__title = title
        self.__startTime = startTime
        self.__endTime = endTime
        self.__participator = set(participators)

    def isParticipator(self, participator):
        return participator in self.__participator

    def getSponsor(self):
        return self.__sponsor

    def getTitle(self):
        return self.__title

    def getStartTime(self):
        return self.__startTime

    def getEndTime(self):
        return self.__endTime

    def getParticipators(self):
        return self.__participator.copy()

    def isOne(self, username):
        return self.isParticipator(username) or self.getSponsor() == username
