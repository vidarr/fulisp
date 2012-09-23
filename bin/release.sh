#!/bin/bash
TARBALL_PREFIX='fulisp'
TARBALL_POSTFIX='tar.gz'
TARBALL="$TARBALL_PREFIX.$TARBALL_POSTFIX"
FULISP_VERSION=$(cat .version.h | sed -e 's/.*"\([^"]*\)"/\1/')
TARBALL_NEW_NAME=$TARBALL_PREFIX-$FULISP_VERSION.$TARBALL_POSTFIX
read -p "Going to build fuLisp  release $FULISP_VERSION. Proceed? [yn]" YES_NO
if [ $YES_NO == "y" ]; then
    if [ -f $TARBALL ]; then 
        mv $TARBALL $TARBALL_NEW_NAME &&
        git tag -a "v$FULISP_VERSION" -m "RELEASE"
    else
        echo "Error: Tarball $TARBALL has not been created!" 
        exit 1;
    fi
fi


