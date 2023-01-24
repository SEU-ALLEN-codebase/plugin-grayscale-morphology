TEMPLATE	= lib
CONFIG	+= qt plugin warn_off
QT	+= widgets
VAA3DPATH = D:\Vaa3D\v3d_external_QT6
INCLUDEPATH	+= $$VAA3DPATH/v3d_main/basic_c_fun
INCLUDEPATH += ../3rdparty/gcem/include
INCLUDEPATH += ../3rdparty/stats/include

QMAKE_CXXFLAGS_RELEASE += -O3       # Release -O3

HEADERS	+= my_image.h argparser.h \
    abstract_actions.h \
    gaussian_filter.h \
    gsdt.h \
    handlers.h \
    hist_eq.h \
    imPreProcess/export.h \
    plugin.h \
    imPreProcess/bilateral_filter.h \
    imPreProcess/export.h \
    imPreProcess/fun_fft.h \
    imPreProcess/helpFunc.h \
    imPreProcess/morphology.h \
    plugin_actions.h \
    thresholding.h \
    utils.h

SOURCES	+= my_image.cpp argparser.cpp \
    $$VAA3DPATH/v3d_main/basic_c_fun/v3d_message.cpp \
    handlers.cpp \
    plugin.cpp \
    imPreProcess/fun_fft.cpp \
    imPreProcess/helpFunc.cpp \
    plugin_actions.cpp

TARGET	= $$qtLibraryTarget(neuron_image_rectifier)
DESTDIR	= D:/Vaa3D/Vaa3D-x.1.1.2_Windows_64bit/plugins/neuron_image_signal_processing/
