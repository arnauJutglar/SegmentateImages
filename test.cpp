#include <opencv2/opencv.hpp>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace cv;
using namespace std;

#include "segmentator.hpp"

struct colComp {
    bool operator()(const Vec3b& c1, const Vec3b& c2) const {
        return c1[0] < c2[0];
    }
};

int main() {
    /*
    Mat image = Mat::zeros(10,10,CV_8UC3);
    Mat boundary = Mat::zeros(10,10,CV_8UC3);
    for (int j=0; j < 10; j++) {
        Point p(5,j);
        image.at<Vec3b>(p) = Vec3b(255,255,255);
    }

    
    Segmentator segmentator(image);

    Mat labels = segmentator.segmentate();

    std::cout << "Boundaries: "<< std::endl;
    for (Segmentator::Node_t region : segmentator.regions) {
        std::cout << "Boundary size: " << region.boundary.size() << std::endl;

        for (Point p : region.boundary) {
            boundary.at<Vec3b>(p) = region.label_color;
        }
    }

    for (int j=0; j < 10; j++) {
        for (int i=0; i < 10; i++) {
            std::cout << image.at<Vec3b>(Point(i,j)) << "\t";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;


    for (int j=0; j < 10; j++) {
        for (int i=0; i < 10; i++) {
            std::cout << labels.at<Vec3b>(Point(i,j)) << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

        for (int j=0; j < 10; j++) {
        for (int i=0; i < 10; i++) {
            std::cout << boundary.at<Vec3b>(Point(i,j)) << "\t";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;

    for (int j=0; j < 10; j++) {
        for (int i=0; i < 10; i++) {
            std::cout << segmentator.isBoundary(segmentator.regions[0], Point(i,j)) << "\t";
        }
        std::cout << std::endl;
    }



    segmentator.print();
    */

    Vec3b c(1,1,1);
    Vec3b c1(1,1,1);
    std::cout << (c == c1) << std::endl;

    set<Vec3b, Segmentator::color_comp> s;
    s.insert(c);
    s.insert(c1);
    std::cout << s.size() << std::endl;




    
    return 0;
}

bool operator<(const Vec3b& c1, const Vec3b& c2) {
    if (c1[0] < c2[0]) {
        return true;
    } else if (c1[0] > c2[0]) {
        return false;
    } else if (c1[1] < c2[1]) {
        return true;
    } else if (c1[1] > c2[1]) {
        return false;
    } else if (c1[2] < c2[2]) {
        return true;
    } else {
        return false;
    }
}
