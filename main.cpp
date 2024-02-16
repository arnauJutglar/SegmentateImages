#include <opencv2/opencv.hpp>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace cv;
using namespace std;

#include "segmentator.hpp"

int main() {
    // Des lambda fonctions qui peuvent être utilisées comme parametre pour la similarityFunction.
    auto brightness = [](Vec3b c1, Vec3b c2) -> bool {
        return abs(c1[0] + c1[1] + c1[2] - (c2[0] + c2[1] + c2[2])) < 50;
    };
    auto average_color = [](Vec3b c1, Vec3b c2) -> bool {
        return norm(c1 - c2) < 50;
    };

    // Lecture de l'image
    Mat image = imread("lena.jpg", IMREAD_COLOR);
   
    // Initialisation et configuration du Segmentator.
    Segmentator segmentator(image);
    segmentator.setSimilarityFunction([](Vec3b c1, Vec3b c2) -> bool {
        return norm(c1 - c2) < 50;
    });
    segmentator.setNumSeeds(32);

    // Démarrage de la segmentation. 
    // Pour calculer les frontières des régions fusionnées, decommenter la deuxième ligne.
    Mat labels = segmentator.segmentate();
    // segmentator.computeBoundary();

    // Affichage des images.
    imshow("Original image", image);
    imshow("Segmented image", labels);
    imshow("Unmerged segmented image", segmentator.getUnmergedLabels());
    imshow("Boundary", segmentator.getBoundary());
    waitKey(0);

    return 0;
}