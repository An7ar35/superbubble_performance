/*!
    @class          eadlib::cli::Parser
    @brief          CLI parser system to process program arguments.

    The Parser uses regular expression to check argument values passed to it.
    Any value that fail its regular expression check during the parsing will trigger
    its associated error message and the parsing method with return false.
    If all values check out then true is returned and anything parsed can be queried.

    @note           If the regex is malformed an std::regex_error exception will be
                    raise on ````option(..)```` by the std::regex() constructor.
    @dependencies   eadlib::cli::ParserOption, eadlib::cli::ParserValue
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License

    @example        Parser_example.cpp
                    Example in parsing a simple username to the program from the cli argument list
*/

#ifndef EADLIB_CLI_PARSER_PARSER_H
#define EADLIB_CLI_PARSER_PARSER_H

#include <iostream>
#include <regex>
#include <unordered_map>
#include <list>
#include <vector>

#include "ParserOption.h"

namespace eadlib {
    namespace cli {
        class Parser {
          public:
            typedef std::vector<std::pair<bool, std::string>> OptionValues_t;
            struct ValueCheck {
                std::regex regex;
                std::string fail_msg;
                std::string default_value = "";
            };
            Parser();
            Parser( const std::string &program_heading );
            ~Parser();
            //Output stream & print
            friend std::ostream &operator <<( std::ostream &os, const Parser &option );
            std::ostream & printInfo( std::ostream &out ) const;
            //Option setup
            void option( const std::string &category,
                         const std::string &name,
                         const std::string &alternative,
                         const std::string &description,
                         const bool &required,
                         std::list<ValueCheck> value_regexs );
            //Information setup
            void addTitle( const std::string &title );
            void addDescriptionLine( const std::string &usage );
            void addExampleLine( const std::string &example );
            //Process arguments
            bool parse( const int &argc, char *argv[] );
            //Properties
            bool requirementFulfilled() const;
            bool optionUsed( const std::string &option_name ) const;
            OptionValues_t getValues( const std::string &option_name ) const;


            size_t size() const;
            bool isEmpty() const;

          private:
            typedef std::unordered_map<std::string, size_t> IndexMap_t;
            typedef std::unordered_map<std::string, std::list<size_t>> MultiIndexMap_t;
            std::vector<ParserOption>              _options;
            IndexMap_t                             _name_map;
            IndexMap_t                             _alt_map;
            MultiIndexMap_t                        _category_map;
            std::vector<MultiIndexMap_t::iterator> _category_order;
            std::string                            _program_title;
            std::vector<std::string>               _usage_description;
            std::vector<std::string>               _usage_examples;
        };

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // ParserOption class public method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         */
        inline eadlib::cli::Parser::Parser() :
            _program_title( "" )
        {}

        /**
         * Constructor
         * @param program_heading Program title
         */
        inline Parser::Parser( const std::string &program_heading ) :
            _program_title( program_heading )
        {}

        /**
         * Destructor
         */
        inline eadlib::cli::Parser::~Parser() {}

        /**
         * Output stream operator
         * @param os Output stream
         * @param parser Parser instance
         * @return Output stream
         */
        inline std::ostream & operator <<( std::ostream &os, const Parser &parser ) {
            return parser.printInfo( os );
        }

        /**
         * Prints the parser information
         * @param out Output stream
         * @return Output stream
         */
        inline std::ostream &Parser::printInfo( std::ostream &out ) const {
            if( !_program_title.empty() ) {
                out << _program_title << std::endl;
            }
            if( !_usage_description.empty() ) {
                out << "-|Description|-" << std::endl;
                for( auto desc : _usage_description ) {
                    out << desc << std::endl;
                }
                out << std::endl;
            }
            for( auto cat : _category_order ) {
                out << "-|" << cat->first << "|-" << std::endl;
                for( auto i : cat->second ) {
                    out << "  " << _options.at( i ) << std::endl;
                }
                out << std::endl;
            }
            if( !_usage_examples.empty() ) {
                out << "-|Example(s)|-" << std::endl;
                for( auto eg : _usage_examples ) {
                    out << eg << std::endl;
                }
                out << std::endl;
            }
        }

        /**
         * Adds option to the parser
         * @param category     Category of the option
         * @param name         Name of option (short version)
         * @param alternative  Alternative name of option (long version)
         * @param description  Description of the option
         * @param required     Required flag
         * @param value_regexs Sets of regex, error msg and default (if any) values for values expected
         */
        inline void Parser::option( const std::string &category,
                                    const std::string &name,
                                    const std::string &alternative,
                                    const std::string &description,
                                    const bool &required,
                                    std::list<Parser::ValueCheck> value_regexs ) {
            //creating/adding index entries for the option
            auto insert_result = _category_map.emplace( category, std::list<size_t>() );
            if( insert_result.second ) { //if category was added
                _category_order.emplace_back( insert_result.first );
            }
            _category_map.at( category ).emplace_back( _options.size() );
            _name_map.emplace( name, _options.size() );
            _alt_map.emplace( alternative, _options.size() );
            //creating the option and adding the expected values if any
            auto option = ParserOption( name, alternative, description, required );
            for( auto regex : value_regexs ) {
                if( regex.default_value.empty() ) {
                    option.addValueRegex( regex.regex, regex.fail_msg );
                } else {
                    option.addValueRegex( regex.regex, regex.fail_msg, regex.default_value );
                }
            }
            _options.emplace_back( option );
        }

