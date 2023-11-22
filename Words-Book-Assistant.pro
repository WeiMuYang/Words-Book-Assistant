QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about_dialog.cpp \
    file_operation.cpp \
    main.cpp \
    mainwindow.cpp \
    network_access.cpp \
    open_ex_program.cpp \
    win_event_filter.cpp

HEADERS += \
    about_dialog.h \
    data_type.h \
    file_operation.h \
    mainwindow.h \
    network_access.h \
    open_ex_program.h \
    win_event_filter.h

FORMS += \
    about_dialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
