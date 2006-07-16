#include "../../lib/parsers/xrb_datafileparser.h"

#include <cstdio>
#include <iomanip>
#include <iostream>

#define DEBUG_SPEW_1(x) if (m_debug_spew_level >= 1) std::cerr << x
#define DEBUG_SPEW_2(x) if (m_debug_spew_level >= 2) std::cerr << x


#line 45 "../../lib/parsers/xrb_datafileparser.trison"

#include "xrb_datafilescanner.h"
#include "xrb_datafilevalue.h"

#undef FL
#define FL FileLocation(m_scanner->GetInputFilename(), m_scanner->GetLineNumber())

namespace Xrb
{

#line 23 "../../lib/parsers/xrb_datafileparser.cpp"

DataFileParser::DataFileParser ()

{

#line 56 "../../lib/parsers/xrb_datafileparser.trison"

    m_scanner = new DataFileScanner();

#line 33 "../../lib/parsers/xrb_datafileparser.cpp"
    m_debug_spew_level = 0;
    DEBUG_SPEW_2("### number of state transitions = " << ms_state_transition_count << std::endl);
}

DataFileParser::~DataFileParser ()
{

#line 60 "../../lib/parsers/xrb_datafileparser.trison"

    delete m_scanner;

#line 45 "../../lib/parsers/xrb_datafileparser.cpp"
}

DataFileParser::ReturnCode DataFileParser::Parse ()
{
    {

    }

    ReturnCode return_code = PrivateParse();

    {

    }

    return return_code;
}

void DataFileParser::CheckStateConsistency ()
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

bool DataFileParser::GetDoesStateAcceptErrorToken (DataFileParser::StateNumber state_number) const
{
    assert(state_number < ms_state_count);
    State const &state = ms_state[state_number];

    for (unsigned int transition = state.m_lookahead_transition_offset,
                      transition_end = state.m_lookahead_transition_offset +
                                       state.m_lookahead_transition_count;
         transition < transition_end;
         ++transition)
    {
        if (ms_state_transition[transition].m_token_type == Token::_ERROR)
            return true;
    }

    return false;
}

DataFileParser::ReturnCode DataFileParser::PrivateParse ()
{
    m_state_stack.clear();
    m_token_stack.clear();

    m_lookahead_token_type = Token::_INVALID;
    m_lookahead_token = NULL;
    m_is_new_lookahead_token_required = true;

    m_saved_lookahead_token_type = Token::_INVALID;
    m_get_new_lookahead_token_type_from_saved = false;
    m_previous_transition_accepted_error_token = false;

    m_is_returning_with_non_terminal = false;
    m_returning_with_this_non_terminal = Token::_INVALID;

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
        Token::Type state_transition_token_type;

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
            // GetLookaheadTokenType may cause Scan to be called, which may
            // block execution.  only scan a token if necessary.
            if (state_transition_count != 0)
            {
                state_transition_token_type = GetLookaheadTokenType();
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
                if (state_transition.m_token_type == Token::_ERROR)
                    m_previous_transition_accepted_error_token = true;
                else
                    m_previous_transition_accepted_error_token = false;

                PrintStateTransition(state_transition_number);
                if (ProcessAction(state_transition.m_action) == ARC_ACCEPT_AND_RETURN)
                    return RC_SUCCESS; // the accepted token is in m_reduction_token
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
                    return RC_SUCCESS; // the accepted token is in m_reduction_token
            }
            // otherwise go into error recovery mode
            else
            {
                assert(!m_is_new_lookahead_token_required);

                DEBUG_SPEW_1("!!! error recovery: begin" << std::endl);

                // if an error was encountered, and this state accepts the %error
                // token, then we don't need to pop states
                if (GetDoesStateAcceptErrorToken(current_state_number))
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
                        m_lookahead_token_type = Token::_ERROR;
                    }
                }
                // otherwise save off the lookahead token for the error panic popping
                else
                {
                    // save off the lookahead token type and set the current to Token::_ERROR
                    m_saved_lookahead_token_type = m_lookahead_token_type;
                    m_get_new_lookahead_token_type_from_saved = true;
                    m_lookahead_token_type = Token::_ERROR;

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
                            return RC_UNHANDLED_ERROR;
                        }

