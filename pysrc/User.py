class User(object):

    def __init__(self, username, password, email, phone):
        self.__username = username
        self.__password = password
        self.__email = email
        self.__phone = phone

    def getName(self):
        return self.__username

    def getPassword(self):
        return self.__password

    def getEmail(self):
        return self.__email

    def getPhone(self):
        return self.__phone

    def setName(self, username):
        self.__username = username

    def setPassword(self, password):
        self.__password = password

    def setEmail(self, email):
        self.__email = email
