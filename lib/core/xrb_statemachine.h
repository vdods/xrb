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

// StateMachine input value typedef
typedef Uint16 StateMachineInput;
// StateMachine mechanism inputs
enum
{
    SM_ENTER = 0,
    SM_EXIT,

    SM_USER_DEFINED_INPUT_STARTS_AT_THIS_VALUE
};

// this class is to be used in composition with another, as opposed to
// using it via inheritance.
template <typename OwnerClass>
class StateMachine
{
public:

    typedef bool (OwnerClass::*State)(StateMachineInput);

    StateMachine (OwnerClass *owner_class);
    ~StateMachine ();

    bool GetIsInitialized () const { return m_current_state != NULL; }
    State GetCurrentState () const { return m_current_state; }
    
    void Initialize (State initial_state);
    void RunCurrentState (StateMachineInput input);
    void Shutdown ();
    
    void RequestStateTransition (State state);

private:

    void RunCurrentStatePrivate (StateMachineInput input);

    OwnerClass *m_owner_class;
    bool m_is_running_a_state;
    State m_current_state;
    State m_next_state;
}; // end of class StateMachine

// template function definitions for StateMachine
#include "xrb_statemachine.tcpp"

} // end of namespace Xrb

#endif // !defined(_XRB_STATEMACHINE_H_)

