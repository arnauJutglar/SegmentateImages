// Ce code utilise la bibliothèque OpenCV pour réaliser la croissance de région
#include <opencv2/opencv.hpp>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "segmentator.hpp"

using namespace cv;
using namespace std;


int main() {
    Mat image = imread("lena.jpg", IMREAD_COLOR);
    
    Segmentator segmentator(image);
    segmentator.setSimilarityFunction([](Vec3b c1, Vec3b c2) -> bool {
        return norm(c1 - c2) < 50;
    });
    segmentator.setNumSeeds(32);

    Mat labels = segmentator.segmentate();
    Mat boundary = segmentator.computeBoundary();

    imshow("Original image", image);
    imshow("Segmented image", labels);
    imshow("Unmerged segmented image", segmentator.getUnmergedLabels());
    imshow("Boundary", segmentator.getBoundary());
    waitKey(0);
    
    return 0;
}
