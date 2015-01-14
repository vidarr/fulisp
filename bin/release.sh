#!/bin/bash
TARGET_DIR=$1
TARBALL_PREFIX='fulisp'
TARBALL_POSTFIX='tar.gz'
TARBALL="$TARBALL_PREFIX.$TARBALL_POSTFIX"
FULISP_VERSION=$(cat .version.h | sed -e 's/.*"\([^"]*\)"/\1/')
TARBALL_NEW_NAME=$TARBALL_PREFIX-$FULISP_VERSION.$TARBALL_POSTFIX

SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
SIGN_COMMAND="$SCRIPT_DIR/create_checksums.sh"

function abort_script() {
    echo $1
    exit 1
}

function clear_target_dir() {
    if [ -d "$1" ]; then
        rm -rf $1
    elif [ -e "$1" ]; then 
        abort_script "Should create release files under $1, but is a file"
    fi
    mkdir $1
}

if [ -z $TARGET_DIR ]; then
    abort_script "No release directory given"
fi

echo "Going to build fuLisp  release $FULISP_VERSIONs in $TARGET_DIR."
read -p "Proceed? [yn]" YES_NO


if [ $YES_NO == "y" ]; then
    clear_target_dir $TARGET_DIR
    if [ -f $TARBALL ]; then 
        mv $TARBALL $TARBALL_NEW_NAME &&
        git tag -a "v$FULISP_VERSION" -m "RELEASE"
    else
        echo "Error: Tarball $TARBALL has not been created!" 
        exit 1;
    fi
    mv $TARBALL_NEW_NAME $TARGET_DIR
    OLD_DIR=$(pwd)
    cd $TARGET_DIR
    $SIGN_COMMAND $TARBALL_NEW_NAME -s
    cd $OLD_DIR 
    echo "Release $FULISP_VERSION ready under $TARGET_DIR"
fi

exit 0




