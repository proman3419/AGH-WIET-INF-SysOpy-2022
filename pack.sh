#!/bin/bash

CW_NO=$1
AUTHOR="RomanPrzemyslaw"

mkdir "$AUTHOR"
cp -r "cw$CW_NO" "$AUTHOR"
if [[ $CW_NO -le 9 ]]; then
    TAR_NAME="$AUTHOR-cw0$CW_NO.tar.gz"
else
    TAR_NAME="$AUTHOR-cw$CW_NO.tar.gz"
fi

tar cvzf "$TAR_NAME" "$AUTHOR"
rm -rf "$AUTHOR"
mv "$TAR_NAME" "cw$CW_NO/"
