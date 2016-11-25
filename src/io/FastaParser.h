#ifndef SUPERBUBBLE_PERFORMANCE_FASTAPARSER_H
#define SUPERBUBBLE_PERFORMANCE_FASTAPARSER_H

#include <string>
#include "eadlib/io/FileReader.h"

namespace sb {
    namespace io {
        /**
         * @brief   Return states enum for the sb::io::FastaParser
         */
        enum class FastaParserState {
            FILE_ERROR,
            PARSER_ERROR,
            SUCCESSFUL,
        };

        /**
         * @brief   FASTA file parser
         */
        class FastaParser {
          public:
            FastaParser();
            ~FastaParser();
            FastaParserState parse( const std::string &file_name );
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_FASTAPARSER_H
