#ifndef SUPERBUBBLE_PERFORMANCE_DATABASE_H
#define SUPERBUBBLE_PERFORMANCE_DATABASE_H

#include "eadlib/logger/Logger.h"
#include "eadlib/wrapper/SQLite/SQLite.h"

namespace sbp {
    namespace io {
        class Database {
          public:
            Database( const std::string &file_name );
            ~Database();
            bool writePair( const size_t &index, const std::string &kmer );
          private:
            eadlib::wrapper::SQLite _database;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_DATABASE_H
