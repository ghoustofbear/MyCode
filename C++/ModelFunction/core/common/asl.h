/**
 * @file      asl.h
 * @copyright Copyright (c) 2018, zhong caiwang. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-07 12:25:09
 *
 * @note

 * purpose:	Cross platform classes and macros to make dynamic loaded module
 * easy to use by using c++ template meta-programming technic.
 *
 * No need to make any changes to existing module code.
 *
 * Support both windows(*.dll) and linux(*.so) platforms (wchar_t & char).
 *
 * SPECIAL THANKS TO TRL (Template Relection Library)
 *
 * usage:
 * Following codes are all in client side:
 *
 * ASL_LIBRARY_BEGIN(ClassName)
 * 	ASL_SYMBOL_OPTIONAL(Func1Type, func1)
 * 	ASL_SYMBOL_EXPLICIT(Func2Type, func2)
 * 	// Enabled only when ' #define  ASL_USE_CPP11 1 ' and compliler supports c++ 11
 * 	ASL_SYMBOL_EXPLICIT_T(func4) // only need to declare the name
 * ASL_LIBRARY_END()
 *
 * ClassName theLib;
 * try {
 * theLib.Load("./1.so");
 * }
 * catch (LibraryNotFoundException& e) {
 * }
 * catch (SymbolNotFoundException& e) {
 * }
 * theLib.func1(1);
 * theLib.func2("aa");
 *
 * // The function type is deduced with the args
 * // retType => int, args => const char* AND float
 * // So this calling is UNSAFE!
 * // You'd better explicitly specifiy the type of args like this
 * theLib.func4<int>(ASL_ARGS_T((const char*)"test", (float)2.3));
 *
 * theLib.Unload();
 */
#ifndef __ASL_INCLUDE_H__
#define __ASL_INCLUDE_H__

#define ASL_USE_CPP11

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


#include <cstdlib>
#include <exception>
#include <string>

#ifdef ASL_USE_CPP11
#include <functional>
#include <tuple>
#endif


namespace ASL
{

namespace Private
{

template <class Head_, class Tail_>
struct TypeList
{
    typedef Head_ Head;
    typedef Tail_ Tail;
};

class NullType {};

template <int i_>
struct Int2Type
{
    enum { value = i_ };
};

template <int condition_, class T0_, class T1_>
struct Select
{
    typedef T0_ Result;
};

template <class T0_, class T1_>
struct Select<false, T0_, T1_>
{
    typedef T1_ Result;
};

template <int condition_, int v0_, int v1_>
struct SelectInt
{
    enum { value = v0_ };
};

template <int v0_, int v1_>
struct SelectInt<false, v0_, v1_>
{
    enum { value = v1_ };
};

template <class Type_, int Ignore_>
struct MemberInfo
{
    typedef Type_ Type;
    enum
    {
        ignore = Ignore_
    };
};


template <class TList_, int startLine_, int endLine_, class ConcreteClass_>
struct CreateMemberIndicesImpl
{
    typedef typename ConcreteClass_::template IsMemberPresent<endLine_> IsMemberPresent;
    enum { isMemberPresent = IsMemberPresent::value };

    typedef typename Select< isMemberPresent
    , TypeList<MemberInfo<Int2Type<IsMemberPresent::index>, IsMemberPresent::ignoreError >, TList_>
    , TList_ >::Result NewTList;

    typedef CreateMemberIndicesImpl < NewTList, startLine_, endLine_ - 1, ConcreteClass_ > MemberIndicesImpl;
    typedef typename MemberIndicesImpl::Indices Indices;
};


template <class TList_, int startLine_, class ConcreteClass_>
struct CreateMemberIndicesImpl<TList_, startLine_, startLine_, ConcreteClass_>
{
    typedef TList_ Indices;
};


template <int startLine_, int endLine_, class ConcreteClass_>
struct CreateMemberIndices
{
    typedef CreateMemberIndicesImpl < NullType, startLine_
    , endLine_ - 1, ConcreteClass_ > MemberIndicesImpl;
    typedef typename MemberIndicesImpl::Indices Indices;
};


template <class ConcreteClass_, int startLine_, int currentLine_>
struct GetMemberIndex
{
    typedef typename ConcreteClass_::template IsMemberPresent<currentLine_> IsMemberPresent;

