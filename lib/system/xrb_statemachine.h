// ///////////////////////////////////////////////////////////////////////////
// xrb_statemachine.h by Victor Dods, created 2006/03/19
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_STATEMACHINE_H_)
#define _XRB_STATEMACHINE_H_

#include "xrb.h"

namespace Xrb
{

// StateMachine mechanism inputs
enum
{
    SM_ENTER = static_cast<StateMachineInput>(-1),
    SM_EXIT = static_cast<StateMachineInput>(-2),

    SM_HIGHEST_USER_INPUT_VALUE = static_cast<StateMachineInput>(-3)
};

// this class is only used so StateMachine can properly static_cast
// method pointers from the class which is using it.
class StateMachineHandler { };

// this class is to be used in composition with another, as opposed to
// using it via inheritance.
class StateMachine
{
public:

    typedef bool (StateMachineHandler::*State)(StateMachineInput);

    StateMachine (StateMachineHandler *owner_class);
    ~StateMachine ();

    inline bool GetIsInitialized () const { return m_current_state != NULL; }
    template <typename StateMachineHandlerSubclass>
    inline bool GetIsCurrentStateEqualTo (bool (StateMachineHandlerSubclass::*test_state)(StateMachineInput)) const
    {
        return m_current_state == static_cast<State>(test_state);
    }

    template <typename StateMachineHandlerSubclass>
    void Initialize (bool (StateMachineHandlerSubclass::*initial_state)(StateMachineInput))
    {
        // just make sure this happens only once at the beginning
        ASSERT1(!m_is_running_a_state && "This method should not be used from inside a state")
        ASSERT1(m_current_state == NULL && "This state machine is already initialized")

        // set the current state and run it with SM_ENTER.
        m_current_state = static_cast<State>(initial_state);
        RunCurrentStatePrivate(SM_ENTER);
    }
    void RunCurrentState (StateMachineInput input);
    void Shutdown ();

    template <typename StateMachineHandlerSubclass>
    inline void SetNextState (bool (StateMachineHandlerSubclass::*state)(StateMachineInput))
    {
        ASSERT1(m_is_running_a_state && "This method should only be used from inside a state")
        ASSERT1(m_current_state != NULL && "This state machine has not been initialized")

        // note: a NULL value for state is acceptable.  it is
        // effectively canceling an earlier requested transition.
        m_next_state = static_cast<State>(state);
    }

private:

    void RunCurrentStatePrivate (StateMachineInput input);

    StateMachineHandler *m_owner_class;
    bool m_is_running_a_state;
    State m_current_state;
    State m_next_state;
}; // end of class StateMachine

} // end of namespace Xrb

#endif // !defined(_XRB_STATEMACHINE_H_)

