#!/usr/bin/env sh
# jargon configuration script (C) 2014- Shingo OKAWA.

TEMP_NAME="./configure-$$"
TEMP_HEADER=${TEMP_NAME}.h

# Default parameters
CC="g++"
CFLAGS=""
LDFLAGS=""
SUFFIX=""
LIB_DIR=""
INCLUDE_DIR=""
USE_LIBGCC="no"
USE_BOOST="no"
WORD_SIZE="2"
MINGW32="no"
ENABLE_ASCII_MODE="no"
ENABLE_STAT64_MODE="no"
USE_FLT_EVAL_METHOD="no"
HAVE_FLOAT_T="no"
HAVE_INT8_T="no"
HAVE_INT16_T="no"
HAVE_INT32_T="no"
HAVE_INT64_T="no"
HAVE_UINT8_T="no"
HAVE_UINT16_T="no"
HAVE_UINT32_T="no"
HAVE_UINT64_T="no"
HAVE_TCHAR="no"
HAVE_WORD="no"
HAVE_DWORD="no"
DEFINE_FLOAT_T="/* undef float_t */"
DEFINE_INT8_T="/* undef int8_t */"
DEFINE_INT16_T="/* undef int16_t */"
DEFINE_INT32_T="/* undef int32_t */"
DEFINE_INT64_T="/* undef int64_t */"
DEFINE_UINT8_T="/* undef uint8_t */"
DEFINE_UINT16_T="/* undef uint16_t */"
DEFINE_UINT32_T="/* undef uint32_t */"
DEFINE_UINT64_T="/* undef uint64_t */"
DEFINE_TCHAR_T="/* undef tchar_t */"
DEFINE_WORD_T="typedef WORD word_t;"
DEFINE_DWORD_T="typedef DWORD dword_t;"
DEFINE_TSTAT_T="/* undef stat_t */"
DEFINE_TSTAT_F="/* undef tstat */"
DEFINE_TFSTAT_F="/* undef tfstat */"
DEFINE_TSEEK_F="/* undef tlseek */"
DEFINE_TUNLINK_F="/* undef tunlink */"
DEFINE_REALPATH_F="#define trealpath(relative, absolute) realpath(relative, absolute)"
HAVE_WINDOWS_H="no"
HAVE_PTHREAD_H="no"
HAVE_STDINT_H="no"
HAVE_STRING_H="no"
HAVE_MEMORY_H="no"
HAVE_UNISTD_H="no"
HAVE_STDIO_H="no"
HAVE_SYS_DIR_H="no"
HAVE_SYS_NDIR_H="no"
HAVE_SYS_TYPES_H="no"
HAVE_ERRNO_H="no"
HAVE_WCHAR_H="no"
HAVE_WCTYPE_H="no"
HAVE_CTYPE_H="no"
HAVE_DLFCN_H="no"
HAVE_FCNTL_H="no"
HAVE_EXT_HASH_MAP_H="no"
HAVE_EXT_HASH_SET_H="no"
HAVE_HASH_MAP_H="no"
HAVE_HASH_SET_H="no"
HAVE_NDIR_H="no"
HAVE_SYS_STAT_H="no"
HAVE_SYS_TIMEB_H="no"
HAVE_SYS_TIME_H="no"
HAVE_SYS_MMAN_H="no"
HAVE_TCHAR_H="no"
HAVE_GLOB_H="no"
HAVE_IO_H="no"
HAVE_STRCASECMP_F="no"
HAVE_STRLWR_F="no"
HAVE_LLTOA_F="no"
HAVE_STRTOLL_F="no"
HAVE_WCSDUP_F="no"
HAVE_WCSCASECMP_F="no"
HAVE_WCSLWR_F="no"
HAVE_LLTOW_F="no"
HAVE_WCSTOLL_F="no"
HAVE_WCSTOD_F="no"
HAVE_SNWPRINTF_F="no"
HAVE_WPRINTF_F="no"
HAVE_VSNWPRINTF_F="no"
HAVE_GETTIMEOFDAY_F="no"
HAVE_USLEEP_F="no"
HAVE_REALPATH_F="no"
BIGENDIAN="no"

# Find source path.
CURRENT_DIR=${0%configure.sh}
CURRENT_DIR=${CURRENT_DIR%/}
CURRENT_DIR_USED="yes"
if test -z "${CURRENT_DIR}" -o "${CURRENT_DIR}" = "." ; then
    CURRENT_DIR=`pwd`
    CURRENT_DIR_USED="no"
fi

# OS specific.
OS=`uname -s`
case ${OS} in
  MINGW32*)  MINGW32="yes";;
  *) ;;
esac

# Detect cpu.
CPU=`uname -m`
case "${CPU}" in
    i386|i486|i586|i686|i86pc|BePC|i686-AT386)
	CPU="x86"
	;;
    x86_64)
	CPU="x86-64"
	;;
    arm*)
	case "${CPU}" in
	    arm|armv4l)
		CPU_VER=4
		;;
	    armv5tel|armv5tejl)
		CPU_VER=5
		;;
	    armv6j|armv6l)
		CPU_VER=6
		;;
	    armv7a|armv7l)
		CPU_VER=7
		;;
	esac
	CPU="armv4l"
	;;
    alpha)
	CPU="alpha"
	;;
    "Power Macintosh"|ppc|ppc64)
	CPU="powerpc"
	;;
    mips)
	CPU="mips"
	;;
    s390)
	CPU="s390"
	;;
    *)
	CPU="unknown"
	;;
esac

