// ///////////////////////////////////////////////////////////////////////////
// xrb_signalhandler.hpp by Victor Dods, created 2005/01/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SIGNALHANDLER_HPP_)
#define _XRB_SIGNALHANDLER_HPP_

#include "xrb.hpp"

#include <algorithm>
#include <list>

/** @file xrb_signalhandler.h
  * Defines the classes involved in the fancied and sugarcoated method
  * callback facility known as SignalHandler.
  * @brief Contains SignalHandler, SignalSender* and SignalReceiver*.
  */

namespace Xrb
{

class SignalHandler;

class SignalSenderBase;
template <typename ReceiverAnalog, typename TransformationSet> class SignalSender;
class SignalSender0;
template <typename T> class SignalSender1;
template <typename T, typename U> class SignalSender2;

class SignalReceiverBase;
template <typename SenderAnalog> class SignalReceiver;
class SignalReceiver0;
template <typename T> class SignalReceiver1;
template <typename T, typename U> class SignalReceiver2;

/** Contains an interface method to detach all attachments, for
  * use by SignalHandler.
  *
  * Also provides the means to refer to differently
  * template-typed subclasses via a common parent class (SignalHandler
  * keeps a list of all of its added SignalSender*s).
  *
  * A SignalSender subclass should be used as a member variable of the
  * SignalHandler-derived class it is being used in.  This is done to ensure
  * that the life of the SignalSender coincides with the life of the
  * SignalHandler (so that it is available to attach to during the entire
  * life of the owning SignalHandler, and that it detaches all of its
  * attachments precisely upon the destruction of its SignalHandler owner.
  *
  * @brief Base class for SignalSender*.
  * @note This should be used only by SignalHandler and SignalSender*.
  */
class SignalSenderBase
{
public:

    /** @brief Boring destructor.
      */
    virtual ~SignalSenderBase () { }

    /** @brief Signal/callback blocking state accessor.
      * @return True iff this SignalSender* is disallowing outgoing signals.
      */
    inline bool IsBlocking () const
    {
        return m_is_blocking;
    }
    /** @brief Accessor of the SignalHandler object which owns this
      *        SignalSender*.  This object should be a member variable
      *        of its owner.
      * @return The owning SignalHandler object.
      */
    inline SignalHandler *Owner () const
    {
        return m_owner;
    }

    /** A SignalSender* can block signals on a per-SignalSender* basis.
      * A SignalSender* that is blocked will not call its attached
      * SignalReceiver*s' callbacks.
      * @brief Signal blocking state modifier.
      * @param is_blocking The signal blocking state to assign.
      */
    inline void SetIsBlocking (bool is_blocking)
    {
        m_is_blocking = is_blocking;
    }

    /** Causes the attached SignalReceiver*s to detach from this SignalSender.
      * @brief Detaches all attached SignalReceiver*s.
      */
    virtual void DetachAll () const = 0;

protected:

    /** Adds this SignalSender* object to the SignalHandler @c owner.
      *
      * This constructor is protected so that SignalSenderBase can only be
      * used by subclassing it.
      *
      * @brief Constructs a SignalSenderBase object.
      * @param owner The SignalHandler that this SignalSenderBase belongs to.
      */
    SignalSenderBase (SignalHandler *owner);

private:

    /// Indicates if this sender is blocking outgoing signals.
    bool m_is_blocking;
    /// The SignalHandler object which owns this SignalSender*.
    SignalHandler *m_owner;
}; // end of class SignalSenderBase

template <typename ReceiverAnalog, typename TransformationSet>
struct Attachment
{
    ReceiverAnalog *m_receiver;
    TransformationSet m_transformation_set;

    inline Attachment (
        ReceiverAnalog *receiver,
        TransformationSet transformation_set)
        :
        m_receiver(receiver),
        m_transformation_set(transformation_set)
    { }
    inline Attachment (Attachment const &attachment)
        :
        m_receiver(attachment.m_receiver),
        m_transformation_set(attachment.m_transformation_set)
    { }

    inline bool operator == (ReceiverAnalog const *receiver) const
    {
        return m_receiver == receiver;
    }
}; // end of struct Attachment<ReceiverAnalog, TransformationSet>

template <typename ReceiverAnalog, typename TransformationSet>
class SignalSender : public SignalSenderBase
{
public:

