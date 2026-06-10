#pragma once

#include "zfsapi.h"

#include <string>

namespace zfs
{

    /**
    * @brief Interface for callbacks like print, progressbar, messagebox etc.
    *
    *        The application and plugins call this interface to propagate their progress and status.
    *        Implement and set this interface to react on this messages.
    *
    *        The application can have only one callback interface. Background tasks are not handled in this interface. This is done separately.
    *
    * Register the interface:
    *
    * <pre>
    *   static ZfsCB callbackInterface;
    *   zfs::SetAppIF(callbackInterface);
    * </pre>
    *
    * Usage:
    * <pre>
    *   // start the progress display
    *   zfs::GetAppIF().progress(zfs::AppIF::PROG_START, MAX_VALUE, L"message to display");
    *   // allow user abortion
    *   zfs::GetAppIF().setCancelAllowed(true);
    *   // set progress
    *   zfs::GetAppIF().progress(zfs::AppIF::PROG, VALUE);
    *   // stop progress display
    *   zfs::GetAppIF().progress(zfs::AppIF::PROG_END);
    * </pre>
    */
#ifdef WIN32
#pragma warning( disable : 4100 4251 )
#endif
    class ZFS_API AppIF
    {

    public:

        virtual ~AppIF() { }

        /**
        * @brief Flag, if updates should be blocked
        */
        bool updatesBlocked = false;

        /**
        * @brief Flag, if the current action can be canceled
            */
        bool cancelAllowed{};

        /**
        * @brief Flag, if the current action is market as canceled
            */
        bool cancelRequested{};

        /**
        * @brief Flag, if the progress display is busy
            */
        bool isRunning{};

        /**
        * @brief minimum and maximum value of the progress bar.
            */
        int minimum{}, maximum{}, current{};

        /**
        * @brief The prog_t enum.
            */
        enum prog_t {	PROG_START, ///< start the progressbar p1=number of steps info=may some information
        PROG,		///< walk trought, return false to break function
            PROG_STR,	///< print out just some information
            PROG_END	///< end progressbar
        };

        /**
        * @brief Set the progress range.
        *
        * @param newMinimum start value of the progress (should be 0 most of the time)
        * @param newMaximum the maximal progress value
        */
        virtual void setProgressRange(int newMinimum, int newMaximum) { 
            minimum = newMinimum; maximum = newMaximum; current = 0;
        }

        /**
        * @brief Helpermethod to perform an action on the progress display. Implementing classes can react on these call to set values,
        *        change texts, hide progress display etc.
        *
        * @param p1        the current value. If the progress starts you can define the maximal value with this. In normal cases, this is the progress
        * @param info      an info message. This can be used to display text about the current step.
        *
        * @return true, if the progress should continue, false if cancelation has been requested.
        */
        virtual bool progress( prog_t, int p1=0, const std::wstring& info=L"" ) { return true; }

        virtual int getCurrentValue() const {
            return current;
        }

        /**
        * @brief Request cancelation of the current action.
        */
        virtual void requestCancel() { cancelRequested = true; }

        /**
        * @brief Define if the current action can be canceled.
        *
        * @param allowed Flag, if cancelation is possible.
        */
        virtual void setCancelAllowed(bool allowed) { cancelAllowed = allowed; }

        /**
        * @brief Returns, if cancelation has been requested.
        *
        * @return true, if the current action should be canceled, false otherwise
        */
        virtual bool isCancelRequested() const { return cancelRequested; }

        /**
        * @brief Returns, if an action is currently running.
        *
        * @return true, if an action has been startet
        */
        virtual bool isInProgress() const { return isRunning; }

        /**
        * @brief Enum for messagebox types
        */
        enum msgtyp_t { MSGTYP_INFORMATION, MSGTYP_WARNING, MSGTYP_ERROR };
        /**
        * @brief Enum for messagebox return types
        */
        enum msg_t { MSG_OK=1, MSG_YES=2, MSG_NO=4 };

        /**
        * @brief Display a message box. The implementing method should show a message box of the given type with the messae and buttons.
        *
        * @param    typ             the messagebox typ
        * @param    message         the message to display
        * @param    buttons         the available buttons
        * @param    buttonDefault   the default button
        *
        * @return the pressed button
        */
        virtual msg_t messagebox( msgtyp_t typ, const std::wstring& message, msg_t buttons=MSG_OK, msg_t buttonDefault=MSG_OK ) { return buttonDefault; }

        /**
        * @brief Print some stuff. Implement this to create logging windows etc.
        *
        * @param str the message to print out
        */
        virtual void print( const std::wstring& str ) {}

        void blockUpdate(bool _block) {
            updatesBlocked = _block;
        }
    };

    /**
    * @brief Set the callback interface by user application.
    *
    * THIS SHOULD ONLY BE CALLED ONCE! Actually, there is no need to call it more often anyway.
    *
    * @param appIF the new interface
    */
    ZFS_API void SetAppIF( AppIF& appIF);

    /**
    * @brief Get a handle to the callback interface. If non is set, a dummy is returned.
    *
    * @return the callback interface or a dummy interface
    */
    ZFS_API AppIF& GetAppIF();

    /**
    * @brief Method to close down the interface.
    */
    ZFS_API void CloseAppIF();

    /**
    * @brief Interface to register progress callbacks to functions.
    *
    * If you have a function you want to track, extend the signature to take a pointer of this class.
    * Then the function can report its progress back to the listener you started the function.
    *
    * This is mainly used to transfer functions to background jobs.
    */
    class ZFS_API ProgressCallbackInterface {

    public:
        virtual ~ProgressCallbackInterface() { }

        /**
        * @brief flag, if cancel was requested
        */
        bool canceled;
        /**
        * @brief Maximal value of the current progress
        */
        int maximalValue;
        /**
        * @brief Current value of the progress
        */
        int currentValue;

        /**
        * @brief Message of the current progress
        */
        std::string currentMessage;

        ProgressCallbackInterface() : canceled(false), maximalValue(0), currentValue(0), currentMessage("") {}

        /**
        * @brief Callback method to set the current values.
        *
        * Override this method, if you want to handle progress changes.
        * This implementation sets the text, if there is one.
        * If id == 0 and mx > 0 the maximum is set, value is set to 0.
        * If id > 0 the current value is set
        *
        * @param text   the new message
        * @param id     the current value
        * @param mx     the maximal value
        *
        * @return true if everything is OK, false if the action should be canceled
        */
        virtual bool setValues(const std::string& text, int id, int mx ) {
            if( text.length() > 0 ) {
                currentMessage = text;
            }

            if( id==0 && mx ) {
                maximalValue = mx;
                currentValue = 0;
            } else if( id ) {
                currentValue = id;
            }

            if (canceled) return false;
            return true;
        }

        /**
        * @brief Signal, that the action should be canceled. It is just a signal, the listener must handle this
        * case on its own.
        */
        void requestCancel() { canceled = true; }


        /**
        *	The methode translate the given sourceText and returns the translatet text.
        */
        virtual std::string translateText( const char* sourceText )=0;//{  return sourceText ? sourceText : ""; }
    };
} // end namespace
