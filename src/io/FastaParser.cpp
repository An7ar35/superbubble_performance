#include "FastaParser.h"

/**
 * Constructor
 * @param reader EADlib File Reader
 */
sbp::io::FastaParser::FastaParser( eadlib::io::FileReader &reader ) :
    _reader( reader )
{}

/**
 * Destructor
 */
sbp::io::FastaParser::~FastaParser() {}

/**
 * Parse FASTA file
 * @return Finish state
 */
sbp::io::FastaParserState sbp::io::FastaParser::parse( std::vector<char> &buffer ) {
    if( !_reader.isOpen() && !_reader.open() ) {
        LOG_ERROR( "[sbp::io::FastaParser::parse(..)] Could not open file '", _reader.getFileName(), "'." );
        return FastaParserState::FILE_ERROR;
    }
    if( _reader.isDone() ) {
        return FastaParserState::EOF_REACHED;
    }

    do {
        std::streamsize size { 0 };
        PeekResult r = peek();
        switch( r ) {
            case PeekResult::DESCRIPTION:
                _reader.readLine( buffer );
                return FastaParserState::DESC_PARSED;
            case PeekResult::READ:
                size = _reader.readLine( buffer );
                while( size > 0 && peek() == PeekResult::READ ) {
                    std::vector<char> temp;
                    size = _reader.readLine( temp );
                    buffer.insert( buffer.end(), temp.begin(), temp.end() );
                }
                return FastaParserState::READ_PARSED;
            case PeekResult::NEW_LINE:
                _reader.readLine( buffer );
                break;
            case PeekResult::EMPTY:
                _reader.readLine( buffer );
                break;
            case PeekResult::ERROR:
                LOG_ERROR( "[sbp::io::FastaParser::parse(..)] Could not peek. Last valid file position = ", _reader.getPosition() );
                return FastaParserState::PARSER_ERROR;
        }
    } while( !_reader.isDone() );

    return FastaParserState::EOF_REACHED;
}

/**
 * Checks the next character in the file
 * @return Type expected
 */
sbp::io::FastaParser::PeekResult sbp::io::FastaParser::peek() {
    std::vector<char> peek_buffer;
    std::streamsize peek_size = _reader.peek( peek_buffer, 1 );
    if( peek_size < 0 )
        return PeekResult::ERROR;
    if( peek_size == 0 )
        return PeekResult::EMPTY;
    if( peek_buffer.front() == '>' )
        return PeekResult::DESCRIPTION;
    if( peek_buffer.front() == '\n' )
        return PeekResult::NEW_LINE;
    return PeekResult::READ;
}