// ///////////////////////////////////////////////////////////////////////////
// xrb_statemachine.cpp by Victor Dods, created 2006/03/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_statemachine.h"

namespace Xrb
{

StateMachine::StateMachine (StateMachineHandler *const owner_class)
{
    ASSERT1(owner_class != NULL)
    m_owner_class = owner_class;
    m_is_running_a_state = false;
    m_current_state = NULL;
    m_next_state = NULL;
}

StateMachine::~StateMachine ()
{
    Shutdown();
}

void StateMachine::RunCurrentState (StateMachineInput const input)
{
    ASSERT1(input <= SM_HIGHEST_USER_INPUT_VALUE && "Users are not allowed to send state-machine-defined input")
    RunCurrentStatePrivate(input);
}

void StateMachine::Shutdown ()
{
    ASSERT1(!m_is_running_a_state)

    // only actually shutdown if we're not already shutdown.
    if (m_current_state != NULL)
    {
        // run the current state with SM_EXIT and nullify it.
        RunCurrentStatePrivate(SM_EXIT);
        m_current_state = NULL;
    }
}

void StateMachine::RunCurrentStatePrivate (StateMachineInput const input)
{
    ASSERT1(!m_is_running_a_state && "This method should not be used from inside a state")
    ASSERT1(m_current_state != NULL && "This state machine has not been initialized")

    // NULL is a sentinel value so we know if the state has transitioned
    m_next_state = NULL;

    // if the state return true, the input was handled.  otherwise not.
    m_is_running_a_state = true;
    bool state_handled_the_input = (m_owner_class->*m_current_state)(input);
    m_is_running_a_state = false;
    // make sure that states always handle all input (with the exception of
    // the StateMachine mechanism inputs)
    if (input <= SM_HIGHEST_USER_INPUT_VALUE)
        ASSERT0(state_handled_the_input && "All user-defined state machine input must be handled")

    if (input == SM_EXIT && m_next_state != NULL)
        ASSERT0(false && "You must not transition while exiting a state")

    // if a transition was requested, perform the necessary exit/enter machinery.
    // this is a while-loop because you can transition during SM_ENTER.
    while (m_next_state != NULL)
    {
        // we have to save off the next state, because the forthcoming SM_EXIT
        // call to the current state might change m_next_state, and we need to
        // detect if they try to transition on SM_EXIT (which is not allowed).
        State real_next_state = m_next_state;
        m_next_state = NULL;
        // call the current state with SM_EXIT, ignoring the return value
        m_is_running_a_state = true;
        (m_owner_class->*m_current_state)(SM_EXIT);
        m_is_running_a_state = false;
        // if they requested a transition, assert
        ASSERT0(m_next_state == NULL && "You must not transition while exiting a state")

        // set the current state to the new state
        m_current_state = real_next_state;
        // call the current state with SM_ENTER, ignoring the return value
        m_is_running_a_state = true;
        (m_owner_class->*m_current_state)(SM_ENTER);
        m_is_running_a_state = false;
    }
}

} // end of namespace Xrb
