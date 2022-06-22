:> $3
grep -n $2 $1 | awk -F ':' '{print $1}' >> $3