    virtual ~SignalSender ()
    {
        // this is a non-virtual call to SignalSender's DetachAll.
        SignalSender<ReceiverAnalog, TransformationSet>::DetachAll();
    }

    inline Attachment<ReceiverAnalog, TransformationSet> const &IteratorAttachment () const
    {
        ASSERT1(m_iterator != m_iterator_end);
        return *m_iterator;
    }
    inline bool IsIteratorAttachmentValid () const
    {
        return m_iterator != m_iterator_end;
    }
    inline void FetchFirstIteratorAttachment () const
    {
        m_iterator = m_attachment_list.begin();
        m_iterator_end = m_attachment_list.end();
    }
    inline void FetchNextIteratorAttachment () const
    {
        ASSERT1(m_iterator != m_iterator_end);
        ++m_iterator;
    }

    inline void Detach (ReceiverAnalog const *receiver) const
    {
        DetachPrivate(receiver, true);
    }

    virtual void DetachAll () const
    {
        for (typename AttachmentList::iterator
                it = m_attachment_list.begin(),
                it_end = m_attachment_list.end();
             it != it_end;
             ++it)
        {
            ASSERT1(it->m_receiver != NULL);
            it->m_receiver->DetachPrivate(this, false);
        }
        m_attachment_list.clear();
    }

protected:

    SignalSender (SignalHandler *owner)
        :
        SignalSenderBase(owner)
    { }

private:

    inline void Attach (
        TransformationSet transformation_set,
        ReceiverAnalog const *receiver) const
    {
        AttachPrivate(transformation_set, receiver, true);
    }

    template <typename SenderAnalog>
    void AttachPrivate (
        TransformationSet transformation_set,
        SignalReceiver<SenderAnalog> const *receiver,
        bool reciprocate) const
    {
        ASSERT1(receiver != NULL);
        // upcast to the real type of ReceiverAnalog (technically invalid,
        // yes, but on the other hand, shut the hell up!)
        ReceiverAnalog const *real_receiver =
            static_cast<ReceiverAnalog const *>(receiver);
        // make sure that this receiver is not already in the attachment list.
        ASSERT1(std::find(m_attachment_list.begin(), m_attachment_list.end(), real_receiver) == m_attachment_list.end() && "receiver is already attached");
        // add it to the attachment list (this const_cast ugliness
        // is limited to this line only).
        m_attachment_list.push_back(
            Attachment<ReceiverAnalog, TransformationSet>(
                const_cast<ReceiverAnalog *>(real_receiver),
                transformation_set));
        // if necessary, have the receiver reciprocate the attachment
        if (reciprocate)
            receiver->AttachPrivate(this);
    }

    template <typename SenderAnalog>
    void DetachPrivate (
        SignalReceiver<SenderAnalog> const *receiver,
        bool reciprocate) const
    {
        ASSERT1(receiver != NULL);
        // upcast to the real type of ReceiverAnalog (technically invalid,
        // yes, but on the other hand, shut the hell up!)
        ReceiverAnalog const *real_receiver =
            static_cast<ReceiverAnalog const *>(receiver);
        // find which attachment matches the given receiver.
        typename AttachmentList::iterator it =
            std::find(
                m_attachment_list.begin(),
                m_attachment_list.end(),
                real_receiver);
        ASSERT1(it != m_attachment_list.end() && "non-existent receiver");
        // remove it from the attachment list
        m_attachment_list.erase(it);
        // if necessary, have the receiver reciprocate the detachment
        if (reciprocate)
            receiver->DetachPrivate(this, false);
    }

    typedef std::list<Attachment<ReceiverAnalog, TransformationSet> > AttachmentList;

    mutable typename AttachmentList::iterator m_iterator;
    mutable typename AttachmentList::iterator m_iterator_end;
    mutable AttachmentList m_attachment_list;

    template <typename SenderAnalog> friend class SignalReceiver;
    friend class SignalHandler;
}; // end of class SignalSender<ReceiverAnalog, TransformationSet>

struct TransformationSet0
{
};

class SignalSender0 : public SignalSender<SignalReceiver0, TransformationSet0>
{
public:

    SignalSender0 (SignalHandler *owner)
        :
        SignalSender<SignalReceiver0, TransformationSet0>(owner)
    { }
    virtual ~SignalSender0 () { }

