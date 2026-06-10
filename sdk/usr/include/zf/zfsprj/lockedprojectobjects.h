#ifndef TYPEDZFPRJOBJNODE_H
#define TYPEDZFPRJOBJNODE_H

#pragma warning(push)
#pragma warning(disable: 4610 4512 4510)
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#pragma warning(pop)

#include "obj.h"


namespace zfprj
{

    //##################################################################################
    /**
    \brief A class that simplifies the handling of ZFPrjObjNodeReadWrite objects.
    
    This class removes the necessity of casting to ZFPrjObjNodeReadWrite::refNode to the desired type.

    T must be a subtype of ZFPrjObjNode

    The class povides operator->() for a more natural access to the contained PrjObjNode like we know it from
    other smart pointers.
    The conversion to bool is also implemented to allow code like:

    \code{.cpp}
    WritableLockedProjectObjects_Ptr ptr = getPtr();
    if( ptr )
    {
        ptr->doStuff();
    }
    \endcode

    It is recommended to use this class instead of using the boost::shared_ptr<ZFPrjObjNodeReadWrite> directly.
    This example demonstrates for a typical use-case why.

    \code{.cpp}
    // use shared pointer directly
    boost::shared_ptr<zfprj::ZFPrjObjNodeReadWrite> objects = project.GetObjectLock( Messungen::getClassName());
    if( objects && objects->refNode)
    {
        Messungen* messungen = static_cast<Messungen*>(objects->refNode);
        messungen->doSomething();
    }

    // This becomes
    zfprj::WritableLockedProjectObjects_Ptr<Messungen> messungen = project.GetWriteableLockedObjects<Messungen>();
    if( messungen)
    {
        messungen->doSomething();
    }

    \endcode

    
    */
    template<typename T>
    class WritableLockedProjectObjects_Ptr
    {
        std::shared_ptr<ZFPrjObjNodeReadWrite> lockedObjects;

    public:
        inline WritableLockedProjectObjects_Ptr(std::shared_ptr<ZFPrjObjNodeReadWrite> lockedObjectsArg)
            : lockedObjects( lockedObjectsArg)
        {

        }

        inline WritableLockedProjectObjects_Ptr(ZFPrjObjNode* objects = NULL)
            : lockedObjects( std::make_shared<ZFPrjObjNodeReadWrite>(objects))
        {

        }

        inline T* get() const
        {
            return static_cast<T*>(lockedObjects->refNode);
        }

        inline T* operator->() const
        {
            return get();
        }

        inline operator bool() const
        {
            return lockedObjects && lockedObjects->refNode != NULL;
        }
    };

    //##################################################################################
    /**
    \brief A class that simplifies the handling of ZFPrjObjNodeRead objects.
    
    This class does the same as WriteLockedProjectObjects_Ptr but only read-locks the project objects that it contains.
    */
    template<typename T>
    class ReadableLockedProjectObjects_Ptr
    {
        std::shared_ptr<ZFPrjObjNodeRead> lockedObjects;

    public:
        inline ReadableLockedProjectObjects_Ptr(std::shared_ptr<ZFPrjObjNodeRead> lockedObjectsArg)
            : lockedObjects( lockedObjectsArg)
        {

        }

        inline ReadableLockedProjectObjects_Ptr(ZFPrjObjNode* objects = NULL)
            : lockedObjects( std::make_shared<ZFPrjObjNodeRead>(objects))
        {

        }

        inline const T* get() const
        {
            return static_cast<const T*>(lockedObjects->refNode);
        }

        inline const T* operator->() const
        {
            return get();
        }

        inline operator bool() const
        {
            return lockedObjects && lockedObjects->refNode != 0;
        }
    };

}


#endif // include guards