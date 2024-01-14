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

    std::cout << "Printing here: " << segmentator.regions.size() << std::endl;
    print_graph(segmentator.region_parents);

    std::cout << "Region values: " << std::endl;
    for (Segmentator::Node_t node : segmentator.regions) {
        std::cout << node.region_value << std::endl;
    }

    std::cout << "Parents values: " << std::endl;
    for (Segmentator::Node_t node : segmentator.region_parents) {
        std::cout << node.region_value << std::endl;
    }



    // Affichage de l'image originale et de l'image segmentée
    imshow("Original image", image);
    imshow("Segmented image", labels);
    imshow("Boundary", segmentator.boundary);
    waitKey(0);
    
    return 0;
}
