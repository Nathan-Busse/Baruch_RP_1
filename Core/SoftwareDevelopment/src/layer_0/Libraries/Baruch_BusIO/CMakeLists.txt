# Baruch Bus IO Library

cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "Baruch_I2CDevice.cpp" "Baruch_BusIO_Register.cpp" "Baruch_SPIDevice.cpp" "Baruch_GenericDevice.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES arduino-esp32)

project(Baruch_BusIO)
