// Copyright (c) 2012-2014 Konstantin Isakov <ikm@zbackup.org> and ZBackup contributors, see CONTRIBUTORS
// Part of ZBackup. Licensed under GNU GPLv2 or later + OpenSSL, see LICENSE

#include <string>
#ifndef ZBACKUP_VERSION
#error "ZBACKUP_VERSION was not defined"
#else
std::string zbackup_version( ZBACKUP_VERSION );
#endif
