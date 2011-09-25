// ///////////////////////////////////////////////////////////////////////////
// xrb_statemachine.hpp by Victor Dods, created 2006/03/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_STATEMACHINE_HPP_)
#define _XRB_STATEMACHINE_HPP_

#include "xrb.hpp"

#include "xrb_emptystring.hpp"

namespace Xrb {

// StateMachine mechanism inputs
enum
{
    SM_ENTER                    = static_cast<StateMachineInput>(-1),
    SM_EXIT                     = static_cast<StateMachineInput>(-2),
    SM_INVALID                  = static_cast<StateMachineInput>(-3),
    SM_HIGHEST_USER_INPUT_VALUE = static_cast<StateMachineInput>(-4)
};

// this class is to be used in composition with another, as opposed to
// using it via inheritance.
template <typename OwnerClass>
class StateMachine
{
public:

    typedef bool (OwnerClass::*State)(StateMachineInput);

    /// @brief Constructs a state machine, indicating the owner class.
    /// @details The name of the owner class for purposes transition_logger_name varThe name variable
    /// name is used for transition logging.  NULL indicates that no logging should be done.
    StateMachine (OwnerClass *owner_class, std::string const &owner_class_name);
    ~StateMachine ();

    bool IsInitialized () const { return m_current_state != NULL; }
    bool IsTransitionLoggerEnabled () const { return m_transition_logger != NULL; }
    std::ostream *TransitionLogger () const { return m_transition_logger; }
    StateMachineInput TransitionLoggerIgnoreInput () const { return m_transition_logger_ignore_input; }
    State CurrentState () const { return m_current_state; }
    /// Will return the empty string if CurrentState returns NULL.
    std::string const &CurrentStateName () const { return m_current_state_name; }
    State NextState () const { return m_next_state; }
    /// Will return the empty string if NextState returns NULL.
    std::string const &NextStateName () const { return m_next_state_name; }

    /// initial_state_name is used for transition logging.
    void Initialize (State initial_state, std::string const &initial_state_name);
    /// Specifying NULL for transition_logger indicates that logging will be disabled.
    void SetTransitionLogger (std::ostream *transition_logger, StateMachineInput transition_logger_ignore_input = SM_INVALID);
    void RunCurrentState (StateMachineInput input);
    void Shutdown ();

    /// state_name is used for transition logging.
    void SetNextState (State state, std::string const &state_name);

private:

    void LogTransition (StateMachineInput input) const;
    void RunCurrentStatePrivate (StateMachineInput input);

