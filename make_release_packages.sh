#!/bin/bash
# Change the folder to YOUR sparrow3d folder!
PROGRAM="puzzletube"
VERSION="1.3.4.2"
DEST=./build/*
ZIP_CALL="7z a -t7z -m0=LZMA -mmt=on -mx=9 -md=256m -mfb=512 -ms=on upload.7z"
RM_CALL="rm upload.7z"
TIME=`date -u +"%d.%m.%Y %R"`

eval "$RM_CALL"

echo "<html>" > index.htm
echo "<head>" >> index.htm
echo "</head>" >> index.htm
echo "<body>" >> index.htm
echo "Updated at the $TIME." >> index.htm
echo "<h1>$PROGRAM download links:</h1>" >> index.htm

echo "<?php" > symlink.php
echo "system('7z e -y upload.7z');" >> symlink.php
echo "unlink('upload.7z');" >> symlink.php
for f in $DEST
do
	if [ -e "$f/$PROGRAM/$PROGRAM" ] || [ -e "$f/$PROGRAM/$PROGRAM.exe" ]; then
		NAME=`echo "$f" | cut -d/ -f3 | cut -d. -f1`
		echo "$NAME:"
		echo "--> Copy temporary folders"
		cp -r font "$f/$PROGRAM"
		cp -r images "$f/$PROGRAM"
		cp -r music "$f/$PROGRAM"
		cp -r sounds "$f/$PROGRAM"
		cp -r translations "$f/$PROGRAM"
		cp README "$f/$PROGRAM"
		cd $f
		echo "--> Create archive"
		if [ $NAME = "pandora" ]; then
			cd $PROGRAM
			../make_package.sh
			cd ..
			echo "<a href=$PROGRAM.pnd>$NAME</a></br>" >> ../../index.htm
			ZIP_CALL+=" $PROGRAM.pnd"
		else
			if [ $NAME = "i386" ] || [ $NAME = "amd64" ]; then
				tar cfvz "$PROGRAM-$NAME-$VERSION.tar.gz" * > /dev/null
				mv "$PROGRAM-$NAME-$VERSION.tar.gz" ../..
				echo "<a href=$PROGRAM-$NAME-$VERSION.tar.gz>$NAME</a></br>" >> ../../index.htm
				echo "unlink('$PROGRAM-$NAME.tar.gz');" >> ../../symlink.php
				echo "symlink('$PROGRAM-$NAME-$VERSION.tar.gz', '$PROGRAM-$NAME.tar.gz');" >> ../../symlink.php
				ZIP_CALL+=" $PROGRAM-$NAME-$VERSION.tar.gz"
			else
				if [ $NAME = "gcw" ]; then
					mksquashfs * "$PROGRAM-$VERSION.opk" -all-root -noappend -no-exports -no-xattrs
					mv "$PROGRAM-$VERSION.opk" ../..
					echo "<a href=$PROGRAM.opk type=\"application/x-opk+squashfs\">$NAME</a></br>" >> ../../index.htm
					echo "unlink('$PROGRAM-$VERSION.opk');" >> ../../symlink.php
					echo "symlink('$PROGRAM-$VERSION.opk', '$PROGRAM.opk');" >> ../../symlink.php
					ZIP_CALL+=" $PROGRAM-$VERSION.opk"
				else
					zip -r "$PROGRAM-$NAME-$VERSION.zip" * > /dev/null
					mv "$PROGRAM-$NAME-$VERSION.zip" ../..
					echo "<a href=$PROGRAM-$NAME-$VERSION.zip>$NAME</a></br>" >> ../../index.htm
					echo "unlink('$PROGRAM-$NAME.zip');" >> ../../symlink.php
					echo "symlink('$PROGRAM-$NAME-$VERSION.zip', '$PROGRAM-$NAME.zip');" >> ../../symlink.php
					ZIP_CALL+=" $PROGRAM-$NAME-$VERSION.zip"
				fi
			fi
		fi
		echo "--> Remove temporary folders"
		rm -r $PROGRAM/font
		rm -r $PROGRAM/images
		rm -r $PROGRAM/music
		rm -r $PROGRAM/sounds
		rm -r $PROGRAM/translations
		rm $PROGRAM/README
		cd ..
		cd ..
	fi
done
echo "?>" >> symlink.php
echo "</body>" >> index.htm
echo "</html>" >> index.htm
eval "$ZIP_CALL"
