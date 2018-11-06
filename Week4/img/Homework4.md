# 计算机视觉 - Homework4

### 姓名：陈明亮

### 学号：16340023

## 一、实验需求
1. 参考课程PPT，采用`CImg`库编写灰度图像与彩色图像直方图均衡化程序，要求拍摄不同光照下的测试图像，分别针对灰度化后的图像，与原本的彩色图像进行直方图均衡化。

2. 获取均衡化之后的结果图像，分析实验结果，同时针对彩色图像直接采取直方图均衡化的效果，提供改进思路。

3. 同样参考课程PPT-Part2，编写颜色变换`Color-Transform`程序，接收参考图与原图，实现参考图上的颜色转换到原图上。分析实验结果，找出存在问题，给出改进建议方法。



## 二、实验步骤解析

1. 对于直方图均衡化代码编写，主要分为以下几点：1) 接收输入图像，根据参数确认进行灰度直方图均衡化，还是彩色直方图均衡化 2) 根据处理图像色彩通道数，构建像素分布直方图 3) 根据直方图进行累加变换，转换为均匀分布的直方图，填充生成均衡化之后的图像。
2. 对于颜色变换程序的编写，我们结合图像像素色彩分布近似于标准高斯分布的依据，设：$y = \alpha_1 x + \mu_1$为原图的像素分布方程，横轴为像素值，纵轴为对应像素值的像素点总数，同理设：$y = \alpha_2 x + \mu_2$为参考图的标准高斯分布方程，那么实际上从参考图到原图的颜色变换方程就为：$y_1 = {\frac {\alpha_2} {\alpha_1 }}(y_2 - \mu_1) + \mu_2 $
3. 结合RGB到Lab色彩通道的变换，以及CImg官方库函数，我们可以围绕以上方程编写代码，最终输出结果并分析。




## 三、实验程序简述

1. Image_Equalizater: 图像均衡化可执行程序，接收参数为：`image_path`，`process_type`

   * 输入参数分别为输入图像的路径path，以及执行灰度直方图均衡化、还是彩色直方图均衡化的标记参数process_type(注：当`type`为0时进行灰度直方图均衡化，接收彩色图程序内部自动转换为灰度图；`type`为1时进行彩色直方图均衡化)。

   * 输出图像依次为：原图、原图的直方图分布、均衡化之后的图像、均衡化图像的直方图分布。

     ​

2. `Color_Transformer`: 图像颜色变换可执行程序，接收参数为：`refer_img_type`，`origin_img_type`

   * 输入参数分别为：颜色参考图图像路径，原图图像路径。
   * 输出图像为：经过颜色变换之后的结果图。



## 四、实验过程

### Part 1、直方图均衡化

1. 接收输入处理类型，若`type`为0，则将当前彩色图像转换为灰度图像，否则保留原图像不变。

   代码：

   ```c++
   // Change colorful image to gray scale
   void Histogram_Equalizater::RGBtoGray(){
   	CImg<unsigned char> gray(destImg.width(), destImg.height(), 1, 1, 0);
   	cimg_forXY(srcImg, x, y){
   		gray(x, y) = srcImg(x, y, 0) * 0.299 + srcImg(x, y, 1) * 0.587 + 
               										srcImg(x, y ,2) * 0.114;
   	}
   	srcImg = gray;
   }
   ```

