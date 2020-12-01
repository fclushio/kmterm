#ifndef KMTERM_LOG_H
#define KMTERM_LOG_H

/*
 * This logger follows the idea of dividing compile-time and run-time log level
 * selection. So, define KMTERM_LOG_MAXIMUM at compile time defines maximum
 * possible log level. And you can change current log level between
 * KMTERM_LOG_MINIMUM and KMTERM_LOG_MAXIMUM.
 */

#define KMTERM_LOG_MINIMUM (0)
#define KMTERM_LOG_NO      KMTERM_LOG_MINIMUM
#define KMTERM_LOG_ERROR   (1)
#define KMTERM_LOG_WARNING (2)
#define KMTERM_LOG_INFO    (3)
#define KMTERM_LOG_DEBUG   (4)

#define KMTERM_LOG_PREFIX "kmterm: "


void kmterm_log_setLevel(int _level);

/*
 * Depends on nothing.
 */
int kmterm_log_init(void);



extern void __kmterm_log(const char * _file, int _line, const char * _function,
                         int _level, char * _fmt, ...);
extern void __kmterm_msg(int _level, char * _fmt, ...);

/*
 * Compile-time defined maximum log level
 */
#ifndef KMTERM_LOG_MAXIMUM
#   define KMTERM_LOG_MAXIMUM KMTERM_LOG_DEBUG
#endif


/* Debug log macro functions */
#if KMTERM_LOG_MAXIMUM < KMTERM_LOG_ERROR
#   define KMTERM_ERR(format, ...)
#else
#   define KMTERM_ERR(format, ...)   __kmterm_log(__FILE__, __LINE__, __FUNCTION__, KMTERM_LOG_ERROR, format, ##__VA_ARGS__)
#endif

#if KMTERM_LOG_MAXIMUM < KMTERM_LOG_WARNING
#   define KMTERM_ERR(format, ...)
#else
#   define KMTERM_WRN(format, ...)   __kmterm_log(__FILE__, __LINE__, __FUNCTION__, KMTERM_LOG_WARNING, format, ##__VA_ARGS__)
#endif

#if KMTERM_LOG_MAXIMUM < KMTERM_LOG_INFO
#   define KMTERM_ERR(format, ...)
#else
#   define KMTERM_INF(format, ...)   __kmterm_log(__FILE__, __LINE__, __FUNCTION__, KMTERM_LOG_INFO, format, ##__VA_ARGS__)
#endif

#if KMTERM_LOG_MAXIMUM < KMTERM_LOG_DEBUG
#   define KMTERM_ERR(format, ...)
#else
#   define KMTERM_DBG(format, ...)   __kmterm_log(__FILE__, __LINE__, __FUNCTION__, KMTERM_LOG_DEBUG, format, ##__VA_ARGS__)
#endif


/* Message log macro functions */
#define KMTERM_MESG(format, ...)  __kmterm_msg(KMTERM_LOG_NO,      format, ##__VA_ARGS__)
#define KMTERM_ERRM(format, ...)  __kmterm_msg(KMTERM_LOG_ERROR,   format, ##__VA_ARGS__)
#define KMTERM_WRNM(format, ...)  __kmterm_msg(KMTERM_LOG_WARNING, format, ##__VA_ARGS__)
#define KMTERM_INFM(format, ...)  __kmterm_msg(KMTERM_LOG_INFO,    format, ##__VA_ARGS__)


#endif // KMTERM_LOG_H
