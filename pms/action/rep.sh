for php in `ls *.php`
do
  echo "$php replace $1 to $2"
  cp $php ${php}.bak
  sed -e "s/$1/$2/g" $php > tmp
  mv tmp $php
  echo "$php replace ok!"
done
