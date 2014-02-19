echo "####-- TEST indexer --####"
echo "./src/indexer -c pytest/csft.py.token.conf xml"
./src/indexer -c pytest/csft.py.token.conf xml

echo
echo
echo 

echo "####-- TEST search --####"
echo "./src/search -c pytest/csft.py.token.conf -i xml -e  '百度李彦宏google'"
./src/search -c pytest/csft.py.token.conf -i xml -e  "百度李彦宏google"

echo 
echo
echo
echo "####-- TEST search extend word --####"
echo ./src/search -c pytest/csft.py.token.conf -i xml -e  "爆炸"
./src/search -c pytest/csft.py.token.conf -i xml -e  "爆炸"