                        assert(m_token_stack.size() > 0);
                        ThrowAwayToken(m_token_stack.back());
                        m_token_stack.pop_back();
                        current_state_number = m_state_stack.back();
                    }
                    while (!GetDoesStateAcceptErrorToken(current_state_number));
                }

                DEBUG_SPEW_1("!!! error recovery: found state which accepts %error token" << std::endl);
                PrintStateStack();
            }
        }
    }

    // this should never happen because the above loop is infinite
    return RC_UNHANDLED_ERROR;
}

DataFileParser::ActionReturnCode DataFileParser::ProcessAction (DataFileParser::Action const &action)
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

void DataFileParser::ShiftLookaheadToken ()
{
    assert(m_lookahead_token_type != Token::_DEFAULT);
    assert(m_lookahead_token_type != Token::_INVALID);
    DEBUG_SPEW_1("*** shifting lookahead token -- type " << m_lookahead_token_type << std::endl);
    m_token_stack.push_back(m_lookahead_token);
    m_is_new_lookahead_token_required = true;
}

void DataFileParser::PushState (StateNumber const state_number)
{
    assert(state_number < ms_state_count);

    DEBUG_SPEW_1("*** going to state " << state_number << std::endl);
    m_state_stack.push_back(state_number);
    PrintStateStack();
}

void DataFileParser::ReduceUsingRule (ReductionRule const &reduction_rule, bool and_accept)
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

void DataFileParser::PopStates (unsigned int number_of_states_to_pop, bool print_state_stack)
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

void DataFileParser::PrintStateStack () const
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

void DataFileParser::PrintStateTransition (unsigned int const state_transition_number) const
{
    assert(state_transition_number < ms_state_transition_count);
    DEBUG_SPEW_2("&&& exercising state transition " << std::setw(4) << std::right << state_transition_number << std::endl);
}

void DataFileParser::ScanANewLookaheadToken ()
{
    assert(!m_is_new_lookahead_token_required);
    m_lookahead_token = NULL;
    m_lookahead_token_type = Scan();
    DEBUG_SPEW_1("*** scanned a new lookahead token -- type " << m_lookahead_token_type << std::endl);
}

void DataFileParser::ThrowAwayToken (DataFileValue * token)
{

#line 64 "../../lib/parsers/xrb_datafileparser.trison"

    delete token;

#line 411 "../../lib/parsers/xrb_datafileparser.cpp"
}

void DataFileParser::ThrowAwayTokenStack ()
{
    while (!m_token_stack.empty())
    {
        ThrowAwayToken(m_token_stack.back());
        m_token_stack.pop_back();
    }
}

