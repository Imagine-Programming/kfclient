#ifndef kfclient_libdef_hpp
#define kfclient_libdef_hpp

#ifdef KFCLIENT_WINDOWS
#ifdef KFCLIENT_BUILD_DLL
#define KFCLIENT_API __declspec(dllexport)
#else
#define KFCLIENT_API __declspec(dllimport)
#endif
#define KFCLIENT_PACKED
#else
#define KFCLIENT_API 
#define KFCLIENT_PACKED __attribute__((packed))
#endif 

#endif 