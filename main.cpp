// Ce code utilise la bibliothèque OpenCV pour réaliser la croissance de région
#include <opencv2/opencv.hpp>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace cv;
using namespace std;

#include "segmentator.hpp"




int main() {
    // Chargement de l'image couleur
    Mat image = imread("lena.jpg", IMREAD_COLOR);
    
    Segmentator segmentator(image);

    Mat labels = segmentator.segmentate();

    // Affichage de l'image originale et de l'image segmentée
    imshow("Original image", image);
    imshow("Segmented image", labels); // On multiplie les labels par 25 pour mieux les distinguer
    waitKey(0);
    
    /*Vec3b p;
    for (int j=0; j < image.size[0]; j += 300) {
        for (int i=0; i < image.size[1]; i += 300) {
            p = image.at<Vec3b>(Point(i,j));
            std::cout << '(' << int(p[0]) << ',' << int(p[1]) << ',' << int(p[2]) << ") ";
        }
        std::cout << std::endl;
    }*/

    return 0;
}
