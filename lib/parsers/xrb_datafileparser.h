#if !defined(_DataFileParser_H_)
#define _DataFileParser_H_

#include <ostream>
#include <string>
#include <vector>


#line 11 "../../lib/parsers/xrb_datafileparser.trison"

#include "xrb.h"

#include <string>

namespace Xrb
{

class DataFileKeyPair;
class DataFileScanner;
class DataFileValue;

#line 23 "../../lib/parsers/xrb_datafileparser.h"

class DataFileParser

{
public:

    enum ReturnCode
    {
        RC_SUCCESS = 0,
        RC_UNHANDLED_ERROR = 1
    }; // end of enum ReturnCode

    struct Token
    {
        enum Type
        {
            // user-defined terminal, non-single-character tokens
            BOOLEAN = 0x100,
            CHARACTER,
            ERROR,
            FLOAT,
            IDENTIFIER,
            INTEGER,
            STRING_FRAGMENT,

            // special end-of-input terminal
            _END,

            // user-defined nonterminal tokens
            __array,
            __data_file,
            __element,
            __element_list,
            __key_pair,
            __list,
            __list_element,
            __string,
            __structure,
            __value,

            // special start nonterminal
            _START,

            // parser's sentinel and special tokens
            _ERROR,
            _DEFAULT,
            _INVALID
        }; // end of enum DataFileParser::Token::Type
    }; // end of struct DataFileParser::Token

    DataFileParser ();
    ~DataFileParser ();

    inline DataFileValue * GetAcceptedToken () const
    {
        return m_reduction_token;
    }
    inline unsigned int GetDebugSpewLevel () const
    {
        return m_debug_spew_level;
    }

    inline void SetDebugSpewLevel (unsigned int debug_spew_level)
    {
        m_debug_spew_level = debug_spew_level;
    }

    ReturnCode Parse ();

    static void CheckStateConsistency ();


#line 26 "../../lib/parsers/xrb_datafileparser.trison"

    inline DataFileKeyPair *GetAcceptedKeyPair () const
    {
        return DStaticCast<DataFileKeyPair *>(GetAcceptedToken());
    }

    bool SetInputFilename (std::string const &input_filename);

    Token::Type Scan ();

private:

    DataFileScanner *m_scanner;

#line 111 "../../lib/parsers/xrb_datafileparser.h"

private:

    typedef unsigned int StateNumber;

    enum TransitionAction
    {
        TA_SHIFT_AND_PUSH_STATE = 0,
        TA_PUSH_STATE,
        TA_REDUCE_USING_RULE,
        TA_REDUCE_AND_ACCEPT_USING_RULE,
        TA_THROW_AWAY_LOOKAHEAD_TOKEN,

        TA_COUNT
    };

    enum ActionReturnCode
    {
        ARC_CONTINUE_PARSING = 0,
        ARC_ACCEPT_AND_RETURN
    };

    struct ReductionRule
    {
        typedef DataFileValue * (DataFileParser::*ReductionRuleHandler)();

        Token::Type m_non_terminal_to_reduce_to;
        unsigned int m_number_of_tokens_to_reduce_by;
        ReductionRuleHandler m_handler;
        std::string m_description;
    };

    struct Action
    {
        TransitionAction m_transition_action;
        unsigned int m_data;
    };

    struct StateTransition
    {
        Token::Type m_token_type;
        Action m_action;
    };

    struct State
    {
        unsigned int m_lookahead_transition_offset;
        unsigned int m_lookahead_transition_count;
        unsigned int m_default_action_offset;
        unsigned int m_non_terminal_transition_offset;
        unsigned int m_non_terminal_transition_count;
    };

