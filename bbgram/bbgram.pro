APP_NAME = bbgram

QT = core network sql

CONFIG += qt warn_on cascades10
LIBS += -lbbdata -lbbsystem -lnbutil -lssl -lcrypto -lz -lbb -lbbmultimedia

include(config.pri)
