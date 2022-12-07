
#define debugf(...) printf( __VA_ARGS__);

#ifdef DEBUG
#define debug(...) debugf(__VA_ARGS__)
#else
#define debug(...)
#endif
