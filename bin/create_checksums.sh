#!/bin/bash
CHECKSUM_TOOL=sha256sum  
CHECKSUM_EXTENSION=$CHECKSUM_TOOL


function usage() {
    echo "$0 FILE  [-s] [KEY_ID]"
    echo "Create checksum for FILE."
    echo "If -s is given, create detached signature for checksum file"
    echo "If KEY_ID is given, use this key instead of default one"
}


if [ -z  "$1" ]; then
    usage
    exit 1
fi

echo "Called $0 $1 $2 $3 $4"

CHECKSUM_FILE=$1.$CHECKSUM_EXTENSION
$CHECKSUM_TOOL $1 > $CHECKSUM_FILE

while shift; do 
    case $1 in
        "-s")
            echo "Signing $CHECKSUM_FILE"
            KEY_OPTS=""
            if [ ! -z "$2" ]; then 
                KEY_OPTS="--default_key $3"
            fi
            gpg --armour --detach-sign $KEY_OPTS $CHECKSUM_FILE
            ;;
    esac
done

