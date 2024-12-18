//
// Created by User on 14/12/2024.
//
#include "Error/Error.h"

using namespace ArgonLang;

Error::Error(std::string errorMsg, Type type): errorMsg(std::move(errorMsg)), errorType(type) {}