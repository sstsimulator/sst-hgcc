/* Mercury runtime calls that hgcc pragmas emit unqualified. */
#ifndef HGCC_APP_H
#define HGCC_APP_H

#include <mercury/common/skeleton.h>

#ifdef __cplusplus
using SST::Hg::ssthg_sleep;
using SST::Hg::ssthg_usleep;
using SST::Hg::ssthg_nanosleep;
#endif

#endif
