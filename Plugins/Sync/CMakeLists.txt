 #plugin build file
cmake_minimum_required(VERSION 3.5.0)

#include common rules
include(../PluginRules.cmake)

#add sources, not including OpenEphysLib.cpp
add_sources(${PLUGIN_NAME}
    SyncNode.cpp
    SyncNode.h
    SyncEditor.cpp
    SyncEditor.h
)