    enum
    {
        index = SelectInt < IsMemberPresent::value
        , IsMemberPresent::index
        , GetMemberIndex < ConcreteClass_, startLine_, currentLine_ - 1 >::index >::value + 1
    };
};

template <class ConcreteClass_, int startLine_>
struct GetMemberIndex<ConcreteClass_, startLine_, startLine_>
{
    enum { index = -1 };
};


#ifdef ASL_USE_CPP11
typedef	void* FuncType;

/**
 * Pack of numbers.
 * Nice idea, found at http://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer
 */
template<int ...> struct Seq {};

/* Metaprogramming Expansion */
template<int N, int ...S> struct GenList : GenList < N - 1, N - 1, S... > {};
template<int ...S> struct GenList < 0, S... >
{
    typedef Seq<S...> Result;
};

/* Function that performs the actual call */
template<typename Ret_, int ...S_, typename...Args_>
Ret_ ActualCall(Seq<S_...>, std::tuple<Args_...> tpl, const std::function<Ret_(Args_...)>& func)
{
    /* It calls the function while expanding the std::tuple to it's arguments via std::get<S> */
    return func(std::get<S_>(tpl) ...);
}
#endif
}

class DefaultLibraryLoader
{
public:
    typedef void* LibHandle;

    DefaultLibraryLoader()
    {
        lib_handle = NULL;
    }

    template<class Char_>
    bool Load(const Char_* name)
    {
#if defined(WIN32)
        lib_handle = LoadLibrary(name);
#else
        lib_handle = dlopen(name, RTLD_LAZY);
#endif
        return lib_handle != NULL;
    }

    void Unload()
    {
        if (!IsLoaded())
        {
            return;
        }

#if defined(WIN32)
        FreeLibrary((HMODULE)lib_handle);
#elif !defined(_ANDROID)
        dlclose(lib_handle);
#endif

        lib_handle = NULL;
    }

    template<class Char_>
    void* LoadSymbol(const Char_* fun_name)
    {
#if defined(WIN32)
        return (void *)GetProcAddress((HMODULE)lib_handle, fun_name);
#elif !defined(_ANDROID)
        return dlsym(lib_handle, fun_name);
#endif
    }

    bool IsLoaded() const
    {
        return lib_handle != NULL;
    }

private:
    LibHandle lib_handle;
};

class LibraryNotFoundException : public std::exception
{
public:
    LibraryNotFoundException(const char* err)
    {
        _err = err;
    }

    LibraryNotFoundException(const wchar_t* err)
    {
        static const size_t CONVERT_LEN = 256;
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
        char mbsBuff[CONVERT_LEN + 1] = { 0 };
        std::wcstombs(mbsBuff, err, CONVERT_LEN);
        _err = mbsBuff;
#if _MSC_VER
#pragma warning(pop)
#endif
    }

    ~LibraryNotFoundException() throw() {}

    virtual const char* what() const throw()
    {
        return _err.c_str();
    }
private:
    std::string _err;
};

class SymbolNotFoundException : public std::exception
{
public:
    SymbolNotFoundException(const char* err)
    {
        _err = err;
    }

    SymbolNotFoundException(const wchar_t* err)
    {
        static const size_t CONVERT_LEN = 256;
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
        char mbsBuff[CONVERT_LEN + 1] = { 0 };
        std::wcstombs(mbsBuff, err, CONVERT_LEN);
        _err = mbsBuff;
#if _MSC_VER
#pragma warning(pop)
#endif
    }

    ~SymbolNotFoundException() throw() { }

    virtual const char* what() const throw()
    {
        return _err.c_str();
    }

private:
    std::string _err;
};

struct DefaultErrorHandler
{
    template<class Char_>
    static void OnLoadLibrary(const Char_* libName)
    {
        throw LibraryNotFoundException(libName);
    }

    template<class Char_>
    static void OnLoadSymbol(const Char_* symbolName, const bool ignore)
    {
        if (!ignore)
        {
            throw SymbolNotFoundException(symbolName);
        }
    }
};

template < class ConcreteClass_,
           class Loader_ = DefaultLibraryLoader,
           class ErrorHandler_ = DefaultErrorHandler >
class AutoSharedLibrary
{
public:
    AutoSharedLibrary()
    {
    }

    ~AutoSharedLibrary()
    {
        Unload();
    }

    template<class Char_>
    void Load(ConcreteClass_& object, const Char_* p)
    {
        if (!_loader.Load(p))
        {
            ErrorHandler_::OnLoadLibrary(p);
        }
        typedef typename ConcreteClass_::MemberIndices Indices;
        LoadSymbols(object, Indices());
    }

    void Unload()
    {
        _loader.Unload();
    }

private:
    template <class Indices_>
    void LoadSymbols(ConcreteClass_& object, Indices_ indices)
    {
        typedef typename Indices_::Head SymInfo;
        typedef typename SymInfo::Type Index;

        bool ret = LoadSymbol(ConcreteClass_::getLoadName(Index()),
                              object.*ConcreteClass_::getMemberPtr(Index()));
        if (!ret)
        {
            ErrorHandler_::OnLoadSymbol(ConcreteClass_::getLoadName(Index()), (bool)SymInfo::ignore);
        }
        LoadSymbols(object, typename Indices_::Tail());
    }