2. 下文将分为两部分讨论直方图均衡化过程，两种方法大致相同，但涉及处理的颜色通道数量不同，故分开讲述：

     1)   灰度图像直方图均衡化过程

   * 单通道的灰度图像只需要针对图像每一像素点进行0~255范围内的分布进行数量统计即可，然后再分别除以图像上的总体像素值之和，获得该图像从0~255像素值的概率分布：

     ```c++
     vector<float> Histogram_Equalizater::calculateHistogram(CImg<unsigned char>& origin){
     	vector<float> hist(256, 0);
     	int total = 0;
     	cimg_forXY(origin, x, y){
     		++hist[origin(x, y)];
     		++total;
     	}
     	// Perform probability transform
     	for(int i=0; i<256; i++){
     		hist[i] = (float)hist[i] / total;
     	}
     	return hist;
     }
     ```

   * 紧接着进行概率分布数组的累加变换，将该图上的像素值转换为[0, 255]上的均匀分布：(部分代码)

     ```c++
     // Perform pixels probability adding
     float sum = 0;
     vector<int> thist(256, 0);
     for(int i=0; i<256; i++){
     	sum += srcHistogram[i];
     	thist[i] = (int)(sum * (256-1) + 0.5f);
     }
     // Perform filling into destination image
     cimg_forXY(destImg, x, y){
     	destImg(x, y) = thist[srcImg(x, y)];
     }
     ```

   2)   彩色图像直方图均衡化过程

   * 平常所拍摄的彩色图像采用RGB三通道记录某像素点对应的像素值，故在进行图像像素点分布直方图统计时，需要分别存储每个像素点对应的R、G、B值。在实际的编码过程中，定义以下结构体并使用`vector`存储整幅图像的每个点RGB值: 

     ```c++
     struct rgb{
     	float r;
     	float g;
     	float b;
     	rgb(float r_, float g_, float b_){
     		r = r_;
     		g = g_;
     		b = b_;
     	}
     	rgb& operator=(const rgb& oth){
     		r = oth.r;
     		g = oth.g;
     		b = oth.b;
     	}
     };
     ```

   * 分别按照不同通道，确定像素点对应色彩的概率分布存储数组：

     ```c++
     vector<rgb> Histogram_Equalizater::calculateRGBHistogram(CImg<unsigned char>& origin){
         vector<rgb> hist;
         int total = 0;
         for(int i=0; i<256; i++){
             hist.push_back(rgb(0, 0, 0));
         }
         cimg_forXY(origin, x, y){
             hist[origin(x, y, 0)].r += 1;
             hist[origin(x, y, 1)].g += 1;
             hist[origin(x, y, 2)].b += 1;
             ++total;
         }
         // Perform probability dividing
         for(int i=0; i<256; i++){
             hist[i].r = (float)hist[i].r / total;
             hist[i].g = (float)hist[i].g / total;
             hist[i].b = (float)hist[i].b / total;
         }
         return hist;
     }
     ```

   * 最后按照三种色彩通道的概率分布，分别进行累加变换，最终分别赋值回处理后图像的对应像素点上：(部分代码)

     ```c++
     float sum1 = 0, sum2 = 0, sum3 = 0;
     vector<rgb> thist;
     for(int i=0; i<256; i++){
         thist.push_back(colorhist[i]);
     }
     // Adding
     for(int i=0; i<256; i++){
         sum1 += thist[i].r;
         sum2 += thist[i].g;
         sum3 += thist[i].b;
         thist[i].r = (sum1 * 255 + 0.5f);
         thist[i].g = (sum2 * 255 + 0.5f);
         thist[i].b = (sum3 * 255 + 0.5f);
     }
     // Finish
     cimg_forXY(destImg, x, y){
         destImg(x, y, 0) = (int)thist[srcImg(x, y, 0)].r;
         destImg(x, y, 1) = (int)thist[srcImg(x, y, 1)].g;
         destImg(x, y, 2) = (int)thist[srcImg(x, y, 2)].b;
     }
     ```

