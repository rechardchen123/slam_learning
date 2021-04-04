//
// Created by richardchen123 on 29/03/2021.
//
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    //read images,读取图像
    Mat img_1 = imread("../1.png", IMREAD_COLOR);
    Mat img_2 = imread("../2.png", IMREAD_COLOR);

    // init, 初始化
    std::vector<KeyPoint> keypoints_1, keypoints_2;
    Mat descriptors_1, descriptors_2;
    Ptr<FeatureDetector> detector = ORB::create();
    Ptr<DescriptorExtractor> descriptor = ORB::create();
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");

    // first step: oriented FAST position, 找到FAST角点位置
    detector->detect(img_1, keypoints_1);
    detector->detect(img_2, keypoints_2);

    // second step: calculate BRIEF descriptors
    descriptor->compute(img_1, keypoints_1, descriptors_1);
    descriptor->compute(img_2, keypoints_2, descriptors_2);

    Mat outimg1;
    drawKeypoints(img_1, keypoints_1, outimg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    imshow("ORB feature points", outimg1);

    //third step: match the BRIEF of the two figures, using Hamming distance
    vector<DMatch> matches;
    matcher->match(descriptors_1, descriptors_2, matches);

    //fourth step: filter match points
    double min_dist = 1000, max_dist = 0;

    //find out all the min and max distance
    for (int i = 0; i < descriptors_1.rows; i++) {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    min_dist = min_element(matches.begin(), matches.end(),
                           [](const DMatch &m1, const DMatch &m2) { return m1.distance < m2.distance; })->distance;
    max_dist = max_element(matches.begin(), matches.end(),
                           [](const DMatch &m1, const DMatch &m2) { return m1.distance < m2.distance; })->distance;

    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

    //当描述子之间的距离大于两倍的最小距离时,即认为匹配有误.但有时候最小距离会非常小,设置一个经验值30作为下限.
    std::vector<DMatch> good_matches;

    for (int i = 0; i < descriptors_1.rows; i++) {
        if (matches[i].distance <= max(2 * min_dist, 30.0)) {
            good_matches.push_back(matches[i]);
        }
    }

    // fifth step: draw the match results
    Mat img_match;
    Mat img_goodmatch;
    drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_match);
    drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, img_goodmatch);
    imshow("所有匹配点对", img_match);
    imshow("优化后匹配点对", img_goodmatch);
    waitKey(0);


    return 0;
}