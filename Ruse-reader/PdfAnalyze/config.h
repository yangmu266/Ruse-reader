#ifndef CONFIG_H
#define CONFIG_H


#define HAVE_FSEEKO 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_GMTIME_R 1
#define HAVE_MKSTEMP 1
#define HAVE_UNISTD_H 1
#define HAVE_DIRENT_H 1
#define TEXTOUT_WORD_LIST 1

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define GCC_PRINTF_FORMAT(fmt_index, va_index) \
__attribute__((__format__(__printf__, fmt_index, va_index)))
#else
#define GCC_PRINTF_FORMAT(fmt_index, va_index)
#endif

#define POPPLER_DATADIR getHomeDir()->getCString()


#endif /* CONFIG_H */