#pragma once

typedef enum {
  en_dbg,
  en_info,
  en_warning,
  en_err,
} en_log_t;

#define LOG(type, format, ...)                                       \
  do {                                                               \
    log_output(type, __FUNCTION__, __LINE__, format, ##__VA_ARGS__); \
  } while (0)

#define LOG_ERR(format, ...) LOG(en_err, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) LOG(en_dbg, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG(en_info, format, ##__VA_ARGS__)
#define LOG_DBG(format, ...) LOG(en_dbg, format, ##__VA_ARGS__)

extern void log_output(en_log_t en_log_type, const char *func,
                       const size_t line, const char *format, ...);
extern void set_log_level(en_log_t en_level);
extern void init_log(void);
