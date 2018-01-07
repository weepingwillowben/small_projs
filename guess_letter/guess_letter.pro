TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

DISTFILES += \
    huck_fin.txt \
    README.md

HEADERS += \
    read_file.h \
    state_map.h \
    sparse_mat_mul.h \
    physical_engine.h \
    utils.h \ 
    physics/full_physics.h \
    physics/letters.h \
    physics/state_manip.h \
    cost_fn.h \
    physics/rand_input.h \
    coststabilzer.h
