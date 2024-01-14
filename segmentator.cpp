#include "segmentator.hpp"

mutex mutex_object;


int callings = 0;


bool operator==(const Segmentator::Node_t& node1, const Segmentator::Node_t& node2) {
    if (node1.label_color == node2.label_color) {
        return true;
    }
    return false;
}

bool operator==(const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

/*
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
*/

Segmentator::Segmentator(Mat& image_) : image(image_) {
    // Création de la matrice des labels, initialisée à zéro
    labels = Mat::zeros(image_.size(), CV_8UC3);
    boundary = Mat::zeros(image_.size(), CV_8UC3);
}


// Fonction utilisée pour tester si un pixel est affecté ou pas.
bool operator==(Vec3b c, int i) {
    return c[0] == i && c[1] == i && c[2] == i;
}

Vec3b Segmentator::randomColor() {
    Vec3b color;
    do {
        color = Vec3b(rand() % 256, rand() % 256, rand() % 256);
    } while (color == 0);

    return color;
}

// Fonction qui renvoie vrai si deux couleurs sont similaires selon un seuil
bool Segmentator::similar(Vec3d c1, Vec3d c2, int threshold) {
    return similarityFunction(c1, c2);
}

// Fonction qui réalise la croissance de région à partir d'un germe
void Segmentator::regionGrowing(Point seed) {
    std::cout << "regionGrowing" << std::endl;
    // Initialisation de la file FIFO
    queue<Point> fifo;
    fifo.push(seed);

    // Initialisation du noeud de l'arbre correspondant à la région
    Node_t node;
    node.region_value = image.at<Vec3b>(seed);
    node.label_color = randomColor();
    node.size = 1;
    node.parent = NULL;
    node.members.push_back(seed);

    // Tant que la file n'est pas vide
    while (!fifo.empty()) {
        // On défile le premier point de la file
        Point p = fifo.front();
        fifo.pop();

        // On parcourt les 8 voisins du point
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                // On vérifie que le voisin est dans les limites de l'image
                Point q(p.x + i, p.y + j);
                if (insideImage(q)) {
                    // On récupère la couleur du voisin
                    Vec3b neighbor = image.at<Vec3b>(q);

                    // On vérifie que le voisin n'a pas déjà été affecté à une région
                    if (labels.at<Vec3b>(q) == 0) {
                        // On vérifie que le voisin est similaire au pixel courant
                        if (similar(node.region_value, neighbor, threshold)) {
                            // On affecte le voisin à la même région que le pixel courant
                            labels.at<Vec3b>(q) = node.label_color;
                            node.members.push_back(q);
                            if (node.members.size() > node.members.max_size() - 100) {
                                std::cout << "Exceeded size." << std::endl;
                            }
                            
                            // On ajoute le voisin à la file
                            fifo.push(q);

                            // On met à jour le noeud de l'arbre
                            double aux;
                            for (int i=0; i < 3; i++) {
                                // On doit le faire channel à channel pour ne pas surpasser 255.
                                aux = node.region_value[i] * node.size + neighbor[i];
                                aux /= node.size + 1;
                                node.region_value[i] = aux;
                            }
                            node.size++;
                        }
                    } else {
                        node.adjacents.insert(labels.at<Vec3b>(q));
                    }
                }
            }
        }
    }

    // On ajoute le noeud de l'arbre à la liste des régions
    mutex_object.lock();
    region_parents.push_back(node);
    regions.push_back(node);
    std::cout << "regions size = " << regions.size() << std::endl;
    mutex_object.unlock();
    std::cout << node.adjacents.size() << std::endl;
}

