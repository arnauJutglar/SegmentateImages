#ifndef segmentator_h
#define segmentator_h

#include <opencv2/opencv.hpp>
#include <queue>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>
#include <chrono>

#define MAX_ITER 1000

using namespace cv;
using namespace std;

class Segmentator {
private:
    struct color_comp {
        bool operator()(const Vec3b& c1, const Vec3b& c2) const {
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
    };
    struct Node_t {
        Vec3d region_value; // La valeur qu'on va considerer pour valider la proprieté de similarité. De type double pour eviter des erreurs numériques.
        Vec3b label_color; // La couleur aleatoire qui sera utilisé comme label et qui sera vu lorse de l'affichage.
        int size; // Le nombre de pixels de la région
        vector<Node_t*> children; // Les enfants du noeud
        Node_t* parent; // Le parent du noeud
        vector<Point> members;
        vector<Point> boundary;
        set<Vec3b, color_comp> adjacents;
    };


    vector<Node_t> regions; // Contient la liste des régions sous forme d'arbre.
    list<Node_t> region_parents;
    vector<Point> seeds; // Vecteur de germes.
    int numSeeds = 8; // Numero de germes.
    int minDist = 20; // Distance minimale entre eux.
    float threshold = 50; // Seuil de similarité
    Mat labels;
    Mat unmerged_labels;
    Mat boundary;
    Mat& image;
    function<bool(Vec3d,Vec3d)> similarityFunction = [this] (Vec3d c1, Vec3d c2) -> bool {
        return norm(c1 - c2) < threshold;
    };

    bool similar(Vec3d c1, Vec3d c2);
    void regionGrowing(Point seed);
    void computeBoundary(Node_t& region);
    void computeBoundary(Node_t& child, Node_t& parent);
    bool areAdjacent(Node_t region1, Node_t region2);
    bool areSimilar(Node_t region1, Node_t region2);
    bool isBoundary(const Node_t& region1, Point p);
    bool insideImage(Point p);
    void regionMerging(Mat& image, Mat& labels, int minDist);
    Node_t* mergeRegions(Node_t* node1, Node_t* node2);
    void colorAssignment();
    void colorAssignment(Node_t node, Vec3b label_color);
    void randomSeeds(Mat& image, vector<Point>& seeds, int numSeeds, int minDist);
    Vec3b randomColor();

public:
    Segmentator(Mat& image);
    Mat segmentate();
    Mat computeBoundary();
    Mat getLabels() { return labels; }
    Mat getUnmergedLabels() { return unmerged_labels; }
    Mat getBoundary() { return boundary; }
    int setNumSeeds(int _numSeeds);
    int setMinDist(int _minDist);
    int setThreshold(float _threshold);
    void setSimilarityFunction(function<bool(Vec3d,Vec3d)> f);
    friend bool operator==(const Segmentator::Node_t& node1, const Segmentator::Node_t& node2);
};

bool operator==(const Segmentator::Node_t& node1, const Segmentator::Node_t& node2);
bool operator<(const Vec3b& c1, const Vec3b& c2);
bool color_comparator(const Vec3b& c1, const Vec3b& c2);









#endif /* segmentator_h */