    /** This is where all of the attached SignalReceiver0s' callback
      * methods are called.  Their callbacks are called upon their
      * corresponding owners.
      * @brief Sends a 0-parameter signal to all attached SignalReceiver0s.
      * @note This is not asynchronous or event-driven.  The callbacks
      *       are done as normal method calls.  Care must be used
      *       in not causing infinite recursion by causing a SignalSender*
      *       to Signal() while it is already executing Signal().
      */
    void Signal ();
}; // end of class SignalSender0

template <typename T>
struct TransformationSet1
{
    typedef T (*TransformationFunction)(T);

    TransformationFunction m_transformation_function;

    inline TransformationSet1 (TransformationFunction transformation_function)
        :
        m_transformation_function(transformation_function)
    { }
};

template <typename T>
class SignalSender1 : public SignalSender<SignalReceiver1<T>, TransformationSet1<T> >
{
public:

    SignalSender1 (SignalHandler *owner)
        :
        SignalSender<SignalReceiver1<T>, TransformationSet1<T> >(owner)
    { }
    virtual ~SignalSender1 () { }

    /** This is where all of the attached SignalReceiver1s' callback
      * methods are called.  Their callbacks are called upon their
      * corresponding owners.
      * @brief Sends a 1-parameter signal to all attached SignalReceiver1s.
      * @note This is not asynchronous or event-driven.  The callbacks
      *       are done as normal method calls.  Care must be used
      *       in not causing infinite recursion by causing a SignalSender*
      *       to Signal() while it is already executing Signal().
      */
    void Signal (T param)
    {
        // we can early out if the sender is blocked (since it can't
        // cause any receiver callbacks, there's no way for it to become
        // unblocked during this call to Signal().
        if (this->Owner()->IsBlockingSenders() || this->IsBlocking())
            return;

        // iterate through all attached ports and call their
        // callbacks, if outgoing signals aren't blocked
        this->FetchFirstIteratorAttachment();
        while (this->IsIteratorAttachmentValid())
        {
            Attachment<SignalReceiver1<T>, TransformationSet1<T> > const &attachment =
                this->IteratorAttachment();

            ASSERT1(attachment.m_receiver != NULL);
            // only proceed with the callback if this sender's owner isn't
            // blocking senders and this sender isn't blocking itself.
            // this code can't be above this for-loop because the blocking
            // status might change during one of the callbacks.
            if (!this->Owner()->IsBlockingSenders() && !this->IsBlocking())
            {
                // only call the callback if the receiver's owner isn't blocking
                // all receivers and if the receiver isn't blocking itself
                if (!attachment.m_receiver->Owner()->IsBlockingReceivers() &&
                    !attachment.m_receiver->IsBlocking())
                {
                    (attachment.m_receiver->Owner()->*attachment.m_receiver->m_callback)(
                        (attachment.m_transformation_set.m_transformation_function != NULL) ?
                        attachment.m_transformation_set.m_transformation_function(param) :
                        param);
                }
            }
            // we can early out if this sender becomes blocked.
            else
                return;

            this->FetchNextIteratorAttachment();
        }
    }
}; // end of class SignalSender1<T>

template <typename T, typename U>
struct TransformationSet2
{
    typedef T (*TransformationFunction1)(T);
    typedef U (*TransformationFunction2)(U);

    TransformationFunction1 m_transformation_function_1;
    TransformationFunction2 m_transformation_function_2;

    inline TransformationSet2 (
        TransformationFunction1 transformation_function_1,
        TransformationFunction2 transformation_function_2)
        :
        m_transformation_function_1(transformation_function_1),
        m_transformation_function_2(transformation_function_2)
    { }
};

template <typename T, typename U>
class SignalSender2 : public SignalSender<SignalReceiver2<T, U>, TransformationSet2<T, U> >
{
public:

    SignalSender2 (SignalHandler *owner)
        :
        SignalSender<SignalReceiver2<T, U>, TransformationSet2<T, U> >(owner)
    { }
    virtual ~SignalSender2 () { }

