#-------------------------------------------------
#
# Project created by QtCreator 2016-04-15T10:06:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChessRevisited2
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
    ../../Source/CapturedPieceWidget.cpp

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
    ../../Source/CapturedPieceWidget.h

FORMS    += ../../Source/Chess.ui \
            ../../Source/Board.ui \
    ../../Source/CapturedPieceWidget.ui

RESOURCES += \
    ../chess.qrc
