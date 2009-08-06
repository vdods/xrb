#include "xrb_arithmeticparser.h"

#include <cstdio>
#include <iomanip>
#include <iostream>

#define DEBUG_SPEW_1(x) if (m_debug_spew_level >= 1) std::cerr << x
#define DEBUG_SPEW_2(x) if (m_debug_spew_level >= 2) std::cerr << x


#line 38 "xrb_arithmeticparser.trison"

#include "xrb_arithmeticscanner.hpp"

#include "xrb_math.hpp"

namespace Xrb
{

#line 21 "xrb_arithmeticparser.cpp"

ArithmeticParser::ArithmeticParser ()

{

#line 47 "xrb_arithmeticparser.trison"

    m_scanner = NULL;

#line 31 "xrb_arithmeticparser.cpp"
    m_debug_spew_level = 0;
    DEBUG_SPEW_2("### number of state transitions = " << ms_state_transition_count << std::endl);
    m_reduction_token = 0.0f;
}

ArithmeticParser::~ArithmeticParser ()
{

#line 51 "xrb_arithmeticparser.trison"

    ASSERT1(m_scanner == NULL);

#line 44 "xrb_arithmeticparser.cpp"
}

void ArithmeticParser::CheckStateConsistency ()
{
    unsigned int counter = 1;
    for (unsigned int i = 0; i < ms_state_count; ++i)
    {
        if (ms_state[i].m_lookahead_transition_offset > 0)
        {
            assert(counter == ms_state[i].m_lookahead_transition_offset);
            assert(ms_state[i].m_lookahead_transition_count > 0);
        }
        else
            assert(ms_state[i].m_lookahead_transition_count == 0);

        counter += ms_state[i].m_lookahead_transition_count;

        if (ms_state[i].m_default_action_offset > 0)
            ++counter;

        if (ms_state[i].m_non_terminal_transition_offset > 0)
        {
            assert(counter == ms_state[i].m_non_terminal_transition_offset);
            assert(ms_state[i].m_non_terminal_transition_offset > 0);
        }
        else
            assert(ms_state[i].m_non_terminal_transition_offset == 0);

        counter += ms_state[i].m_non_terminal_transition_count;
    }
    assert(counter == ms_state_transition_count);
}

ArithmeticParser::ParserReturnCode ArithmeticParser::Parse ()
{


    ParserReturnCode return_code = PrivateParse();



    return return_code;
}

bool ArithmeticParser::DoesStateAcceptErrorToken (ArithmeticParser::StateNumber state_number) const
{
    assert(state_number < ms_state_count);
    State const &state = ms_state[state_number];

    for (unsigned int transition = state.m_lookahead_transition_offset,
                      transition_end = state.m_lookahead_transition_offset +
                                       state.m_lookahead_transition_count;
         transition < transition_end;
         ++transition)
    {
        if (ms_state_transition[transition].m_token_type == Token::ERROR_)
            return true;
    }

    return false;
}

ArithmeticParser::ParserReturnCode ArithmeticParser::PrivateParse ()
{
    m_state_stack.clear();
    m_token_stack.clear();

    m_lookahead_token_type = Token::INVALID_;
    m_lookahead_token = 0.0f;
    m_is_new_lookahead_token_required = true;

    m_saved_lookahead_token_type = Token::INVALID_;
    m_get_new_lookahead_token_type_from_saved = false;
    m_previous_transition_accepted_error_token = false;

    m_is_returning_with_non_terminal = false;
    m_returning_with_this_non_terminal = Token::INVALID_;

    // start in state 0
    PushState(0);

    while (true)
    {
        StateNumber current_state_number = m_state_stack.back();
        assert(current_state_number < ms_state_count);
        State const &current_state = ms_state[current_state_number];

        unsigned int state_transition_number;
        unsigned int state_transition_count;
        unsigned int default_action_state_transition_number;
        Token::Type state_transition_token_type = Token::INVALID_;

        // if we've just reduced to a non-terminal, coming from
        // another state, use the non-terminal transitions.
        if (m_is_returning_with_non_terminal)
        {
            m_is_returning_with_non_terminal = false;
            state_transition_number = current_state.m_non_terminal_transition_offset;
            state_transition_count = current_state.m_non_terminal_transition_count;
            default_action_state_transition_number = 0;
            state_transition_token_type = m_returning_with_this_non_terminal;
        }
        // otherwise use the lookahead transitions, with the default action
        else
        {
            state_transition_number = current_state.m_lookahead_transition_offset;
            state_transition_count = current_state.m_lookahead_transition_count;
            default_action_state_transition_number = current_state.m_default_action_offset;
            // LookaheadTokenType may cause Scan to be called, which may
            // block execution.  only scan a token if necessary.
            if (state_transition_count != 0)
            {
                state_transition_token_type = LookaheadTokenType();
                DEBUG_SPEW_1("*** lookahead token type: " << state_transition_token_type << std::endl);
            }
        }

        unsigned int i;
        for (i = 0;
             i < state_transition_count;
             ++i, ++state_transition_number)
        {
            StateTransition const &state_transition =
                ms_state_transition[state_transition_number];
            // if this token matches the current transition, do its action
            if (state_transition.m_token_type == state_transition_token_type)
            {
                if (state_transition.m_token_type == Token::ERROR_)
                    m_previous_transition_accepted_error_token = true;
                else
                    m_previous_transition_accepted_error_token = false;

                PrintStateTransition(state_transition_number);
                if (ProcessAction(state_transition.m_action) == ARC_ACCEPT_AND_RETURN)
                    return PRC_SUCCESS; // the accepted token is in m_reduction_token
                else
                    break;
            }
        }

        // if no transition matched, check for a default action.
        if (i == state_transition_count)
        {
            // check for the default action
            if (default_action_state_transition_number != 0)
            {
                PrintStateTransition(default_action_state_transition_number);
                Action const &default_action =
                    ms_state_transition[default_action_state_transition_number].m_action;
                if (ProcessAction(default_action) == ARC_ACCEPT_AND_RETURN)
                    return PRC_SUCCESS; // the accepted token is in m_reduction_token
            }
            // otherwise go into error recovery mode
            else
            {
                assert(!m_is_new_lookahead_token_required);

                DEBUG_SPEW_1("!!! error recovery: begin" << std::endl);

                // if an error was encountered, and this state accepts the %error
                // token, then we don't need to pop states
                if (DoesStateAcceptErrorToken(current_state_number))
                {
                    // if an error token was previously accepted, then throw
                    // away the lookahead token, because whatever the lookahead
                    // was didn't match.  this prevents an infinite loop.
                    if (m_previous_transition_accepted_error_token)
                    {
                        ThrowAwayToken(m_lookahead_token);
                        m_is_new_lookahead_token_required = true;
                    }
                    // otherwise, save off the lookahead token so that once the
                    // %error token has been shifted, the lookahead can be
                    // re-analyzed.
                    else
                    {
                        m_saved_lookahead_token_type = m_lookahead_token_type;
                        m_get_new_lookahead_token_type_from_saved = true;
                        m_lookahead_token_type = Token::ERROR_;
                    }
                }
                // otherwise save off the lookahead token for the error panic popping
                else
                {
                    // save off the lookahead token type and set the current to Token::ERROR_
                    m_saved_lookahead_token_type = m_lookahead_token_type;
                    m_get_new_lookahead_token_type_from_saved = true;
                    m_lookahead_token_type = Token::ERROR_;

                    // pop until we either run off the stack, or find a state
                    // which accepts the %error token.
                    assert(m_state_stack.size() > 0);
                    do
                    {
                        DEBUG_SPEW_1("!!! error recovery: popping state " << current_state_number << std::endl);
                        m_state_stack.pop_back();

                        if (m_state_stack.size() == 0)
                        {
                            ThrowAwayTokenStack();
                            DEBUG_SPEW_1("!!! error recovery: unhandled error -- quitting" << std::endl);
                            return PRC_UNHANDLED_PARSE_ERROR;
                        }

                        assert(m_token_stack.size() > 0);
                        ThrowAwayToken(m_token_stack.back());
                        m_token_stack.pop_back();
                        current_state_number = m_state_stack.back();
                    }
                    while (!DoesStateAcceptErrorToken(current_state_number));
                }

                DEBUG_SPEW_1("!!! error recovery: found state which accepts %error token" << std::endl);
                PrintStateStack();
            }
        }
    }

    // this should never happen because the above loop is infinite
    return PRC_UNHANDLED_PARSE_ERROR;
}

ArithmeticParser::ActionReturnCode ArithmeticParser::ProcessAction (ArithmeticParser::Action const &action)
{
    if (action.m_transition_action == TA_SHIFT_AND_PUSH_STATE)
    {
        ShiftLookaheadToken();
        PushState(action.m_data);
    }
    else if (action.m_transition_action == TA_PUSH_STATE)
    {
        PushState(action.m_data);
    }
    else if (action.m_transition_action == TA_REDUCE_USING_RULE)
    {
        unsigned int reduction_rule_number = action.m_data;
        assert(reduction_rule_number < ms_reduction_rule_count);
        ReductionRule const &reduction_rule = ms_reduction_rule[reduction_rule_number];
        ReduceUsingRule(reduction_rule, false);
    }
    else if (action.m_transition_action == TA_REDUCE_AND_ACCEPT_USING_RULE)
    {
        unsigned int reduction_rule_number = action.m_data;
        assert(reduction_rule_number < ms_reduction_rule_count);
        ReductionRule const &reduction_rule = ms_reduction_rule[reduction_rule_number];
        ReduceUsingRule(reduction_rule, true);
        DEBUG_SPEW_1("*** accept" << std::endl);
        // everything is done, so just return.
        return ARC_ACCEPT_AND_RETURN;
    }
    else if (action.m_transition_action == TA_THROW_AWAY_LOOKAHEAD_TOKEN)
    {
        assert(!m_is_new_lookahead_token_required);
        ThrowAwayToken(m_lookahead_token);
        m_is_new_lookahead_token_required = true;
    }

    return ARC_CONTINUE_PARSING;
}

void ArithmeticParser::ShiftLookaheadToken ()
{
    assert(m_lookahead_token_type != Token::DEFAULT_);
    assert(m_lookahead_token_type != Token::INVALID_);
    DEBUG_SPEW_1("*** shifting lookahead token -- type " << m_lookahead_token_type << std::endl);
    m_token_stack.push_back(m_lookahead_token);
    m_is_new_lookahead_token_required = true;
}

void ArithmeticParser::PushState (StateNumber const state_number)
{
    assert(state_number < ms_state_count);

    DEBUG_SPEW_1("*** going to state " << state_number << std::endl);
    m_state_stack.push_back(state_number);
    PrintStateStack();
}

void ArithmeticParser::ReduceUsingRule (ReductionRule const &reduction_rule, bool and_accept)
{
    if (and_accept)
    {
        assert(reduction_rule.m_number_of_tokens_to_reduce_by == m_state_stack.size() - 1);
        assert(reduction_rule.m_number_of_tokens_to_reduce_by == m_token_stack.size());
    }
    else
    {
        assert(reduction_rule.m_number_of_tokens_to_reduce_by < m_state_stack.size());
        assert(reduction_rule.m_number_of_tokens_to_reduce_by <= m_token_stack.size());
    }

    DEBUG_SPEW_1("*** reducing: " << reduction_rule.m_description << std::endl);

    m_is_returning_with_non_terminal = true;
    m_returning_with_this_non_terminal = reduction_rule.m_non_terminal_to_reduce_to;
    m_reduction_rule_token_count = reduction_rule.m_number_of_tokens_to_reduce_by;

    // call the reduction rule handler if it exists
    if (reduction_rule.m_handler != NULL)
        m_reduction_token = (this->*(reduction_rule.m_handler))();
    // pop the states and tokens
    PopStates(reduction_rule.m_number_of_tokens_to_reduce_by, false);

    // only push the reduced token if we aren't accepting yet
    if (!and_accept)
    {
        // push the token that resulted from the reduction
        m_token_stack.push_back(m_reduction_token);
        PrintStateStack();
    }
}

void ArithmeticParser::PopStates (unsigned int number_of_states_to_pop, bool print_state_stack)
{
    assert(number_of_states_to_pop < m_state_stack.size());
    assert(number_of_states_to_pop <= m_token_stack.size());

    while (number_of_states_to_pop-- > 0)
    {
        m_state_stack.pop_back();
        m_token_stack.pop_back();
    }

    if (print_state_stack)
        PrintStateStack();
}

void ArithmeticParser::PrintStateStack () const
{
    DEBUG_SPEW_2("*** state stack: ");
    for (StateStackConstIterator it = m_state_stack.begin(),
                                 it_end = m_state_stack.end();
         it != it_end;
         ++it)
    {
        DEBUG_SPEW_2(*it << " ");
    }
    DEBUG_SPEW_2(std::endl);
}

void ArithmeticParser::PrintStateTransition (unsigned int const state_transition_number) const
{
    assert(state_transition_number < ms_state_transition_count);
    DEBUG_SPEW_2("&&& exercising state transition " << std::setw(4) << std::right << state_transition_number << std::endl);
}

void ArithmeticParser::ScanANewLookaheadToken ()
{
    assert(!m_is_new_lookahead_token_required);
    m_lookahead_token = 0.0f;
    m_lookahead_token_type = Scan();
    DEBUG_SPEW_1("*** scanned a new lookahead token -- type " << m_lookahead_token_type << std::endl);
}

void ArithmeticParser::ThrowAwayToken (Float token)
{

}

void ArithmeticParser::ThrowAwayTokenStack ()
{
    while (!m_token_stack.empty())
    {
        ThrowAwayToken(m_token_stack.back());
        m_token_stack.pop_back();
    }
}

std::ostream &operator << (std::ostream &stream, ArithmeticParser::Token::Type token_type)
{
    static std::string const s_token_type_string[] =
    {
        "BAD_TOKEN",
        "NUMERIC",
        "END_",

        "exp",
        "START_",

        "%error",
        "DEFAULT_",
        "INVALID_"
    };
    static unsigned int const s_token_type_string_count =
        sizeof(s_token_type_string) /
        sizeof(std::string);

    unsigned token_type_value = static_cast<unsigned int>(token_type);
    if (token_type_value < 0x20)
        stream << token_type_value;
    else if (token_type_value < 0x7F)
        stream << "'" << static_cast<char>(token_type) << "'";
    else if (token_type_value < 0x100)
        stream << token_type_value;
    else if (token_type_value < 0x100 + s_token_type_string_count)
        stream << s_token_type_string[token_type_value - 0x100];
    else
        stream << token_type_value;

    return stream;
}

// ///////////////////////////////////////////////////////////////////////////
// state machine reduction rule handlers
// ///////////////////////////////////////////////////////////////////////////

// rule 0: %start <- exp END_    
Float ArithmeticParser::ReductionRuleHandler0000 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    return m_token_stack[m_token_stack.size() - m_reduction_rule_token_count];