    /** This is where all of the attached SignalReceiver2s' callback
      * methods are called.  Their callbacks are called upon their
      * corresponding owners.
      * @brief Sends a 2-parameter signal to all attached SignalReceiver2s.
      * @note This is not asynchronous or event-driven.  The callbacks
      *       are done as normal method calls.  Care must be used
      *       in not causing infinite recursion by causing a SignalSender*
      *       to Signal() while it is already executing Signal().
      */
    void Signal (T param_1, U param_2)
    {
        // we can early out if the sender is blocked (since it can't
        // cause any receiver callbacks, there's no way for it to become
        // unblocked during this call to Signal().
        if (this->Owner()->IsBlockingSenders() || this->IsBlocking())
            return;

        // iterate through all attached ports and call their
        // callbacks, if outgoing signals aren't blocked
        this->FetchFirstIteratorAttachment();
        while (this->IsIteratorAttachmentValid())
        {
            Attachment<SignalReceiver2<T, U>, TransformationSet2<T, U> > const &attachment =
                this->IteratorAttachment();

            ASSERT1(attachment.m_receiver != NULL);
            // only proceed with the callback if this sender's owner isn't
            // blocking senders and this sender isn't blocking itself.
            // this code can't be above this for-loop because the blocking
            // status might change during one of the callbacks.
            if (!this->Owner()->IsBlockingSenders() && !this->IsBlocking())
            {
                // only call the callback if the receiver's owner isn't blocking
                // all receivers and if the receiver isn't blocking itself
                if (!attachment.m_receiver->Owner()->IsBlockingReceivers() &&
                    !attachment.m_receiver->IsBlocking())
                {
                    (attachment.m_receiver->Owner()->*attachment.m_receiver->m_callback)(
                        (attachment.m_transformation_set.m_transformation_function_1 != NULL) ?
                        attachment.m_transformation_set.m_transformation_function_1(param_1) :
                        param_1,
                        (attachment.m_transformation_set.m_transformation_function_2 != NULL) ?
                        attachment.m_transformation_set.m_transformation_function_2(param_2) :
                        param_2
                        );
                }
            }
            // we can early out if this sender becomes blocked.
            else
                return;

            this->FetchNextIteratorAttachment();
        }
    }
}; // end of class SignalSender2<T, U>





/** Contains an interface method to detach all attachments, for
  * use by SignalHandler.
  *
  * Also provides the means to refer to differently
  * template-typed subclasses via a common parent class (SignalHandler
  * keeps a list of all of its added SignalReceiver*s).
  *
  * A SignalReceiver subclass should be used as a member variable of the
  * SignalHandler-derived class it is being used in.  This is done to ensure
  * that the life of the SignalReceiver coincides with the life of the
  * SignalHandler (so that it is available to attach to during the entire
  * life of the owning SignalHandler, and that it detaches all of its
  * attachments precisely upon the destruction of its SignalHandler owner.
  *
  * @brief Base class for SignalReceiver*.
  * @note This should be used only by SignalHandler and SignalReceiver*.
  */
class SignalReceiverBase
{
public:

    /** @brief Boring destructor.
      */
    virtual ~SignalReceiverBase () { }

    /** @brief Callback blocking state accessor.
      * @return True iff this SignalReceiver* is disallowing its
      *         callback to be called.
      */
    inline bool IsBlocking () const
    {
        return m_is_blocking;
    }
    /** @brief Accessor of the SignalHandler object which owns this
      *        SignalReceiver*.  This object should be a member variable
      *        of its owner.
      * @return The owning SignalHandler object.
      */
    inline SignalHandler *Owner () const
    {
        return m_owner;
    }

    /** A SignalReceiver* can block callbacks on a per-SignalReceiver* basis.
      * A SignalReceiver* that is blocked will not allow its callback method
      * to be called.
      * @brief Signal blocking state modifier.
      * @param is_blocking The callback blocking state to assign.
      */
    inline void SetIsBlocking (bool is_blocking)
    {
        m_is_blocking = is_blocking;
    }

    /** Causes the attached SignalSender*s to detach from this SignalReceiver.
      * @brief Detaches all attached SignalSender*s.
      */
    virtual void DetachAll () const = 0;

protected:

    /** Adds this SignalReceiver* object to the SignalHandler @c owner.
      *
      * This constructor is protected so that SignalReceiverBase can only be
      * used by subclassing it.
      *
      * @brief Constructs a SignalReceiverBase object.
      * @param owner The SignalHandler that this SignalReceiverBase belongs to.
      */
    SignalReceiverBase (SignalHandler *owner);

private:

    /// Indicates if this receiver is blocking calls to its callback method.
    bool m_is_blocking;
    /// The SignalHandler object which owns this SignalReceiver*.
    SignalHandler *m_owner;
}; // end of class SignalReceiverBase

template <typename SenderAnalog>
class SignalReceiver : public SignalReceiverBase
{
public:

