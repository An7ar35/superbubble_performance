/**
    @class          eadlib::cli::ParserOption
    @brief          Option container for eadlib::cli::Parser
    @dependencies   eadlib::cli::ParserValue
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef EADLIB_CLI_PARSER_OPTION_H
#define EADLIB_CLI_PARSER_OPTION_H

#include <string>
#include <regex>
#include <vector>
#include <iostream>

#include "ParserValue.h"

namespace eadlib {
    namespace cli {
        class ParserOption {
          public:
            ParserOption( const std::string &name,
                          const std::string &alt,
                          const std::string &description,
                          const bool &required = false );
            ~ParserOption();
            //Output stream
            friend std::ostream &operator <<( std::ostream &os, const ParserOption &option );
            //Setup
            void addValueRegex( const std::regex  &regex,
                                const std::string &fail_msg );
            void addValueRegex( const std::regex  &regex,
                                const std::string &fail_msg,
                                const std::string &default_value );
            //Fill option with values
            void setUsedFlag();
            bool setValue( const size_t &index, const std::string &value );
            //Properties
            bool isNeeded() const;
            bool isUsed() const;
            size_t valuesSetCount() const;
            size_t valuesExpectedCount() const;
            //Get Values
            std::vector<std::pair<bool, std::string>> getValues() const;
          private:
            bool _used_flag;
            bool _needed_flag;
            bool _has_defaults;
            std::string _name;
            std::string _alt;
            std::string _description;
            std::vector<ParserValue> _values;
            size_t _values_set;
        };

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // ParserOption class public method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         * @param name        Option name (short version)
         * @param alt         Option name (long version)
         * @param description Option description
         * @param required    Required flag (default = false)
         */
        inline ParserOption::ParserOption( const std::string &name,
                                    const std::string &alt,
                                    const std::string &description,
                                    const bool &required ) :
            _used_flag( false ),
            _needed_flag( required ),
            _has_defaults( false ),
            _name( name ),
            _alt( alt ),
            _description( description ),
            _values_set( 0 )
        {}

        /**
         * Destructor
         */
        inline ParserOption::~ParserOption() {}

        /**
         * Output stream operator
         * @param os     Output stream
         * @param option ParserOption instance
         * @return Output stream
         */
        inline std::ostream & operator <<( std::ostream &os, const ParserOption &option ) {
            os << option._name
               << "\t" << option._alt
               << "\t" << option._description;
            if( option._needed_flag ) {
                os << "\t\t\t[REQUIRED]";
            }
            if( option._has_defaults ) {
                os << "\t[DEFAULT='";
                for( auto it = option._values.begin(); it != option._values.end(); ++it ) {
                    if( it->hasDefault() ) {
                        os << it->getDefault() << "'";
                    }
                    if( std::next( it ) != option._values.end() ) {
                        os << ", ";
                    }
                }
                os << "]";
            }
            return os;
        }

        /**
         * Adds a new value regex in order
         * @param regex    Regular expression for the value
         * @param fail_msg Error message for a regex fail
         */
        inline void ParserOption::addValueRegex( const std::regex &regex,
                                                 const std::string &fail_msg ) {
            _values.emplace_back( ParserValue( regex, fail_msg ) );
        }

        /**
         * Adds a new value regex in order
         * @param regex         Regular expression for the value
         * @param fail_msg      Error message for a regex fail
         * @param default_value Default value
         * @throws std::invalid_argument when default value given fails the regex
         */
        inline void ParserOption::addValueRegex( const std::regex  &regex,
                                                 const std::string &fail_msg,
                                                 const std::string &default_value ) {
            if( !std::regex_match( default_value, regex ) ) {
                std::cerr << "Invalid default value '" << default_value << "' passed for cli parser option '"
                          << _name << "/" << _alt << "' (regex fail)." << std::endl;
                throw std::invalid_argument( "Default value does not pass regex check given for option." );
            }
            _values_set++;
            _has_defaults = true;
            _values.emplace_back( ParserValue( regex, fail_msg, default_value ) );
        }

        /**
         * Raises the flag for the ParserOption being used
         */
        inline void ParserOption::setUsedFlag() {
            _used_flag = true;
        }

        /**
         * Sets an option's value
         * @param index Index of the value store
         * @param value Value to add
         * @return Success
         */
        inline bool ParserOption::setValue( const size_t &index, const std::string &value ) {
            if( _values.empty() ) {
                std::cerr << "Trying to set a value '" << value << "' on cli Parser option '"
                          << _name << "' that was not expected." << std::endl;
                return false;
            }
            if( index >= _values.size() ) {
                std::cerr << "Out of bound index: trying to set a value '" << value << "' in index '" << index
                          << "/" << _values.size() << "' on cli Parser option '" << _name << "'." << std::endl;
                return false;
            }
            _values.at( index ).setValue( value );
            if( !( _values.at( index ).hasDefault() ) ) {
                _values_set++;
            }
            if( !std::regex_match( _values.at( index ).getValue(), _values.at( index ).getRegex() ) ) {
                std::cerr << "Invalid value '" << value << "': " << _values.at( index ).getErrorMsg() << std::endl;
                return false;
            }
            return true;
        }

        /**
         * Checks if the ParserOption is required
         * @return Requirement state
         */
        inline bool ParserOption::isNeeded() const {
            return _needed_flag;
        }

        /**
         * Checks if the ParserOption was used
         * @return Usage flag
         */
        inline bool ParserOption::isUsed() const {
            return _used_flag;
        }

        /**
         * Gets the number of values that were set in the option
         * @return Number of set values
         */
        inline size_t ParserOption::valuesSetCount() const {
            return _values_set;
        }

        /**
         * Gets the number of values expected for this option
         * @return Number of expected values
         */
        inline size_t ParserOption::valuesExpectedCount() const {
            return _values.size();
        }

        /**
         * Gets a set of all values set/or defaults the option has
         * @return collection of pairs consisting of a value flag and a value
         */
        inline std::vector<std::pair<bool, std::string>> ParserOption::getValues() const {
            std::vector<std::pair<bool, std::string>> v;
            for( auto it = _values.begin(); it != _values.end(); ++it ) {
                if( it->isSet() ) { //Value was set
                    v.emplace_back( std::make_pair( it->isSet(), it->getValue() ) );
                } else if( it->hasDefault() ) { //No value set but default present
                    v.emplace_back( std::make_pair( it->hasDefault(), it->getDefault() ) );
                } else { //No value, no default
                    v.emplace_back( std::make_pair( false, "" ) );
                }
            }
            return v;
        }
    }
}

#endif //EADLIB_CLI_PARSER_OPTION_H