# Handle options.
for OPTION do
    eval OPTION=\"${OPTION}\"
    case "${OPTION}" in
	--path=*) CURRENT_DIR=`echo ${OPTION} | cut -d '=' -f 2`
	    ;;
	--cc=*) CC=`echo ${OPTION} | cut -d '=' -f 2`
	    ;;
	--cpu=*) CPU=`echo ${OPTION} | cut -d '=' -f 2`
	    ;;
	--lib-dir=*) LIB_DIR=`echo ${OPTION} | cut -d '=' -f 2`
	    ;;
	--include-dir=*) INCLUDE_DIR=`echo ${OPTION} | cut -d '=' -f 2`
	    ;;
	--extra-cflags=*) CFLAGS="${OPTION#--extra-cflags=}"
	    ;;
	--extra-ldflags=*) LDFLAGS="${OPTION#--extra-ldflags=}"
	    ;;
	--with-boost) USE_BOOST="yes"
	    ;;
	--enable-ascii) ENABLE_ASCII_MODE="yes"
	    ;;
	--enable-stat64) ENABLE_STAT64_MODE="yes"
	    ;;
	--word-size=*) WORD_SIZE=`echo ${OPTION} | cut -d '=' -f 2`
	    ;;
	--help|-h) SHOW_HELP="yes"
	    ;;
	*) echo "configure: WARNING: unrecognized option ${OPTION}"
	    ;;
    esac
done

# Show configuration helps.
if test x"${SHOW_HELP}" = x"yes" ; then
    cat << EOF
Usage: configure.sh [options]
Options: [defaults in brackets after descriptions]

Standard options:
  --help                   print this message
  --lib-dir=DIR            object code libraries in DIR [EPREFIX/lib]
  --include-dir=DIR        C header files in DIR [PREFIX/include]

Advanced options (experts only):
  --path=PATH              path of source code [$source_path]
  --cross-prefix=PREFIX    use PREFIX for compile tools [$cross_prefix]
  --cc=CC                  use C compiler CC [$cc]
  --extra-cflags=          specify compiler flags [$CFLAGS]
  --extra-ldflags=         specify linker options []
  --with-boost             use Boost template library
  --enable-ascii           treat characters as ASCII
  --enable-stat64          enable treating big size files
  --word-size=             specify cpu word size
EOF
    exit 1
fi

# Check CFLAGS
if test -z "${CFLAGS}"; then
    CFLAGS="-Wall -g -O2"
fi

# Set source directory for configuration.
if test x"${SOURCE_DIR}" = x""; then
    SOURCE_DIR="${CURRENT_DIR}/config"
fi

# Set resulting output directory.
if test x"${CONFIG_DIR}" = x""; then
    CONFIG_DIR="${CURRENT_DIR}/src"
fi

# Test code utilities.
TEST=${TEMP_NAME}${SUFFIX}

function compile() {
    if ! test -n ${1}; then
	return 1
    fi
    SOURCE=${1}
    ARGS=${@:2}

    DEFINES=""
    for ARG in ${ARGS}; do
	if ! test x"${ARG}" = x"" ; then
	    DEFINES="${DEFINES} -D${ARG}"
	fi
    done

    OPTIONS="${CFLAGS} ${LDFLAGS} ${DEFINES}"
    if ! ${CC} ${OPTIONS} -o ${TEST} ${SOURCE_DIR}/${SOURCE} 2>/dev/null; then
	return 1
    else
	return 0
    fi
}

function clean() {
    rm -rf ${TEST}*
}

function check() {
    echo "$(${TEST} ${@})"
}

function check_type_size() {
    if ! test -n ${4}; then
	return 1
    fi

    NAME=${1}
    UPPER_NAME=`echo ${NAME} | tr 'a-z' 'A-Z'`
    SIZE=${2}
    SIGN=${3}
    OPTIONS=${@:4}

    if test x"${HAVE_WINDOWS_H}" = x"yes" ; then
	D_WINDOWS_H="HAVE_WINDOWS_H"
    else
	D_WINDOWS_H=""
    fi

    if test x"${HAVE_TCHAR_H}" = x"yes" ; then
	D_TCHAR_H="HAVE_TCHAR_H"
    else
	D_TCHAR_H=""
    fi

    for OPTION in ${OPTIONS}; do
	if ! compile "size.cc" "CHECKING_TYPE=${OPTION}" "${D_WINDOWS_H}" "${D_TCHAR_H}" ; then
	    continue
	else
	    CALCULATED_SIZE=`check`
	    if [ -n "${SIZE}" -a x"${CALCULATED_SIZE}" == x"${SIZE}" ] ; then
		eval "DEFINE_${UPPER_NAME}=\"typedef ${SIGN} ${OPTION} ${NAME};\""
		return 0
	    elif [ -z "${SIZE}" ] ; then
		eval "DEFINE_${UPPER_NAME}=\"typedef ${SIGN} ${OPTION} ${NAME};\""
		return 0
	    fi
	fi
    done

    return 1
}

function check_stat_type() {
    if ! test -n ${1}; then
	return 1
    fi

    NAME=${1}
    OPTIONS=${@:2}

    if test x"${HAVE_WINDOWS_H}" = x"yes" ; then
	D_WINDOWS_H="HAVE_WINDOWS_H"
    else
	D_WINDOWS_H=""
    fi

    if test x"${HAVE_SYS_STAT_H}" = x"yes" ; then
	D_SYS_STAT_H="HAVE_SYS_STAT_H"
    else
	D_SYS_STAT_H=""
    fi

    for OPTION in ${OPTIONS}; do
	if ! compile "stat.cc" "CHECKING_STAT_TYPE=${OPTION}" "CHECKING_FSTAT_FUNC=${OPTION/stat/fstat}" "CHECKING_LSEEK_FUNC=${OPTION/stat/lseek}" "${D_WINDOWS_H}" "${D_SYS_STAT_H}" ; then
	    continue
	else
	    eval "DEFINE_TSTAT_T=\"typedef struct ${OPTION} ${NAME};\""
	    eval "DEFINE_TSTAT_F=\"#define tstat ${OPTION/stat/stat}\""
	    eval "DEFINE_TFSTAT_F=\"#define tfstat ${OPTION/stat/fstat}\""
	    eval "DEFINE_TSEEK_F=\"#define tlseek ${OPTION/stat/lseek}\""
	    return 0
	fi
    done

    return 1
}

