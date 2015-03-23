APP_NAME = bbgramHeadless

QT = core network sql

CONFIG += qt warn_on cascades10

LIBS += -lbbsystem -lbb -lbbnetwork -lbbplatform -lunifieddatasourcec -lbbpim -lbbdata

include(config.pri)