    void LoadSymbols(ConcreteClass_& object, Private::NullType indices)
    {
    }

    template <class FuncType_, class Char_>
    bool LoadSymbol(const Char_* funcName, FuncType_& func)
    {
        func = (FuncType_)_loader.LoadSymbol(funcName);
        return func != NULL;
    }

    Loader_ _loader;
};

}

#define ASL_LIBRARY_BEGIN(ConcreteClass_) \
    ASL_LIBRARY_BEGIN_2(ConcreteClass_, ASL::DefaultLibraryLoader, ASL::DefaultErrorHandler)

#define ASL_LIBRARY_BEGIN_2(ConcreteClass_, LibraryLoader_, ErrorHandler_) \
class ConcreteClass_ {                                  \
private:                                                \
    typedef ConcreteClass_ ConcreteClass;               \
enum { startLine = __LINE__ };                          \
    ASL::AutoSharedLibrary<ConcreteClass_, LibraryLoader_, ErrorHandler_> _libLoader;  \
public:                                                 \
    ConcreteClass_() { }                                \
    \
    ~ConcreteClass_() { Unload(); }                     \
    \
    template<class Char_> void Load(const Char_* p)     \
{                                                       \
    _libLoader.Load(*this, p);                          \
}                                                       \
    void Unload()                                       \
{                                                       \
    _libLoader.Unload();                                \
}                                                       \
    template <int lineNb_, class Dummy_ = ASL::Private::NullType> \
struct IsMemberPresent                                  \
{                                                    \
enum { value = false };                                 \
enum { index = 0 };                                     \
enum { ignoreError = false };                           \
};

#define ASL_SYMBOL(DataType, name, loadName, ignoreNotFound)          \
public:                                                 \
    DataType name;                                      \
private:                                                \
    typedef DataType ConcreteClass::* MemberPtr##name;  \
public:                                                 \
    template <class Dummy_>                             \
struct IsMemberPresent<__LINE__, Dummy_>                \
{                                                    \
enum { value = true };                                  \
enum { index = ASL::Private::GetMemberIndex<            \
    ConcreteClass, startLine, __LINE__ - 1>::index }; \
enum { ignoreError = ignoreNotFound};           \
};                                                  \
    static const char* getLoadName(                     \
    ASL::Private::Int2Type<IsMemberPresent<__LINE__>::index >) \
{ return #loadName; }                           \
    static MemberPtr##name getMemberPtr(            \
    ASL::Private::Int2Type< IsMemberPresent<__LINE__>::index >) \
{ return &ConcreteClass::name; }

#ifdef ASL_USE_CPP11

#define ASL_SYMBOL_T(name, loadName, ignoreNotFound)					\
		ASL_SYMBOL(ASL::Private::FuncType, name##_private_, loadName, ignoreNotFound)   \
		template<class Ret_, class... Args_> Ret_ name (std::tuple<Args_...> args)		\
		{ \
			typedef Ret_(*FuncPointer)(Args_...); \
			std::function<Ret_(Args_...)> func = reinterpret_cast<FuncPointer>(name##_private_);  \
			return ASL::Private::ActualCall(typename ASL::Private::GenList<sizeof...(Args_)>::Result(), args, func); \
		}

#define ASL_SYMBOL_EXPLICIT_T(name) \
		ASL_SYMBOL_T(name, name, false)

#define ASL_ARGS_T(...) (std::make_tuple<>(__VA_ARGS__))

#endif

#define ASL_SYMBOL_DEFAULT(DataType, name, ignoreNotFound) \
		ASL_SYMBOL(DataType, name, name, ignoreNotFound)

#define ASL_SYMBOL_OPTIONAL(DataType, name) \
	    ASL_SYMBOL_DEFAULT(DataType, name, true)

#define ASL_SYMBOL_EXPLICIT(DataType, name) \
	    ASL_SYMBOL_DEFAULT(DataType, name, false)


#define ASL_LIBRARY_END()                               \
private:                                                \
enum { endLine = __LINE__ };                            \
public:                                                 \
    typedef ASL::Private::CreateMemberIndices<startLine, endLine, ConcreteClass> \
    ::Indices MemberIndices;                            \
};

#ifdef _WIN32
#ifdef _DEBUG
#define ASL_LIBRARY_NAME(name) (name##"_d.dll")
#else
#define ASL_LIBRARY_NAME(name) (name##".dll")
#endif  

#else

#ifdef _DEBUG
#define ASL_LIBRARY_NAME(name) ("lib" name "_d.so")
#else
#define ASL_LIBRARY_NAME(name) ("lib" name ".so")
#endif  
#endif

#endif
