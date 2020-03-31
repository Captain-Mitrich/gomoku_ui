QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    glsdial.cpp \
    gmodel.cpp \
    gridview.cpp \
    gslider.cpp \
    gview.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    celldata.h \
    gcelldata.h \
    glsdial.h \
    gmodel.h \
    gridview.h \
    gslider.h \
    gview.h \
    mainwindow.h

TRANSLATIONS += \
    gomoku_ru.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    gomoku.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../gomoku_ai/.release/ -lgomoku_ai
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../gomoku_ai/.debug/ -lgomoku_ai
else:unix: LIBS += -L$$PWD/../gomoku_ai/.build/ -lgomoku_ai

INCLUDEPATH += $$PWD/../gomoku_ai/include
DEPENDPATH += $$PWD/../gomoku_ai/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../gomoku_ai/.release/libgomoku_ai.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../gomoku_ai/.debug/libgomoku_ai.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../gomoku_ai/.release/gomoku_ai.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../gomoku_ai/.debug/gomoku_ai.lib
else:unix: PRE_TARGETDEPS += $$PWD/../gomoku_ai/.build/libgomoku_ai.a

CONFIG(debug, debug|release):DEFINES += _DEBUG
CONFIG(release, debug|release):DEFINES += NDEBUG
