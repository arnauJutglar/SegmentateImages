#ifndef segmentator_h
#define segmentator_h

#include <opencv2/opencv.hpp>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace cv;
using namespace std;

class Segmentator {
private:
    struct Node_t;
    vector<Node_t*> regions; // Contient la liste des régions sous forme d'arbre.
    vector<Point> seeds; // Vecteur de germes.
    int numSeeds = 15; // Numero de germes.
    int minDist = 20; // Distance minimale entre eux.
    int threshold = 50; // Seuil de similarité
    Mat labels;
    Mat& image;


    bool similar(Vec3b c1, Vec3b c2, int threshold);
    void regionGrowing(Mat& image, Mat& labels, Point seed, int threshold);
    bool areAdjacent(Mat& labels, Vec3b label1, Vec3b label2);
    void regionMerging(Mat& image, Mat& labels, int minDist);
    void mergeRegions(Mat& labels, Node_t* node1, Node_t* node2);
    void randomSeeds(Mat& image, vector<Point>& seeds, int numSeeds, int minDist);
    Vec3b randomColor();

public:
    Segmentator(Mat& image);
    Mat segmentate();
};















#endif /* segmentator_h */