TEMPLATE        = lib
TARGET          = light

CONFIG          += warn_on release
CONFIG 	-= qt
#CONFIG         += warn_on debug
QMAKE_INCDIR  += ../common

SOURCES   = bspfile.c cmdlib.c lbmlib.c mathlib.c polylib.c scriplib.c threads.c trilib.c
# wadlib.c
