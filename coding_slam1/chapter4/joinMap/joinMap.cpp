//
// Created by richardchen123 on 28/03/2021.
//

#include <iostream>
#include <fstream>
using namespace std;
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Eigen/Geometry>
#include <boost/format.hpp>  // for formating strings
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>

int main(int argc, char** argv){
    vector<cv::Mat> colorImgs, depthImgs;  //彩色图和深度图
    vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>> poses; //相机位姿

    ifstream fin("./pose.txt");
    if (!fin){
        cerr<<"请在有pose.txt的目录下运行此程序"<<endl;
        return 1;
    }

    for(int i=0; i < 5; i++){
        boost::format fmt("../%s/%d.%s" ); //图像文件格式    "../%s/%d.%s"   ../ 表示可执行文件在build中,图像在上一个目录,所以用../
        /*这里的%对应./ color对应%s  下面的符号就是与上面一致对应的 */
        colorImgs.push_back( cv::imread( (fmt%"color"%(i+1)%"png").str() ));
        depthImgs.push_back( cv::imread( (fmt%"depth"%(i+1)%"pgm").str(), -1 )); // 使用-1读取原始图像

        /*基于范围的for循环，表示从data数组的第一项开始 循环遍历  auto表示自动根据后面的元素 获得符合要求的类型*/
        double data[7] = {0};
        for (auto & d:data) //auto 自动文件类型转换
            fin>>d; //文件流类型的变量fin将pose.txt中的数据给了d数组
        Eigen::Quaterniond q(data[6], data[3], data[4], data[5]); //四元数 data[6]是实数 但是coeffis输出的是先虚数后实数
        Eigen::Isometry3d T(q);                                             //变换矩阵初始化旋转部分，
        T.pretranslate( Eigen::Vector3d( data[0], data[1], data[2] )); //变换矩阵初始化平移向量部分
        poses.push_back( T );    //存储变换矩阵到位姿数组中
    }

    // 计算点云并拼接
    // 相机内参
    double cx = 325.5;
    double cy = 253.5;
    double fx = 518.0;
    double fy = 519.0;
    double depthScale = 1000.0;

    cout<<"正在将图像转换为点云..."<<endl;

    // 定义点云使用的格式：这里用的是XYZRGB
    typedef pcl::PointXYZRGB PointT;
    typedef pcl::PointCloud<PointT> PointCloud;

    //新建一个点云
    //PointCoud::Ptr是一个智能指针类 通过构造函数初始化指针指向的申请的空间
    PointCloud::Ptr pointCloud( new PointCloud );
    for(int i=0; i<5;i++){
        cout<<"转换图像中: "<<i+1<<endl;
        cv::Mat color = colorImgs[i];
        cv::Mat depth = depthImgs[i];
        Eigen::Isometry3d T = poses[i];
        /*对图像像素进行坐标转换，将图像的坐标通过内参矩阵K转换为相机坐标系下的坐标，之后通过外参矩阵T 转化为世界坐标系下的坐标*/
        for(int v = 0; v < color.rows; v++)
            for(int u=0; u < color.cols; u++){
                /*通过用Mat中的ptr模板函数 返回一个unsigned short类型的指针。v表示行 根据内部计算返回data头指针 + 偏移量来计算v行的头指针
                 * 图像为单通道的   depth.ptr<unsigned short> ( v ) 来获取行指针*/

                unsigned int d = depth.ptr<unsigned short> ( v )[u]; // 深度值
                if(d==0) continue;
                Eigen::Vector3d point;
                point[2] = double(d)/depthScale;
                point[0] = (u-cx)*point[2]/fx;
                point[1] = (v-cy)*point[2]/fy;
                Eigen::Vector3d pointWorld = T*point;

                PointT p;
                p.x = pointWorld[0];
                p.y = pointWorld[1];
                p.z = pointWorld[2];
                p.b = color.data[ v*color.step+u*color.channels() ];
                p.g = color.data[ v*color.step+u*color.channels()+1 ];
                p.r = color.data[ v*color.step+u*color.channels()+2 ];
                /*  -> 是智能指针重载的 然后返回的类型就是输入的类型 可以看上面Ptr的解释说明 */
                pointCloud->points.push_back( p );//存储格式好的点
            }
    }
    pointCloud->is_dense = false;
    cout<<"点云共有"<<pointCloud->size()<<"个点."<<endl;
    pcl::io::savePCDFileBinary("map.pcd", *pointCloud );
    return 0;
}