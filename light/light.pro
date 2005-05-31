TEMPLATE        = app
TARGET          = light

CONFIG          += warn_on release
#CONFIG         += warn_on debug
QMAKE_INCDIR  += ../common
LIBS         += -L../common -llight
SOURCES   = entities.c light.c ltface.c threads.c trace.c
