echo "@@@@@@@@@@@@@@@@@@ TEST mmseg token @@@@@@@@@@@@@@@@@@@"
echo "####-- TEST indexer --####"
echo "../src/indexer -c csft.py.mmseg.conf xml"
../src/indexer -c csft.py.mmseg.conf xml --rotate

echo
echo
echo 

echo "####-- TEST search --####"
echo "../src/search -c csft.py.mmseg.conf -i xml -e  '百度李彦宏google'"
../src/search -c csft.py.mmseg.conf -i xml -e  "百度李彦宏google"

echo 
echo
echo
echo "#### --TEST adv search -- ###"
echo ../src/search -c csft.py.mmseg.conf -i xml -e '蛊惑 NEAR/3 最佳 NEAR/4 "愚人节"'
../src/search -c csft.py.mmseg.conf -i xml -e '蛊惑 NEAR/3 最佳 NEAR/4 "愚人节"'
