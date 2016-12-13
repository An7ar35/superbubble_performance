#ifndef SUPERBUBBLE_PERFORMANCE_DATABASE_H
#define SUPERBUBBLE_PERFORMANCE_DATABASE_H

#include <eadlib/datastructure/WeightedGraph.h>
#include "eadlib/logger/Logger.h"
#include "eadlib/wrapper/SQLite/SQLite.h"

namespace sbp {
    namespace io {
        class Database {
          public:
            Database();
            Database( const Database &db ) = delete;
            Database( Database &&db ) noexcept;
            ~Database();
            Database & operator =( const Database &rhs ) = delete;
            Database & operator =( Database &&rhs ) noexcept;
            bool open( const std::string &file_name );
            bool close();
            bool isOpen() const;
            bool create( const std::string &graph_name );
            bool remove( const std::string &graph_name );
            bool exists( const std::string &graph_name );
            std::string getID( const std::string &graph_name );
            bool writeNode( const std::string &graph_id,
                            const size_t &index,
                            const std::string &kmer );
            bool writeEdge( const std::string &graph_id, const size_t &from, const size_t &to, const size_t &weight );
            void beginTransaction();
            void commitTransaction();
            void rollbackTransaction();
            bool loadGraph( const std::string &graph_name, eadlib::WeightedGraph<size_t> &graph );
            bool loadGraph( const std::string &graph_name, eadlib::WeightedGraph<std::string> &graph );
          private:
            signed long long getGraphID( const std::string &graph_name );
            eadlib::wrapper::SQLite _database;
            std::string _graph_name;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_DATABASE_H
