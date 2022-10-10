#-------------------------------------------------
#
# Project created by QtCreator 2018-01-28T13:48:23
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Clinic_Serwer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    c_actionexecutive.cpp \
    c_logscontroller.cpp \
    c_myfiles.cpp \
    c_myparser.cpp \
    c_sessioncontroller.cpp \
    c_user.cpp \
        main.cpp \
    w_logswindow.cpp \
        w_mainwindow.cpp \
    c_clinictcpserver.cpp \
    w_serverconfigurationdialog.cpp \
    i_observer.cpp \
    i_watched.cpp \
    c_settingscontroller.cpp \
    w_initializedialog.cpp \
    w_interfaceinformationswidget.cpp \
    w_serverinformationswidget.cpp \
    w_databaseinformationswidget.cpp \
    c_mysqldatabasecontroller.cpp \
    w_windowconfigurationdialog.cpp \
    w_runserverdialog.cpp \
    w_databaseconnectionwidget.cpp \
    w_databaseconnectionsconfigurationdialog.cpp \
    c_myconnection.cpp \
    c_mydatabaseconnection.cpp \
    c_clientconnection.cpp \
    w_hostslistdialog.cpp \
    w_hostslistwindow.cpp

HEADERS += \
    c_actionexecutive.h \
    c_logscontroller.h \
    c_myfiles.h \
    c_myparser.h \
    c_sessioncontroller.h \
    c_structures.h \
    c_user.h \
    mcs_DbQueriesSessionController.h \
    mcs_DbQueriesUser.h \
    w_logswindow.h \
        w_mainwindow.h \
    c_clinictcpserver.h \
    w_serverconfigurationdialog.h \
    i_observer.h \
    i_watched.h \
    c_settingscontroller.h \
    w_initializedialog.h \
    m_defaultinifile.h \
    w_interfaceinformationswidget.h \
    w_serverinformationswidget.h \
    w_databaseinformationswidget.h \
    c_mysqldatabasecontroller.h \
    w_windowconfigurationdialog.h \
    w_runserverdialog.h \
    w_databaseconnectionwidget.h \
    w_databaseconnectionsconfigurationdialog.h \
    m_databasedrivers.h \
    c_myconnection.h \
    c_mydatabaseconnection.h \
    c_clientconnection.h \
    w_hostslistdialog.h \
    w_hostslistwindow.h

FORMS += \
    w_logswindow.ui \
        w_mainwindow.ui \
    w_serverconfigurationdialog.ui \
    w_initializedialog.ui \
    w_interfaceinformationswidget.ui \
    w_serverinformationswidget.ui \
    w_databaseinformationswidget.ui \
    w_windowconfigurationdialog.ui \
    w_runserverdialog.ui \
    w_databaseconnectionwidget.ui \
    w_databaseconnectionsconfigurationdialog.ui \
    w_hostslistdialog.ui \
    w_hostslistwindow.ui

DISTFILES += \
    styles.css \
    config.ini

RESOURCES += \
    icons.qrc \
    gifs.qrc

RC_ICONS += Clinic_Serwer.ico
