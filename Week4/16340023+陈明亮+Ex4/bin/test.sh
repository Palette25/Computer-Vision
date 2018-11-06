echo "Testing Image Equalization......"
echo "1) Gray Scale Equalization: "
./Image_Equalizater ../testData1/1.bmp 0
./Image_Equalizater ../testData1/2.bmp 0
./Image_Equalizater ../testData1/3.bmp 0
./Image_Equalizater ../testData1/4.bmp 0
./Image_Equalizater ../testData1/5.bmp 0
echo "2) Colorful Scale Equalization: "
./Image_Equalizater ../testData1/1.bmp 1
./Image_Equalizater ../testData1/2.bmp 1
./Image_Equalizater ../testData1/3.bmp 1
./Image_Equalizater ../testData1/4.bmp 1
./Image_Equalizater ../testData1/5.bmp 1

echo "Testing Color Transform......"
./Color_Transformer ../testData2/2.bmp ../testData2/1.bmp
./Color_Transformer ../testData2/3.bmp ../testData2/4.bmp
./Color_Transformer ../testData2/5.bmp ../testData2/6.bmp
./Color_Transformer ../testData2/7.bmp ../testData2/8.bmp
./Color_Transformer ../testData2/10.bmp ../testData2/9.bmp