        /**
         * Adds a title heading for the information of the parser
         * @param title Program title
         */
        inline void Parser::addTitle( const std::string &title ) {
            _program_title = title;
        }

        /**
         * Adds a description line for the information of the parser
         * @param usage Usage description line
         */
        inline void Parser::addDescriptionLine( const std::string &usage ) {
            _usage_description.emplace_back( usage );
        }

        /**
         * Adds an example line for the information of the parser
         * @param example Example of usage line
         */
        inline void Parser::addExampleLine( const std::string &example ) {
            _usage_examples.emplace_back( example );
        }

        /**
         * Parses the command line arguments
         * @param argc Number of arguments
         * @param argv Arguments
         * @return Success
         */
        inline bool Parser::parse( const int &argc, char **argv ) {
            if( argc < 2 ) {
                printInfo( std::cout );
                return false;
            } else {
                std::vector<std::string> arguments;
                for( int i = 1; i < argc; i++ ) {
                    arguments.emplace_back( argv[ i ] );
                }
                //Processing all args
                std::vector<std::string>::iterator it = arguments.begin();
                while( it != arguments.end() ) {
                    if( _name_map.find( *it ) != _name_map.end()) { //found as std short name
                        size_t index = _name_map.at( *it );
                        std::string name = *it;
                        size_t count { 0 };
                        _options.at( index ).setUsedFlag();
                        while( count < _options.at( index ).valuesExpectedCount() ) {
                            it++;
                            if( !_options.at( index ).setValue( count, *it ) ) {
                                std::cerr << "Value '" << *it << "' for Option '"
                                          << name << "' is not valid." << std::endl;
                                return false;
                            }
                            count++;
                        }
                    } else if( _alt_map.find( *it ) != _alt_map.end()) { //found as alternative long name
                        size_t index = _alt_map.at( *it );
                        std::string name = *it;
                        size_t count { 0 };
                        _options.at( index ).setUsedFlag();
                        while( count < _options.at( index ).valuesExpectedCount() ) {
                            it++;
                            if( !_options.at( index ).setValue( count, *it ) ) {
                                std::cerr << "Value '" << *it << "' for Option '"
                                          << name << "' is not valid." << std::endl;
                                return false;
                            }
                            count++;
                        }
                    } else { //Not a valid option
                        std::cerr << "Argument '" << *it << "' is not recognised." << std::endl;
                        return false;
                    }
                    it++;
                }
            }
            return true;
        }

        /**
         * Checks if all the required options where in the argument list
         * @return Fulfilled requirement state
         */
        inline bool Parser::requirementFulfilled() const {
            for( auto it = _options.begin(); it != _options.end(); ++it ) {
                if( it->isNeeded() && !it->isUsed() ) {
                    return false;
                }
            }
            return true;
        }

        /**
         * Checks if an option was used (good when no values are expected for option)
         * @return Usage state
         */
        inline bool Parser::optionUsed( const std::string &option_name ) const {
            if( _name_map.find( option_name ) != _name_map.end()) { //found as std short name
                size_t index = _name_map.at( option_name );
                return _options.at( index ).isUsed();
            } else if( _alt_map.find( option_name ) != _alt_map.end()) { //found as alternative long name
                size_t index = _alt_map.at( option_name );
                return _options.at( index ).isUsed();
            } else { //Not a valid option
                std::cerr << "Argument '" << option_name << "' is not recognised." << std::endl;
                return false;
            }
        }

        /**
         * Gets the values stored in the option
         * @param option_name Name of the option
         * @return Vector of pairs consisting of a set flag and the value
         */
        inline Parser::OptionValues_t Parser::getValues( const std::string &option_name ) const {
            if( _name_map.find( option_name ) != _name_map.end() ) { //found as std short name
                size_t index = _name_map.at( option_name );
                return _options.at( index ).getValues();
            } else if( _alt_map.find( option_name ) != _alt_map.end() ) { //found as alternative long name
                size_t index = _alt_map.at( option_name );
                return _options.at( index ).getValues();
            } else {
                std::cerr << "Option '" << option_name << "' does not exist." << std::endl;
                throw std::out_of_range( "[eadlib::cli::Parser::getValues( std::string )] "
                                             "Name of option given does not exist." );
            }
        }

        /**
         * Gets the number of options in the parser
         * @return Number of options in parser
         */
        inline size_t Parser::size() const {
            return _options.size();
        }

        /**
         * Checks if there are options in parser
         * @return Empty state of parser
         */
        inline bool Parser::isEmpty() const {
            return _options.empty();
        }
    }
}

#endif //EADLIB_CLI_PARSER_PARSER_H
