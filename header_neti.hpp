#ifndef header_neti_h
#define header_neti_h


bool similar(Vec3b c1, Vec3b c2, int threshold);
void regionGrowing(Mat& image, Mat& labels, Point seed, int threshold);
bool areAdjacent(Mat& labels, Vec3b label1, Vec3b label2);
void regionMerging(Mat& image, Mat& labels, int minDist);
void mergeRegions(Mat& labels, Node_t* node1, Node_t* node2);
void randomSeeds(Mat& image, vector<Point>& seeds, int numSeeds, int minDist);
bool operator==(Vec3b c, int i);












#endif /* header_neti_h */