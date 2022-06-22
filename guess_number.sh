sed 's/?//g' $1 >  input.txt
grep "legal" input.txt
a=$?
if [ $a -eq 0 ];
then
	echo "yes" >> input.txt
	sed -n '1p' input.txt > buff.txt
	./ff
	cat buff1.txt >> input.txt	
	rm buff.txt buff1.txt
fi
