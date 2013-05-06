#!/bin/sh
# Change the folder to YOUR sparrow3d folder!
VERSION="1.2.0.0"
DEST=./build/*
echo "<html>" > index.htm
echo "<head>" >> index.htm
echo "</head>" >> index.htm
echo "<body>" >> index.htm
echo "<h1>Puzzletube Download links:</h1>" >> index.htm
for f in $DEST
do
  NAME=`echo "$f" | cut -d/ -f3 | cut -d. -f1`
  echo "Copy to: $NAME"
  cp -r font "$f/puzzletube"
  cp -r images "$f/puzzletube"
  cp -r music "$f/puzzletube"
  cp -r sounds "$f/puzzletube"
  cp -r translations "$f/puzzletube"
  cd $f
  if [ $NAME = "pandora" ]; then
    cd puzzletube
    ../make_package.sh
    cd ..
		echo "<a href=puzzletube.pnd>$NAME</a></br>" >> ../../index.htm
  else
    if [ $NAME = "i386" ]; then
      tar cfvz "puzzletube-$NAME-$VERSION.tar.gz" puzzletube
      mv "puzzletube-$NAME-$VERSION.tar.gz" ../..
  		echo "<a href=puzzletube-$NAME-$VERSION.tar.gz>$NAME</a></br>" >> ../../index.htm
    else
      zip -r "puzzletube-$NAME-$VERSION.zip" puzzletube
      mv "puzzletube-$NAME-$VERSION.zip" ../..
  		echo "<a href=puzzletube-$NAME-$VERSION.zip>$NAME</a></br>" >> ../../index.htm
    fi
  fi
  cd ..
  cd ..
done
echo "</body>" >> index.htm
echo "</html>" >> index.htm
