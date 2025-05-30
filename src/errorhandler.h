#if !defined(ErrorHandler_DEF)
#define ErrorHandler_DEF

#define ERRORCODE_GLFW 2
#define ERRORCODE_GLAD 3

extern void Info(const char *__format, ...);
extern void Warning(const char *__format, ...);
extern void Error(const char *error_message);
extern void PANIC(int error_code, const char* error_message);

#endif // ErrorHandler_DEF
