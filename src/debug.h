#ifndef __INCLUDE_DEBUG_H__
#define __INCLUDE_DEBUG_H__

#ifdef ENABLE_DEBUG
#define DEBUG(x) printf x
#else
#define DEBUG(x)
#endif

#endif /* __INCLUDE_DEBUG_H__ */
