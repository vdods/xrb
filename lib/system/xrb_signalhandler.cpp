// ///////////////////////////////////////////////////////////////////////////
// xrb_signalhandler.cpp by Victor Dods, created 2005/01/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_signalhandler.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// SignalSenderBase
// ///////////////////////////////////////////////////////////////////////////

SignalSenderBase::SignalSenderBase (SignalHandler *const owner)
{
    ASSERT1(owner != NULL);
    m_owner = owner;
    m_owner->AddSender(this);
    m_is_blocking = false;
}

// ///////////////////////////////////////////////////////////////////////////
// SignalSender0
// ///////////////////////////////////////////////////////////////////////////

void SignalSender0::Signal ()
{
    // we can early out if the sender is blocked (since it can't
    // cause any receiver callbacks, there's no way for it to become
    // unblocked during this call to Signal().
    if (GetOwner()->IsBlockingSenders() || IsBlocking())
        return;

    // iterate through all attached ports and call their
    // callbacks, if outgoing signals aren't blocked
    FetchFirstIteratorAttachment();
    while (IsIteratorAttachmentValid())
    {
        Attachment<SignalReceiver0, TransformationSet0 > const &attachment =
            GetIteratorAttachment();
            
        ASSERT1(attachment.m_receiver != NULL);
        // only proceed with the callback if this sender's owner isn't
        // blocking senders and this sender isn't blocking itself.
        // this code can't be above this for-loop because the blocking
        // status might change during one of the callbacks.
        if (!GetOwner()->IsBlockingSenders() && !IsBlocking())
        {
            // only call the callback if the receiver's owner isn't blocking
            // all receivers and if the receiver isn't blocking itself
            if (!attachment.m_receiver->GetOwner()->IsBlockingReceivers() &&
                !attachment.m_receiver->IsBlocking())
            {
                (attachment.m_receiver->GetOwner()->*attachment.m_receiver->m_callback)();
            }
        }
        // we can early out if this sender becomes blocked.
        else
            return;

        FetchNextIteratorAttachment();
    }
}

// ///////////////////////////////////////////////////////////////////////////
// SignalReceiverBase
// ///////////////////////////////////////////////////////////////////////////

SignalReceiverBase::SignalReceiverBase (SignalHandler *const owner)
{
    ASSERT1(owner != NULL);
    m_owner = owner;
    m_owner->AddReceiver(this);
    m_is_blocking = false;
}

// ///////////////////////////////////////////////////////////////////////////
// SignalHandler
// ///////////////////////////////////////////////////////////////////////////

SignalHandler::SignalHandler ()
{
    m_is_blocking_senders = false;
    m_is_blocking_receivers = false;
}

SignalHandler::~SignalHandler ()
{
    m_sender_list.clear();
    m_receiver_list.clear();
}

void SignalHandler::DetachAll ()
{
    for (SignalSenderListIterator it = m_sender_list.begin(),
                                  it_end = m_sender_list.end();
         it != it_end;
         ++it)
    {
        SignalSenderBase const *sender = *it;
        ASSERT1(sender != NULL);
        sender->DetachAll();
    }

    for (SignalReceiverListIterator it = m_receiver_list.begin(),
                                    it_end = m_receiver_list.end();
         it != it_end;
         ++it)
    {
        SignalReceiverBase const *receiver = *it;
        ASSERT1(receiver != NULL);
        receiver->DetachAll();
    }
}

void SignalHandler::AddSender (SignalSenderBase const *sender)
{
    ASSERT1(sender != NULL);
    m_sender_list.push_back(sender);
}

void SignalHandler::AddReceiver (SignalReceiverBase const *receiver)
{
    ASSERT1(receiver != NULL);
    m_receiver_list.push_back(receiver);
}

} // end of namespace Xrb