std::ostream &operator << (std::ostream &stream, DataFileParser::Token::Type token_type)
{
    static std::string const s_token_type_string[] =
    {
        "BOOLEAN",
        "CHARACTER",
        "ERROR",
        "FLOAT",
        "IDENTIFIER",
        "INTEGER",
        "STRING_FRAGMENT",
        "_END",

        "array",
        "data_file",
        "element",
        "element_list",
        "key_pair",
        "list",
        "list_element",
        "string",
        "structure",
        "value",
        "_START",

        "%error",
        "_DEFAULT",
        "_INVALID"
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

// rule 0: %start <- data_file _END    
DataFileValue * DataFileParser::ReductionRuleHandler0000 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    return m_token_stack[m_token_stack.size() - m_reduction_rule_token_count];

    return NULL;
}

// rule 1: data_file <- element_list:element_list    
DataFileValue * DataFileParser::ReductionRuleHandler0001 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileStructure * element_list = DStaticCast< DataFileStructure * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 107 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(element_list != NULL)
        return new DataFileKeyPair(m_scanner->GetInputFilename(), element_list);
    
#line 495 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 2: element_list <- element_list:element_list element:element    
DataFileValue * DataFileParser::ReductionRuleHandler0002 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileStructure * element_list = DStaticCast< DataFileStructure * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileKeyPair * element = DStaticCast< DataFileKeyPair * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 116 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(element_list != NULL)
        ASSERT1(element != NULL)
        try
        {
            element_list->AddKeyPair(element);
        }
        catch (char const *exception)
        {
            // TODO: emit error message
        }
        return element_list;
    
#line 521 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 3: element_list <-     
DataFileValue * DataFileParser::ReductionRuleHandler0003 ()
{

#line 130 "../../lib/parsers/xrb_datafileparser.trison"

        return new DataFileStructure();
    
#line 533 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 4: element <- key_pair:key_pair ';'    
DataFileValue * DataFileParser::ReductionRuleHandler0004 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileKeyPair * key_pair = DStaticCast< DataFileKeyPair * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 138 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(key_pair != NULL)
        return key_pair;
    
#line 548 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 5: key_pair <- IDENTIFIER:key value:value    
DataFileValue * DataFileParser::ReductionRuleHandler0005 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * key = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileValue * value = DStaticCast< DataFileValue * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 147 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(key != NULL)
        ASSERT1(value != NULL)
        DataFileKeyPair *key_pair = new DataFileKeyPair(key->GetValue(), value);
        Delete(key);
        return key_pair;
    
#line 568 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 6: structure <- '{' element_list:element_list '}'    
DataFileValue * DataFileParser::ReductionRuleHandler0006 ()
{
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileStructure * element_list = DStaticCast< DataFileStructure * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 159 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(element_list != NULL)
        return element_list;
    
#line 583 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 7: array <- '[' list:list ']'    
DataFileValue * DataFileParser::ReductionRuleHandler0007 ()
{
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileArray * list = DStaticCast< DataFileArray * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 168 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(list != NULL)
        return list;
    
#line 598 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 8: array <- '[' ']'    
DataFileValue * DataFileParser::ReductionRuleHandler0008 ()
{

#line 174 "../../lib/parsers/xrb_datafileparser.trison"

        return new DataFileArray();
    
#line 610 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 9: list <- list:list ',' list_element:list_element    
DataFileValue * DataFileParser::ReductionRuleHandler0009 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileArray * list = DStaticCast< DataFileArray * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(2) < m_reduction_rule_token_count);
    DataFileValue * list_element = DStaticCast< DataFileValue * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 182 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(list != NULL)
        ASSERT1(list_element != NULL)
        try
        {
            list->AppendValue(list_element);
        }
        catch (char const *exception)
        {
            // TODO: emit error message
            Delete(list_element);
        }
        return list;
    
#line 637 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 10: list <- list_element:list_element    
DataFileValue * DataFileParser::ReductionRuleHandler0010 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileValue * list_element = DStaticCast< DataFileValue * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 198 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(list_element != NULL)
        DataFileArray *list = new DataFileArray();
        try
        {
            list->AppendValue(list_element);
        }
        catch (char const *exception)
        {
            ASSERT1(false && "this should never happen")
            Delete(list_element);
        }
        return list;
    
#line 662 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 11: list_element <- key_pair:key_pair    
DataFileValue * DataFileParser::ReductionRuleHandler0011 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileKeyPair * key_pair = DStaticCast< DataFileKeyPair * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 217 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(key_pair != NULL)
        return key_pair;
    
#line 677 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 12: list_element <- value:value    
DataFileValue * DataFileParser::ReductionRuleHandler0012 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileValue * value = DStaticCast< DataFileValue * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 223 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(value != NULL)
        return value;
    
#line 692 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 13: value <- BOOLEAN:value    
DataFileValue * DataFileParser::ReductionRuleHandler0013 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileBoolean * value = DStaticCast< DataFileBoolean * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 232 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(value != NULL)
        return value;
    
#line 707 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 14: value <- INTEGER:value    
DataFileValue * DataFileParser::ReductionRuleHandler0014 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileInteger * value = DStaticCast< DataFileInteger * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 238 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(value != NULL)
        return value;
    
#line 722 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 15: value <- FLOAT:value    
DataFileValue * DataFileParser::ReductionRuleHandler0015 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileFloat * value = DStaticCast< DataFileFloat * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 244 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(value != NULL)
        return value;
    
#line 737 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 16: value <- CHARACTER:value    
DataFileValue * DataFileParser::ReductionRuleHandler0016 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileCharacter * value = DStaticCast< DataFileCharacter * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 250 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(value != NULL)
        return value;
    
#line 752 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 17: value <- string:value    
DataFileValue * DataFileParser::ReductionRuleHandler0017 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * value = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 256 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(value != NULL)
        return value;
    
#line 767 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 18: value <- structure:value    
DataFileValue * DataFileParser::ReductionRuleHandler0018 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileStructure * value = DStaticCast< DataFileStructure * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 262 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(value != NULL)
        return value;
    
#line 782 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 19: value <- array:value    
DataFileValue * DataFileParser::ReductionRuleHandler0019 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileArray * value = DStaticCast< DataFileArray * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 268 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(value != NULL)
        return value;
    
#line 797 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 20: string <- string:string STRING_FRAGMENT:string_fragment    
DataFileValue * DataFileParser::ReductionRuleHandler0020 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * string = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileString * string_fragment = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 277 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(string != NULL)
        ASSERT1(string_fragment != NULL)
        string->AppendString(string_fragment->GetValue());
        Delete(string_fragment);
        return string;
    
#line 817 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}

// rule 21: string <- STRING_FRAGMENT:string_fragment    
DataFileValue * DataFileParser::ReductionRuleHandler0021 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * string_fragment = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 286 "../../lib/parsers/xrb_datafileparser.trison"

        ASSERT1(string_fragment != NULL)
        return string_fragment;
    
#line 832 "../../lib/parsers/xrb_datafileparser.cpp"
    return NULL;
}



// ///////////////////////////////////////////////////////////////////////////
// reduction rule lookup table
// ///////////////////////////////////////////////////////////////////////////

DataFileParser::ReductionRule const DataFileParser::ms_reduction_rule[] =
{
    {                 Token::_START,  2, &DataFileParser::ReductionRuleHandler0000, "rule 0: %start <- data_file _END    "},
    {            Token::__data_file,  1, &DataFileParser::ReductionRuleHandler0001, "rule 1: data_file <- element_list    "},
    {         Token::__element_list,  2, &DataFileParser::ReductionRuleHandler0002, "rule 2: element_list <- element_list element    "},
    {         Token::__element_list,  0, &DataFileParser::ReductionRuleHandler0003, "rule 3: element_list <-     "},
    {              Token::__element,  2, &DataFileParser::ReductionRuleHandler0004, "rule 4: element <- key_pair ';'    "},
    {             Token::__key_pair,  2, &DataFileParser::ReductionRuleHandler0005, "rule 5: key_pair <- IDENTIFIER value    "},
    {            Token::__structure,  3, &DataFileParser::ReductionRuleHandler0006, "rule 6: structure <- '{' element_list '}'    "},
    {                Token::__array,  3, &DataFileParser::ReductionRuleHandler0007, "rule 7: array <- '[' list ']'    "},
    {                Token::__array,  2, &DataFileParser::ReductionRuleHandler0008, "rule 8: array <- '[' ']'    "},
    {                 Token::__list,  3, &DataFileParser::ReductionRuleHandler0009, "rule 9: list <- list ',' list_element    "},
    {                 Token::__list,  1, &DataFileParser::ReductionRuleHandler0010, "rule 10: list <- list_element    "},
    {         Token::__list_element,  1, &DataFileParser::ReductionRuleHandler0011, "rule 11: list_element <- key_pair    "},
    {         Token::__list_element,  1, &DataFileParser::ReductionRuleHandler0012, "rule 12: list_element <- value    "},
    {                Token::__value,  1, &DataFileParser::ReductionRuleHandler0013, "rule 13: value <- BOOLEAN    "},
    {                Token::__value,  1, &DataFileParser::ReductionRuleHandler0014, "rule 14: value <- INTEGER    "},
    {                Token::__value,  1, &DataFileParser::ReductionRuleHandler0015, "rule 15: value <- FLOAT    "},
    {                Token::__value,  1, &DataFileParser::ReductionRuleHandler0016, "rule 16: value <- CHARACTER    "},
    {                Token::__value,  1, &DataFileParser::ReductionRuleHandler0017, "rule 17: value <- string    "},
    {                Token::__value,  1, &DataFileParser::ReductionRuleHandler0018, "rule 18: value <- structure    "},
    {                Token::__value,  1, &DataFileParser::ReductionRuleHandler0019, "rule 19: value <- array    "},
    {               Token::__string,  2, &DataFileParser::ReductionRuleHandler0020, "rule 20: string <- string STRING_FRAGMENT    "},
    {               Token::__string,  1, &DataFileParser::ReductionRuleHandler0021, "rule 21: string <- STRING_FRAGMENT    "},

    // special error panic reduction rule
    {                 Token::_ERROR,  1,                                     NULL, "* -> error"}
};

unsigned int const DataFileParser::ms_reduction_rule_count =
    sizeof(DataFileParser::ms_reduction_rule) /
    sizeof(DataFileParser::ReductionRule);

// ///////////////////////////////////////////////////////////////////////////
// state transition lookup table
// ///////////////////////////////////////////////////////////////////////////

DataFileParser::State const DataFileParser::ms_state[] =
{
    {   0,    0,    1,    2,    2}, // state    0
    {   4,    1,    0,    0,    0}, // state    1
    {   5,    1,    6,    7,    2}, // state    2
    {   0,    0,    9,    0,    0}, // state    3
    {  10,    7,    0,   17,    4}, // state    4
    {   0,    0,   21,    0,    0}, // state    5
    {  22,    1,    0,    0,    0}, // state    6
    {   0,    0,   23,    0,    0}, // state    7
    {   0,    0,   24,    0,    0}, // state    8
    {   0,    0,   25,    0,    0}, // state    9
    {   0,    0,   26,    0,    0}, // state   10
    {   0,    0,   27,    0,    0}, // state   11
    {   0,    0,   28,   29,    1}, // state   12
    {  30,    9,    0,   39,    7}, // state   13
    {   0,    0,   46,    0,    0}, // state   14
    {   0,    0,   47,    0,    0}, // state   15
    {   0,    0,   48,    0,    0}, // state   16
    {  49,    1,   50,    0,    0}, // state   17
    {   0,    0,   51,    0,    0}, // state   18
    {  52,    2,    0,   54,    2}, // state   19
    {   0,    0,   56,    0,    0}, // state   20
    {   0,    0,   57,    0,    0}, // state   21
    {  58,    2,    0,    0,    0}, // state   22
    {   0,    0,   60,    0,    0}, // state   23
    {   0,    0,   61,    0,    0}, // state   24
    {   0,    0,   62,    0,    0}, // state   25
    {   0,    0,   63,    0,    0}, // state   26
    {  64,    8,    0,   72,    6}, // state   27
    {   0,    0,   78,    0,    0}, // state   28
    {   0,    0,   79,    0,    0}  // state   29

};

unsigned int const DataFileParser::ms_state_count =
    sizeof(DataFileParser::ms_state) /
    sizeof(DataFileParser::State);

// ///////////////////////////////////////////////////////////////////////////
// state transition table
// ///////////////////////////////////////////////////////////////////////////

DataFileParser::StateTransition const DataFileParser::ms_state_transition[] =
{
    // dummy transition in the NULL transition
    {               Token::_INVALID, {                       TA_COUNT,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state    0
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    3}},
    // nonterminal transitions
    {            Token::__data_file, {                  TA_PUSH_STATE,    1}},
    {         Token::__element_list, {                  TA_PUSH_STATE,    2}},

// ///////////////////////////////////////////////////////////////////////////
// state    1
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::_END, {        TA_SHIFT_AND_PUSH_STATE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state    2
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {             Token::IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,    4}},
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    1}},
    // nonterminal transitions
    {              Token::__element, {                  TA_PUSH_STATE,    5}},
    {             Token::__key_pair, {                  TA_PUSH_STATE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state    3
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {TA_REDUCE_AND_ACCEPT_USING_RULE,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state    4
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::BOOLEAN, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                Token::INTEGER, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::CHARACTER, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {        Token::STRING_FRAGMENT, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    { static_cast<Token::Type>('{'), {        TA_SHIFT_AND_PUSH_STATE,   12}},
    { static_cast<Token::Type>('['), {        TA_SHIFT_AND_PUSH_STATE,   13}},
    // nonterminal transitions
    {            Token::__structure, {                  TA_PUSH_STATE,   14}},
    {                Token::__array, {                  TA_PUSH_STATE,   15}},
    {                Token::__value, {                  TA_PUSH_STATE,   16}},
    {               Token::__string, {                  TA_PUSH_STATE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state    5
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    2}},

// ///////////////////////////////////////////////////////////////////////////
// state    6
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>(';'), {        TA_SHIFT_AND_PUSH_STATE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state    7
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   13}},

// ///////////////////////////////////////////////////////////////////////////
// state    8
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   14}},

// ///////////////////////////////////////////////////////////////////////////
// state    9
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   15}},

// ///////////////////////////////////////////////////////////////////////////
// state   10
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   16}},

