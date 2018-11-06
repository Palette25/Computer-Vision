echo "Welcome to the testing file....."
echo "Testing A4: "
# For A4
./A4_Edge_detector 1.bmp 10.5
./A4_Edge_detector 2.bmp 10.5
./A4_Edge_detector 3.bmp 10.5
./A4_Edge_detector 4.bmp 5.5
./A4_Edge_detector 5.bmp 10.5
./A4_Edge_detector 6.bmp 10.5

echo "Testing Coins: "
# For Coins
./Coin_Edge_detector 7.bmp 150 170 120 20
./Coin_Edge_detector 8.bmp 180 250 100 20
./Coin_Edge_detector 9.bmp 150 180 115 60
./Coin_Edge_detector 10.bmp 180 210 85 20
./Coin_Edge_detector 11.bmp 420 540 76 20
./Coin_Edge_detector 12.bmp 40 70 120 20

echo "End of testing."