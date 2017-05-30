for htmlfile in `ls *.html`
do
  echo "$htmlfile replace $1 to $2"
  cp $htmlfile ${htmlfile}.bak
  sed -e "s/$1/$2/g" $htmlfile > tmp
  mv tmp $htmlfile
  echo "$htmlfile replace ok!"
done