function check_unlink_func() {
    if ! test -n ${1}; then
	return 1
    fi

    NAME=${1}
    OPTIONS=${@:2}

    if test x"${HAVE_WINDOWS_H}" = x"yes" ; then
	D_WINDOWS_H="HAVE_WINDOWS_H"
    else
	D_WINDOWS_H=""
    fi

    for OPTION in ${OPTIONS}; do
	if ! compile "unlink.cc" "CHECKING_UNLINK_FUNC=${OPTION}" "${D_WINDOWS_H}" "${D_SYS_STAT_H}" ; then
	    continue
	else
	    eval "DEFINE_TUNLINK_F=\"#define tunlink ${OPTION}\""
	    return 0
	fi
    done

    return 1
}

# Execute test codes.
if compile "endian.cc" ; then
    BIGENDIAN=`check "bigendian"`
fi

if compile "version.cc" ; then
    GCC_MAJOR=`check "version"`
    GCC_MINOR=`check "minor"`
fi

if compile "header.cc" "CHECK_IF_WINDOWS_H_EXIST" ; then
    HAVE_WINDOWS_H="yes"
fi

if compile "header.cc" "CHECK_IF_PTHREAD_H_EXIST" ; then
    HAVE_PTHREAD_H="yes"
fi

if compile "header.cc" "CHECK_IF_STDINT_H_EXIST" ; then
    HAVE_STDINT_H="yes"
fi

if compile "header.cc" "CHECK_IF_STRING_H_EXIST" ; then
    HAVE_STRING_H="yes"
fi

if compile "header.cc" "CHECK_IF_MEMORY_H_EXIST" ; then
    HAVE_MEMORY_H="yes"
fi

if compile "header.cc" "CHECK_IF_UNISTD_H_EXIST" ; then
    HAVE_UNISTD_H="yes"
fi

if compile "header.cc" "CHECK_IF_STDIO_H_EXIST" ; then
    HAVE_STDIO_H="yes"
fi

if compile "header.cc" "CHECK_IF_SYS_DIR_H_EXIST" ; then
    HAVE_SYS_DIR_H="yes"
fi

if compile "header.cc" "CHECK_IF_SYS_NDIR_H_EXIST" ; then
    HAVE_SYS_NDIR_H="yes"
fi

if compile "header.cc" "CHECK_IF_SYS_TYPES_H_EXIST" ; then
    HAVE_SYS_TYPES_H="yes"
fi

if compile "header.cc" "CHECK_IF_ERRNO_H_EXIST" ; then
    HAVE_ERRNO_H="yes"
fi

if compile "header.cc" "CHECK_IF_WCHAR_H_EXIST" ; then
    HAVE_WCHAR_H="yes"
fi

if compile "header.cc" "CHECK_IF_WCTYPE_H_EXIST" ; then
    HAVE_WCTYPE_H="yes"
fi

if compile "header.cc" "CHECK_IF_CTYPE_H_EXIST" ; then
    HAVE_CTYPE_H="yes"
fi

if compile "header.cc" "CHECK_IF_DLFCN_H_EXIST" ; then
    HAVE_DLFCN_H="yes"
fi

if compile "header.cc" "CHECK_IF_FCNTL_H_EXIST" ; then
    HAVE_FCNTL_H="yes"
fi

if compile "header.cc" "CHECK_IF_EXT_HASH_MAP_H_EXIST" ; then
    HAVE_EXT_HASH_MAP_H="yes"
fi

if compile "header.cc" "CHECK_IF_EXT_HASH_SET_H_EXIST" ; then
    HAVE_EXT_HASH_SET_H="yes"
fi

if compile "header.cc" "CHECK_IF_HASH_MAP_H_EXIST" ; then
    HAVE_HASH_MAP_H="yes"
fi

if compile "header.cc" "CHECK_IF_HASH_SET_H_EXIST" ; then
    HAVE_HASH_SET_H="yes"
fi

if compile "header.cc" "CHECK_IF_NDIR_H_EXIST" ; then
    HAVE_NDIR_H="yes"
fi

if compile "header.cc" "CHECK_IF_SYS_STAT_H_EXIST" ; then
    HAVE_SYS_STAT_H="yes"
fi

if compile "header.cc" "CHECK_IF_SYS_TIMEB_H_EXIST" ; then
    HAVE_SYS_TIMEB_H="yes"
fi

if compile "header.cc" "CHECK_IF_SYS_TIME_H_EXIST" ; then
    HAVE_SYS_TIME_H="yes"
fi

if compile "header.cc" "CHECK_IF_SYS_MMAN_H_EXIST" ; then
    HAVE_SYS_MMAN_H="yes"
fi

if compile "header.cc" "CHECK_IF_TCHAR_H_EXIST" ; then
    HAVE_TCHAR_H="yes"
fi

if compile "header.cc" "CHECK_IF_GLOB_H_EXIST" ; then
    HAVE_GLOB_H="yes"
fi