void Segmentator::computeBoundary(Node_t& region) {
    std::cout << "computeBoundary()" << std::endl;
    // Get a point from the region.
    Point p = region.members.back();

    while (not isBoundary(region, p)) {
        p = Point(p.x + 1, p.y);
    }

    list<Point> pending;
    pending.push_back(p);

    while (not pending.empty()) {
        Point q = pending.back();
        pending.pop_back();
        region.boundary.push_back(q);

        for (int j = -1; j <= 1; j++) {
            for (int i = -1; i <= 1; i++) {
                if (j != 0 || i != 0) {
                    Point qq(q.x + j, q.y + i);
                    if (insideImage(qq)) {
                        if (isBoundary(region, qq)) {
                            if (find(region.boundary.begin(), region.boundary.end(), qq) == region.boundary.end()) {
                                region.boundary.push_back(qq);
                                if (similar(region.region_value, image.at<Vec3b>(qq), threshold)) {
                                    pending.push_back(qq);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return;
}

// Fonction qui vérifie si deux régions sont adjacentes
bool Segmentator::areAdjacent(Node_t region1, Node_t region2) {
    std::cout << "areAdjacent()" << std::endl;

    /*
    for (Point p : region1.boundary) {
        // On parcourt les 8 voisins du pixel
        for (int k = -1; k <= 1; k++) {
            for (int l = -1; l <= 1; l++) {
                // On vérifie que le voisin est dans les limites de l'image
                if (p.x + k >= 0 && p.x + k < labels.rows && p.y + l >= 0 && p.y + l < labels.cols) {
                    // On vérifie si le voisin appartient à la région 2
                    Point q(p.x + k, p.y + l);
                    if (find(region2.boundary.begin(), region2.boundary.end(), q) != region2.boundary.end()) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
    */
    for (Vec3b label : region1.adjacents) {
        if (find(region2.adjacents.begin(), region2.adjacents.end(), label) != region2.adjacents.end()) {
            return true;
        }
    }

    for (Vec3b label : region2.adjacents) {
        if (find(region1.adjacents.begin(), region1.adjacents.end(), label) != region1.adjacents.end()) {
            return true;
        }
    }


    return false;
}

bool Segmentator::areSimilar(Node_t region1, Node_t region2) {
    std::cout << "areSimilar()" << std::endl;
    bool b = similar(region1.region_value, region2.region_value, 2*threshold);

    if (!b) {
        std::cout << region1.region_value << " and " << region2.region_value << " were considered different." << std::endl;
    } else {
        std::cout << region1.region_value << " and " << region2.region_value << " were considered equal." << std::endl;
    }

    return b;
}

bool Segmentator::isBoundary(const Node_t& region, Point p) {
    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            if (j != 0 || i != 0) {
                Point q(p.x + j, p.y + i);

                // Check if inside the image:
                if (insideImage(q)) {
                    // Check if it is from another region:
                    if (not similar(image.at<Vec3b>(q), region.region_value, threshold)) {
                        return true;
                    }
                } else {
                    // Then, the point is in the boundary of the image.
                    return true;
                }
            }
        }
    }
    // Then, all neighbors are similar and hence it is not in the boundary.
    return false;
}

bool Segmentator::insideImage(Point p) {
    return (0 <= p.x) && (p.x < image.cols) && (0 <= p.y) && (p.y < image.rows);
}


// Fonction qui fusionne deux régions adjacentes selon un critère de distance
void Segmentator::regionMerging(Mat& image, Mat& labels, int minDist) {
    std::cout << "regionMerging()" << std::endl;

    int iter=0;

    list<Node_t*> remaining;
    for (Node_t& node : regions) {
        remaining.push_back(&node);
        std::cout << "adress = " << &node << std::endl;
    }
    while (remaining.size() > 1) {
        std::cout << remaining.size() << std::endl;
        Node_t* node1 = remaining.back();
        remaining.remove(node1);
        std::cout << remaining.size() << std::endl;
        list<Node_t*> aux_remaining(remaining);

        for (Node_t* node2 : aux_remaining) {
            std::cout << "iter = " << ++iter << std::endl;
            if (areSimilar(*node1, *node2) && areAdjacent(*node1, *node2)) {
                Node_t* parent = mergeRegions(node1, node2);
                remaining.remove(node2);
                remaining.push_back(parent);
                std::cout << "a removal here" << std::endl;
                break;
            }
        }
        std::cout << remaining.size() << std::endl;
    }
}

// Fonction qui fusionne deux régions et met à jour l'arbre
Segmentator::Node_t* Segmentator::mergeRegions(Node_t* node1, Node_t* node2) {
    std::cout << "mergeRegions()" << std::endl;

    // On crée un nouveau noeud qui sera le parent des deux régions
    Node_t* node = new Node_t;
    node->region_value = (node1->region_value * node1->size + node2->region_value * node2->size) / (node1->size + node2->size); // On calcule la couleur moyenne des deux régions
    node->label_color = node1->label_color;
    node->size = node1->size + node2->size; // On calcule la taille totale des deux régions
    node->children.push_back(node1); // On ajoute le noeud de la première région comme enfant
    node->children.push_back(node2); // On ajoute le noeud de la deuxième région comme enfant
    node->parent = NULL; // On met le parent à NULL
    node1->parent = node; // On met le parent de la première région à node
    node2->parent = node; // On met le parent de la deuxième région à node

    for (Vec3b label : (*node1).adjacents) {
        node->adjacents.insert(label);
    }

    for (Vec3b label : (*node2).adjacents) {
        node->adjacents.insert(label);
    }


    /*
    for (Point p : node1.boundary) {
        node->boundary.push_back(p);
    }
    for (Point p : node2.boundary) {
        node->boundary.push_back(p);
    }
    */
    std::cout << (*node1).region_value << (*node2).region_value << " should not appear in parents." << std::endl;
    region_parents.remove(*node1);
    region_parents.remove(*node2);
    region_parents.push_back(*node);

    return node;
}

void Segmentator::colorAssignment() {
    for (Node_t parent : region_parents) {
        std::cout << "colorAssignment()" << std::endl;
        colorAssignment(parent, parent.label_color);
    }
}

void Segmentator::colorAssignment(Node_t node, Vec3b label_color) {
    //std::cout << "colorAssignment()" << std::endl;
    if (node.children.size() == 0) {
        for (Point p : node.members) {
            boundary.at<Vec3b>(p) = label_color;
        }

        return;
    }

    for (Node_t* children : node.children) {
        colorAssignment(*children, label_color);
    }

    return;
}

// Fonction qui pose plusieurs germes aléatoires dans une image
void Segmentator::randomSeeds(Mat& image, vector<Point>& seeds, int numSeeds, int minDist) {
    // Initialisation du générateur aléatoire
    srand(time(NULL));

    // Tant qu'on n'a pas le nombre de germes souhaité
    while (seeds.size() < numSeeds) {
        // On génère un point aléatoire dans l'image
        Point p(rand() % image.cols, rand() % image.rows);

        // On vérifie qu'il n'est pas trop proche d'un autre germe
        bool valid = true;
        for (Point q : seeds) {
            if (norm(p - q) < minDist) {
                valid = false;
                break;
            }
        }

        // Si le point est valide, on l'ajoute au vecteur des germes
        if (valid) {
            seeds.push_back(p);
            std::cout << "Seed: " << p << std::endl;
        }
    }
}

Mat Segmentator::segmentate() {
    // Pose des germes aléatoires dans l'image
    randomSeeds(image, seeds, numSeeds, minDist);
    thread* threads = new thread[numSeeds];

    // Application de la croissance de région pour chaque germe
    for (int i = 0; i < numSeeds; i++) {
        threads[i] = thread(
            [this](Point seed) -> void {
                regionGrowing(seed);
                return;
            }, seeds[i]);
    }

    for (int i=0; i < numSeeds; i++) {
        threads[i].join();
    }

    /*for (int i=0; i < numSeeds; i++) {
        regionGrowing(seeds[i]);
    }*/

    std::cout << "regions size = " << regions.size() << " vs " << numSeeds << std::endl;
    
    /*for (int i=0; i < numSeeds; i++) {
        threads[i] = thread(
            [this](Node_t& region) -> void {
                computeBoundary(region);
                return;
            }, ref(regions[i]));
    }

    for (int i=0; i < numSeeds; i++) {
        threads[i].join();
    }*/

    
    // Application de la fusion de région pour les régions adjacentes
    regionMerging(image, labels, minDist);

    for (Node_t region : region_parents) {
        std::cout << "Members size: " << region.members.size() << " " << std::endl;
        for (Node_t* children : region.children) {
            std::cout << "Members size: " << (*children).members.size() << " " << std::endl;
        }
    }

    colorAssignment();

    /*for (Node_t region : regions) {
        for (Point p : region.boundary) {
            boundary.at<Vec3b>(p) = region.label_color;
        }
    }*/

    delete[] threads;

    return labels;
}

void print_graph(list<Segmentator::Node_t> nodes) {
    for (Segmentator::Node_t node : nodes) {
        print_graph(node);
    }
}

void print_graph(Segmentator::Node_t node) {
    if (node.children.size() != 0) {
        std::cout << "There is a parent" << std::endl;
        for (Segmentator::Node_t* child : node.children) {
            print_graph(*child);
        }
    } else {
        std::cout << "Members size: " << node.members.size() << std::endl;
        if (node.members.size() == 0) {
            int parents = 0;
            Segmentator::Node_t* aux = node.parent;
            while (aux != nullptr) {
                aux = aux->parent;
                parents++;
            }
            std::cout << "n parents = " << parents << std::endl;
        }
    }
}