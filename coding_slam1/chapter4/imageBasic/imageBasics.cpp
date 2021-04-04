//
// Created by richardchen123 on 28/03/2021.
//
#include <iostream>
#include <chrono>

using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char **argv) {
    //读取argv[1]指定的图像
    cv::Mat image;
    image = cv::imread(argv[1]);
    //判断图像文件是否正确读取
    if (image.data == nullptr) {
        cerr << 'file' << argv[1] << 'does not exist.' << endl;
        return 0;
    }

    //文件顺利读取，首先输出一些基本信息
    cout << 'Image width:' << image.cols << ', height:' << image.rows << ',channel:' << image.channels() << endl;
    cv::imshow("image", image);
    cv::waitKey(0);
    //判断image的类型
    if(image.type() !=CV_8UC1 && image.type() !=CV_8UC3)
    {
        cout << 'please input a color image or grey image.' <<endl;
        return 0;
    }

    //遍历图像，请注意一下遍历方式亦可使用于随机像素访问
    //使用 std::chrono来给算法计时
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    for(size_t y = 0; y < image.rows; y++){
        //用cv::mat::ptr获得图像的行指针
        unsigned char* row_ptr = image.ptr<unsigned char>(y); //row_ptr是第y行的行指针
        for(size_t x = 0; x < image.cols; x++){
            //访问位于x, y的元素
            unsigned char* data_ptr = &row_ptr[x*image.channels()]; // data_ptr指向待访问的像素数据
            //输出该像素的每个通道，如果是灰度图就输出一个通道
            for(int c = 0; c !=image.channels(); c++){
                unsigned char data = data_ptr[c];  // data为I(x, y )第c个通道的值
            }
        }
    }

    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>( t2-t1 );
    cout<<"遍历图像用时："<<time_used.count()<<" 秒。"<<endl;

    //关于cv::Mat的拷贝
    //直接赋值并不会拷贝数据
    cv::Mat image_another = image;
    //修改 image_another会导致Image发生变化
    image_another(cv::Rect(0, 0, 100, 100)).setTo(0); // 将左上角100*100的块置零
    cv::imshow ( "image", image );
    cv::waitKey ( 0 );

    // 使用clone函数来拷贝数据
    cv::Mat image_clone = image.clone();
    image_clone ( cv::Rect ( 0,0,100,100 ) ).setTo ( 255 );
    cv::imshow ( "image", image );
    cv::imshow ( "image_clone", image_clone );
    cv::waitKey ( 0 );

    cv::destroyAllWindows();
    return 0;

}