if compile "header.cc" "CHECK_IF_IO_H_EXIST" ; then
    HAVE_IO_H="yes"
fi

if compile "float.cc" ; then
    HAVE_FLOAT_T="yes"
    USE_FLT_EVAL_METHOD="no"
fi

if ! test x"${HAVE_FLOAT_T}" = x"yes" ; then
    if ! compile "float.cc" "ENABLE_FLT_EVAL_METHOD" ; then
	HAVE_FLOAT_T="no"
    else
	HAVE_FLOAT_T="yes"
	USE_FLT_EVAL_METHOD="yes"
    fi
fi

if test x"${HAVE_FLOAT_T}" = x"no" ; then
    DEFINE_FLOAT_T="typedef double float_t;"
    echo "configure: notice: float_t is not defined."
    echo "configure: notice: float_t is now defined as follows - '${DEFINE_FLOAT_TYPE}'."
fi

if compile "integer.cc" "CHECK_IF_INT8_T_EXIST" ; then
    HAVE_INT8_T="yes"
fi

if compile "integer.cc" "CHECK_IF_INT16_T_EXIST" ; then
    HAVE_INT16_T="yes"
fi

if compile "integer.cc" "CHECK_IF_INT32_T_EXIST" ; then
    HAVE_INT32_T="yes"
fi

if compile "integer.cc" "CHECK_IF_INT64_T_EXIST" ; then
    HAVE_INT64_T="yes"
fi

if compile "integer.cc" "CHECK_IF_UINT8_T_EXIST" ; then
    HAVE_UINT8_T="yes"
fi

if compile "integer.cc" "CHECK_IF_UINT16_T_EXIST" ; then
    HAVE_UINT16_T="yes"
fi

if compile "integer.cc" "CHECK_IF_UINT32_T_EXIST" ; then
    HAVE_UINT32_T="yes"
fi

if compile "integer.cc" "CHECK_IF_UINT64_T_EXIST" ; then
    HAVE_UINT64_T="yes"
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "integer.cc" "CHECK_IF_WORD_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_WORD="yes"
    fi
else
    if compile "integer.cc" "CHECK_IF_WORD_EXIST" ; then
	HAVE_WORD="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "integer.cc" "CHECK_IF_DWORD_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_DWORD="yes"
    fi
else
    if compile "integer.cc" "CHECK_IF_DWORD_EXIST" ; then
	HAVE_DWORD="yes"
    fi
fi

if test x"${HAVE_TCHAR_H}" = x"yes" ; then
    if compile "integer.cc" "CHECK_IF_DWORD_EXIST" "HAVE_TCHAR_H" ; then
	HAVE_TCHAR="yes"
    fi
else
    if compile "integer.cc" "CHECK_IF_DWORD_EXIST" ; then
	HAVE_TCHAR="yes"
    fi
fi

if test x"${HAVE_INT8_T}" = x"no" ; then
    if ! check_type_size "int8_t" "1" "signed" "int8_t" "char" ; then
	echo "configure: error: type 'int8_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: int8_t is not defined."
	echo "configure: notice: int8_t is now defined as follows - '${DEFINE_INT8_T}',"
    fi
fi

if test x"${HAVE_INT16_T}" = x"no" ; then
    if ! check_type_size "int16_t" "2" "signed" "int16_t" "short" ; then
	echo "configure: error: type 'int16_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: int16_t is not defined."
	echo "configure: notice: int16_t is now defined as follows - '${DEFINE_INT16_T}'."
    fi
fi

if test x"${HAVE_INT32_T}" = x"no" ; then
    if ! check_type_size "int32_t" "4" "signed" "int32_t" "int" "long" ; then
	echo "configure: error: type 'int32_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: int32_t is not defined."
	echo "configure: notice: int32_t is now defined as follows - '${DEFINE_INT32_T}'."
    fi
fi

if test x"${HAVE_INT64_T}" = x"no" ; then
    if ! check_type_size "int64_t" "8" "signed" "int64_t" "long long" "__int64" ; then
	echo "configure: error: type 'int64_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: int64_t is not defined."
	echo "configure: notice: int64_t is now defined as follows - '${DEFINE_INT64_T}'."
    fi
fi

if test x"${HAVE_UINT8_T}" = x"no" ; then
    if ! check_type_size "uint8_t" "1" "unsigned" "uint8_t" "char" ; then
	echo "configure: error: type 'uint8_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: uint8_t is not defined."
	echo "configure: notice: uint8_t is now defined as follows - '${DEFINE_UINT8_T}'."
    fi
fi

if test x"${HAVE_UINT16_T}" = x"no" ; then
    if ! check_type_size "uint16_t" "2" "unsigned" "uint16_t" "short" ; then
	echo "configure: error: type 'uint16_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: uint16_t is not defined."
	echo "configure: notice: uint16_t is now defined as follows - '${DEFINE_UINT16_T}'."
    fi
fi

if test x"${HAVE_UINT32_T}" = x"no" ; then
    if ! check_type_size "uint32_t" "4" "unsigned" "uint32_t" "int" "long" ; then
	echo "configure: error: type 'uint32_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: uint32_t is not defined."
	echo "configure: notice: uint32_t is now defined as follows - '${DEFINE_UINT32_T}'."
    fi
fi

if test x"${HAVE_UINT64_T}" = x"no" ; then
    if ! check_type_size "uint64_t" "8" "unsigned" "uint64_t" "long long" "__int64" ; then
	echo "configure: error: type 'uint64_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: uint64_t is not defined."
	echo "configure: notice: uint64_t is now defined as follows - '${DEFINE_UINT64_T}'."
    fi
fi