// ///////////////////////////////////////////////////////////////////////////
// state   11
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state   12
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    3}},
    // nonterminal transitions
    {         Token::__element_list, {                  TA_PUSH_STATE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state   13
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::BOOLEAN, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                Token::INTEGER, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::CHARACTER, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {        Token::STRING_FRAGMENT, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {             Token::IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,    4}},
    { static_cast<Token::Type>('{'), {        TA_SHIFT_AND_PUSH_STATE,   12}},
    { static_cast<Token::Type>('['), {        TA_SHIFT_AND_PUSH_STATE,   13}},
    { static_cast<Token::Type>(']'), {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {             Token::__key_pair, {                  TA_PUSH_STATE,   21}},
    {            Token::__structure, {                  TA_PUSH_STATE,   14}},
    {                Token::__array, {                  TA_PUSH_STATE,   15}},
    {                 Token::__list, {                  TA_PUSH_STATE,   22}},
    {         Token::__list_element, {                  TA_PUSH_STATE,   23}},
    {                Token::__value, {                  TA_PUSH_STATE,   24}},
    {               Token::__string, {                  TA_PUSH_STATE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state   14
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state   15
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state   16
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    5}},

// ///////////////////////////////////////////////////////////////////////////
// state   17
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {        Token::STRING_FRAGMENT, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state   18
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    4}},

// ///////////////////////////////////////////////////////////////////////////
// state   19
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {             Token::IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,    4}},
    { static_cast<Token::Type>('}'), {        TA_SHIFT_AND_PUSH_STATE,   26}},
    // nonterminal transitions
    {              Token::__element, {                  TA_PUSH_STATE,    5}},
    {             Token::__key_pair, {                  TA_PUSH_STATE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state   20
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state   21
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   11}},

// ///////////////////////////////////////////////////////////////////////////
// state   22
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>(','), {        TA_SHIFT_AND_PUSH_STATE,   27}},
    { static_cast<Token::Type>(']'), {        TA_SHIFT_AND_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   23
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   10}},

// ///////////////////////////////////////////////////////////////////////////
// state   24
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   12}},

