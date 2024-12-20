#ifndef __PCH__
#define __PCH__
#define __OS_LOG_H__
//#include <os/log.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <os/availability.h>
#include <os/base.h>

__BEGIN_DECLS
/*!
 * @enum os_log_type_t
 *
 * @discussion
 * Supported log message types.
 *
 * @constant OS_LOG_TYPE_DEFAULT
 * Equivalent type for "os_log()" messages, i.e., default messages that are always
 * captured to memory or disk.
 *
 * @constant OS_LOG_TYPE_INFO
 * Equivalent type for "os_log_info()" messages, i.e., Additional informational messages.
 *
 * @constant OS_LOG_TYPE_DEBUG
 * Equivalent type for "os_log_debug()" messages, i.e., Debug messages.
 *
 * @constant OS_LOG_TYPE_ERROR
 * Equivalent type for "os_log_error()" messages, i.e., local process error messages.
 *
 * @constant OS_LOG_TYPE_FAULT
 * Equivalent type for "os_log_fault()" messages, i.e., a system error that involves
 * potentially more than one process, usually used by daemons and services.
 */
OS_ENUM(os_log_type, uint8_t,
    OS_LOG_TYPE_DEFAULT = 0x00,
    OS_LOG_TYPE_INFO    = 0x01,
    OS_LOG_TYPE_DEBUG   = 0x02,
    OS_LOG_TYPE_ERROR   = 0x10,
    OS_LOG_TYPE_FAULT  = 0x11
);

struct os_log_s;
typedef struct os_log_s *os_log_t;

#define OS_LOG_DEFAULT ((os_log_t)&_os_log_default)

API_AVAILABLE(driverkit(19.0))
OS_EXPORT
struct os_log_s _os_log_default;

API_AVAILABLE(driverkit(19.0))
OS_EXPORT OS_NOTHROW OS_NOT_TAIL_CALLED
void
_os_log_impl(void *dso, os_log_t log, os_log_type_t type,
        const char *format, uint8_t *buf, uint32_t size);

API_AVAILABLE(driverkit(19.0))
OS_EXPORT OS_NOTHROW OS_WARN_RESULT OS_PURE
bool
os_log_type_enabled(os_log_t oslog, os_log_type_t type);

extern struct mach_header __dso_handle;

#ifdef OS_LOG_FORMAT_WARNINGS
#define OS_LOG_FORMAT_ERRORS _Pragma("clang diagnostic warning \"-Wformat\"")
#else
#define OS_LOG_FORMAT_ERRORS _Pragma("clang diagnostic error \"-Wformat\"")
#endif

#define OS_LOG_PRAGMA_PUSH \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wvla\"") \
    OS_LOG_FORMAT_ERRORS

#define OS_LOG_PRAGMA_POP \
    _Pragma("clang diagnostic pop")

#if __has_attribute(uninitialized)
#define OS_LOG_UNINITIALIZED __attribute__((uninitialized))
#else
#define OS_LOG_UNINITIALIZED
#endif

#define OS_LOG_STRING(_ns, _var, _str) \
    _Static_assert(__builtin_constant_p(_str), \
            "format/label/description argument must be a string constant"); \
    __attribute__((section("__TEXT,__oslogstring,cstring_literals"),internal_linkage)) \
    static const char _var[] __asm(OS_STRINGIFY(OS_CONCAT(LOS_##_ns, __COUNTER__))) = _str

#define OS_LOG_REMOVE_PARENS(...) __VA_ARGS__

#define OS_LOG_CALL_WITH_FORMAT(fun, fun_args, fmt, ...) __extension__({ \
    OS_LOG_PRAGMA_PUSH OS_LOG_STRING(LOG, _os_fmt_str, fmt); \
    uint8_t _Alignas(16) OS_LOG_UNINITIALIZED _os_fmt_buf[__builtin_os_log_format_buffer_size(fmt, ##__VA_ARGS__)]; \
    fun(OS_LOG_REMOVE_PARENS fun_args, _os_fmt_str, \
            (uint8_t *)__builtin_os_log_format(_os_fmt_buf, fmt, ##__VA_ARGS__), \
            (uint32_t)sizeof(_os_fmt_buf)) OS_LOG_PRAGMA_POP; \
})

#define os_log_n(log, type, format, ...) __extension__({ \
    os_log_t _log_tmp = (log); \
    os_log_type_t _type_tmp = type; \
    OS_LOG_CALL_WITH_FORMAT(_os_log_impl, \
        (&__dso_handle, _log_tmp, _type_tmp), format, ##__VA_ARGS__); \
})
    //if (os_log_type_enabled(_log_tmp, _type_tmp)) { \

//} \


#define LogI(fmt, ...) os_log_n(OS_LOG_DEFAULT, OS_LOG_TYPE_INFO, "Info  [%{public}s] " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__)
#define LogD(fmt, ...) os_log_n(OS_LOG_DEFAULT, OS_LOG_TYPE_DEBUG, "Debug [%{public}s] " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__)
#define LogE(fmt, ...) os_log_n(OS_LOG_DEFAULT, OS_LOG_TYPE_ERROR, "Error [%{public}s] " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__)
#define LogF(fmt, ...) os_log_n(OS_LOG_DEFAULT, OS_LOG_TYPE_FAULT, "Fault [%{public}s] " fmt "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__)

__END_DECLS

#endif