if test x"${HAVE_WORD}" = x"no" ; then
    if ! check_type_size "word_t" "${WORD_SIZE}" "unsigned" "short" "int" ; then
	echo "configure: error: type 'word_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: word_t is not defined."
	echo "configure: notice: word_t is now defined as follows - '${DEFINE_WORD_T}'."
    fi
fi

if test x"${HAVE_DWORD}" = x"no" ; then
    DWORD_SIZE=`expr ${WORD_SIZE} \* 2`
    if ! check_type_size "dword_t" "${DWORD_SIZE}" "unsigned" "int" "long" ; then
	echo "configure: error: type 'dword_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: dword_t is not defined."
	echo "configure: notice: dword_t is now defined as follows - '${DEFINE_DWORD_T}'."
    fi
fi

if test x"${HAVE_TCHAR}" = x"no" ; then
    if test x"${ENABLE_ASCII_MODE}" = x"yes" ; then
	if ! check_type_size "tchar_t" "1" "" "TCHAR" "char" ; then
	    echo "configure: error: type 'tchar_t' not found."
	    clean
	    exit 1
	else
	    echo "configure: notice: tchar_t is not defined."
	    echo "configure: notice: tchar_t is now defined as follows - '${DEFINE_TCHAR_T}'."
	fi
    else
	if ! check_type_size "tchar_t" "" "" "wchar_t" "unsigned short" ; then
	    echo "configure: error: type 'tchar_t' not found."
	    clean
	    exit 1
	else
	    echo "configure: notice: tchar_t is not defined."
	    echo "configure: notice: tchar_t is now defined as follows - '${DEFINE_TCHAR_T}'."
	fi
    fi
fi

if test x"${ENABLE_STAT64_MODE}" = x"yes" ; then
    if ! check_stat_type "tstat_t" "stati64" "_stati64" "stat64"; then
	echo "configure: error: type 'tstat_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: tstat_t is not defined."
	echo "configure: notice: tstat_t is now defined as follows - '${DEFINE_TSTAT_T}'."
	echo "configure: notice: tstat is now defined as follows - '${DEFINE_TSTAT_F}'."
	echo "configure: notice: tfstat is now defined as follows - '${DEFINE_TFSTAT_F}'."
	echo "configure: notice: tlseek is now defined as follows - '${DEFINE_TSEEK_F}'."
    fi
else
    if ! check_stat_type "tstat_t" "stat" "_stat"; then
	echo "configure: error: type 'tstat_t' not found."
	clean
	exit 1
    else
	echo "configure: notice: tstat_t is not defined."
	echo "configure: notice: tstat_t is now defined as follows - '${DEFINE_TSTAT_T}'."
	echo "configure: notice: tstat is now defined as follows - '${DEFINE_TSTAT_F}'."
	echo "configure: notice: tfstat is now defined as follows - '${DEFINE_TFSTAT_F}'."
	echo "configure: notice: tlseek is now defined as follows - '${DEFINE_TSEEK_F}'."
    fi
fi

if ! check_unlink_func "tunlink" "_unlink" "unlink" ; then
    echo "configure: error: type 'tunlink' not found."
    clean
    exit 1
else
    echo "configure: notice: tunlink is not defined."
    echo "configure: notice: tunlink is now defined as follows - '${DEFINE_TUNLINK_F}'."
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "string.cc" "CHECK_IF_STRCASECMP_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_STRCASECMP_F="yes"
    fi
else
    if compile "string.cc" "CHECK_IF_STRCASECMP_F_EXIST" ; then
	HAVE_STRCASECMP_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "string.cc" "CHECK_IF_STRLWR_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_STRLWR_F="yes"
    fi
else
    if compile "string.cc" "CHECK_IF_STRLWR_F_EXIST" ; then
	HAVE_STRLWR_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "string.cc" "CHECK_IF_LLTOA_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_LLTOA_F="yes"
    fi
else
    if compile "string.cc" "CHECK_IF_LLTOA_F_EXIST" ; then
	HAVE_LLTOA_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "string.cc" "CHECK_IF_STRTOLL_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_LLTOA_F="yes"
    fi
else
    if compile "string.cc" "CHECK_IF_STRTOLL_F_EXIST" ; then
	HAVE_LLTOA_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "wchar.cc" "CHECK_IF_WCSCASECMP_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_WCSCASECMP_F="yes"
    fi
else
    if compile "wchar.cc" "CHECK_IF_WCSCASECMP_F_EXIST" ; then
	HAVE_WCSCASECMP_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "wchar.cc" "CHECK_IF_WCSLWR_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_WCSLWR_F="yes"
    fi
else
    if compile "wchar.cc" "CHECK_IF_WCSLWR_F_EXIST" ; then
	HAVE_WCSLWR_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "wchar.cc" "CHECK_IF_LLTOW_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_LLTOW_F="yes"
    fi
else
    if compile "wchar.cc" "CHECK_IF_LLTOW_F_EXIST" ; then
	HAVE_LLTOW_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "wchar.cc" "CHECK_IF_WCSTOLL_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_WCSTOLL_F="yes"
    fi
else
    if compile "wchar.cc" "CHECK_IF_WCSTOLL_F_EXIST" ; then
	HAVE_WCSTOLL_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "wchar.cc" "CHECK_IF_WCSTOD_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_WCSTOD_F="yes"
    fi
else
    if compile "wchar.cc" "CHECK_IF_WCSTOD_F_EXIST" ; then
	HAVE_WCSTOD_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "wchar.cc" "CHECK_IF_WCSDUP_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_WCSDUP_F="yes"
    fi
