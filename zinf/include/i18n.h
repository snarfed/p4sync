/* @(#)i18n.h
 */

#ifndef _I18N_H
#define _I18N_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "gettext.h"
# define _(X) gettext(X)
# define N_(X) gettext_noop(X)

#ifdef __cplusplus
}
#endif

#endif /* _I18N_H */

/* arch-tag: c4417c73-abde-4925-b59f-e84179e33dff
   (do not change this comment) */
