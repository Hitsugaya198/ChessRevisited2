#-------------------------------------------------
#
# Project created by QtCreator 2016-04-15T10:06:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleChess
TEMPLATE = app

INCLUDEPATH+= ../../Source

SOURCES += \
           ../../Source/main.cpp \
           ../../Source/Chess.cpp \
           ../../Source/Board.cpp \
           ../../Source/Cell.cpp \
           ../../Source/Piece.cpp \
           ../../Source/StyleSheetProcessor.cpp \
           ../../Source/UserIdentity.cpp \
    ../../Source/MoveRules.cpp \
    ../../Source/Player.cpp \
    ../../Source/TurnManager.cpp \
    ../../Source/MoveGenerator.cpp \
    ../../Source/PixmapWidget.cpp \
    ../../Source/CapturedPieceWidget.cpp \
    ../../Source/AppFilter.cpp \
    ../../Source/MoveMapper.cpp

HEADERS  += \
    ../../Source/Chess.h \
    ../../Source/Board.h \
    ../../Source/Cell.h \
    ../../Source/Piece.h \
    ../../Source/Pieces.h \
    ../../Source/Colors.h \
    ../../Source/StyleSheetProcessor.h \
    ../../Source/UserIdentity.h \
    ../../Source/MoveRules.h \
    ../../Source/Player.h \
    ../../Source/TurnManager.h \
    ../../Source/MoveGenerator.h \
    ../../Source/PixmapWidget.h \
    ../../Source/CapturedPieceWidget.h \
    ../../Source/AppFilter.h \
    ../../Source/MoveMapper.h \
    ../../Source/CommonTypeDefinitions.h \
    ../../Source/Direction.h \
    ../../Source/Magnitude.h

FORMS    += ../../Source/Chess.ui \
            ../../Source/Board.ui \
    ../../Source/CapturedPieceWidget.ui

RESOURCES += \
    ../chess.qrc

DISTFILES += \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