else
    if compile "wchar.cc" "CHECK_IF_WCSDUP_F_EXIST" ; then
	HAVE_WCSDUP_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "printf.cc" "CHECK_IF_SNWPRINTF_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_SNWPRINTF_F="yes"
    fi
else
    if compile "printf.cc" "CHECK_IF_SNWPRINTF_F_EXIST" ; then
	HAVE_SNWPRINTF_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "printf.cc" "CHECK_IF_WPRINTF_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_WPRINTF_F="yes"
    fi
else
    if compile "printf.cc" "CHECK_IF_WPRINTF_F_EXIST" ; then
	HAVE_WPRINTF_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "printf.cc" "CHECK_IF_VSNWPRINTF_F_EXIST" "HAVE_WINDOWS_H" ; then
	HAVE_VSNWPRINTF_F="yes"
    fi
else
    if compile "printf.cc" "CHECK_IF_VSNWPRINTF_F_EXIST" ; then
	HAVE_VSNWPRINTF_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "time.cc" "HAVE_WINDOWS_H" ; then
	HAVE_GETTIMEOFDAY_F="yes"
    fi
else
    if compile "time.cc" ; then
	HAVE_GETTIMEOFDAY_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "sleep.cc" "HAVE_WINDOWS_H" ; then
	HAVE_USLEEP_F="yes"
    fi
else
    if compile "sleep.cc" ; then
	HAVE_USLEEP_F="yes"
    fi
fi

if test x"${HAVE_WINDOW_H}" = x"yes" ; then
    if compile "realpath.cc" "HAVE_WINDOWS_H" ; then
	HAVE_REALPATH_F="yes"
    fi
else
    if compile "realpath.cc" ; then
	HAVE_REALPATH_F="yes"
    fi
fi

if test x"${HAVE_REALPATH_F}" = x"no" ; then
    DEFINE_REALPATH_F="#define trealpath(relative, absolute) ::_fullpath(absolute, relative, JARGON_MAX_PATH)"
fi

# Clean up resulting test codes.
clean

# Display configuration result.
cat << EOF

Basic configuration result:
Library directory      ${LIB_DIR}
Include directory      ${INCLUDE_DIR}
C++ compiler           ${CC}
CPU                    ${CPU}
Enable FLT_EVAL_METHOD ${USE_FLT_EVAL_METHOD}

Type configuration result:
Have float_t           ${HAVE_FLOAT_T}
Have int8_t            ${HAVE_INT8_T}
Have int16_t           ${HAVE_INT16_T}
Have int32_t           ${HAVE_INT32_T}
Have int64_t           ${HAVE_INT64_T}
Have uint8_t           ${HAVE_UINT8_T}
Have uint16_t          ${HAVE_UINT16_T}
Have uint32_t          ${HAVE_UINT32_T}
Have uint64_t          ${HAVE_UINT64_T}
Have WORD              ${HAVE_WORD}
Have DWORD             ${HAVE_DWORD}
Have TCHAR             ${HAVE_TCHAR}

Header file configuration result:
Have <windows.h>       ${HAVE_WINDOWS_H}
Have <pthread.h>       ${HAVE_PTHREAD_H}
Have <cstdint>         ${HAVE_STDINT_H}
Have <string>          ${HAVE_STRING_H}
Have <memory>          ${HAVE_MEMORY_H}
Have <unistd.h>        ${HAVE_UNISTD_H}
Have <cstdio>          ${HAVE_STDIO_H}
Have <sys/dir.h>       ${HAVE_SYS_DIR_H}
Have <sys/ndir.h>      ${HAVE_SYS_NDIR_H}
Have <sys/type.h>      ${HAVE_SYS_TYPES_H}
Have <cerrno>          ${HAVE_ERRNO_H}
Have <cwchar>          ${HAVE_WCHAR_H}
Have <cwctype>         ${HAVE_WCTYPE_H}
Have <cctype>          ${HAVE_CTYPE_H}
Have <dlfcn.h>         ${HAVE_DLFCN_H}
Have <fcntl.h>         ${HAVE_FCNTL_H}
Have <ext/hash_map>    ${HAVE_EXT_HASH_MAP_H}
Have <ext/hash_set>    ${HAVE_EXT_HASH_SET_H}
Have <hash_map>        ${HAVE_HASH_MAP_H}
Have <hash_set>        ${HAVE_HASH_SET_H}
Have <ndir.h>          ${HAVE_NDIR_H}
Have <sys/stat.h>      ${HAVE_SYS_STAT_H}
Have <sys/timeb.h>     ${HAVE_SYS_TIMEB_H}
Have <sys/time.h>      ${HAVE_SYS_TIME_H}
Have <sys/mman.h>      ${HAVE_SYS_MMAN_H}
Have <tchar.h>         ${HAVE_TCHAR_H}
Have <glob.h>          ${HAVE_GLOB_H}
Have <io.h>            ${HAVE_IO_H}

Function configuration result:
Have strcasecmp        ${HAVE_STRCASECMP}
Have strlwr            ${HAVE_STRLWR_F}
Have lltoa             ${HAVE_LLTOA_F}
Have strtoll           ${HAVE_STRTOLL_F}
Have wcsdup            ${HAVE_WCSDUP_F}
Have wcscasecmp        ${HAVE_WCSCASECMP_F}
Have wcslwr            ${HAVE_WCSLWR_F}
Have lltow             ${HAVE_LLTOW_F}
Have wcstoll           ${HAVE_WCSTOLL_F}
Have wcstod            ${HAVE_WCSTOD_F}
Have snwprintf         ${HAVE_SNWPRINTF_F}
Have wprintf           ${HAVE_WPRINTF_F}
Have vsnwprintf        ${HAVE_VSNWPRINTF_F}
Have gettimeofday      ${HAVE_GETTIMEOFDAY_F}
Have usleep            ${HAVE_USLEEP_F}
Have realpath          ${HAVE_REALPATH_F}

