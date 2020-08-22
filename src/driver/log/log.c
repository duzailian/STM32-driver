#include "include.h"

typedef struct {
  en_log_t en_log_level;
  OS_MUTEX mutex;
} st_info_t;

static st_info_t st_info = {
    .en_log_level = en_dbg,
};

extern void set_log_level(en_log_t en_level) {
  st_info.en_log_level = en_level;
  return;
}

#ifdef BOOT_PRJ

extern void log_output(en_log_t en_log_type, const char *func,
                       const size_t line, const char *format, ...) {
  va_list args;
  const char *apc_level[] = {
      [en_dbg] = "[DBG]",
      [en_info] = "[LOG]",
      [en_warning] = "[WARNING]",
      [en_err] = "[ERROR]",
  };

  if (en_log_type < st_info.en_log_level) goto Return;

  printf("%sFunc:%s,Line:%zd,", apc_level[en_log_type], func, line);
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
Return:
  return;
}
extern void init_log(void) {
  open_usart(PRINT_USART);
#if DRV_DBG
  set_log_level(en_dbg);
#else
  set_log_level(en_info);
#endif
  return;
}
#else
extern void log_output(en_log_t en_log_type, const char *func,
                       const size_t line, const char *format, ...) {
  va_list args;
  const char *apc_level[] = {
      [en_dbg] = "[DBG]",
      [en_info] = "[LOG]",
      [en_warning] = "[WARNING]",
      [en_err] = "[ERROR]",
  };
  OS_ERR err;

  if (en_log_type < st_info.en_log_level) goto Return;

  OSMutexPend(&st_info.mutex, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
  if (OS_ERR_NONE != err) {
    printf("Log get mutex error!");
    goto Return;
  }
  printf("%sFunc:%s,Line:%zd,", apc_level[en_log_type], func, line);
  va_start(args, format);
  vprintf(format, args);
  OSMutexPost(&st_info.mutex, OS_OPT_POST_NONE, &err);
  va_end(args);
Return:
  return;
}

extern void init_log(void) {
  OS_ERR err;
  open_usart(PRINT_USART);

  OSMutexCreate(&st_info.mutex, "log mutex", &err);
  if (OS_ERR_NONE != err) printf("log init error!");
#if DRV_DBG
  set_log_level(en_dbg);
#else
  set_log_level(en_info);
#endif
  return;
}
#endif