    virtual ~SignalReceiver ()
    {
        // this is a non-virtual call to SignalReceiver's DetachAll.
        SignalReceiver<SenderAnalog>::DetachAll();
    }

    inline void Detach (SenderAnalog const *sender) const
    {
        DetachPrivate(sender, true);
    }

    virtual void DetachAll () const
    {
        for (typename AttachmentList::iterator
                it = m_attachment_list.begin(),
                it_end = m_attachment_list.end();
             it != it_end;
             ++it)
        {
            SenderAnalog const *sender = *it;
            ASSERT1(sender != NULL);
            sender->DetachPrivate(this, false);
        }
        m_attachment_list.clear();
    }

protected:

    SignalReceiver (SignalHandler *owner)
        :
        SignalReceiverBase(owner)
    { }

private:

    template <typename ReceiverAnalog, typename TransformationSet>
    void AttachPrivate (
        SignalSender<ReceiverAnalog, TransformationSet> const *sender) const
    {
        ASSERT1(sender != NULL);
        // upcast to the real type of SenderAnalog (technically invalid,
        // yes, but on the other hand, shut the hell up!)
        SenderAnalog const *real_sender =
            static_cast<SenderAnalog const *>(sender);
        // make sure that this sender is not already in the attachment list
        ASSERT1(std::find(m_attachment_list.begin(), m_attachment_list.end(), real_sender) == m_attachment_list.end() && "sender is already attached");
        // add it to the attachment list
        m_attachment_list.push_back(real_sender);
    }

    template <typename ReceiverAnalog, typename TransformationSet>
    void DetachPrivate (
        SignalSender<ReceiverAnalog, TransformationSet> const *sender,
        bool reciprocate) const
    {
        ASSERT1(sender != NULL);
        // upcast to the real type of SenderAnalog (technically invalid,
        // yes, but on the other hand, shut the hell up!)
        SenderAnalog const *real_sender =
            static_cast<SenderAnalog const *>(sender);
        // find which attachment matches the given sender.
        typename AttachmentList::iterator it =
            std::find(
                m_attachment_list.begin(),
                m_attachment_list.end(),
                real_sender);
        ASSERT1(it != m_attachment_list.end() && "non-existent sender");
        // remove it from the attachment list
        m_attachment_list.erase(it);
        // if necessary, have the sender reciprocate the detachment
        if (reciprocate)
            sender->DetachPrivate(this, false);
    }

    typedef std::list<SenderAnalog const *> AttachmentList;

    mutable AttachmentList m_attachment_list;

    template <typename ReceiverAnalog, typename TransformationSet> friend class SignalSender;
}; // end of class SignalReceiver<SenderAnalog>

class SignalReceiver0 : public SignalReceiver<SignalSender0>
{
public:

    typedef void (SignalHandler::*CallbackType)();

    template <typename SignalHandlerSubclass>
    SignalReceiver0 (
        void (SignalHandlerSubclass::*callback)(),
        SignalHandler *owner)
        :
        SignalReceiver<SignalSender0>(owner)
    {
        ASSERT1(callback != NULL);
        m_callback = static_cast<CallbackType>(callback);
    }

private:

    CallbackType m_callback;

    /// So that SignalSender0 can use @c m_callback directly,
    friend class SignalSender0;
}; // end of class SignalReceiver0

template <typename T>
class SignalReceiver1 : public SignalReceiver<SignalSender1<T> >
{
public:

    typedef void (SignalHandler::*CallbackType)(T);

    template <typename SignalHandlerSubclass>
    SignalReceiver1 (
        void (SignalHandlerSubclass::*callback)(T),
        SignalHandler *owner)
        :
        SignalReceiver<SignalSender1<T> >(owner)
    {
        ASSERT1(callback != NULL);
        m_callback = static_cast<CallbackType>(callback);
    }

private:

    CallbackType m_callback;

    /// So that SignalSender1<T> can use @c m_callback directly,
    friend class SignalSender1<T>;
}; // end of class SignalReceiver1<T>

template <typename T, typename U>
class SignalReceiver2 : public SignalReceiver<SignalSender2<T, U> >
{
public:

    typedef void (SignalHandler::*CallbackType)(T, U);