    return 0.0f;
}

// rule 1: exp <- exp:left '+' exp:right    %left %prec ADDITION
Float ArithmeticParser::ReductionRuleHandler0001 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    Float left = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(2) < m_reduction_rule_token_count);
    Float right = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 99 "xrb_arithmeticparser.trison"
 return left + right; 
#line 470 "xrb_arithmeticparser.cpp"
    return 0.0f;
}

// rule 2: exp <- exp:left '-' exp:right    %left %prec ADDITION
Float ArithmeticParser::ReductionRuleHandler0002 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    Float left = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(2) < m_reduction_rule_token_count);
    Float right = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 101 "xrb_arithmeticparser.trison"
 return left - right; 
#line 484 "xrb_arithmeticparser.cpp"
    return 0.0f;
}

// rule 3: exp <- exp:left '*' exp:right    %left %prec MULTIPLICATION
Float ArithmeticParser::ReductionRuleHandler0003 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    Float left = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(2) < m_reduction_rule_token_count);
    Float right = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 103 "xrb_arithmeticparser.trison"
 return left * right; 
#line 498 "xrb_arithmeticparser.cpp"
    return 0.0f;
}

// rule 4: exp <- exp:left '/' exp:right    %left %prec MULTIPLICATION
Float ArithmeticParser::ReductionRuleHandler0004 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    Float left = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(2) < m_reduction_rule_token_count);
    Float right = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 105 "xrb_arithmeticparser.trison"
 return (right == 0.0f) ? Math::Nan() : left / right; 