3. 直方图均衡化结果分析

   1)  灰度图像直方图均衡化

   * 灰度图一 (1.bmp)

     ![img](11.bmp)

     原图像对应的像素直方图

     ![img](1.png)

     灰度直方图均衡化之后的结果图像

     ![img](1.bmp)

     均衡化后图像对应的直方图

     ![img](2.png)

     ​

   * 灰度图二 (2.bmp)

     ![img](12.bmp)

       原图像对应的灰度直方图

     ![img](3.png)

       灰度图像直方图均衡化之后的结果图像

     ![img](2.bmp)

       均衡化图像对应的直方图

     ![img](4.png)

   * 灰度图三 (3.bmp)

     ![img](13.bmp)

     原图像对应的灰度直方图

     ![img](5.png)

     灰度图像均衡化之后的处理图像

     ![img](3.bmp)

     处理后图像的灰度直方图

     ![img](6.png)

     ​

   * 灰度图四 (4.bmp)

     ![img](14.bmp)		

      原图像对应的灰度直方图

     ![img](7.png)

     灰度直方图处理之后图

     ![img](4.bmp)

     处理之后图像灰度直方图

     ![img](8.png)

     ​

   * 灰度图五 (5.bmp)

     ![img](15.bmp)

     原图像对应的灰度直方图

     ![img](9.png)

     灰度直方图均衡化后图像

     ![img](5.bmp)

     处理后图像直方图

     ![img](10.png)

     ​

   * 总体结果分析：灰度图像的均衡化效果不错，从结果图像的直方图可以看出，均衡化使得像素点的像素值不呈连续分布，图像上各点的灰度值差距变大，均衡化效果明显。从结果图像上来看，明显相比于原图对比度升高。


   2) 彩色图像直方图均衡化

* 彩色图一 (6.bmp)

  ![img](E:\Github\Computer_Vision\Week4\16340023+陈明亮+Ex4\testData1\1.bmp)

  原图像对应的像素直方图

  ![img](11.png)

  直方图均衡化之后图像

  ![img](6.bmp)

  对应直方图

  ![img](12.png)

  ​

* 彩色图二 (7.bmp)

  ![img](E:\Github\Computer_Vision\Week4\16340023+陈明亮+Ex4\testData1\2.bmp)

  原图对应的直方图

  ![img](13.png)

  均衡化图像

  ![img](7.bmp)

  对应直方图

  ![img](14.png)

  ​

* 彩色图三 (8.bmp)

  ![img](E:\Github\Computer_Vision\Week4\16340023+陈明亮+Ex4\testData1\3.bmp)

  原图像素直方图

  ![img](15.png)

  均衡化后图像

  ![img](8.bmp)

  对应的直方图

  ![img](16.png)

  ​

* 彩色图四 (9.bmp)

  ![img](E:\Github\Computer_Vision\Week4\16340023+陈明亮+Ex4\testData1\4.bmp)

  对应直方图

  ![img](17.png)

  均衡化之后图像

  ![img](9.bmp)

  对应的直方图

  ![img](18.png)

  ​

* 彩色图五 (10.bmp)

  ![img](E:\Github\Computer_Vision\Week4\16340023+陈明亮+Ex4\testData1\5.bmp)

  对应的直方图

  ![img](19.png)

  均衡化处理之后的图像

  ![img](10.bmp)

  对应的直方图

  ![img](20.png)

  ​

* 总体结果分析：将彩色图像直接采用直方图均衡化方法处理，实际上也能够做到增大对比度，使得整体的像素分布较为均匀。但是实际上对于某些彩色图，直方图均衡化可能会使得图像整体较模糊，噪点增加，色调也随之改变。

* 改进建议：可以采用BGR彩色通道空间，替换直接均衡化方法采用的RGB三通道，针对亮度进行每一通道的均衡化，最后再合并起来。




### Part 2、颜色变换

1. 接收输入的参考图与原图，并且使用CImg库函数实现从RGB通道到Lab通道的图像变换

   ```c++
   // Color channels transform tool functions
   void Color_Transformer::RGBtoLab(){
   	colorImg = colorImg.get_RGBtoLab();
   	srcImg = srcImg.get_RGBtoLab();
   }
   ```

   ​

