#!/bin/bash
mkdir test
cp -r code test/
cat code/14.c
i=0
while(($i<=15))
do
    gcc -c test/code/$i.c -o test/code/$i.o
    let i=i+1
done
gcc test/code/*.o -o test/hello
test/hello 2> test/err.txt
mv test/err.txt ./err.txt
chmod u=rw err.txt
chmod g=r err.txt
chmod o=xr err.txt
chmod a=x err.txt
if (($# == 0))
then
    sed -n '2p' err.txt >&2
elif (($# == 1))
then
    let tmp=$(($1+1))
    sed -n "$tmp p" err.txt >&2
else
    let tmp=$(($1+$2))
    sed -n "$tmp p" err.txt >&2
fi
