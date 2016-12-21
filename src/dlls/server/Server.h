#ifdef CLASSICOFFENSIVE_EXPORTS
#define CLASSICOFFENSIVE_API __declspec(dllexport)
#else
#define CLASSICOFFENSIVE_API __declspec(dllimport)
#endif

extern uintptr_t g_dwServerBase;

extern char g_szBuffer[ 1024 ];
extern wchar_t g_wzBuffer[ 1024 ];