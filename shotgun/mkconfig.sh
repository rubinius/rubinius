#!/bin/bash

cp vars.mk config.mk

(

. vars.mk

echo "HOST=$(./config.guess)"
HOST=$(./config.guess)
if test "$BUILDREV" == "svn"; then
  BUILDREV=$(svn info | grep Revision  | cut -f2 -d" ")
fi

if echo "$HOST" | grep -q darwin; then
  DARWIN=1
  DISABLE_KQUEUE=1
else
  DARWIN=0
  DISABLE_KQUEUE=0
fi

echo "CC=$CC"

if which glibtool > /dev/null; then
  echo "LIBTOOL=glibtool"
else
  echo "LIBTOOL=libtool"
fi
echo "DARWIN=$DARWIN"
echo "DISABLE_KQUEUE=$DISABLE_KQUEUE"
echo "BINPATH=$PREFIX/bin"
echo "LIBPATH=$PREFIX/lib"
echo "CODEPATH=$PREFIX/lib/rubinius/$LIBVER"
echo "RBAPATH=$PREFIX/lib/rubinius/$LIBVER/runtime"
echo "EXTPATH=$PREFIX/lib/rubinius/$LIBVER/$HOST"
echo "BUILDREV=r$BUILDREV"
) >> config.mk

(

. config.mk

echo "#define CONFIG_DARWIN \"$DARWIN\""
echo "#define CONFIG_DISABLE_KQUEUE $DISABLE_KQUEUE"
echo "#define CONFIG_HOST \"$HOST\""
echo "#define CONFIG_PREFIX \"$PREFIX\""
echo "#define CONFIG_VERSION \"$VERSION\""
echo "#define CONFIG_RELDATE \"$(date +%m/%d/%Y)\""
echo "#define CONFIG_CODEPATH \"$CODEPATH\""
echo "#define CONFIG_RBAPATH \"$RBAPATH\""
echo "#define CONFIG_EXTPATH \"$EXTPATH\""
echo "#define CONFIG_BUILDREV \"$BUILDREV\""
echo "#define CONFIG_ENGINE \"$ENGINE\""

) > config.h