#line 512 "xrb_arithmeticparser.cpp"
    return 0.0f;
}

// rule 5: exp <- '+' exp:exp     %prec UNARY
Float ArithmeticParser::ReductionRuleHandler0005 ()
{
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    Float exp = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 107 "xrb_arithmeticparser.trison"
 return exp; 
#line 524 "xrb_arithmeticparser.cpp"
    return 0.0f;
}

// rule 6: exp <- '-' exp:exp     %prec UNARY
Float ArithmeticParser::ReductionRuleHandler0006 ()
{
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    Float exp = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 109 "xrb_arithmeticparser.trison"
 return -exp; 
#line 536 "xrb_arithmeticparser.cpp"
    return 0.0f;
}

// rule 7: exp <- exp:left '^' exp:right    %left %prec EXPONENTIATION
Float ArithmeticParser::ReductionRuleHandler0007 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    Float left = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(2) < m_reduction_rule_token_count);
    Float right = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 111 "xrb_arithmeticparser.trison"
 return Math::Pow(left, right); 
#line 550 "xrb_arithmeticparser.cpp"
    return 0.0f;
}

// rule 8: exp <- '(' exp:exp ')'    
Float ArithmeticParser::ReductionRuleHandler0008 ()
{
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    Float exp = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 113 "xrb_arithmeticparser.trison"
 return exp; 
#line 562 "xrb_arithmeticparser.cpp"
    return 0.0f;
}