    OwnerClass *const m_owner_class;
    std::string const m_owner_class_name;
    std::ostream *m_transition_logger;
    StateMachineInput m_transition_logger_ignore_input;
    bool m_is_running_a_state;
    State m_current_state;
    std::string m_current_state_name;
    State m_next_state;
    std::string m_next_state_name;
}; // end of class StateMachine

template <typename OwnerClass>
StateMachine<OwnerClass>::StateMachine (OwnerClass *owner_class, std::string const &owner_class_name)
    :
    m_owner_class(owner_class),
    m_owner_class_name(owner_class_name),
    m_transition_logger(NULL),
    m_transition_logger_ignore_input(SM_INVALID)
{
    ASSERT1(m_owner_class != NULL);
//     ASSERT1(!m_owner_class_name.empty());
    m_is_running_a_state = false;
    m_current_state = NULL;
    m_next_state = NULL;
}

template <typename OwnerClass>
StateMachine<OwnerClass>::~StateMachine ()
{
    Shutdown();
}

template <typename OwnerClass>
void StateMachine<OwnerClass>::Initialize (State initial_state, std::string const &initial_state_name)
{
    // just make sure this happens only once at the beginning
    ASSERT1(!m_is_running_a_state && "This method should not be used from inside a state");
    ASSERT1(m_current_state == NULL && "This state machine is already initialized");

    // set the current state and run it with SM_ENTER.
    m_current_state = initial_state;
    m_current_state_name = initial_state_name;
    this->RunCurrentStatePrivate(SM_ENTER);
}

template <typename OwnerClass>
void StateMachine<OwnerClass>::SetTransitionLogger (std::ostream *transition_logger, StateMachineInput transition_logger_ignore_input)
{
    ASSERT1(transition_logger_ignore_input == SM_INVALID || transition_logger_ignore_input <= SM_HIGHEST_USER_INPUT_VALUE);
    m_transition_logger = transition_logger;
    m_transition_logger_ignore_input = transition_logger_ignore_input;
}

template <typename OwnerClass>
void StateMachine<OwnerClass>::RunCurrentState (StateMachineInput input)
{
    ASSERT1(input <= SM_HIGHEST_USER_INPUT_VALUE && "Users are not allowed to send state-machine-defined input");
    this->RunCurrentStatePrivate(input);
}

template <typename OwnerClass>
void StateMachine<OwnerClass>::Shutdown ()
{
    ASSERT1(!m_is_running_a_state);

    // only actually shutdown if we're not already shutdown.
    if (m_current_state != NULL)
    {
        // run the current state with SM_EXIT and nullify it.
        this->RunCurrentStatePrivate(SM_EXIT);
        m_current_state = NULL;
        m_current_state_name.clear();
    }
}

template <typename OwnerClass>
void StateMachine<OwnerClass>::SetNextState (State state, std::string const &state_name)
{
    ASSERT1(m_is_running_a_state && "This method should only be used from inside a state");
    ASSERT1(m_current_state != NULL && "This state machine has not been initialized");

    // note: a NULL value for state is acceptable.  it is
    // effectively canceling an earlier requested transition.
    m_next_state = state;
    m_next_state_name = state_name;
}

template <typename OwnerClass>
void StateMachine<OwnerClass>::LogTransition (StateMachineInput input) const
{
    ASSERT1(input != SM_INVALID);
    
    if (m_transition_logger == NULL || input == m_transition_logger_ignore_input)
        return; // logging disabled or we want to ignore this particular input

    *m_transition_logger << m_owner_class_name << ": ";
    if (input == SM_ENTER)
        *m_transition_logger << "--> " << m_current_state_name;
    else if (input == SM_EXIT)
        *m_transition_logger << "<-- " << m_current_state_name;
    else
        *m_transition_logger << "input: " << input;
    *m_transition_logger << std::endl;
}

template <typename OwnerClass>
void StateMachine<OwnerClass>::RunCurrentStatePrivate (StateMachineInput input)
{
    ASSERT1(!m_is_running_a_state && "This method should not be used from inside a state");
    ASSERT1(m_current_state != NULL && "This state machine has not been initialized");
    
    // NULL is a sentinel value so we know if the state has transitioned
    m_next_state = NULL;
    m_next_state_name.clear();

    // if the state return true, the input was handled.  otherwise not.
    m_is_running_a_state = true;
    LogTransition(input);
    bool state_handled_the_input = (m_owner_class->*m_current_state)(input);
    m_is_running_a_state = false;
    // make sure that states always handle all input (with the exception of
    // the StateMachine mechanism inputs)
    if (input <= SM_HIGHEST_USER_INPUT_VALUE)
        ASSERT0(state_handled_the_input && "All user-defined state machine input must be handled");

    if (input == SM_EXIT && m_next_state != NULL)
        ASSERT0(false && "You must not transition while exiting a state");
        
    // if a transition was requested, perform the necessary exit/enter machinery.
    // this is a while-loop because you can transition during SM_ENTER.
    while (m_next_state != NULL)
    {
        // we have to save off the next state, because the forthcoming SM_EXIT
        // call to the current state might change m_next_state, and we need to
        // detect if they try to transition on SM_EXIT (which is not allowed).
        State real_next_state = m_next_state;
        std::string real_next_state_name = m_next_state_name;
        m_next_state = NULL;
        m_next_state_name.clear();
        // call the current state with SM_EXIT, ignoring the return value
        m_is_running_a_state = true;
        LogTransition(SM_EXIT);
        (m_owner_class->*m_current_state)(SM_EXIT);
        m_is_running_a_state = false;
        // if they requested a transition, assert
        ASSERT0(m_next_state == NULL && "You must not transition while exiting a state");

        // set the current state to the new state
        m_current_state = real_next_state;
        m_current_state_name = real_next_state_name;
        // call the current state with SM_ENTER, ignoring the return value
        m_is_running_a_state = true;
        LogTransition(SM_ENTER);
        (m_owner_class->*m_current_state)(SM_ENTER);
        m_is_running_a_state = false;
    }

    // since m_next_state is NULL, clear the next state name
    m_next_state_name.clear();
}

} // end of namespace Xrb

#endif // !defined(_XRB_STATEMACHINE_HPP_)

