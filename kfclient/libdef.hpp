#ifndef kfclient_libdef_hpp
#define kfclient_libdef_hpp

#ifdef KFCLIENT_WINDOWS
#ifdef KFCLIENT_BUILD_DLL
#define KFCLIENT_API __declspec(dllexport) // NOLINT(cppcoreguidelines-macro-usage)
#else
#define KFCLIENT_API __declspec(dllimport) // NOLINT(cppcoreguidelines-macro-usage)
#endif
#else
#define KFCLIENT_API 
#endif 

#endif 