// rule 9: exp <- NUMERIC:numeric    
Float ArithmeticParser::ReductionRuleHandler0009 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    Float numeric = static_cast< Float >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 115 "xrb_arithmeticparser.trison"
 return numeric; 
#line 574 "xrb_arithmeticparser.cpp"
    return 0.0f;
}



// ///////////////////////////////////////////////////////////////////////////
// reduction rule lookup table
// ///////////////////////////////////////////////////////////////////////////

ArithmeticParser::ReductionRule const ArithmeticParser::ms_reduction_rule[] =
{
    {                 Token::START_,  2, &ArithmeticParser::ReductionRuleHandler0000, "rule 0: %start <- exp END_    "},
    {                  Token::exp__,  3, &ArithmeticParser::ReductionRuleHandler0001, "rule 1: exp <- exp '+' exp    %left %prec ADDITION"},
    {                  Token::exp__,  3, &ArithmeticParser::ReductionRuleHandler0002, "rule 2: exp <- exp '-' exp    %left %prec ADDITION"},
    {                  Token::exp__,  3, &ArithmeticParser::ReductionRuleHandler0003, "rule 3: exp <- exp '*' exp    %left %prec MULTIPLICATION"},
    {                  Token::exp__,  3, &ArithmeticParser::ReductionRuleHandler0004, "rule 4: exp <- exp '/' exp    %left %prec MULTIPLICATION"},
    {                  Token::exp__,  2, &ArithmeticParser::ReductionRuleHandler0005, "rule 5: exp <- '+' exp     %prec UNARY"},
    {                  Token::exp__,  2, &ArithmeticParser::ReductionRuleHandler0006, "rule 6: exp <- '-' exp     %prec UNARY"},
    {                  Token::exp__,  3, &ArithmeticParser::ReductionRuleHandler0007, "rule 7: exp <- exp '^' exp    %left %prec EXPONENTIATION"},
    {                  Token::exp__,  3, &ArithmeticParser::ReductionRuleHandler0008, "rule 8: exp <- '(' exp ')'    "},
    {                  Token::exp__,  1, &ArithmeticParser::ReductionRuleHandler0009, "rule 9: exp <- NUMERIC    "},

    // special error panic reduction rule
    {                 Token::ERROR_,  1,                                     NULL, "* -> error"}
};

