#ifndef SUPERBUBBLE_PERFORMANCE_CLIOPTIONS_H
#define SUPERBUBBLE_PERFORMANCE_CLIOPTIONS_H

#include <eadlib/cli/parser/Parser.h>

namespace sbp {
    namespace cli {
        class CliOptions {
          public:
            CliOptions( eadlib::cli::Parser &parser, char **argv );
            bool parse( const int &argc, char **argv );




          private:
            void init( char *argv[] );
            eadlib::cli::Parser &_parser;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_CLIOPTIONS_H
