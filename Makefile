target = ${BIN_PATH}/Agenda
BASIC_HEADERS = ${HEADER_PATH}/Date.hpp ${HEADER_PATH}/User.hpp \
               ${HEADER_PATH}/Meeting.hpp ${HEADER_PATH}/Exception.hpp \
			   ${HEADER_PATH}/Path.hpp

HIGH_HEADERS = ${HEADER_PATH}/Storage.hpp ${HEADER_PATH}/AgendaService.hpp \
               ${HEADER_PATH}/AgendaUI.hpp

CXX = clang++
CPPFLAG = -I ${HEADER_PATH} -g
CXXFLAG = -std=c++14 -Wall -Wextra

BIN_PATH = bin/
HEADER_PATH = include/
BUILD_PATH = build/
SRC_PATH = src/

OBJECTS = ${BUILD_PATH}/Agenda.o ${BUILD_PATH}/AgendaUI.o \
          ${BUILD_PATH}/AgendaService.o ${BUILD_PATH}/Storage.o \
          ${BUILD_PATH}/Date.o ${BUILD_PATH}/User.o ${BUILD_PATH}/Meeting.o\
		  ${BUILD_PATH}/Path.o ${BUILD_PATH}/Logger.o

all: ${target}

clean:
	rm -f bin/* build/*

${target}: ${OBJECTS}
	${CXX} $^ -o $@ ${CXXFLAG}

${BUILD_PATH}/Agenda.o: ${SRC_PATH}/Agenda.cpp ${BASIC_HEADERS} ${HIGH_HEADERS} ${HEADER_PATH}/Logger.hpp
	${CXX} $< -c -o $@ ${CPPFLAG} ${CXXFLAG}

${BUILD_PATH}/AgendaUI.o: ${SRC_PATH}/AgendaUI.cpp ${BASIC_HEADERS} ${HIGH_HEADERS}
	${CXX} $< -c -o $@ ${CPPFLAG} ${CXXFLAG}

${BUILD_PATH}/Logger.o: ${SRC_PATH}/Logger.cpp ${HEADER_PATH}/Logger.hpp ${HEADER_PATH}/Path.hpp
	${CXX} $< -c -o $@ ${CPPFLAG} ${CXXFLAG}

${BUILD_PATH}/AgendaService.o: ${SRC_PATH}/AgendaService.cpp ${HEADER_PATH}/AgendaService.hpp \
                              ${HEADER_PATH}/Storage.hpp ${BASIC_HEADERS}
	${CXX} $< -c -o $@ ${CPPFLAG} ${CXXFLAG}

${BUILD_PATH}/Storage.o: ${SRC_PATH}/Storage.cpp ${HEADER_PATH}/Storage.hpp \
                         ${BASIC_HEADERS}
	${CXX} $< -c -o $@ ${CPPFLAG} ${CXXFLAG}

${BUILD_PATH}/Date.o: ${SRC_PATH}/Date.cpp ${HEADER_PATH}/Date.hpp ${HEADER_PATH}/Exception.hpp
	${CXX} $< -c -o $@ ${CPPFLAG} ${CXXFLAG}

${BUILD_PATH}/User.o: ${SRC_PATH}/User.cpp ${HEADER_PATH}/User.hpp
	${CXX} $< -c -o $@ ${CPPFLAG} ${CXXFLAG}

${BUILD_PATH}/Meeting.o: ${SRC_PATH}/Meeting.cpp ${HEADER_PATH}/Meeting.hpp
	${CXX} $< -c -o $@ ${CPPFLAG} ${CXXFLAG}

${BUILD_PATH}/Path.o: ${SRC_PATH}/Path.cpp ${HEADER_PATH}/Path.hpp
	${CXX} $< -c -o $@ ${CPPFLAG} ${CXXFLAG}