unsigned int const ArithmeticParser::ms_reduction_rule_count =
    sizeof(ArithmeticParser::ms_reduction_rule) /
    sizeof(ArithmeticParser::ReductionRule);

// ///////////////////////////////////////////////////////////////////////////
// state transition lookup table
// ///////////////////////////////////////////////////////////////////////////

ArithmeticParser::State const ArithmeticParser::ms_state[] =
{
    {   1,    4,    0,    5,    1}, // state    0
    {   0,    0,    6,    0,    0}, // state    1
    {   7,    4,    0,   11,    1}, // state    2
    {  12,    4,    0,   16,    1}, // state    3
    {  17,    4,    0,   21,    1}, // state    4
    {  22,    6,    0,    0,    0}, // state    5
    {  28,    1,   29,    0,    0}, // state    6
    {  30,    1,   31,    0,    0}, // state    7
    {  32,    6,    0,    0,    0}, // state    8
    {   0,    0,   38,    0,    0}, // state    9
    {  39,    4,    0,   43,    1}, // state   10
    {  44,    4,    0,   48,    1}, // state   11
    {  49,    4,    0,   53,    1}, // state   12
    {  54,    4,    0,   58,    1}, // state   13
    {  59,    4,    0,   63,    1}, // state   14
    {   0,    0,   64,    0,    0}, // state   15
    {  65,    3,   68,    0,    0}, // state   16
    {  69,    3,   72,    0,    0}, // state   17
    {  73,    1,   74,    0,    0}, // state   18
    {  75,    1,   76,    0,    0}, // state   19
    {   0,    0,   77,    0,    0}  // state   20

};

unsigned int const ArithmeticParser::ms_state_count =
    sizeof(ArithmeticParser::ms_state) /
    sizeof(ArithmeticParser::State);

// ///////////////////////////////////////////////////////////////////////////
// state transition table
// ///////////////////////////////////////////////////////////////////////////