// ///////////////////////////////////////////////////////////////////////////
// state   25
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,   20}},

// ///////////////////////////////////////////////////////////////////////////
// state   26
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state   27
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::BOOLEAN, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                Token::INTEGER, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::CHARACTER, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {        Token::STRING_FRAGMENT, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {             Token::IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,    4}},
    { static_cast<Token::Type>('{'), {        TA_SHIFT_AND_PUSH_STATE,   12}},
    { static_cast<Token::Type>('['), {        TA_SHIFT_AND_PUSH_STATE,   13}},
    // nonterminal transitions
    {             Token::__key_pair, {                  TA_PUSH_STATE,   21}},
    {            Token::__structure, {                  TA_PUSH_STATE,   14}},
    {                Token::__array, {                  TA_PUSH_STATE,   15}},
    {         Token::__list_element, {                  TA_PUSH_STATE,   29}},
    {                Token::__value, {                  TA_PUSH_STATE,   24}},
    {               Token::__string, {                  TA_PUSH_STATE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state   28
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state   29
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::_DEFAULT, {           TA_REDUCE_USING_RULE,    9}}

};

unsigned int const DataFileParser::ms_state_transition_count =
    sizeof(DataFileParser::ms_state_transition) /
    sizeof(DataFileParser::StateTransition);


#line 68 "../../lib/parsers/xrb_datafileparser.trison"

bool DataFileParser::SetInputFilename (std::string const &input_filename)
{
    assert(m_scanner != NULL);
    m_scanner->Close();
    // TODO: make sure there are no pipe characters in the input filename
    return m_scanner->Open(input_filename);
}

DataFileParser::Token::Type DataFileParser::Scan ()
{
    assert(m_scanner != NULL);
    return m_scanner->Scan(&m_lookahead_token);
}

} // end of namespace Xrb

#line 1190 "../../lib/parsers/xrb_datafileparser.cpp"

