#if !defined(_ArithmeticParser_H_)
#define _ArithmeticParser_H_

#include <ostream>
#include <string>
#include <vector>


#line 11 "xrb_arithmeticparser.trison"

#include "xrb.hpp"

namespace Xrb
{

class ArithmeticScanner;

#line 19 "xrb_arithmeticparser.h"

class ArithmeticParser

{
public:

    struct Token
    {
        enum Type
        {
            // user-defined terminal, non-single-character tokens
            BAD_TOKEN = 0x100,
            NUMERIC,

            // special end-of-input terminal
            END_,

            // user-defined nonterminal tokens
            exp__,

            // special start nonterminal
            START_,

            // parser's sentinel and special tokens
            ERROR_,
            DEFAULT_,
            INVALID_
        }; // end of enum ArithmeticParser::Token::Type
    }; // end of struct ArithmeticParser::Token

    ArithmeticParser ();
    ~ArithmeticParser ();

    inline Float const &GetAcceptedToken () const { return m_reduction_token; }
    inline void ClearAcceptedToken () { m_reduction_token = 0.0f; }

    inline unsigned int DebugSpewLevel () const { return m_debug_spew_level; }
    inline void SetDebugSpewLevel (unsigned int debug_spew_level) { m_debug_spew_level = debug_spew_level; }

    static void CheckStateConsistency ();

private:

    enum ParserReturnCode
    {
        PRC_SUCCESS = 0,
        PRC_UNHANDLED_PARSE_ERROR = 1
    }; // end of enum ParserReturnCode

    ParserReturnCode Parse ();

public:


#line 24 "xrb_arithmeticparser.trison"

    Float Parse (std::string const &input_string);

private:

    Token::Type Scan ();

    ArithmeticScanner *m_scanner;

#line 84 "xrb_arithmeticparser.h"

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
        typedef Float (ArithmeticParser::*ReductionRuleHandler)();

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

    inline void NewLookaheadToken ()
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
    inline Token::Type LookaheadTokenType ()
    {
        NewLookaheadToken();
        return m_lookahead_token_type;
    }
    inline Float const &LookaheadToken ()
    {
        NewLookaheadToken();
        return m_lookahead_token;
    }
    bool DoesStateAcceptErrorToken (StateNumber state_number) const;

    ParserReturnCode PrivateParse ();

    ActionReturnCode ProcessAction (Action const &action);
    void ShiftLookaheadToken ();
    void PushState (StateNumber state_number);
    void ReduceUsingRule (ReductionRule const &reduction_rule, bool and_accept);
    void PopStates (unsigned int number_of_states_to_pop, bool print_state_stack = true);
    void PrintStateStack () const;
    void PrintTokenStack () const;
    void PrintStateTransition (unsigned int state_transition_number) const;
    void ScanANewLookaheadToken ();
    void ThrowAwayToken (Float token);
    void ThrowAwayTokenStack ();

    typedef std::vector<StateNumber> StateStack;
    typedef std::vector<Float> TokenStack;

    unsigned int m_debug_spew_level;

    StateStack m_state_stack;
    TokenStack m_token_stack;

    Token::Type m_lookahead_token_type;
    Float m_lookahead_token;
    bool m_is_new_lookahead_token_required;

    Token::Type m_saved_lookahead_token_type;
    bool m_get_new_lookahead_token_type_from_saved;
    bool m_previous_transition_accepted_error_token;

    bool m_is_returning_with_non_terminal;
    Token::Type m_returning_with_this_non_terminal;

    Float m_reduction_token;
    unsigned int m_reduction_rule_token_count;

    static State const ms_state[];
    static unsigned int const ms_state_count;
    static ReductionRule const ms_reduction_rule[];
    static unsigned int const ms_reduction_rule_count;
    static StateTransition const ms_state_transition[];
    static unsigned int const ms_state_transition_count;

    Float ReductionRuleHandler0000 ();
    Float ReductionRuleHandler0001 ();
    Float ReductionRuleHandler0002 ();
    Float ReductionRuleHandler0003 ();
    Float ReductionRuleHandler0004 ();
    Float ReductionRuleHandler0005 ();
    Float ReductionRuleHandler0006 ();
    Float ReductionRuleHandler0007 ();
    Float ReductionRuleHandler0008 ();
    Float ReductionRuleHandler0009 ();

}; // end of class ArithmeticParser

std::ostream &operator << (std::ostream &stream, ArithmeticParser::Token::Type token_type);


#line 34 "xrb_arithmeticparser.trison"

} // end of namespace Xrb

#line 230 "xrb_arithmeticparser.h"

#endif // !defined(_ArithmeticParser_H_)

