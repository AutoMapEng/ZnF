#pragma once

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace zftools
{
    /**
    Helper struct for the ValuesResetter. You will no need to use it directly.
    */
    template<class T, typename valueType>
    struct Deleter
    {
        boost::function<void(valueType)> setterFunction;
        valueType originalValue;

        void operator()(T*&)
        {
            setterFunction(originalValue);
        }
    };

    /**
    Template class that can be used to temporarily change member variables with the RAII idiom.

    This works similar to the boost::shared_ptr but instead of deleting an object, it resets a
    variable to an original value, when the ValueResetter goes out of scope.
    */
    template<typename valueType,
    typename functionType = boost::function<void(valueType)>>
    class ValueResetter
    {
        boost::shared_ptr<int> resettingPtr;    ///< pointer that calls the resetting function when the object is destroyed.

        valueType originalValue;
        functionType setterFunction;

        valueType& refToVariable;

        int dummyAddress;

    public:
        /**
        Use this when you have direct access to the variable that will be temporarily changed.

        Example:

        \code{.cpp}

        struct Foo
        {
            double member;
        }

        void func(Foo& foo)
        {
            ValueResetter<double> resetter( foo.member);
            foo.member = 1.;

            //... other code

        }   // foo.member will be reset to its original value here

        \endcode
        */
        ValueResetter(valueType& variableToReset)  ///<
            : originalValue(variableToReset)
            , refToVariable(variableToReset)
        {
            boost::function<void(ValueResetter<valueType>*, valueType)> f =
                    &ValueResetter<valueType>::defaultSetterFunction;

            setterFunction = boost::bind( f, this, _1);
            initResettingPointer();
        }

        /**
        Use this when you can only access the variable through setter and getter functions.

        Example:

        \code{.cpp}

        class Foo
        {
            double member;

        public:
            void setMember(double member);
            double getMember();
        }

        void func(Foo& foo)
        {
            ValueResetter<double> resetter( foo.getMember(), std::bind1st(std::mem_fun(&Foo::setMember), &foo));
            foo.setMember(1);

            //... other code

        }   // foo.member will be reset to its original value here

        \endcode
        */
        ValueResetter(valueType originalValueArg, functionType setterFunctionArg)
            : originalValue( originalValueArg)
            , refToVariable( originalValueArg)
        {
            setterFunction = setterFunctionArg;
            initResettingPointer();
        }

        ValueResetter& operator=(const ValueResetter& other)
        {
            resettingPtr = other.resettingPtr;
            originalValue = other.originalValue;
            setterFunction = other.setterFunction;
            refToVariable = other.refToVariable;
            dummyAddress = other.dummyAddress;
            return *this;
        }

    private:
        void defaultSetterFunction(valueType val)
        {
            refToVariable = val;
        }

        void initResettingPointer()
        {
            Deleter<int,valueType> deleter;
            deleter.setterFunction = setterFunction;
            deleter.originalValue = originalValue;

            resettingPtr = boost::shared_ptr<int>( &dummyAddress, deleter); // this needs some address to compile so we hand it the unused dummyAddress.
        }

    };

} // end namespace