    template <typename SignalHandlerSubclass>
    SignalReceiver2 (
        void (SignalHandlerSubclass::*callback)(T, U),
        SignalHandler *owner)
        :
        SignalReceiver<SignalSender2<T, U> >(owner)
    {
        ASSERT1(callback != NULL);
        m_callback = static_cast<CallbackType>(callback);
    }

private:

    CallbackType m_callback;

    /// So that SignalSender2<T, U> can use @c m_callback directly,
    friend class SignalSender2<T, U>;
}; // end of class SignalReceiver2<T, U>

/** Provides the necessary framework to SignalSender* and SignalReceiver*.
  * A class must derive from SignalHandler to use any SignalSender*s or
  * SignalReceiver*s.  The SignalSender*s and SignalReceiver*s must be
  * non-dynamically-allocated member variables that are constructed in
  * the constructor of the SignalHandler subclass.  This requirement
  * is so that the life of the SignalSender*s and SignalReceiver*s is
  * ensured to be exactly that of the SignalHandler subclass.  When
  * the member SignalSender*s and SignalReceiver*s are destructed during
  * the destruction of their owner SignalHandler subclass, they will
  * automatically detach all of their connections, conveniently cleaning
  * themselves up in an intuitive manner.
  *
  * A SignalHandler can block all of its SignalSender*s' signals, as
  * well as all of its SignalReceiver*s' callbacks.  Contrast this with
  * the individual signal blocking done in @c SignalPort.
  *
  * @brief The base class for anything that contains a SignalSender* or
  *        SignalReceiver*.
  */
class SignalHandler
{
public:

    /// Boring default constructor.
    SignalHandler ();
    /** All the added SignalSender*s and SignalReceiver*s will have
      * detached all their attachments by the time this happens (that is,
      * if the SignalHandler subclass correctly instantiated and constructed
      * its SignalSender*s and SignalReceiver*s).
      * @brief Slightly boring destructor.
      */
    virtual ~SignalHandler ();

    /** @brief All-SignalSender* signal-blocking state accessor.
      * @returns True iff the subordinate SignalSender*s are being blocked
      *          by this SignalHandler (has no connection to the individual
      *          SignalSender*'s blocking state).
      */
    bool IsBlockingSenders () const
    {
        return m_is_blocking_senders;
    }
    /** @brief All-SignalReceiver* callback-blocking state accessor.
      * @returns True iff the subordinate SignalReceiver*s are being blocked
      *          by this SignalHandler (has no connection to the individual
      *          SignalReceiver*'s blocking state).
      */
    bool IsBlockingReceivers () const
    {
        return m_is_blocking_receivers;
    }

    /** @brief All-SignalSender* signal-blocking state modifier.
      * @param is_blocking_senders The SignalSender* signal-blocking state
      *                            to assign.
      */
    void SetIsBlockingSenders (bool const is_blocking_senders)
    {
        m_is_blocking_senders = is_blocking_senders;
    }
    /** @brief All-SignalReceiver* callback-blocking state modifier.
      * @param is_blocking_receivers The SignalReceiver* callback-blocking
      *                              state to assign.
      */
    void SetIsBlockingReceivers (bool const is_blocking_receivers)
    {
        m_is_blocking_receivers = is_blocking_receivers;
    }

    /** @brief Causes all added SignalSender*s and SignalReceiver*s to
      *        detach all their attachments, via SignalPort::DetachAll().
      */
    void DetachAll ();

    /** @brief The official way to connect a SignalSender0 to a
      *        SignalReceiver0.
      * @param sender This object will be connected to @c receiver.
      * @param receiver This object will be connected to @c sender.
      */
    static inline void Connect0 (
        SignalSender0 const *sender,
        SignalReceiver0 const *receiver)
    {
        ASSERT1(sender != NULL);
        ASSERT1(receiver != NULL);
        sender->Attach(TransformationSet0(), receiver);
    }