Other settings:
Big Endian             ${BIGENDIAN}
ASCII mode             ${ENABLE_ASCII_MODE}
STAT64 mode            ${ENABLE_STAT64_MODE}
Use libgcc             ${USE_LIBGCC}
Use Boost              ${USE_BOOST}
EOF

# Generate platform-dependent header file.
echo ""
echo "Creating configure.h"
echo "/* Automatically generated by configure - do not modify */" > ${TEMP_HEADER}

VERSION=`head ${CURRENT_DIR}/VERSION`
echo "#define GCC_MAJOR ${GCC_MAJOR}"    >> ${TEMP_HEADER}
echo "#define GCC_MINOR ${GCC_MINOR}"    >> ${TEMP_HEADER}
echo "#define JARGON_VERSION ${VERSION}" >> ${TEMP_HEADER}

if test x"${CPU}" = x"x86" ; then
    echo "#define HOST_I386 1"   >> ${TEMP_HEADER}
elif test x"${CPU}" = x"x86-64" ; then
    echo "#define HOST_X86_64 1" >> ${TEMP_HEADER}
elif test x"${CPU}" = x"armv4l" ; then
    echo "#define HOST_ARM 1"    >> ${TEMP_HEADER}
elif test x"${CPU}" = x"powerpc" ; then
    echo "#define HOST_PPC 1"    >> ${TEMP_HEADER}
elif test x"${CPU}" = x"mips" ; then
    echo "#define HOST_MIPS 1"   >> ${TEMP_HEADER}
elif test x"${CPU}" = x"s390" ; then
    echo "#define HOST_S390 1"   >> ${TEMP_HEADER}
elif test x"${CPU}" = x"alpha" ; then
    echo "#define HOST_ALPHA 1"  >> ${TEMP_HEADER}
else
    echo "Unsupported CPU"
    exit 1
fi

if test x"${CYGWIN}" = x"yes" ; then
    echo "#ifndef _WIN32"   >> ${TEMP_HEADER}
    echo "#  define _WIN32" >> ${TEMP_HEADER}
    echo "#endif"           >> ${TEMP_HEADER}
fi

if test x"${BIGENDIAN}" = x"yes" ; then
    echo "#define WORDS_BIGENDIAN 1"             >> ${TEMP_HEADER}
fi

if test x"${ENABLE_ASCII_MODE}" = x"yes" ; then
    echo "#define JARGON_ASCII_MODE 1"           >> ${TEMP_HEADER}
fi

if test x"${HAVE_WINDOWS_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_WINDOWS_H 1"       >> ${TEMP_HEADER}
fi

if test x"${HAVE_PTHREAD_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_PTHREAD_H 1"       >> ${TEMP_HEADER}
fi

if test x"${HAVE_STDINT_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_STDINT_H 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_String_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_STRING_H 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_MEMORY_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_MEMORY_H 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_UNISTD_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_UNISTD_H 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_STDIO_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_STDIO_H 1"         >> ${TEMP_HEADER}
fi

if test x"${HAVE_SYS_DIR_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_SYS_DIR_H 1"       >> ${TEMP_HEADER}
fi

if test x"${HAVE_SYS_NDIR_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_SYS_NDIR_H 1"      >> ${TEMP_HEADER}
fi

if test x"${HAVE_SYS_TYPES_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_SYS_TYPES_H 1"     >> ${TEMP_HEADER}
fi

if test x"${HAVE_ERRNO_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_ERRNO_H 1"         >> ${TEMP_HEADER}
fi

if test x"${HAVE_WCHAR_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_WCHAR_H 1"         >> ${TEMP_HEADER}
fi

if test x"${HAVE_WCTYPE_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_WCTYPE_H 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_CTYPE_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_CTYPE_H 1"         >> ${TEMP_HEADER}
fi

if test x"${HAVE_DLFCN_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_DLFCN_H 1"         >> ${TEMP_HEADER}
fi

if test x"${HAVE_FCNTL_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_FCNTL_H 1"         >> ${TEMP_HEADER}
fi

if test x"${HAVE_EXT_HASH_MAP_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_EXT_HASH_MAP_H 1"  >> ${TEMP_HEADER}
fi

if test x"${HAVE_EXT_HASH_SET_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_EXT_HASH_SET_H 1"  >> ${TEMP_HEADER}
fi

if test x"${HAVE_HASH_MAP_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_HASH_MAP_H 1"      >> ${TEMP_HEADER}
fi

if test x"${HAVE_HASH_SET_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_HASH_SET_H 1"      >> ${TEMP_HEADER}
fi

if test x"${HAVE_NDIR_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_NDIR_H 1"          >> ${TEMP_HEADER}
fi

if test x"${HAVE_SYS_STAT_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_SYS_STAT_H 1"      >> ${TEMP_HEADER}
fi

if test x"${HAVE_SYS_TIMEB_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_SYS_TIMEB_H 1"     >> ${TEMP_HEADER}
fi

if test x"${HAVE_SYS_TIME_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_SYS_TIME_H 1"      >> ${TEMP_HEADER}
fi

if test x"${HAVE_SYS_MMAN_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_SYS_MMAN_H 1"      >> ${TEMP_HEADER}
fi

if test x"${HAVE_TCHAR_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_TCHAR_H 1"         >> ${TEMP_HEADER}
fi

if test x"${HAVE_GLOB_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_GLOB_H 1"          >> ${TEMP_HEADER}
fi

