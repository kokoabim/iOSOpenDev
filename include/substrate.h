/* Cydia Substrate - Powerful Code Insertion Platform
 * Copyright (C) 2008-2012  Jay Freeman (saurik)
*/

/* GNU Lesser General Public License, Version 3 {{{ */
/*
 * Substrate is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Substrate is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Substrate.  If not, see <http://www.gnu.org/licenses/>.
**/
/* }}} */

#ifndef SUBSTRATE_H_
#define SUBSTRATE_H_

#ifdef __APPLE__
#ifdef __cplusplus
extern "C" {
#endif
#include <mach-o/nlist.h>
#ifdef __cplusplus
}
#endif

#include <objc/runtime.h>
#include <objc/message.h>
#endif

#include <dlfcn.h>
#include <stdlib.h>

#define _finline \
    inline __attribute__((__always_inline__))
#define _disused \
    __attribute__((__unused__))

#define _extern \
    extern "C" __attribute__((__visibility__("default")))

#ifdef __cplusplus
#define _default(value) = value
#else
#define _default(value)
#endif

#ifdef __cplusplus
extern "C" {
#endif

bool MSHookProcess(pid_t pid, const char *library);

typedef const void *MSImageRef;

MSImageRef MSGetImageByName(const char *file);
void *MSFindSymbol(MSImageRef image, const char *name);

void MSHookFunction(void *symbol, void *replace, void **result);

#ifdef __APPLE__
#ifdef __arm__
__attribute__((__deprecated__))
IMP MSHookMessage(Class _class, SEL sel, IMP imp, const char *prefix _default(NULL));
#endif
void MSHookMessageEx(Class _class, SEL sel, IMP imp, IMP *result);
#endif

#ifdef SubstrateInternal
typedef void *SubstrateAllocatorRef;
typedef struct __SubstrateProcess *SubstrateProcessRef;
typedef struct __SubstrateMemory *SubstrateMemoryRef;

SubstrateProcessRef SubstrateProcessCreate(SubstrateAllocatorRef allocator, pid_t pid);
void SubstrateProcessRelease(SubstrateProcessRef process);

SubstrateMemoryRef SubstrateMemoryCreate(SubstrateAllocatorRef allocator, SubstrateProcessRef process, void *data, size_t size);
void SubstrateMemoryRelease(SubstrateMemoryRef memory);
#endif

#ifdef __ANDROID__
#include <jni.h>
_extern void MSJavaHookClassLoad(JNIEnv *jni, const char *name, void (*callback)(void *, JNIEnv *, jclass), void *data _default(NULL));
_extern void MSJavaHookMethod(JNIEnv *jni, jclass _class, jmethodID methodID, void *function, void **result);
_extern void MSJavaBlessClassLoader(JNIEnv *jni, jobject loader);

typedef struct MSJavaObjectKey_ *MSJavaObjectKey;
_extern MSJavaObjectKey MSJavaNewObjectKey();
_extern void *MSJavaGetObjectKey(JNIEnv *jni, jobject object, MSJavaObjectKey key);
_extern void MSJavaSetObjectKey(JNIEnv *jni, jobject object, MSJavaObjectKey key, void *value, void (*clean)(void *, JNIEnv *, void *) _default(NULL), void *data _default(NULL));
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#ifdef SubstrateInternal
struct SubstrateHookMemory {
    SubstrateMemoryRef handle_;

    SubstrateHookMemory(SubstrateProcessRef process, void *data, size_t size) :
        handle_(SubstrateMemoryCreate(NULL, NULL, data, size))
    {
    }

    ~SubstrateHookMemory() {
        if (handle_ != NULL)
            SubstrateMemoryRelease(handle_);
    }
};
#endif

#ifdef __APPLE__

namespace etl {

template <unsigned Case_>
struct Case {
    static char value[Case_ + 1];
};

typedef Case<true> Yes;
typedef Case<false> No;

namespace be {
    template <typename Checked_>
    static Yes CheckClass_(void (Checked_::*)());

    template <typename Checked_>
    static No CheckClass_(...);
}

template <typename Type_>
struct IsClass {
    void gcc32();

    static const bool value = (sizeof(be::CheckClass_<Type_>(0).value) == sizeof(Yes::value));
};

}

#ifdef __arm__
template <typename Type_>
__attribute__((__deprecated__))
static inline Type_ *MSHookMessage(Class _class, SEL sel, Type_ *imp, const char *prefix = NULL) {
    return reinterpret_cast<Type_ *>(MSHookMessage(_class, sel, reinterpret_cast<IMP>(imp), prefix));
}
#endif

template <typename Type_>
static inline void MSHookMessage(Class _class, SEL sel, Type_ *imp, Type_ **result) {
    return MSHookMessageEx(_class, sel, reinterpret_cast<IMP>(imp), reinterpret_cast<IMP *>(result));
}

template <typename Type_>
static inline Type_ &MSHookIvar(id self, const char *name) {
    Ivar ivar(class_getInstanceVariable(object_getClass(self), name));
    #if __has_feature(objc_arc)
        void *pointer(ivar == NULL ? NULL : reinterpret_cast<char *>((__bridge void *)self) + ivar_getOffset(ivar));
    #else
        void *pointer(ivar == NULL ? NULL : reinterpret_cast<char *>(self) + ivar_getOffset(ivar));
    #endif
    return *reinterpret_cast<Type_ *>(pointer);
}

#define MSAddMessage0(_class, type, arg0) \
    class_addMethod($ ## _class, @selector(arg0), (IMP) &$ ## _class ## $ ## arg0, type);
#define MSAddMessage1(_class, type, arg0) \
    class_addMethod($ ## _class, @selector(arg0:), (IMP) &$ ## _class ## $ ## arg0 ## $, type);
#define MSAddMessage2(_class, type, arg0, arg1) \
    class_addMethod($ ## _class, @selector(arg0:arg1:), (IMP) &$ ## _class ## $ ## arg0 ## $ ## arg1 ## $, type);
#define MSAddMessage3(_class, type, arg0, arg1, arg2) \
    class_addMethod($ ## _class, @selector(arg0:arg1:arg2:), (IMP) &$ ## _class ## $ ## arg0 ## $ ## arg1 ## $ ## arg2 ## $, type);
#define MSAddMessage4(_class, type, arg0, arg1, arg2, arg3) \
    class_addMethod($ ## _class, @selector(arg0:arg1:arg2:arg3:), (IMP) &$ ## _class ## $ ## arg0 ## $ ## arg1 ## $ ## arg2 ## $ ## arg3 ## $, type);
#define MSAddMessage5(_class, type, arg0, arg1, arg2, arg3, arg4) \
    class_addMethod($ ## _class, @selector(arg0:arg1:arg2:arg3:arg4:), (IMP) &$ ## _class ## $ ## arg0 ## $ ## arg1 ## $ ## arg2 ## $ ## arg3 ## $ ## arg4 ## $, type);
#define MSAddMessage6(_class, type, arg0, arg1, arg2, arg3, arg4, arg5) \
    class_addMethod($ ## _class, @selector(arg0:arg1:arg2:arg3:arg4:arg5:), (IMP) &$ ## _class ## $ ## arg0 ## $ ## arg1 ## $ ## arg2 ## $ ## arg3 ## $ ## arg4 ## $ ## arg5 ## $, type);

#define MSHookMessage0(_class, arg0) \
    MSHookMessage($ ## _class, @selector(arg0), MSHake(_class ## $ ## arg0))
#define MSHookMessage1(_class, arg0) \
    MSHookMessage($ ## _class, @selector(arg0:), MSHake(_class ## $ ## arg0 ## $))
#define MSHookMessage2(_class, arg0, arg1) \
    MSHookMessage($ ## _class, @selector(arg0:arg1:), MSHake(_class ## $ ## arg0 ## $ ## arg1 ## $))
#define MSHookMessage3(_class, arg0, arg1, arg2) \
    MSHookMessage($ ## _class, @selector(arg0:arg1:arg2:), MSHake(_class ## $ ## arg0 ## $ ## arg1 ## $ ## arg2 ## $))
#define MSHookMessage4(_class, arg0, arg1, arg2, arg3) \
    MSHookMessage($ ## _class, @selector(arg0:arg1:arg2:arg3:), MSHake(_class ## $ ## arg0 ## $ ## arg1 ## $ ## arg2 ## $ ## arg3 ## $))
#define MSHookMessage5(_class, arg0, arg1, arg2, arg3, arg4) \
    MSHookMessage($ ## _class, @selector(arg0:arg1:arg2:arg3:arg4:), MSHake(_class ## $ ## arg0 ## $ ## arg1 ## $ ## arg2 ## $ ## arg3 ## $ ## arg4 ## $))
#define MSHookMessage6(_class, arg0, arg1, arg2, arg3, arg4, arg5) \
    MSHookMessage($ ## _class, @selector(arg0:arg1:arg2:arg3:arg4:arg5:), MSHake(_class ## $ ## arg0 ## $ ## arg1 ## $ ## arg2 ## $ ## arg3 ## $ ## arg4 ## $ ## arg5 ## $))

#define MSRegister_(name, dollar, colon) \
    namespace { static class C_$ ## name ## $ ## dollar { public: _finline C_$ ## name ## $ ##dollar() { \
        MSHookMessage($ ## name, @selector(colon), MSHake(name ## $ ## dollar)); \
    } } V_$ ## name ## $ ## dollar; } \

#define MSIgnore_(name, dollar, colon)

#define MSMessage_(extra, type, _class, name, dollar, colon, call, args...) \
    static type _$ ## name ## $ ## dollar(Class _cls, type (*_old)(_class, SEL, ## args, ...), type (*_spr)(struct objc_super *, SEL, ## args, ...), _class self, SEL _cmd, ## args); \
    MSHook(type, name ## $ ## dollar, _class self, SEL _cmd, ## args) { \
        Class const _cls($ ## name); \
        type (* const _old)(_class, SEL, ## args, ...) = reinterpret_cast<type (* const)(_class, SEL, ## args, ...)>(_ ## name ## $ ## dollar); \
        typedef type (*msgSendSuper_t)(struct objc_super *, SEL, ## args, ...); \
        msgSendSuper_t const _spr(::etl::IsClass<type>::value ? reinterpret_cast<msgSendSuper_t>(&objc_msgSendSuper_stret) : reinterpret_cast<msgSendSuper_t>(&objc_msgSendSuper)); \
        return _$ ## name ## $ ## dollar call; \
    } \
    extra(name, dollar, colon) \
    static _finline type _$ ## name ## $ ## dollar(Class _cls, type (*_old)(_class, SEL, ## args, ...), type (*_spr)(struct objc_super *, SEL, ## args, ...), _class self, SEL _cmd, ## args)

/* for((x=1;x!=7;++x)){ echo -n "#define MSMessage${x}_(extra, type, _class, name";for((y=0;y!=x;++y));do echo -n ", sel$y";done;for((y=0;y!=x;++y));do echo -n ", type$y, arg$y";done;echo ") \\";echo -n "    MSMessage_(extra, type, _class, name,";for((y=0;y!=x;++y));do if [[ $y -ne 0 ]];then echo -n " ##";fi;echo -n " sel$y ## $";done;echo -n ", ";for((y=0;y!=x;++y));do echo -n "sel$y:";done;echo -n ", (_cls, _old, _spr, self, _cmd";for((y=0;y!=x;++y));do echo -n ", arg$y";done;echo -n ")";for((y=0;y!=x;++y));do echo -n ", type$y arg$y";done;echo ")";} */

#define MSMessage0_(extra, type, _class, name, sel0) \
    MSMessage_(extra, type, _class, name, sel0, sel0, (_cls, _old, _spr, self, _cmd))
#define MSMessage1_(extra, type, _class, name, sel0, type0, arg0) \
    MSMessage_(extra, type, _class, name, sel0 ## $, sel0:, (_cls, _old, _spr, self, _cmd, arg0), type0 arg0)
#define MSMessage2_(extra, type, _class, name, sel0, sel1, type0, arg0, type1, arg1) \
    MSMessage_(extra, type, _class, name, sel0 ## $ ## sel1 ## $, sel0:sel1:, (_cls, _old, _spr, self, _cmd, arg0, arg1), type0 arg0, type1 arg1)
#define MSMessage3_(extra, type, _class, name, sel0, sel1, sel2, type0, arg0, type1, arg1, type2, arg2) \
    MSMessage_(extra, type, _class, name, sel0 ## $ ## sel1 ## $ ## sel2 ## $, sel0:sel1:sel2:, (_cls, _old, _spr, self, _cmd, arg0, arg1, arg2), type0 arg0, type1 arg1, type2 arg2)
#define MSMessage4_(extra, type, _class, name, sel0, sel1, sel2, sel3, type0, arg0, type1, arg1, type2, arg2, type3, arg3) \
    MSMessage_(extra, type, _class, name, sel0 ## $ ## sel1 ## $ ## sel2 ## $ ## sel3 ## $, sel0:sel1:sel2:sel3:, (_cls, _old, _spr, self, _cmd, arg0, arg1, arg2, arg3), type0 arg0, type1 arg1, type2 arg2, type3 arg3)
#define MSMessage5_(extra, type, _class, name, sel0, sel1, sel2, sel3, sel4, type0, arg0, type1, arg1, type2, arg2, type3, arg3, type4, arg4) \
    MSMessage_(extra, type, _class, name, sel0 ## $ ## sel1 ## $ ## sel2 ## $ ## sel3 ## $ ## sel4 ## $, sel0:sel1:sel2:sel3:sel4:, (_cls, _old, _spr, self, _cmd, arg0, arg1, arg2, arg3, arg4), type0 arg0, type1 arg1, type2 arg2, type3 arg3, type4 arg4)
#define MSMessage6_(extra, type, _class, name, sel0, sel1, sel2, sel3, sel4, sel5, type0, arg0, type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5) \
    MSMessage_(extra, type, _class, name, sel0 ## $ ## sel1 ## $ ## sel2 ## $ ## sel3 ## $ ## sel4 ## $ ## sel5 ## $, sel0:sel1:sel2:sel3:sel4:sel5:, (_cls, _old, _spr, self, _cmd, arg0, arg1, arg2, arg3, arg4, arg5), type0 arg0, type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5)

#define MSInstanceMessage0(type, _class, args...) MSMessage0_(MSIgnore_, type, _class *, _class, ## args)
#define MSInstanceMessage1(type, _class, args...) MSMessage1_(MSIgnore_, type, _class *, _class, ## args)
#define MSInstanceMessage2(type, _class, args...) MSMessage2_(MSIgnore_, type, _class *, _class, ## args)
#define MSInstanceMessage3(type, _class, args...) MSMessage3_(MSIgnore_, type, _class *, _class, ## args)
#define MSInstanceMessage4(type, _class, args...) MSMessage4_(MSIgnore_, type, _class *, _class, ## args)
#define MSInstanceMessage5(type, _class, args...) MSMessage5_(MSIgnore_, type, _class *, _class, ## args)
#define MSInstanceMessage6(type, _class, args...) MSMessage6_(MSIgnore_, type, _class *, _class, ## args)

#define MSClassMessage0(type, _class, args...) MSMessage0_(MSIgnore_, type, Class, $ ## _class, ## args)
#define MSClassMessage1(type, _class, args...) MSMessage1_(MSIgnore_, type, Class, $ ## _class, ## args)
#define MSClassMessage2(type, _class, args...) MSMessage2_(MSIgnore_, type, Class, $ ## _class, ## args)
#define MSClassMessage3(type, _class, args...) MSMessage3_(MSIgnore_, type, Class, $ ## _class, ## args)
#define MSClassMessage4(type, _class, args...) MSMessage4_(MSIgnore_, type, Class, $ ## _class, ## args)
#define MSClassMessage5(type, _class, args...) MSMessage5_(MSIgnore_, type, Class, $ ## _class, ## args)
#define MSClassMessage6(type, _class, args...) MSMessage6_(MSIgnore_, type, Class, $ ## _class, ## args)

#define MSInstanceMessageHook0(type, _class, args...) MSMessage0_(MSRegister_, type, _class *, _class, ## args)
#define MSInstanceMessageHook1(type, _class, args...) MSMessage1_(MSRegister_, type, _class *, _class, ## args)
#define MSInstanceMessageHook2(type, _class, args...) MSMessage2_(MSRegister_, type, _class *, _class, ## args)
#define MSInstanceMessageHook3(type, _class, args...) MSMessage3_(MSRegister_, type, _class *, _class, ## args)
#define MSInstanceMessageHook4(type, _class, args...) MSMessage4_(MSRegister_, type, _class *, _class, ## args)
#define MSInstanceMessageHook5(type, _class, args...) MSMessage5_(MSRegister_, type, _class *, _class, ## args)
#define MSInstanceMessageHook6(type, _class, args...) MSMessage6_(MSRegister_, type, _class *, _class, ## args)

#define MSClassMessageHook0(type, _class, args...) MSMessage0_(MSRegister_, type, Class, $ ## _class, ## args)
#define MSClassMessageHook1(type, _class, args...) MSMessage1_(MSRegister_, type, Class, $ ## _class, ## args)
#define MSClassMessageHook2(type, _class, args...) MSMessage2_(MSRegister_, type, Class, $ ## _class, ## args)
#define MSClassMessageHook3(type, _class, args...) MSMessage3_(MSRegister_, type, Class, $ ## _class, ## args)
#define MSClassMessageHook4(type, _class, args...) MSMessage4_(MSRegister_, type, Class, $ ## _class, ## args)
#define MSClassMessageHook5(type, _class, args...) MSMessage5_(MSRegister_, type, Class, $ ## _class, ## args)
#define MSClassMessageHook6(type, _class, args...) MSMessage6_(MSRegister_, type, Class, $ ## _class, ## args)

#define MSOldCall(args...) \
    _old(self, _cmd, ## args)
#define MSSuperCall(args...) \
    _spr(& (struct objc_super) {self, class_getSuperclass(_cls)}, _cmd, ## args)

#define MSIvarHook(type, name) \
    type &name(MSHookIvar<type>(self, #name))

#define MSClassHook(name) \
    @class name; \
    static Class $ ## name = objc_getClass(#name);
#define MSMetaClassHook(name) \
    @class name; \
    static Class $$ ## name = object_getClass($ ## name);

#endif/*__APPLE__*/

template <typename Type_>
static inline void MSHookFunction(Type_ *symbol, Type_ *replace, Type_ **result) {
    return MSHookFunction(
        reinterpret_cast<void *>(symbol),
        reinterpret_cast<void *>(replace),
        reinterpret_cast<void **>(result)
    );
}

template <typename Type_>
static inline void MSHookFunction(Type_ *symbol, Type_ *replace) {
    return MSHookFunction(symbol, replace, reinterpret_cast<Type_ **>(NULL));
}

template <typename Type_>
static inline void MSHookSymbol(Type_ *&value, const char *name, MSImageRef image = NULL) {
    value = reinterpret_cast<Type_ *>(MSFindSymbol(image, name));
}

template <typename Type_>
static inline void MSHookFunction(const char *name, Type_ *replace, Type_ **result = NULL) {
    Type_ *symbol;
    MSHookSymbol(symbol, name);
    return MSHookFunction(symbol, replace, result);
}

template <typename Type_>
static inline void MSHookFunction(MSImageRef image, const char *name, Type_ *replace, Type_ **result = NULL) {
    Type_ *symbol;
    MSHookSymbol(symbol, name, image);
    return MSHookFunction(symbol, replace, result);
}

#endif

#ifdef __ANDROID__

template <typename Type_>
static inline void MSJavaHookMethod(JNIEnv *jni, jclass _class, jmethodID method, Type_ *replace, Type_ **result) {
    return MSJavaHookMethod(
        jni, _class, method,
        reinterpret_cast<void *>(replace),
        reinterpret_cast<void **>(result)
    );
}

static inline void MSAndroidGetPackage(JNIEnv *jni, jobject global, const char *name, jobject &local, jobject &loader) {
    jclass Context(jni->FindClass("android/content/Context"));
    jmethodID Context$createPackageContext(jni->GetMethodID(Context, "createPackageContext", "(Ljava/lang/String;I)Landroid/content/Context;"));
    jmethodID Context$getClassLoader(jni->GetMethodID(Context, "getClassLoader", "()Ljava/lang/ClassLoader;"));

    jstring string(jni->NewStringUTF(name));
    local = jni->CallObjectMethod(global, Context$createPackageContext, string, 3);
    loader = jni->CallObjectMethod(local, Context$getClassLoader);
}

static inline jclass MSJavaFindClass(JNIEnv *jni, jobject loader, const char *name) {
    jclass Class(jni->FindClass("java/lang/Class"));
    jmethodID Class$forName(jni->GetStaticMethodID(Class, "forName", "(Ljava/lang/String;ZLjava/lang/ClassLoader;)Ljava/lang/Class;"));

    jstring string(jni->NewStringUTF(name));
    jobject _class(jni->CallStaticObjectMethod(Class, Class$forName, string, JNI_TRUE, loader));
    if (jni->ExceptionCheck())
        return NULL;

    return reinterpret_cast<jclass>(_class);
}

_disused static void MSJavaCleanWeak(void *data, JNIEnv *jni, void *value) {
    jni->DeleteWeakGlobalRef(reinterpret_cast<jweak>(value));
}

#endif

#define MSHook(type, name, args...) \
    _disused static type (*_ ## name)(args); \
    static type $ ## name(args)

#define MSJavaHook(type, name, args...) \
    MSHook(type, name, JNIEnv *jni, ## args)

#ifdef __cplusplus
#define MSHake(name) \
    &$ ## name, &_ ## name
#else
#define MSHake(name) \
    &$ ## name, (void **) &_ ## name
#endif

#define SubstrateConcat_(lhs, rhs) \
    lhs ## rhs
#define SubstrateConcat(lhs, rhs) \
    SubstrateConcat_(lhs, rhs)

#ifdef __APPLE__
    #define SubstrateSection \
        __attribute__((__section__("__TEXT, __substrate")))
#else
    #define SubstrateSection \
        __attribute__((__section__(".substrate")))
#endif

#ifdef __APPLE__
#define MSFilterCFBundleID "Filter:CFBundleID"
#define MSFilterObjC_Class "Filter:ObjC.Class"
#endif

#define MSFilterExecutable "Filter:Executable"

#define MSConfig(name, value) \
    extern const char SubstrateConcat(_substrate_, __LINE__)[] SubstrateSection = name "=" value;

#ifdef __cplusplus
#define MSInitialize \
    static void _MSInitialize(void); \
    namespace { static class $MSInitialize { public: _finline $MSInitialize() { \
        _MSInitialize(); \
    } } $MSInitialize; } \
    static void _MSInitialize()
#else
#define MSInitialize \
    __attribute__((__constructor__)) static void _MSInitialize(void)
#endif

#define Foundation_f "/System/Library/Frameworks/Foundation.framework/Foundation"
#define UIKit_f "/System/Library/Frameworks/UIKit.framework/UIKit"
#define JavaScriptCore_f "/System/Library/PrivateFrameworks/JavaScriptCore.framework/JavaScriptCore"
#define IOKit_f "/System/Library/Frameworks/IOKit.framework/IOKit"

#endif//SUBSTRATE_H_
