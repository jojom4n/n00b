#ifndef PROTOS_H
#define PROTOS_H

#include <string>
#include "defs.h"

// from commands.cpp
void read_commands();
Options resolve_buffer(const std::string const& buffer);
void sys_info();

#endif