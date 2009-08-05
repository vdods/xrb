#include "xrb_datafileparser.h"

#include <cstdio>
#include <iomanip>
#include <iostream>

#define DEBUG_SPEW_1(x) if (m_debug_spew_level >= 1) std::cerr << x
#define DEBUG_SPEW_2(x) if (m_debug_spew_level >= 2) std::cerr << x


#line 69 "xrb_datafileparser.trison"

#include <sstream>

#include "xrb_datafilelocation.hpp"
#include "xrb_datafilescanner.hpp"
#include "xrb_datafilevalue.hpp"

#undef FL
#define FL DataFileLocation(m_scanner->GetInputFilename(), m_scanner->GetLineNumber())

namespace Xrb
{

#line 26 "xrb_datafileparser.cpp"

DataFileParser::DataFileParser ()

{

#line 83 "xrb_datafileparser.trison"

    m_scanner = new DataFileScanner();

#line 36 "xrb_datafileparser.cpp"
    m_debug_spew_level = 0;
    DEBUG_SPEW_2("### number of state transitions = " << ms_state_transition_count << std::endl);
    m_reduction_token = NULL;
}

DataFileParser::~DataFileParser ()
{

#line 87 "xrb_datafileparser.trison"

    ASSERT1(m_scanner != NULL);
    Delete(m_scanner);
    delete StealAcceptedStructure();

#line 51 "xrb_datafileparser.cpp"
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

DataFileParser::ParserReturnCode DataFileParser::Parse ()
{

#line 93 "xrb_datafileparser.trison"

    delete StealAcceptedStructure();

#line 92 "xrb_datafileparser.cpp"

    ParserReturnCode return_code = PrivateParse();


#line 97 "xrb_datafileparser.trison"

    m_scanner->Close();

#line 101 "xrb_datafileparser.cpp"

    return return_code;
}

bool DataFileParser::DoesStateAcceptErrorToken (DataFileParser::StateNumber state_number) const
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

DataFileParser::ParserReturnCode DataFileParser::PrivateParse ()
{
    m_state_stack.clear();
    m_token_stack.clear();

    m_lookahead_token_type = Token::INVALID_;
    m_lookahead_token = NULL;
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
    assert(m_lookahead_token_type != Token::DEFAULT_);
    assert(m_lookahead_token_type != Token::INVALID_);
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

#line 101 "xrb_datafileparser.trison"

    Delete(token);

#line 423 "xrb_datafileparser.cpp"
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
        "BAD_TOKEN",
        "BOOLEAN",
        "CHARACTER",
        "FLOAT",
        "IDENTIFIER",
        "SINT32",
        "STRING_FRAGMENT",
        "UINT32",
        "END_",

        "array",
        "data_file",
        "element",
        "element_list",
        "string",
        "structure",
        "value",
        "value_list",
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

// rule 0: %start <- data_file END_    
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

#line 185 "xrb_datafileparser.trison"

        ASSERT1(element_list != NULL);
        return element_list;
    
#line 506 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 2: data_file <- %error    
DataFileValue * DataFileParser::ReductionRuleHandler0002 ()
{

#line 191 "xrb_datafileparser.trison"

        EmitError(FL, "general syntax error");
        return new DataFileStructure();
    
#line 519 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 3: element_list <- element_list:element_list element:element    
DataFileValue * DataFileParser::ReductionRuleHandler0003 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileStructure * element_list = DStaticCast< DataFileStructure * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileKeyPair * element = DStaticCast< DataFileKeyPair * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 200 "xrb_datafileparser.trison"

        ASSERT1(element_list != NULL);

        if (element != NULL)
        {
            try
            {
                element_list->AddKeyPair(element);
            }
            catch (std::string const &exception)
            {
                EmitError(FL, exception);
                Delete(element);
            }
        }
        return element_list;
    
#line 549 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 4: element_list <-     
DataFileValue * DataFileParser::ReductionRuleHandler0004 ()
{

#line 218 "xrb_datafileparser.trison"

        return new DataFileStructure();
    
#line 561 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 5: element <- IDENTIFIER:key value:value ';'    
DataFileValue * DataFileParser::ReductionRuleHandler0005 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * key = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileValue * value = DStaticCast< DataFileValue * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 226 "xrb_datafileparser.trison"

        ASSERT1(key != NULL);
        if (value == NULL)
        {
            Delete(key);
            return NULL;
        }

        DataFileKeyPair *key_pair = new DataFileKeyPair(key->GetValue(), value);
        Delete(key);
        return key_pair;
    
#line 586 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 6: element <- IDENTIFIER:key %error ';'    
DataFileValue * DataFileParser::ReductionRuleHandler0006 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * key = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 240 "xrb_datafileparser.trison"

        ASSERT1(key != NULL);
        std::ostringstream out;
        out << "syntax error in element with key \"" << key->GetValue() << "\"";
        EmitError(FL, out.str());
        Delete(key);
        return NULL;
    
#line 605 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 7: element <- %error ';'    
DataFileValue * DataFileParser::ReductionRuleHandler0007 ()
{

#line 250 "xrb_datafileparser.trison"

        EmitError(FL, "syntax error in element");
        return NULL;
    
#line 618 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 8: structure <- '{' element_list:element_list '}'    
DataFileValue * DataFileParser::ReductionRuleHandler0008 ()
{
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileStructure * element_list = DStaticCast< DataFileStructure * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 259 "xrb_datafileparser.trison"

        ASSERT1(element_list != NULL);
        return element_list;
    
#line 633 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 9: structure <- '{' %error '}'    
DataFileValue * DataFileParser::ReductionRuleHandler0009 ()
{

#line 265 "xrb_datafileparser.trison"

        EmitError(FL, "syntax error in structure");
        return new DataFileStructure();
    
#line 646 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 10: array <- '[' value_list:value_list ']'    
DataFileValue * DataFileParser::ReductionRuleHandler0010 ()
{
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileArray * value_list = DStaticCast< DataFileArray * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 274 "xrb_datafileparser.trison"

        ASSERT1(value_list != NULL);
        return value_list;
    
#line 661 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 11: array <- '[' value_list:value_list ',' ']'    
DataFileValue * DataFileParser::ReductionRuleHandler0011 ()
{
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileArray * value_list = DStaticCast< DataFileArray * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 280 "xrb_datafileparser.trison"

        ASSERT1(value_list != NULL);
        return value_list;
    
#line 676 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 12: array <- '[' ']'    
DataFileValue * DataFileParser::ReductionRuleHandler0012 ()
{

#line 286 "xrb_datafileparser.trison"

        return new DataFileArray();
    
#line 688 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 13: array <- '[' %error ']'    
DataFileValue * DataFileParser::ReductionRuleHandler0013 ()
{

#line 291 "xrb_datafileparser.trison"

        EmitError(FL, "syntax error in array");
        return NULL;
    
#line 701 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 14: value_list <- value_list:value_list ',' value:value    
DataFileValue * DataFileParser::ReductionRuleHandler0014 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileArray * value_list = DStaticCast< DataFileArray * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(2) < m_reduction_rule_token_count);
    DataFileValue * value = DStaticCast< DataFileValue * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 300 "xrb_datafileparser.trison"

        ASSERT1(value_list != NULL);
        if (value != NULL)
        {
            try
            {
                value_list->AppendValue(value);
            }
            catch (std::string const &exception)
            {
                EmitError(FL, exception);
                Delete(value);
            }
        }
        return value_list;
    
#line 730 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 15: value_list <- value:value    
DataFileValue * DataFileParser::ReductionRuleHandler0015 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileValue * value = DStaticCast< DataFileValue * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 318 "xrb_datafileparser.trison"

        DataFileArray *value_list = new DataFileArray();
        if (value != NULL)
        {
            try
            {
                value_list->AppendValue(value);
            }
            catch (std::string const &)
            {
                ASSERT1(false && "this should never happen");
                Delete(value);
            }
        }
        return value_list;
    
#line 757 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 16: value <- BOOLEAN:value    
DataFileValue * DataFileParser::ReductionRuleHandler0016 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileBoolean * value = DStaticCast< DataFileBoolean * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 339 "xrb_datafileparser.trison"

        return value;
    
#line 771 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 17: value <- SINT32:value    
DataFileValue * DataFileParser::ReductionRuleHandler0017 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileSint32 * value = DStaticCast< DataFileSint32 * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 344 "xrb_datafileparser.trison"

        return value;
    
#line 785 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 18: value <- UINT32:value    
DataFileValue * DataFileParser::ReductionRuleHandler0018 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileUint32 * value = DStaticCast< DataFileUint32 * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 349 "xrb_datafileparser.trison"

        return value;
    
#line 799 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 19: value <- FLOAT:value    
DataFileValue * DataFileParser::ReductionRuleHandler0019 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileFloat * value = DStaticCast< DataFileFloat * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 354 "xrb_datafileparser.trison"

        return value;
    
#line 813 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 20: value <- CHARACTER:value    
DataFileValue * DataFileParser::ReductionRuleHandler0020 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileCharacter * value = DStaticCast< DataFileCharacter * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 359 "xrb_datafileparser.trison"

        return value;
    
#line 827 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 21: value <- string:value    
DataFileValue * DataFileParser::ReductionRuleHandler0021 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * value = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 364 "xrb_datafileparser.trison"

        return value;
    
#line 841 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 22: value <- structure:value    
DataFileValue * DataFileParser::ReductionRuleHandler0022 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileStructure * value = DStaticCast< DataFileStructure * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 369 "xrb_datafileparser.trison"

        return value;
    
#line 855 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 23: value <- array:value    
DataFileValue * DataFileParser::ReductionRuleHandler0023 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileArray * value = DStaticCast< DataFileArray * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 374 "xrb_datafileparser.trison"

        return value;
    
#line 869 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 24: string <- string:string '+' STRING_FRAGMENT:string_fragment    
DataFileValue * DataFileParser::ReductionRuleHandler0024 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * string = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(2) < m_reduction_rule_token_count);
    DataFileString * string_fragment = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 390 "xrb_datafileparser.trison"

        ASSERT1(string != NULL);
        ASSERT1(string_fragment != NULL);
        string->AppendString(string_fragment->GetValue());
        Delete(string_fragment);
        return string;
    
#line 889 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 25: string <- STRING_FRAGMENT:string_fragment    
DataFileValue * DataFileParser::ReductionRuleHandler0025 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * string_fragment = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 399 "xrb_datafileparser.trison"

        ASSERT1(string_fragment != NULL);
        return string_fragment;
    
#line 904 "xrb_datafileparser.cpp"
    return NULL;
}

// rule 26: string <- string:string STRING_FRAGMENT:string_fragment    
DataFileValue * DataFileParser::ReductionRuleHandler0026 ()
{
    assert(static_cast<unsigned int>(0) < m_reduction_rule_token_count);
    DataFileString * string = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(static_cast<unsigned int>(1) < m_reduction_rule_token_count);
    DataFileString * string_fragment = DStaticCast< DataFileString * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 405 "xrb_datafileparser.trison"

        ASSERT1(string != NULL);
        ASSERT1(string_fragment != NULL);
        string->AppendString(string_fragment->GetValue());
        Delete(string_fragment);
        EmitError(FL, "use + to concatenate strings (or did you forget a comma?)");
        return string;
    
#line 925 "xrb_datafileparser.cpp"
    return NULL;
}



// ///////////////////////////////////////////////////////////////////////////
// reduction rule lookup table
// ///////////////////////////////////////////////////////////////////////////

DataFileParser::ReductionRule const DataFileParser::ms_reduction_rule[] =
{
    {                 Token::START_,  2, &DataFileParser::ReductionRuleHandler0000, "rule 0: %start <- data_file END_    "},
    {            Token::data_file__,  1, &DataFileParser::ReductionRuleHandler0001, "rule 1: data_file <- element_list    "},
    {            Token::data_file__,  1, &DataFileParser::ReductionRuleHandler0002, "rule 2: data_file <- %error    "},
    {         Token::element_list__,  2, &DataFileParser::ReductionRuleHandler0003, "rule 3: element_list <- element_list element    "},
    {         Token::element_list__,  0, &DataFileParser::ReductionRuleHandler0004, "rule 4: element_list <-     "},
    {              Token::element__,  3, &DataFileParser::ReductionRuleHandler0005, "rule 5: element <- IDENTIFIER value ';'    "},
    {              Token::element__,  3, &DataFileParser::ReductionRuleHandler0006, "rule 6: element <- IDENTIFIER %error ';'    "},
    {              Token::element__,  2, &DataFileParser::ReductionRuleHandler0007, "rule 7: element <- %error ';'    "},
    {            Token::structure__,  3, &DataFileParser::ReductionRuleHandler0008, "rule 8: structure <- '{' element_list '}'    "},
    {            Token::structure__,  3, &DataFileParser::ReductionRuleHandler0009, "rule 9: structure <- '{' %error '}'    "},
    {                Token::array__,  3, &DataFileParser::ReductionRuleHandler0010, "rule 10: array <- '[' value_list ']'    "},
    {                Token::array__,  4, &DataFileParser::ReductionRuleHandler0011, "rule 11: array <- '[' value_list ',' ']'    "},
    {                Token::array__,  2, &DataFileParser::ReductionRuleHandler0012, "rule 12: array <- '[' ']'    "},
    {                Token::array__,  3, &DataFileParser::ReductionRuleHandler0013, "rule 13: array <- '[' %error ']'    "},
    {           Token::value_list__,  3, &DataFileParser::ReductionRuleHandler0014, "rule 14: value_list <- value_list ',' value    "},
    {           Token::value_list__,  1, &DataFileParser::ReductionRuleHandler0015, "rule 15: value_list <- value    "},
    {                Token::value__,  1, &DataFileParser::ReductionRuleHandler0016, "rule 16: value <- BOOLEAN    "},
    {                Token::value__,  1, &DataFileParser::ReductionRuleHandler0017, "rule 17: value <- SINT32    "},
    {                Token::value__,  1, &DataFileParser::ReductionRuleHandler0018, "rule 18: value <- UINT32    "},
    {                Token::value__,  1, &DataFileParser::ReductionRuleHandler0019, "rule 19: value <- FLOAT    "},
    {                Token::value__,  1, &DataFileParser::ReductionRuleHandler0020, "rule 20: value <- CHARACTER    "},
    {                Token::value__,  1, &DataFileParser::ReductionRuleHandler0021, "rule 21: value <- string    "},
    {                Token::value__,  1, &DataFileParser::ReductionRuleHandler0022, "rule 22: value <- structure    "},
    {                Token::value__,  1, &DataFileParser::ReductionRuleHandler0023, "rule 23: value <- array    "},
    {               Token::string__,  3, &DataFileParser::ReductionRuleHandler0024, "rule 24: string <- string '+' STRING_FRAGMENT    "},
    {               Token::string__,  1, &DataFileParser::ReductionRuleHandler0025, "rule 25: string <- STRING_FRAGMENT    "},
    {               Token::string__,  2, &DataFileParser::ReductionRuleHandler0026, "rule 26: string <- string STRING_FRAGMENT    "},

    // special error panic reduction rule
    {                 Token::ERROR_,  1,                                     NULL, "* -> error"}
};

unsigned int const DataFileParser::ms_reduction_rule_count =
    sizeof(DataFileParser::ms_reduction_rule) /
    sizeof(DataFileParser::ReductionRule);

// ///////////////////////////////////////////////////////////////////////////
// state transition lookup table
// ///////////////////////////////////////////////////////////////////////////

DataFileParser::State const DataFileParser::ms_state[] =
{
    {   1,    4,    0,    5,    2}, // state    0
    {   7,    2,    0,    0,    0}, // state    1
    {   9,    1,    0,    0,    0}, // state    2
    {  10,    3,    0,   13,    1}, // state    3
    {   0,    0,   14,    0,    0}, // state    4
    {  15,    2,    0,    0,    0}, // state    5
    {  17,    9,    0,   26,    4}, // state    6
    {   0,    0,   30,    0,    0}, // state    7
    {   0,    0,   31,    0,    0}, // state    8
    {  32,    2,    0,    0,    0}, // state    9
    {   0,    0,   34,    0,    0}, // state   10
    {   0,    0,   35,    0,    0}, // state   11
    {   0,    0,   36,    0,    0}, // state   12
    {   0,    0,   37,    0,    0}, // state   13
    {   0,    0,   38,    0,    0}, // state   14
    {   0,    0,   39,    0,    0}, // state   15
    {  40,    4,    0,   44,    1}, // state   16
    {  45,   10,    0,   55,    5}, // state   17
    {   0,    0,   60,    0,    0}, // state   18
    {   0,    0,   61,    0,    0}, // state   19
    {  62,    1,    0,    0,    0}, // state   20
    {  63,    2,   65,    0,    0}, // state   21
    {   0,    0,   66,    0,    0}, // state   22
    {  67,    2,    0,    0,    0}, // state   23
    {  69,    3,    0,   72,    1}, // state   24
    {  73,    2,    0,    0,    0}, // state   25
    {   0,    0,   75,    0,    0}, // state   26
    {  76,    2,    0,    0,    0}, // state   27
    {   0,    0,   78,    0,    0}, // state   28
    {   0,    0,   79,    0,    0}, // state   29
    {   0,    0,   80,    0,    0}, // state   30
    {  81,    1,    0,    0,    0}, // state   31
    {   0,    0,   82,    0,    0}, // state   32
    {   0,    0,   83,    0,    0}, // state   33
    {   0,    0,   84,    0,    0}, // state   34
    {  85,    9,    0,   94,    4}, // state   35
    {   0,    0,   98,    0,    0}, // state   36
    {   0,    0,   99,    0,    0}, // state   37
    {   0,    0,  100,    0,    0}, // state   38
    {   0,    0,  101,    0,    0}  // state   39

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
    {               Token::INVALID_, {                       TA_COUNT,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state    0
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {           TA_REDUCE_USING_RULE,    4}},
    {                 Token::ERROR_, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {             Token::IDENTIFIER, {           TA_REDUCE_USING_RULE,    4}},
    { static_cast<Token::Type>('}'), {           TA_REDUCE_USING_RULE,    4}},
    // nonterminal transitions
    {            Token::data_file__, {                  TA_PUSH_STATE,    2}},
    {         Token::element_list__, {                  TA_PUSH_STATE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state    1
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {           TA_REDUCE_USING_RULE,    2}},
    {                 Token::ERROR_, {  TA_THROW_AWAY_LOOKAHEAD_TOKEN,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state    2
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {        TA_SHIFT_AND_PUSH_STATE,    4}},

// ///////////////////////////////////////////////////////////////////////////
// state    3
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {           TA_REDUCE_USING_RULE,    1}},
    {                 Token::ERROR_, {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {             Token::IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    // nonterminal transitions
    {              Token::element__, {                  TA_PUSH_STATE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state    4
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {TA_REDUCE_AND_ACCEPT_USING_RULE,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state    5
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                 Token::ERROR_, {  TA_THROW_AWAY_LOOKAHEAD_TOKEN,    0}},
    { static_cast<Token::Type>(';'), {        TA_SHIFT_AND_PUSH_STATE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state    6
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                 Token::ERROR_, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                Token::BOOLEAN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                 Token::SINT32, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                 Token::UINT32, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {              Token::CHARACTER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {        Token::STRING_FRAGMENT, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    { static_cast<Token::Type>('{'), {        TA_SHIFT_AND_PUSH_STATE,   16}},
    { static_cast<Token::Type>('['), {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {            Token::structure__, {                  TA_PUSH_STATE,   18}},
    {                Token::array__, {                  TA_PUSH_STATE,   19}},
    {                Token::value__, {                  TA_PUSH_STATE,   20}},
    {               Token::string__, {                  TA_PUSH_STATE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state    7
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state    8
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state    9
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                 Token::ERROR_, {  TA_THROW_AWAY_LOOKAHEAD_TOKEN,    0}},
    { static_cast<Token::Type>(';'), {        TA_SHIFT_AND_PUSH_STATE,   22}},

// ///////////////////////////////////////////////////////////////////////////
// state   10
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   16}},

// ///////////////////////////////////////////////////////////////////////////
// state   11
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state   12
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state   13
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state   14
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   20}},

// ///////////////////////////////////////////////////////////////////////////
// state   15
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   25}},

// ///////////////////////////////////////////////////////////////////////////
// state   16
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {           TA_REDUCE_USING_RULE,    4}},
    {                 Token::ERROR_, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {             Token::IDENTIFIER, {           TA_REDUCE_USING_RULE,    4}},
    { static_cast<Token::Type>('}'), {           TA_REDUCE_USING_RULE,    4}},
    // nonterminal transitions
    {         Token::element_list__, {                  TA_PUSH_STATE,   24}},

// ///////////////////////////////////////////////////////////////////////////
// state   17
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                 Token::ERROR_, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    {                Token::BOOLEAN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                 Token::SINT32, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                 Token::UINT32, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {              Token::CHARACTER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {        Token::STRING_FRAGMENT, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    { static_cast<Token::Type>('{'), {        TA_SHIFT_AND_PUSH_STATE,   16}},
    { static_cast<Token::Type>('['), {        TA_SHIFT_AND_PUSH_STATE,   17}},
    { static_cast<Token::Type>(']'), {        TA_SHIFT_AND_PUSH_STATE,   26}},
    // nonterminal transitions
    {            Token::structure__, {                  TA_PUSH_STATE,   18}},
    {                Token::array__, {                  TA_PUSH_STATE,   19}},
    {           Token::value_list__, {                  TA_PUSH_STATE,   27}},
    {                Token::value__, {                  TA_PUSH_STATE,   28}},
    {               Token::string__, {                  TA_PUSH_STATE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state   18
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   22}},

// ///////////////////////////////////////////////////////////////////////////
// state   19
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   23}},

// ///////////////////////////////////////////////////////////////////////////
// state   20
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>(';'), {        TA_SHIFT_AND_PUSH_STATE,   29}},

// ///////////////////////////////////////////////////////////////////////////
// state   21
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {        Token::STRING_FRAGMENT, {        TA_SHIFT_AND_PUSH_STATE,   30}},
    { static_cast<Token::Type>('+'), {        TA_SHIFT_AND_PUSH_STATE,   31}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state   22
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state   23
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                 Token::ERROR_, {  TA_THROW_AWAY_LOOKAHEAD_TOKEN,    0}},
    { static_cast<Token::Type>('}'), {        TA_SHIFT_AND_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   24
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                 Token::ERROR_, {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {             Token::IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    { static_cast<Token::Type>('}'), {        TA_SHIFT_AND_PUSH_STATE,   33}},
    // nonterminal transitions
    {              Token::element__, {                  TA_PUSH_STATE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state   25
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                 Token::ERROR_, {  TA_THROW_AWAY_LOOKAHEAD_TOKEN,    0}},
    { static_cast<Token::Type>(']'), {        TA_SHIFT_AND_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   26
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   12}},

// ///////////////////////////////////////////////////////////////////////////
// state   27
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    { static_cast<Token::Type>(','), {        TA_SHIFT_AND_PUSH_STATE,   35}},
    { static_cast<Token::Type>(']'), {        TA_SHIFT_AND_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state   28
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   15}},

// ///////////////////////////////////////////////////////////////////////////
// state   29
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    5}},

// ///////////////////////////////////////////////////////////////////////////
// state   30
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   26}},

// ///////////////////////////////////////////////////////////////////////////
// state   31
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {        Token::STRING_FRAGMENT, {        TA_SHIFT_AND_PUSH_STATE,   37}},

// ///////////////////////////////////////////////////////////////////////////
// state   32
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    9}},

// ///////////////////////////////////////////////////////////////////////////
// state   33
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state   34
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   13}},

// ///////////////////////////////////////////////////////////////////////////
// state   35
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                Token::BOOLEAN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                 Token::SINT32, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                 Token::UINT32, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {              Token::CHARACTER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {        Token::STRING_FRAGMENT, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    { static_cast<Token::Type>('{'), {        TA_SHIFT_AND_PUSH_STATE,   16}},
    { static_cast<Token::Type>('['), {        TA_SHIFT_AND_PUSH_STATE,   17}},
    { static_cast<Token::Type>(']'), {        TA_SHIFT_AND_PUSH_STATE,   38}},
    // nonterminal transitions
    {            Token::structure__, {                  TA_PUSH_STATE,   18}},
    {                Token::array__, {                  TA_PUSH_STATE,   19}},
    {                Token::value__, {                  TA_PUSH_STATE,   39}},
    {               Token::string__, {                  TA_PUSH_STATE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state   36
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   10}},

// ///////////////////////////////////////////////////////////////////////////
// state   37
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   24}},

// ///////////////////////////////////////////////////////////////////////////
// state   38
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   11}},

// ///////////////////////////////////////////////////////////////////////////
// state   39
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   14}}

};

unsigned int const DataFileParser::ms_state_transition_count =
    sizeof(DataFileParser::ms_state_transition) /
    sizeof(DataFileParser::StateTransition);


#line 105 "xrb_datafileparser.trison"

DataFileParser::ReturnCode DataFileParser::Parse (std::string const &input_filename)
{
    ASSERT1(m_scanner != NULL);
    ASSERT1(!m_scanner->IsOpen());
    // if there are any pipe characters in the filename, return failure
    if (input_filename.find_first_of("|") != static_cast<std::string::size_type>(-1))
        return RC_INVALID_FILENAME;

    // attempt to open the filename and return the result
    if (!m_scanner->Open(input_filename))
        return RC_FILE_OPEN_FAILURE;

    // start parsing, and return parse error if we got an unhandled error
    if (Parse() == PRC_UNHANDLED_PARSE_ERROR)
        return RC_PARSE_ERROR;

    // if any errors were encountered, return parse error
    if (m_scanner->GetWereErrorsEncountered())
        return RC_ERRORS_ENCOUNTERED;

    // otherwise return success
    return RC_SUCCESS;
}

DataFileParser::Token::Type DataFileParser::Scan ()
{
    ASSERT1(m_scanner != NULL);
    return m_scanner->Scan(&m_lookahead_token);
}

void DataFileParser::EmitWarning (std::string const &message)
{
    ASSERT1(m_scanner != NULL);
    m_scanner->EmitWarning(message);
}

void DataFileParser::EmitWarning (DataFileLocation const &file_location, std::string const &message)
{
    ASSERT1(m_scanner != NULL);
    m_scanner->EmitWarning(file_location, message);
}

void DataFileParser::EmitError (std::string const &message)
{
    ASSERT1(m_scanner != NULL);
    m_scanner->EmitError(message);
}

void DataFileParser::EmitError (DataFileLocation const &file_location, std::string const &message)
{
    ASSERT1(m_scanner != NULL);
    m_scanner->EmitError(file_location, message);
}

} // end of namespace Xrb

#line 1409 "xrb_datafileparser.cpp"

