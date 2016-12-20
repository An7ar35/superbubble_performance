/**
    @class          eadlib::cli::ParserValue
    @brief          Value container for eadlib::cli::ParserOption
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef EADLIB_CLI_PARSER_VALUE_H
#define EADLIB_CLI_PARSER_VALUE_H

#include <string>
#include <regex>
#include <vector>

namespace eadlib {
    namespace cli {
        class ParserValue {
          public:
            ParserValue( const std::regex &regex, const std::string &fail_msg );
            ParserValue( const std::regex &regex, const std::string &fail_msg, const std::string &default_value );
            void setValue( const std::string &value );
            bool isSet() const;
            bool hasDefault() const;
            std::string getValue() const;
            std::string getDefault() const;
            std::string getErrorMsg() const;
            std::regex getRegex() const;
          private:
            bool _flag;
            std::regex _regex;
            std::string _value;
            bool _has_default;
            std::string _default;
            std::string _error_msg;
        };

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // ParserValue class public method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         * @param regex    Regex string
         * @param fail_msg Regex failure error message
         */
        inline ParserValue::ParserValue( const std::regex &regex, const std::string &fail_msg )  :
            _flag( false ),
            _regex( regex ),
            _error_msg( fail_msg ),
            _value( "" ),
            _has_default( false ),
            _default( "" )
        {}

        /**
         * Constructor
         * @param regex         Regex string
         * @param fail_msg      Regex failure error message
         * @param default_value Default value
         */
        inline ParserValue::ParserValue( const std::regex &regex, const std::string &fail_msg, const std::string &default_value )  :
            _flag( false ),
            _regex( regex ),
            _error_msg( fail_msg ),
            _value( "" ),
            _has_default( true ),
            _default( default_value )
        {}

        /**
         * Sets the value
         * @param value Value
         */
        inline void ParserValue::setValue( const std::string &value ) {
            _flag = true;
            _value = value;
        }

        /**
         * Checks if value has been set
         * @return
         */
        inline bool ParserValue::isSet() const {
            return _flag;
        }

        /**
         * Checks if Value has a default
         * @return Default existence
         */
        inline bool ParserValue::hasDefault() const {
            return _has_default;
        }

        /**
         * Gets the value set
         * @return Set value
         */
        inline std::string ParserValue::getValue() const {
            return _value;
        }

        /**
         * Gets the default value
         * @return Default value
         */
        inline std::string ParserValue::getDefault() const {
            return _default;
        }

        /**
         * Gets the error string for a regex fail
         * @return Error message
         */
        inline std::string ParserValue::getErrorMsg() const {
            return _error_msg;
        }

        /**
         * Gets the regular expression for the value check
         * @return Regular expression
         */
        inline std::regex ParserValue::getRegex() const {
            return _regex;
        }

    }
}

#endif //EADLIB_CLI_PARSER_VALUE_H