    inline void GetNewLookaheadToken ()
    {
        if (m_is_new_lookahead_token_required)
        {
            m_is_new_lookahead_token_required = false;
            if (m_get_new_lookahead_token_type_from_saved)
            {
                m_get_new_lookahead_token_type_from_saved = false;
                m_lookahead_token_type = m_saved_lookahead_token_type;
            }
            else
                ScanANewLookaheadToken();
        }
    }
    inline Token::Type GetLookaheadTokenType ()
    {
        GetNewLookaheadToken();
        return m_lookahead_token_type;
    }
    inline DataFileValue * const &GetLookaheadToken ()
    {
        GetNewLookaheadToken();
        return m_lookahead_token;
    }
    bool GetDoesStateAcceptErrorToken (StateNumber state_number) const;

    ReturnCode PrivateParse ();

    ActionReturnCode ProcessAction (Action const &action);
    void ShiftLookaheadToken ();
    void PushState (StateNumber state_number);
    void ReduceUsingRule (ReductionRule const &reduction_rule, bool and_accept);
    void PopStates (unsigned int number_of_states_to_pop, bool print_state_stack = true);
    void PrintStateStack () const;
    void PrintTokenStack () const;
    void PrintStateTransition (unsigned int state_transition_number) const;
    void ScanANewLookaheadToken ();
    void ThrowAwayToken (DataFileValue * token);
    void ThrowAwayTokenStack ();

    typedef std::vector<StateNumber> StateStack;
    typedef StateStack::const_iterator StateStackConstIterator;

    typedef std::vector< DataFileValue * > TokenStack;
    typedef TokenStack::const_iterator TokenStackConstIterator;

    unsigned int m_debug_spew_level;

    StateStack m_state_stack;
    TokenStack m_token_stack;

    Token::Type m_lookahead_token_type;
    DataFileValue * m_lookahead_token;
    bool m_is_new_lookahead_token_required;

    Token::Type m_saved_lookahead_token_type;
    bool m_get_new_lookahead_token_type_from_saved;
    bool m_previous_transition_accepted_error_token;

    bool m_is_returning_with_non_terminal;
    Token::Type m_returning_with_this_non_terminal;

    DataFileValue * m_reduction_token;
    unsigned int m_reduction_rule_token_count;

    static State const ms_state[];
    static unsigned int const ms_state_count;
    static ReductionRule const ms_reduction_rule[];
    static unsigned int const ms_reduction_rule_count;
    static StateTransition const ms_state_transition[];
    static unsigned int const ms_state_transition_count;

    DataFileValue * ReductionRuleHandler0000 ();
    DataFileValue * ReductionRuleHandler0001 ();
    DataFileValue * ReductionRuleHandler0002 ();
    DataFileValue * ReductionRuleHandler0003 ();
    DataFileValue * ReductionRuleHandler0004 ();
    DataFileValue * ReductionRuleHandler0005 ();
    DataFileValue * ReductionRuleHandler0006 ();
    DataFileValue * ReductionRuleHandler0007 ();
    DataFileValue * ReductionRuleHandler0008 ();
    DataFileValue * ReductionRuleHandler0009 ();
    DataFileValue * ReductionRuleHandler0010 ();
    DataFileValue * ReductionRuleHandler0011 ();
    DataFileValue * ReductionRuleHandler0012 ();
    DataFileValue * ReductionRuleHandler0013 ();
    DataFileValue * ReductionRuleHandler0014 ();
    DataFileValue * ReductionRuleHandler0015 ();
    DataFileValue * ReductionRuleHandler0016 ();
    DataFileValue * ReductionRuleHandler0017 ();
    DataFileValue * ReductionRuleHandler0018 ();
    DataFileValue * ReductionRuleHandler0019 ();
    DataFileValue * ReductionRuleHandler0020 ();
    DataFileValue * ReductionRuleHandler0021 ();

}; // end of class DataFileParser

std::ostream &operator << (std::ostream &stream, DataFileParser::Token::Type token_type);


#line 41 "../../lib/parsers/xrb_datafileparser.trison"

} // end of namespace Xrb

#line 269 "../../lib/parsers/xrb_datafileparser.h"

#endif // !defined(_DataFileParser_H_)