ArithmeticParser::StateTransition const ArithmeticParser::ms_state_transition[] =
{
    // dummy transition in the NULL transition
    {               Token::INVALID_, {                       TA_COUNT,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state    0
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::NUMERIC, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    { static_cast<Token::Type>('('), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,    5}},

// ///////////////////////////////////////////////////////////////////////////
// state    1
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    9}},

// ///////////////////////////////////////////////////////////////////////////
// state    2
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::NUMERIC, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    { static_cast<Token::Type>('('), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state    3
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::NUMERIC, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    { static_cast<Token::Type>('('), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state    4
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::NUMERIC, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    { static_cast<Token::Type>('('), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state    5
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,   10}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    { static_cast<Token::Type>('*'), {        TA_SHIFT_AND_PUSH_STATE,   12}},
    { static_cast<Token::Type>('/'), {        TA_SHIFT_AND_PUSH_STATE,   13}},
    { static_cast<Token::Type>('^'), {        TA_SHIFT_AND_PUSH_STATE,   14}},

// ///////////////////////////////////////////////////////////////////////////
// state    6
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>('^'), {        TA_SHIFT_AND_PUSH_STATE,   14}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    5}},

// ///////////////////////////////////////////////////////////////////////////
// state    7
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>('^'), {        TA_SHIFT_AND_PUSH_STATE,   14}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state    8
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,   10}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    { static_cast<Token::Type>('*'), {        TA_SHIFT_AND_PUSH_STATE,   12}},
    { static_cast<Token::Type>('/'), {        TA_SHIFT_AND_PUSH_STATE,   13}},
    { static_cast<Token::Type>('^'), {        TA_SHIFT_AND_PUSH_STATE,   14}},
    { static_cast<Token::Type>(')'), {        TA_SHIFT_AND_PUSH_STATE,   15}},

// ///////////////////////////////////////////////////////////////////////////
// state    9
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {TA_REDUCE_AND_ACCEPT_USING_RULE,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state   10
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::NUMERIC, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    { static_cast<Token::Type>('('), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   16}},

// ///////////////////////////////////////////////////////////////////////////
// state   11
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::NUMERIC, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    { static_cast<Token::Type>('('), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state   12
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::NUMERIC, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    { static_cast<Token::Type>('('), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state   13
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::NUMERIC, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    { static_cast<Token::Type>('('), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state   14
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::NUMERIC, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    { static_cast<Token::Type>('-'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    { static_cast<Token::Type>('('), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   20}},

// ///////////////////////////////////////////////////////////////////////////
// state   15
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state   16
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>('*'), {        TA_SHIFT_AND_PUSH_STATE,   12}},
    { static_cast<Token::Type>('/'), {        TA_SHIFT_AND_PUSH_STATE,   13}},
    { static_cast<Token::Type>('^'), {        TA_SHIFT_AND_PUSH_STATE,   14}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    1}},

// ///////////////////////////////////////////////////////////////////////////
// state   17
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>('*'), {        TA_SHIFT_AND_PUSH_STATE,   12}},
    { static_cast<Token::Type>('/'), {        TA_SHIFT_AND_PUSH_STATE,   13}},
    { static_cast<Token::Type>('^'), {        TA_SHIFT_AND_PUSH_STATE,   14}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    2}},

// ///////////////////////////////////////////////////////////////////////////
// state   18
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>('^'), {        TA_SHIFT_AND_PUSH_STATE,   14}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state   19
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>('^'), {        TA_SHIFT_AND_PUSH_STATE,   14}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    4}},

// ///////////////////////////////////////////////////////////////////////////
// state   20
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    7}}

};

unsigned int const ArithmeticParser::ms_state_transition_count =
    sizeof(ArithmeticParser::ms_state_transition) /
    sizeof(ArithmeticParser::StateTransition);


#line 55 "xrb_arithmeticparser.trison"

Float ArithmeticParser::Parse (std::string const &input_string)
{
    ASSERT1(m_scanner == NULL);

    Float retval;

    m_scanner = new ArithmeticScanner(input_string);
    if (Parse() == PRC_SUCCESS)
        retval = GetAcceptedToken();
    else
        retval = Math::Nan();
    DeleteAndNullify(m_scanner);

    return retval;
}

ArithmeticParser::Token::Type ArithmeticParser::Scan ()
{
    ASSERT1(m_scanner != NULL);
    return m_scanner->Scan(&m_lookahead_token);
}

} // end of namespace Xrb

#line 878 "xrb_arithmeticparser.cpp"