if test x"${HAVE_IO_H}" = x"yes" ; then
    echo "#define JARGON_HAVE_IO_H 1"            >> ${TEMP_HEADER}
fi

if test x"${HAVE_STRCASECMP_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_STRCASECMP_F 1"    >> ${TEMP_HEADER}
fi

if test x"${HAVE_STRLWR_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_STRLWR_F 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_LLTOA_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_LLTOA_F 1"         >> ${TEMP_HEADER}
fi

if test x"${HAVE_STRTOLL_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_STRTOLL_F 1"       >> ${TEMP_HEADER}
fi

if test x"${HAVE_WCSDUP_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_WCSDUP_F 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_WCSCASECMP_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_WCSCASECMP_F 1"    >> ${TEMP_HEADER}
fi

if test x"${HAVE_WCSLWR_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_WCSLWR_F 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_LLTOW_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_LLTOW_F 1"         >> ${TEMP_HEADER}
fi

if test x"${HAVE_WCSTOLL_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_WCSTOLL_F 1"       >> ${TEMP_HEADER}
fi

if test x"${HAVE_WCSTOD_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_WCSTOD_F 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_SNWPRINTF_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_SNWPRINTF_F 1"     >> ${TEMP_HEADER}
fi

if test x"${HAVE_WPRINTF_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_WPRINTF_F 1"       >> ${TEMP_HEADER}
fi

if test x"${HAVE_VSNWPRINTF_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_VSNWPRINTF_F 1"    >> ${TEMP_HEADER}
fi

if test x"${HAVE_GETTIMEOFDAY_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_GETTIMEOFDAY_F 1"  >> ${TEMP_HEADER}
fi

if test x"${HAVE_USLEEP_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_USLEEP_F 1"        >> ${TEMP_HEADER}
fi

if test x"${HAVE_REALPATH_F}" = x"yes" ; then
    echo "#define JARGON_HAVE_REALPATH_F 1"      >> ${TEMP_HEADER}
fi

echo "/* Platform includes that MUST be included for the public headers to work. */" >> ${TEMP_HEADER}
echo "#include <fcntl.h>"           >> ${TEMP_HEADER}
echo "#include <unistd.h>"          >> ${TEMP_HEADER}
echo "#include <sys/types.h>"       >> ${TEMP_HEADER}
echo "#include <cmath>"             >> ${TEMP_HEADER}
echo "#include <cstdlib>"           >> ${TEMP_HEADER}
echo "#include <cstddef>"           >> ${TEMP_HEADER}
echo "#include <cstdint>"           >> ${TEMP_HEADER}
echo "#include <string>"            >> ${TEMP_HEADER}
echo "#include <cwchar>"            >> ${TEMP_HEADER}
echo "#ifdef JARGON_HAVE_WINDOWS_H" >> ${TEMP_HEADER}
echo "#  include <windows.h>"       >> ${TEMP_HEADER}
echo "#endif"                       >> ${TEMP_HEADER}
echo "#ifdef JARGON_HAVE_TCHAR_H"   >> ${TEMP_HEADER}
echo "#  include <tchar.h>"         >> ${TEMP_HEADER}
echo "#endif"                       >> ${TEMP_HEADER}

if test x"${USE_EVAL_METHOD}" = x"yes" ; then
    echo "#define FLT_EVAL_METHOD 2"  >> ${TEMP_HEADER}
fi
echo "${DEFINE_FLOAT_T}"    >> ${TEMP_HEADER}
echo "${DEFINE_INT8_T}"     >> ${TEMP_HEADER}
echo "${DEFINE_INT16_T}"    >> ${TEMP_HEADER}
echo "${DEFINE_INT32_T}"    >> ${TEMP_HEADER}
echo "${DEFINE_INT64_T}"    >> ${TEMP_HEADER}
echo "${DEFINE_UINT8_T}"    >> ${TEMP_HEADER}
echo "${DEFINE_UINT16_T}"   >> ${TEMP_HEADER}
echo "${DEFINE_UINT32_T}"   >> ${TEMP_HEADER}
echo "${DEFINE_UINT64_T}"   >> ${TEMP_HEADER}
echo "${DEFINE_WORD_T}"     >> ${TEMP_HEADER}
echo "${DEFINE_DWORD_T}"    >> ${TEMP_HEADER}
echo "${DEFINE_TCHAR_T}"    >> ${TEMP_HEADER}
echo "${DEFINE_TSTAT_T}"    >> ${TEMP_HEADER}
echo "${DEFINE_TSTAT_F}"    >> ${TEMP_HEADER}
echo "${DEFINE_TFSTAT_F}"   >> ${TEMP_HEADER}
echo "${DEFINE_TSEEK_F}"    >> ${TEMP_HEADER}
echo "${DEFINE_TUNLINK_F}"  >> ${TEMP_HEADER}
echo "${DEFINE_REALPATH_F}" >> ${TEMP_HEADER}

# MEMO:
# The following header files must exist for compiling.
#
# vector
# map
# iostream
# string.h
# cwchar
# cstdlib
# cctypes
# limits.h
# glob.h
# time.h | timeb.h
# cassert
# sys/time.h
# sys/stat.h
# unistd.h
# io.h sys/uio.h
#

# If difference between the current header file and the previous one, if possible,
# remove the previous one and update.
diff ${TEMP_HEADER} ${CONFIG_DIR}/configure.h >/dev/null 2>&1
if test $? -ne 0 ; then
    mv -f ${TEMP_HEADER} ${CONFIG_DIR}/configure.h
else
    rm ${TEMP_HEADER}
    echo "${CONFIG_DIR}/configure.h is unchanged"
fi