2. 根据颜色分布变换公式: $y_1 = \frac {\alpha_2} {\alpha_1} (y_2 - \mu_1) + \mu_2$，分别借助CImg库函数`mean()`， `variance()`，获取Lab通道图像的颜色像素分布的均值和方差数据(部分代码)

   ```c++
   // Get channel
   CImg<float> color_l = colorImg.get_channel(0);
   CImg<float> color_a = colorImg.get_channel(1);
   CImg<float> color_b = colorImg.get_channel(2);
   // Get mean
   float c1_mean = color_l.mean();
   float c2_mean = color_a.mean();
   float c3_mean = color_b.mean();
   // Get variance
   float c1_variance = color_l.variance(0);
   float c2_variance = color_a.variance(0);
   float c3_variance = color_b.variance(0);
   ```

   ​

3. 最终结合变换公式，完成结果图像的像素填充与输出，以及从Lab空间到RGB空间的转回

   ```c++
   cimg_forXY(destImg, x, y){
   destImg(x, y, 0) = (sqrtf(c1_variance)/sqrtf(s1_variance)) * (src_l(x, y) - s1_mean) + c1_mean;
   destImg(x, y, 1) = (sqrtf(c2_variance)/sqrtf(s2_variance)) * (src_a(x, y) - s2_mean) + c2_mean;
   destImg(x, y, 2) = (sqrtf(c3_variance)/sqrtf(s3_variance)) * (src_b(x, y) - s3_mean) + c3_mean;
   }

   destImg = destImg.get_LabtoRGB();
   ```

   ​

4. 测试结果输出：

   * Case 1

     原图  ![img](testData2/1.bmp)                          参考图  ![img](testData2/2.bmp)

     ​

     ​					结果图  ![img](k1.bmp)

     ​

   * Case 2

     ​					原图![img](testData2/4.bmp)

     ​				    参考图![img](testData2/3.bmp)

   ​					结果图 ![img](k2.bmp)

   ​

   * Case 3

     原图![img](testData2/6.bmp)参考图![img](testData2/5.bmp)

     ​

     ​			     结果图  ![img](k3.bmp)

     ​

   * Case 4

     原图      ![img](testData2/8.bmp)

     参考图  ![img](testData2/7.bmp)

     结果图   ![img](k4.bmp)

     ​

   * Case 5

     原图 ![img](testData2/9.bmp)

     参考图 ![img](testData2/10.bmp)

     结果图 ![img](k5.bmp)

     ​

     * 总体结果分析：
       1. 本程序总体上来看，颜色变换的处理效果还是不错的，能够真正地实现从参考图到原图的颜色迁移，任务完成度高。
       2. 然而，存在的问题还是有的。`Swatches`，色样，我们可以观察到每当原图与参考图的某些区域，色样区别过大时，变换完成的效果就会显得有点奇怪，导致对应区域的颜色不太自然。
       3. 上文结果问题不明显，参考课程ppt，可以得出Lab空间颜色变换特点：该颜色迁移方法为整体色彩迁移，因此它对全局颜色基调单一的图像的有着良好的迁移效果。而对于颜色内容丰富的图像，则效果并不那么明显，甚至会显得不自然。
       4. 解决参考方法：
          * 引入用户自定义模块，要求使用者指定图像块之间的迁移关系(缺点：用户不友好，工作量大)
          * 采用灰度图像彩色化迁移法，利用查找匹配像素来实现灰度图像的色彩迁移，因为灰度图像只有亮度信息，所以该算法主要通过像素的亮度值匹配，来实现灰度图像的自动彩色化。
       5. 参考文献：[颜色迁移之三——Welsh经典算法](https://blog.csdn.net/sin_geek/article/details/22609171)



## 五、实验感想

1. 本次实验是计算机视觉课的第一次附加作业，整体上感觉比正式作业难度要低，整体参考老师上课讲述的ppt，便可以编写出相应的程序代码，运行效果完成度高，于是本次实验自评96分。

   ​

2. 不论是直方图均衡化、还是颜色迁移算法，实验中采取的都算是最基础的做法，在查阅资料的过程中也见识到了现代工业级CV采用的算法，也算是课程知识的一个拓展，分析当前做法的不足之处，思考并求出，找出更好的方法，学习的过程应该就是如此的循环往复过程。