#include <os/log.h>

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
enum os_log_type: uint8_t
{
    OS_LOG_TYPE_INFO    = 0x01,
    OS_LOG_TYPE_DEBUG   = 0x02,
    OS_LOG_TYPE_ERROR   = 0x10,
    OS_LOG_TYPE_FAULT  = 0x11
};

#define os_log_n(log, type, format, ...) __extension__({ \
    os_log_t _log_tmp = (log); \
    os_log_type_t _type_tmp = static_cast<os_log_type_t>(type); \
    if (os_log_type_enabled(_log_tmp, _type_tmp)) { \
} \
})
//        OS_LOG_CALL_WITH_FORMAT(_os_log_impl, \
//                (&__dso_handle, _log_tmp, _type_tmp), format, ##__VA_ARGS__); \


#define Log(fmt, ...) os_log_n(OS_LOG_DEFAULT, OS_LOG_TYPE_DEFAULT, "[\(__FUNCTION__)]" fmt "\n", ##__VA_ARGS__)
#define LogI(fmt, ...) os_log_n(OS_LOG_DEFAULT, OS_LOG_TYPE_INFO, "[\(__FUNCTION__)]" fmt "\n", ##__VA_ARGS__)
#define LogD(fmt, ...) os_log_n(OS_LOG_DEFAULT, OS_LOG_TYPE_DEBUG, "[\(__FUNCTION__)]" fmt "\n", ##__VA_ARGS__)
#define LogE(fmt, ...) os_log_n(OS_LOG_DEFAULT, OS_LOG_TYPE_ERROR, "[\(__FUNCTION__)]" fmt "\n", ##__VA_ARGS__)
#define LogF(fmt, ...) os_log_n(OS_LOG_DEFAULT, OS_LOG_TYPE_FAULT, "[\(__FUNCTION__)]" fmt "\n", ##__VA_ARGS__)
