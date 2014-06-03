#-------------------------------------------------
#
# Project created by QtCreator 2014-05-26T12:02:50
#
#-------------------------------------------------

QT		+= core network

CONFIG	+= console

DESTDIR = $$PWD/../Bin

win32:{
RUNTIME_PATH = $$[QT_INSTALL_DATA]/bin
runtime.files +=						\
	$$RUNTIME_PATH/QtCore4.dll			\
	$$RUNTIME_PATH/QtGui4.dll			\
	$$RUNTIME_PATH/QtNetwork4.dll
runtime.path += $$DESTDIR
INSTALLS += runtime
}


SRC		= $$PWD
DATA	= $$SRC/Data
LOGIC	= $$SRC/Logic

MOC_DIR = $$PWD/../Build
OBJECTS_DIR = $$MOC_DIR

TARGET = UnitSyncServer

INCLUDEPATH = $$PWD

HEADERS +=									\
#	$$SRC/*.h								\
	$$DATA/*.h								\
	$$LOGIC/*.h								\
	$$LOGIC/UnitSyncLib/*.h					\
	$$LOGIC/Processors/*.h					\



SOURCES +=									\
	$$SRC/*.cpp								\
	$$LOGIC/*.cpp							\
	$$LOGIC/UnitSyncLib/*.cpp				\
	$$LOGIC/Processors/*.cpp				\
