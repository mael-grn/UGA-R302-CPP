#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

template<class T>
void afficheVecteur(const vector<T> v) {
    // {v.size() > 0} => {pretty print de v}
    cout << "[" << v[0];
    int i = 1;
    while (i < v.size()) {
        cout << ", " << v[i];
        i++;
    }
    cout << "]" << endl;
}

template<class T>
void swap(vector<T> &v, const int first, const int second) {
    // {v.size() >0} => {permutation des valeurs de v aux indices first et second}

    /*
     * À COMPLETER
     */

}

template<class T>
T partage(vector<T> &v, const int inf, const int sup) {
// {v.size()≥ 1} => {résultat = valeur du médian tel que défini dans l’étape 1)
//                   et mise en place du plus petit et du plus grand dans v telles que définies dans l’étape 2)}
    T realInf = min(v.at(sup), min(v.at(inf), v.at((inf+sup)/2)));
    T realSup = max(v.at(sup), max(v.at(inf), v.at((inf+sup)/2)));
    T realMid;

    if (v.at(inf) > realInf && v.at(inf) < realSup) {
        realMid = v.at(inf);
    } else if (v.at((inf+sup)/2) > realInf && v.at((inf+sup)/2) < realSup) {
        realMid = v.at((inf+sup)/2);
    } else {
        realMid = v.at(sup);
    }
    cout << "inf " << inf << ":" << v.at(inf) << " devient " << realInf << " mid " << (inf+sup)/2 << ":" << v.at((inf+sup)/2) << " devient " << realMid << " sup " << sup << ":" << v.at(sup) << " devient " << realSup << endl;
    v.at(inf) = realInf;
    v.at((inf+sup)/2) = realMid;
    v.at(sup) = realSup;

    return realMid;

}

template<class T>
void insertionSort(vector<T> &a) {

    /*
     * À COMPLETER avec un tri trouvé sur le Web
     */

}

template<class T>
void triR302Worker(vector<T> &v, const int inf, const int sup) {
    if (inf < sup) {
        partage(v, inf, sup);
        triR302Worker(v, inf, sup-1);
    }
}

template<class T>
void triR302(vector<T> &v, const int inf, const int sup) {
    if (inf < sup && sup <= v.size()) {
        triR302Worker(v, inf, sup);
    }
}

void testTriR302() {
    cout << "***************************" << endl;
    cout << "*          triR302        *" << endl;
    cout << "***************************" << endl;
    vector<int> v = {10, 3, 6, 8, 67, 2, 9, 1};
    cout << "vecteur initial : ";
    afficheVecteur(v);
    triR302(v, 0, (int) v.size() - 1);
    cout << "vecteur trié : ";
    afficheVecteur(v);

    v.clear();
    srand((unsigned) time(NULL));
    for (int i = 0; i < 128; i++) {
        int b = rand() % 128 + 1;
        v.push_back(b);
    }
    cout << "vecteur initial : ";
    afficheVecteur(v);
    triR302(v, 0, (int) v.size() - 1);
    cout << "vecteur trié : ";
    afficheVecteur(v);
}

int main(int argc, char **argv) {
    testTriR302();
    return 0;
}