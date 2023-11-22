/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uv.h"
#include "internal.h"

#define __NEW_STARLET 1
#include <climsgdef.h>
#include <libdef.h>
#include <rmsdef.h>
#include <ssdef.h>
#include <stsdef.h>

/* Translate OpenVMS condition values to libuv errors. */
int uv__translate_vms_error(unsigned int sys_errno) {

#define VMS_ERROR_CASE(sts, e)  case ((sts) >> STS$S_SEVERITY):  return (e)

  /* Mask/shift out the severity bits before comparing. */
  switch ((sys_errno & STS$M_COND_ID) >> STS$S_SEVERITY) {
    /* system service severe errors part 1 */
    VMS_ERROR_CASE(SS$_ACCVIO,              UV_EFAULT);
    VMS_ERROR_CASE(SS$_BADPARAM,            UV_EINVAL);
    VMS_ERROR_CASE(SS$_EXQUOTA,             UV_EPERM);
    VMS_ERROR_CASE(SS$_NOPRIV,              UV_EPERM);
    VMS_ERROR_CASE(SS$_ABORT,               UV_EIO);
    VMS_ERROR_CASE(SS$_BADATTRIB,           UV_EINVAL);
    VMS_ERROR_CASE(SS$_BADESCAPE,           UV_EINVAL);
    VMS_ERROR_CASE(SS$_CHANINTLK,           UV_EBUSY);
    VMS_ERROR_CASE(SS$_CTRLERR,             UV_EIO);
    VMS_ERROR_CASE(SS$_DATACHECK,           UV_EIO);
    VMS_ERROR_CASE(SS$_DEVFOREIGN,          UV_ENODEV);
    VMS_ERROR_CASE(SS$_DEVMOUNT,            UV_EBUSY);
    VMS_ERROR_CASE(SS$_DEVNOTMBX,           UV_ENOTTY);
    VMS_ERROR_CASE(SS$_DEVNOTMOUNT,         UV_ENODEV);
    VMS_ERROR_CASE(SS$_DEVOFFLINE,          UV_ENODEV);
    VMS_ERROR_CASE(SS$_DRVERR,              UV_EIO);
    VMS_ERROR_CASE(SS$_FILALRACC,           UV_EBUSY);
    VMS_ERROR_CASE(SS$_FILNOTACC,           UV_EBADF);
    VMS_ERROR_CASE(SS$_FORMAT,              UV_EINVAL);
    VMS_ERROR_CASE(SS$_GPTFULL,             UV_ENOMEM);
    VMS_ERROR_CASE(SS$_GSDFULL,             UV_ENOMEM);
    VMS_ERROR_CASE(SS$_LCKPAGFUL,           UV_ENOMEM);
    VMS_ERROR_CASE(SS$_ILLBLKNUM,           UV_EINVAL);
    VMS_ERROR_CASE(SS$_ILLCNTRFUNC,         UV_EINVAL);
    VMS_ERROR_CASE(SS$_ILLEFC,              UV_EINVAL);
    VMS_ERROR_CASE(SS$_ILLIOFUNC,           UV_EINVAL);
    VMS_ERROR_CASE(SS$_INSFARG,             UV_EINVAL);
    VMS_ERROR_CASE(SS$_INSFWSL,             UV_ENOMEM);
    VMS_ERROR_CASE(SS$_INSFMEM,             UV_ENOMEM);
    VMS_ERROR_CASE(SS$_INSFRAME,            UV_EINVAL);
    VMS_ERROR_CASE(SS$_IVADDR,              UV_EIO);
    VMS_ERROR_CASE(SS$_IVCHAN,              UV_EBADF);
    VMS_ERROR_CASE(SS$_IVDEVNAM,            UV_EINVAL);
    VMS_ERROR_CASE(SS$_IVGSDNAM,            UV_EINVAL);
    VMS_ERROR_CASE(SS$_IVLOGNAM,            UV_EINVAL);
    VMS_ERROR_CASE(SS$_IVLOGTAB,            UV_EINVAL);
    VMS_ERROR_CASE(SS$_IVQUOTAL,            UV_EINVAL);
    VMS_ERROR_CASE(SS$_IVSECFLG,            UV_EINVAL);
    VMS_ERROR_CASE(SS$_IVSSRQ,              UV_EINVAL);
    VMS_ERROR_CASE(SS$_IVSTSFLG,            UV_EINVAL);
    VMS_ERROR_CASE(SS$_IVTIME,              UV_EINVAL);
    VMS_ERROR_CASE(SS$_LENVIO,              UV_EFAULT);
    VMS_ERROR_CASE(SS$_LKWSETFUL,           UV_ENOMEM);
    VMS_ERROR_CASE(SS$_MBTOOSML,            UV_EMSGSIZE);
    VMS_ERROR_CASE(SS$_MEDOFL,              UV_EIO);
    VMS_ERROR_CASE(SS$_NOIOCHAN,            UV_EMFILE);
    VMS_ERROR_CASE(SS$_NOLOGNAM,            UV_ENOENT);
    VMS_ERROR_CASE(SS$_NONEXDRV,            UV_ENODEV);
    VMS_ERROR_CASE(SS$_NOTSQDEV,            UV_ENOTTY);
    VMS_ERROR_CASE(SS$_PAGOWNVIO,           UV_EFAULT);
    VMS_ERROR_CASE(SS$_PARITY,              UV_EIO);
    VMS_ERROR_CASE(SS$_PARTESCAPE,          UV_ENOBUFS);
    VMS_ERROR_CASE(SS$_PSTFULL,             UV_ENOMEM);
    VMS_ERROR_CASE(SS$_RESULTOVF,           UV_ENOBUFS);
    VMS_ERROR_CASE(SS$_SECTBLFUL,           UV_ENOMEM);
    VMS_ERROR_CASE(SS$_TIMEOUT,             UV_ETIMEDOUT);
    VMS_ERROR_CASE(SS$_UNASEFC,             UV_EINVAL);
    VMS_ERROR_CASE(SS$_UNSAFE,              UV_EIO);
    VMS_ERROR_CASE(SS$_VASFULL,             UV_ENOMEM);
    VMS_ERROR_CASE(SS$_VOLINV,              UV_ENODEV);
    VMS_ERROR_CASE(SS$_WRITLCK,             UV_EROFS);
    VMS_ERROR_CASE(SS$_NOSUCHNODE,          UV_ENOENT);
    VMS_ERROR_CASE(SS$_REJECT,              UV_ECONNREFUSED);
    VMS_ERROR_CASE(SS$_BUGCHECK,            UV_UNKNOWN);
    VMS_ERROR_CASE(SS$_IVBUFLEN,            UV_EIO);
    VMS_ERROR_CASE(SS$_SUSPENDED,           UV_EINVAL);
    VMS_ERROR_CASE(SS$_NODISKQUOTA,         UV_EPERM);
    VMS_ERROR_CASE(SS$_EXDISKQUOTA,         UV_EPERM);

    /* system service warning codes */
    VMS_ERROR_CASE(SS$_ACCONFLICT,          UV_EBUSY);
    VMS_ERROR_CASE(SS$_BADFILENAME,         UV_EINVAL);
    VMS_ERROR_CASE(SS$_BADFILEVER,          UV_EINVAL);
    VMS_ERROR_CASE(SS$_BADIRECTORY,         UV_ENOENT);
    VMS_ERROR_CASE(SS$_CANCEL,              UV_ECANCELED);
    VMS_ERROR_CASE(SS$_DATAOVERUN,          UV_ENOBUFS);
    VMS_ERROR_CASE(SS$_DEVALLOC,            UV_EBUSY);
    VMS_ERROR_CASE(SS$_DEVICEFULL,          UV_ENOSPC);
    VMS_ERROR_CASE(SS$_DIRFULL,             UV_ENOSPC);
    VMS_ERROR_CASE(SS$_DUPFILENAME,         UV_EEXIST);
    VMS_ERROR_CASE(SS$_ENDOFFILE,           UV_EOF);
    VMS_ERROR_CASE(SS$_MBFULL,              UV_EBUSY);
    VMS_ERROR_CASE(SS$_NONEXPR,             UV_ENOENT);
    VMS_ERROR_CASE(SS$_NONLOCAL,            UV_ENODEV);
    VMS_ERROR_CASE(SS$_NOSUCHDEV,           UV_ENODEV);
    VMS_ERROR_CASE(SS$_NOSUCHFILE,          UV_ENOENT);
    VMS_ERROR_CASE(SS$_NOMOREPROC,          UV_ENOENT);
    VMS_ERROR_CASE(SS$_NOTQUEUED,           UV_EBUSY);
    VMS_ERROR_CASE(SS$_ACLEMPTY,            UV_ENOENT);
    VMS_ERROR_CASE(SS$_ITEMNOTFOUND,        UV_EINVAL);
    VMS_ERROR_CASE(SS$_NOTF11ODS5,          UV_EINVAL);

    /* system service nonfatal error codes */
    VMS_ERROR_CASE(SS$_UNSUPPORTED,         UV_EINVAL);
    VMS_ERROR_CASE(SS$_INVARG,              UV_EINVAL);

    /* system service severe errors part 2 */
    VMS_ERROR_CASE(SS$_PROTOCOL,            UV_EPROTO);
    VMS_ERROR_CASE(SS$_SHUT,                UV_ECONNREFUSED);
    VMS_ERROR_CASE(SS$_UNREACHABLE,         UV_EHOSTUNREACH);
    VMS_ERROR_CASE(SS$_CONNECFAIL,          UV_ECONNABORTED);
    VMS_ERROR_CASE(SS$_LINKABORT,           UV_ECONNABORTED);
    VMS_ERROR_CASE(SS$_LINKDISCON,          UV_EPIPE);
    VMS_ERROR_CASE(SS$_TOOMANYREDS,         UV_ELOOP);
    VMS_ERROR_CASE(SS$_DIRNOTEMPTY,         UV_ENOTEMPTY);
    VMS_ERROR_CASE(SS$_NOSUCHID,            UV_ENOENT);
    VMS_ERROR_CASE(SS$_OVRMAXARG,           UV_EINVAL);
    VMS_ERROR_CASE(SS$_DATALOST,            UV_ENOBUFS);
    VMS_ERROR_CASE(SS$_BADBUFLEN,           UV_EINVAL);
    VMS_ERROR_CASE(SS$_INVUSER,             UV_ENOENT);
    VMS_ERROR_CASE(SS$_NOSYSPRV,            UV_EACCES);

    /* system service exceeded quota errors */
    VMS_ERROR_CASE(SS$_EXASTLM,             UV_EPERM);
    VMS_ERROR_CASE(SS$_EXBIOLM,             UV_EPERM);
    VMS_ERROR_CASE(SS$_EXBYTLM,             UV_EPERM);
    VMS_ERROR_CASE(SS$_EXDIOLM,             UV_EPERM);
    VMS_ERROR_CASE(SS$_EXFILLM,             UV_EPERM);
    VMS_ERROR_CASE(SS$_EXPGFLQUOTA,         UV_EPERM);
    VMS_ERROR_CASE(SS$_EXPRCLM,             UV_EPERM);
    VMS_ERROR_CASE(SS$_EXTQELM,             UV_EPERM);
    VMS_ERROR_CASE(SS$_EXENQLM,             UV_EPERM);
    VMS_ERROR_CASE(SS$_EXBUFOBJLM,          UV_EPERM);

    VMS_ERROR_CASE(LIB$_DELTIMREQ,          UV_EINVAL);
    VMS_ERROR_CASE(LIB$_EF_ALRFRE,          UV_EINVAL);
    VMS_ERROR_CASE(LIB$_EF_RESSYS,          UV_EINVAL);
    VMS_ERROR_CASE(LIB$_INSEF,              UV_ENOMEM);
    VMS_ERROR_CASE(LIB$_INSVIRMEM,          UV_ENOMEM);
    VMS_ERROR_CASE(LIB$_INVSTRDES,          UV_EINVAL);
    VMS_ERROR_CASE(LIB$_KEYNOTFOU,          UV_ENOENT);
    VMS_ERROR_CASE(LIB$_NEGTIM,             UV_EINVAL);
    VMS_ERROR_CASE(LIB$_NOSUCHSYM,          UV_ENOENT);

    VMS_ERROR_CASE(CLI$_BUFOVF,             UV_EINVAL);
    VMS_ERROR_CASE(CLI$_INSFPRM,            UV_EINVAL);
    VMS_ERROR_CASE(CLI$_IVKEYW,             UV_EINVAL);
    VMS_ERROR_CASE(CLI$_MAXPARM,            UV_EINVAL);

    VMS_ERROR_CASE(RMS$_DEV,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_DIR,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_DNF,                UV_ENOENT);
    VMS_ERROR_CASE(RMS$_ELOOP,              UV_ELOOP);
    VMS_ERROR_CASE(RMS$_EOF,                UV_EOF);
    VMS_ERROR_CASE(RMS$_ESL,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_FLK,                UV_EBUSY);
    VMS_ERROR_CASE(RMS$_FNF,                UV_ENOENT);
    VMS_ERROR_CASE(RMS$_FNM,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_IDR,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_IVSF,               UV_EINVAL);
    VMS_ERROR_CASE(RMS$_MKD,                UV_EBUSY);
    VMS_ERROR_CASE(RMS$_PRV,                UV_EACCES);
    VMS_ERROR_CASE(RMS$_RMV,                UV_EIO);
    VMS_ERROR_CASE(RMS$_RSL,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_SUPPORT,            UV_ENOSYS);
    VMS_ERROR_CASE(RMS$_SYN,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_TMO,                UV_ETIMEDOUT);
    VMS_ERROR_CASE(RMS$_TYP,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_UBF,                UV_EFAULT);
    VMS_ERROR_CASE(RMS$_VER,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_WLD,                UV_EINVAL);
    VMS_ERROR_CASE(RMS$_WLK,                UV_EROFS);

    default:                                return UV_UNKNOWN;
  }
}