    /** @brief The official way to connect a SignalSender1<T> to a
      *        SignalReceiver1<T>.
      * @param sender This object will be connected to @c receiver.
      * @param receiver This object will be connected to @c sender.
      */
    template <typename T>
    static inline void Connect1 (
        SignalSender1<T> const *sender,
        SignalReceiver1<T> const *receiver)
    {
        ASSERT1(sender != NULL);
        ASSERT1(receiver != NULL);
        sender->Attach(TransformationSet1<T>(NULL), receiver);
    }
    /** @brief The official way to connect a SignalSender1<T> to a
      *        SignalReceiver1<T>, providing a transformation function
      *        for the signal value.
      * @param sender This object will be connected to @c receiver.
      * @param transformation_function The function pointer for the
      *                                transformation to apply to each
      *                                value signaled by this connection.
      * @param receiver This object will be connected to @c sender.
      */
    template <typename T>
    static inline void Connect1 (
        SignalSender1<T> const *sender,
        T (*transformation_function)(T),
        SignalReceiver1<T> const *receiver)
    {
        ASSERT1(sender != NULL);
        ASSERT1(receiver != NULL);
        sender->Attach(TransformationSet1<T>(transformation_function), receiver);
    }

    /** @brief The official way to connect a SignalSender2<T, U> to a
      *        SignalReceiver2<T, U>.
      * @param sender This object will be connected to @c receiver.
      * @param receiver This object will be connected to @c sender.
      */
    template <typename T, typename U>
    static inline void Connect2 (
        SignalSender2<T, U> const *sender,
        SignalReceiver2<T, U> const *receiver)
    {
        ASSERT1(sender != NULL);
        ASSERT1(receiver != NULL);
        sender->Attach(
            TransformationSet2<T, U>(NULL, NULL),
            receiver);
    }
    /** @brief The official way to connect a SignalSender2<T, U> to a
      *        SignalReceiver2<T, U>.
      * @param sender This object will be connected to @c receiver.
      * @param transformation_function_1 The function pointer for the
      *                                  transformation to apply to each
      *                                  of the first parameter values
      *                                  signaled by this connection.
      * @param transformation_function_2 The function pointer for the
      *                                  transformation to apply to each
      *                                  of the second parameter values
      *                                  signaled by this connection.
      * @param receiver This object will be connected to @c sender.
      */
    template <typename T, typename U>
    static inline void Connect2 (
        SignalSender2<T, U> const *sender,
        T (*transformation_function_1)(T),
        U (*transformation_function_2)(U),
        SignalReceiver2<T, U> const *receiver)
    {
        ASSERT1(sender != NULL);
        ASSERT1(receiver != NULL);
        sender->Attach(
            TransformationSet2<T, U>(transformation_function_1, transformation_function_2),
            receiver);
    }

private:

    /** This function is and should only be used by the constructor of
      * SignalSenderBase.
      * @brief Adds a SignalSender* to be owned by this SignalHandler.
      * @param sender A SignalSender* object to add to this SignalHandler.
      *               @code sender->IsSender() @endcode must return true.
      */
    void AddSender (SignalSenderBase const *sender);
    /** This function is and should only be used by the constructor of
      * SignalReceiverBase.
      * @brief Adds a SignalReceiver* to be owned by this SignalHandler.
      * @param receiver A SignalReceiver* object to add to this SignalHandler.
      *                 @code receiver->IsReceiver() @endcode must return
      *                 true.
      */
    void AddReceiver (SignalReceiverBase const *receiver);

    /** A list of all added SignalSender*s is kept so that a call to
      * DetachAll() will work as expected.
      * @brief A typedef for a list to store added SignalSender*s.
      */
    typedef std::list<SignalSenderBase const *> SignalSenderList;

    /** A list of all added SignalReceiver*s is kept so that a call to
      * DetachAll() will work as expected.
      * @brief A typedef for a list to store added SignalReceiver*s.
      */
    typedef std::list<SignalReceiverBase const *> SignalReceiverList;

    /** @brief Indicates if all SignalSender*s owned by this SignalHandler are
      *        blocked from signalling.
      */
    bool m_is_blocking_senders;
    /** @brief Indicates if all SignalReceiver*s owned by this SignalHandler
      *        are blocked from calling their callbacks.
      */
    bool m_is_blocking_receivers;
    /// List of added SignalSender*s.
    SignalSenderList m_sender_list;
    /// List of added SignalReceiver*s.
    SignalReceiverList m_receiver_list;

    /// So that SignalSenderBase can call @c AddSender().
    friend class SignalSenderBase;
    /// So that SignalReceiverBase can call @c AddReceiver().
    friend class SignalReceiverBase;
}; // end of class SignalHandler

} // end of namespace Xrb

#endif // !defined(_XRB_SIGNALHANDLER_HPP_)

