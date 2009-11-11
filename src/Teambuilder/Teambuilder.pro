QT += network
TARGET = Teambuilder
DESTDIR = ../../bin
TEMPLATE = app
SOURCES += main.cpp \
    teambuilder.cpp \
    advanced.cpp \
    menu.cpp \
    mainwindow.cpp \
    ../PokemonInfo/pokemoninfo.cpp \
    ../PokemonInfo/pokemonstructs.cpp \
    ../Utilities/otherwidgets.cpp \
    network.cpp \
    ../Utilities/dockinterface.cpp \
    client.cpp \
    analyze.cpp \
    serverchoice.cpp \
    ../PokemonInfo/battlestructs.cpp \
    ../PokemonInfo/networkstructs.cpp \
    challenge.cpp
HEADERS += teambuilder.h \
    ../PokemonInfo/pokemoninfo.h \
    advanced.h \
    menu.h \
    mainwindow.h \
    ../PokemonInfo/pokemonstructs.h \
    ../Utilities/otherwidgets.h \
    network.h \
    ../Utilities/dockinterface.h \
    client.h \
    analyze.h \
    serverchoice.h \
    ../PokemonInfo/battlestructs.h \
    ../PokemonInfo/networkstructs.h \
    challenge.h
LIBS += -L../../bin \
    -lzip
