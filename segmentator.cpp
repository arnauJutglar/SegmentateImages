#include "segmentator.hpp"

int debug=0;

// Structure qui représente un noeud de l'arbre des régions
struct Segmentator::Node_t {
    Vec3d region_value; // La valeur qu'on va considerer pour valider la proprieté de similarité. De type double pour eviter des erreurs numériques.
    Vec3b label_color; // La couleur aleatoire qui sera utilisé comme label et qui sera vu lorse de l'affichage.
    int size; // Le nombre de pixels de la région
    vector<Node_t*> children; // Les enfants du noeud
    Node_t* parent; // Le parent du noeud
};

Segmentator::Segmentator(Mat& image_) : image(image_) {
    // Création de la matrice des labels, initialisée à zéro
    labels = Mat::zeros(image_.size(), CV_8UC3);
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
bool Segmentator::similar(Vec3b c1, Vec3b c2, int threshold) {
    /*if (debug % 10 == 0) {
        std::cout << c1 << " is similar to " << c2 << std::endl;
    }
    debug++;*/
    return norm(c1 - c2) <= threshold;
}

// Fonction qui réalise la croissance de région à partir d'un germe
void Segmentator::regionGrowing(Mat& image, Mat& labels, Point seed, int threshold) {
    // Initialisation de la file FIFO
    queue<Point> fifo;
    fifo.push(seed);

    // Initialisation du noeud de l'arbre correspondant à la région
    Node_t* node = new Node_t();
    node->region_value = image.at<Vec3b>(seed);
    node->label_color = randomColor();
    node->size = 1;
    node->parent = NULL;

    // Tant que la file n'est pas vide
    while (!fifo.empty()) {
        // On défile le premier point de la file
        Point p = fifo.front();
        fifo.pop();

        // On récupère la couleur du pixel correspondant
        Vec3b color = image.at<Vec3b>(p);

        // On parcourt les 8 voisins du point
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                // On vérifie que le voisin est dans les limites de l'image
                if (p.x + i >= 0 && p.x + i < image.cols && p.y + j >= 0 && p.y + j < image.rows) {
                    // On récupère le voisin et sa couleur
                    Point q(p.x + i, p.y + j);
                    Vec3b neighbor = image.at<Vec3b>(q);

                    // On vérifie que le voisin n'a pas déjà été affecté à une région
                    if (labels.at<Vec3b>(q) == 0) {
                        // On vérifie que le voisin est similaire au pixel courant
                        if (similar(node->region_value, neighbor, threshold)) {
                            // On affecte le voisin à la même région que le pixel courant
                            labels.at<Vec3b>(q) = node->label_color;

                            // On ajoute le voisin à la file
                            fifo.push(q);

                            // On met à jour le noeud de l'arbre
                            double aux;
                            for (int i=0; i < 3; i++) {
                                // On doit le faire channel à channel pour ne pas surpasser 255.
                                aux = node->region_value[i] * node->size + neighbor[i];
                                aux /= node->size + 1;
                                node->region_value[i] = aux;
                            }
                            if (debug % 10 == 0) {
                                std::cout << node->region_value << std::endl;
                            }
                            debug++;
                            node->size++;
                        }
                    }
                }
            }
        }
    }

    // On ajoute le noeud de l'arbre à la liste des régions
    regions.push_back(node);
}

// Fonction qui vérifie si deux régions sont adjacentes
bool Segmentator::areAdjacent(Mat& labels, Vec3b label1, Vec3b label2) {
    // On parcourt la matrice des labels
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            // On vérifie si le pixel appartient à la région 1
            if (labels.at<Vec3b>(i, j) == label1) {
                // On parcourt les 8 voisins du pixel
                for (int k = -1; k <= 1; k++) {
                    for (int l = -1; l <= 1; l++) {
                        // On vérifie que le voisin est dans les limites de l'image
                        if (i + k >= 0 && i + k < labels.rows && j + l >= 0 && j + l < labels.cols) {
                            // On vérifie si le voisin appartient à la région 2
                            if (labels.at<Vec3b>(i + k, j + l) == label2) {
                                // On renvoie vrai
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    // On renvoie faux
    return false;
}

// Fonction qui fusionne deux régions adjacentes selon un critère de distance
void Segmentator::regionMerging(Mat& image, Mat& labels, int minDist) {
    // On parcourt la liste des régions
    for (int i = 0; i < regions.size(); i++) {
        // On récupère le noeud de la région i
        Node_t* node_i = regions[i];
        // On parcourt les régions suivantes
        for (int j = i + 1; j < regions.size(); j++) {
            // On récupère le noeud de la région j
            Node_t* node_j = regions[j];
            // On vérifie que les régions sont adjacentes
            if (areAdjacent(labels, node_i->region_value, node_j->region_value)) {
                // On vérifie que la distance entre les couleurs des régions est inférieure au seuil
                if (norm(node_i->region_value - node_j->region_value) < minDist) {
                    // On fusionne les régions i et j
                    mergeRegions(labels, node_i, node_j);
                    // On supprime le noeud de la région j de la liste
                    regions.erase(regions.begin() + j);
                    // On décrémente l'indice j
                    j--;
                }
            }
        }
    }
}

// Fonction qui fusionne deux régions et met à jour l'arbre
void Segmentator::mergeRegions(Mat& labels, Node_t* node1, Node_t* node2) {
    // On crée un nouveau noeud qui sera le parent des deux régions
    imshow("labels", labels);
    waitKey(5);
    Node_t* node = new Node_t();
    node->region_value = (node1->region_value * node1->size + node2->region_value * node2->size) / (node1->size + node2->size); // On calcule la couleur moyenne des deux régions
    node->size = node1->size + node2->size; // On calcule la taille totale des deux régions
    node->children.push_back(node1); // On ajoute le noeud de la première région comme enfant
    node->children.push_back(node2); // On ajoute le noeud de la deuxième région comme enfant
    node->parent = NULL; // On met le parent à NULL
    node1->parent = node; // On met le parent de la première région à node
    node2->parent = node; // On met le parent de la deuxième région à node

    // On parcourt la matrice des labels
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            // On vérifie si le pixel appartient à la deuxième région
            if (labels.at<Vec3b>(i, j) == node2->label_color) {
                // On change le label du pixel pour celui du père
                labels.at<Vec3b>(i, j) = node->label_color;
            }
        }
    }
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
        }
    }
}

Mat Segmentator::segmentate() {
    // Pose des germes aléatoires dans l'image
    randomSeeds(image, seeds, numSeeds, minDist);

    // Application de la croissance de région pour chaque germe
    for (int i = 0; i < seeds.size(); i++) {
        regionGrowing(image, labels, seeds[i], threshold);
    }

    // Application de la fusion de région pour les régions adjacentes
    //regionMerging(image, labels, minDist);

    return labels;
}