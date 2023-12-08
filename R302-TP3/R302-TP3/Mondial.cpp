/* 
 * File:   Mondial.cpp
 * Author: hb
 * 
 * Created on 22 novembre 2018, 16:05
 */

#include "Mondial.h"


#include <iostream>     // pour cout
#include <iomanip>      // pour setw()
#include <sstream>
#include <iterator>

Mondial::Mondial(const char* filename) {
    // Chargement du fichier XML en mémoire
    imageMondial.LoadFile(filename);
    // Initialisation de l'attribut racineMondial avec la racine (élément <mondial>)
    racineMondial = imageMondial.FirstChildElement();
}

void Mondial::Print() {
    imageMondial.Print();
}

/*
 * FOURNIE
 */
int Mondial::getNbAirports() const {
    // initialisation du nombre d’aéroports
    int nb = 0;
    // accéder à <airportscategory>, c’est un fils de l'élément <racineMondial>
    XMLElement* airportsCategory = racineMondial->FirstChildElement("airportscategory");
    // parcours complet des fils de <airportscategory> en les comptants
    // 1) accéder au premier fils <airport> de <airportscategory>
    XMLElement* currentAirport = airportsCategory->FirstChildElement();
    // 2) parcourir tous les <airport> qui sont des frères
    while (currentAirport != nullptr) {
        // un aéroport supplémentaire
        nb = nb + 1;
        // avancer au frère <airport> suivant de currentAirport
        currentAirport = currentAirport->NextSiblingElement();
    }
    // currentAirport n’a plus de frère {currentAirport == nullptr}, c’est le dernier
    return nb;
}

/*
 * FOURNIE
 */
void Mondial::printCountriesCode() const {
    int rank = 1; // rang du pays
    string carcodeValue; // valeur de l'attribut "car_cod" du pays courant
    // accéder à <countriescategory>, c’est un fils de l'élément <racineMondial>)
    XMLElement* countriesCategory = racineMondial->FirstChildElement("countriescategory");
    // parcours complet des fils de <countriescategory> en affichant le rang et le code
    // 1) accéder au premier fils <country> de <countriescategory>
    XMLElement* currentCountry = countriesCategory->FirstChildElement();
    // 2) parcourir tous les <country> qui sont des frères
    while (currentCountry != nullptr) {
        // traiter le pays courant
        //      1) récupérer la valeur de l’attribut "car_code"
        carcodeValue = currentCountry->Attribute("car_code");
        //      2) faire l’affichage
        cout << setw(5) << rank << " : " << carcodeValue << endl;
        // avancer au frère <country> suivant de currentCountry
        currentCountry = currentCountry->NextSiblingElement();
        // mettre à jour le rang
        rank = rank + 1;
    }
    // currentCountry n’a pas de frère {currentCountry == nullptr}, c’est fini
}


/*
 * A COMPLETER
 */
int Mondial::getNbDeserts() const {
    //decompte du nombre de desert
    int nb =0;

    XMLElement* desert = racineMondial->FirstChildElement("desertscategory");
    XMLElement* currentDesert = desert->FirstChildElement();

    //tant qu'il existe des desert, incrementer le compteur
    while (currentDesert != nullptr) {
        nb+=1;
        //selectionner le desert suivant
        currentDesert = currentDesert->NextSiblingElement();
    }
    return nb;
}

/*
 * A COMPLETER
 */
int Mondial::getNbElemCat(const string categoryName) {
    //decompte du nombre d'elements
    int nb =0;
    //decode_category permet de traduire le nom de la categorie. c_str permet de convertir le type string en tableau de caractere
    XMLElement* elemCat = racineMondial->FirstChildElement(decod_category.at(categoryName).c_str());
    XMLElement* currentElemCat = elemCat->FirstChildElement();

    //tant qu'il existe un element, incrementer le compteur
    while (currentElemCat != nullptr) {
        nb+=1;
        currentElemCat = currentElemCat->NextSiblingElement();
    }
    return nb;
}


XMLElement* Mondial::getCountryXmlelementFromNameRec(string countryName) const {
    //declaration du pointeur vers le resultat
    XMLElement* country;
    //recherche seulement si le string n'est pas vide
    if (countryName.empty()) {
        return nullptr;
    }
    //on recupere l'element (balise) "countriescategory". Celle contient tous les pays.
    XMLElement* countriesCategory = racineMondial->FirstChildElement("countriescategory");
    //utilisation du worker, a partir du premier enfant de la "balise" countries category. Celui ci correspond au premier pays de la liste de pays.
    country = getCountryXmlelementFromNameRecWorker(countriesCategory->FirstChildElement(), countryName);

    return country;
}


XMLElement* Mondial::getCountryXmlelementFromNameRecWorker(XMLElement* currentCountryElement, string countryName) const {
    //Premier point d'arret : ou trouve le pays (le premier element de la balise country correspond au nom du pays, donc on la recupere et on la compare)
    if (currentCountryElement->FirstChildElement("name")->GetText() == countryName) {
        //on retourne le pays trouvé si succes
        return currentCountryElement;
    //si le pays n'est pas trouvé, en renvoi nullptr
    } else if (currentCountryElement->NextSiblingElement() == nullptr) {
        return nullptr;
    } else {
        //boucle recursive, avec passage en parametre du prochain pays
        return getCountryXmlelementFromNameRecWorker(currentCountryElement->NextSiblingElement(), countryName);
    }

}

/*
 * A COMPLETER
 */
string Mondial::getCountryCodeFromName(string countryName) const throw (PrecondVioleeExcep) {
    //recuperer l'objet XMLElement correspondant au pays countryName
    XMLElement* country = getCountryXmlelementFromNameIter(countryName);

    //si le pays n'existe pas, throw une exception
    if (country == nullptr) {
        throw PrecondVioleeExcep("Le pays \"" + countryName + "\" n'existe pas...");
    }
    //retourne le code du pays
    return country->Attribute("car_code");

}

/*
 * A COMPLETER
 */
/**
 * élément <country> d'un pays identifié par son nom countryName
 * @param countryName
 * @return pointeur sur l'élément <country> dont la valeur du fils <name> est égal à countryName, nullprt sinon
 */
XMLElement* Mondial::getCountryXmlelementFromNameIter(string countryName) const {
    //on recupere l'element (balise) "countriescategory". Celle contient tous les pays.
    XMLElement* countriesCategory = racineMondial->FirstChildElement("countriescategory");
    //premier enfant de la "balise" countries category. Celui ci correspond au premier pays de la liste de pays.
    XMLElement* country = countriesCategory->FirstChildElement();
    //variable booleeene indiquant si le pays à été trouvé (boucle while)
    bool countryTrouve = false;
    //parcours de tous les pays. S'arrete si le pays est trouvé, auquel cas country contiendra le bon pays
    while (country != nullptr && !countryTrouve) {
        if (strcmp(country->FirstChildElement("name")->GetText(), countryName.c_str())==0) {
            countryTrouve = true;
        } else {
            country = country->NextSiblingElement();
        }
    }
    return country;
}

/*
 * A COMPLETER
 */
int Mondial::getCountryPopulationFromName(string countryName) const {
    //on recupere l'element xml correspondant au pays countryName
    XMLElement* country = getCountryXmlelementFromNameRec(countryName);

    //si le pays n'existe pas, on retourne -1
    if (country == nullptr) {
        return -1;
    } else {

        //on recupere le
        //On recupere les données contenues dans la balises du dernier element de nom population.
        //Les balises populations sont classé par ordre croissant, ainsi, la derniere balise étudiée sera forcement la plus recente.
        XMLElement* countryElem = country->LastChildElement("population");
        //initialisation de la variable contenant la population
        int pop = stoi(countryElem->GetText());

        return pop;
    }

}


XMLElement* Mondial::getCountryXmlelementFromCode(string countryCode) const {
    //on recupere l'element (balise) "countriescategory". Celle contient tous les pays.
    XMLElement* countriesCategory = racineMondial->FirstChildElement("countriescategory");
    //premier enfant de la "balise" countries category. Celui ci correspond au premier pays de la liste de pays.
    XMLElement* country = countriesCategory->FirstChildElement();
    //variable booleeene indiquant si le pays à été trouvé (boucle while)
    bool countryTrouve = false;
    //parcours de tous les pays. S'arrete si le pays est trouvé, auquel cas country contiendra le bon pays
    while (country != nullptr && !countryTrouve) {
        if (strcmp(country->Attribute("car_code"), countryCode.c_str())==0) {
            countryTrouve = true;
        } else {
            country = country->NextSiblingElement();
        }
    }
    return country;
}


void Mondial::printCountryBorders(string countryName) const {

    //On recupere le pays de nom countryName
    XMLElement* country = getCountryXmlelementFromNameRec(countryName);

    //si le pays n'existe pas, affichage d'un message approprié
    if (country == nullptr) {
        cout << "LE PAYS " + countryName + " EXISTE PAS !" << endl;

    //si le pays existe
    } else {
        //liste, qui contiendra tous les pays frontaliers
        vector<string> borderList;
        //on se deplace directement au premier element de nom "border"
        XMLElement* countryElem = country->FirstChildElement("border");

        //si le pays possede des frontiere :
        if (countryElem != nullptr && strcmp(countryElem->Name(), "border")==0) {
            //afficher un message appriprié
            cout << "Les pays frontaliers de " + countryName + " sont :" << endl;
            //afficher tous les pays frontaliers
            while (countryElem != nullptr && strcmp(countryElem->Name(), "border")==0) {
                //on recupere le pays, ayant pour code le code contenu dans l'element border
                XMLElement* countryBorder = getCountryXmlelementFromCode(countryElem->Attribute("country"));

                //si le pays existe, on ajoute son nom a la liste
                if (countryBorder != nullptr) {
                    cout << "\t- " << countryBorder->FirstChildElement("name")->GetText() << ", length : " << countryElem->Attribute("length") << " km" << endl;
                }
                //passage à l'element suivant
                countryElem = countryElem->NextSiblingElement();
            }
        //si le pays n'a pas de pays frontalier
        } else {
            //affichage d'un message approprié
            cout << "Le pays " + countryName + " ne possede aucun pays frontaliers...";
        }
    }
}

/*
 * A COMPLETER
 */
XMLElement* Mondial::getRiverXmlelementFromNameIter(string riverName) const {
    //on recupere l'element (balise) "riverscategory". Celle contient tous les pays.
    XMLElement* riversCategory = racineMondial->FirstChildElement("riverscategory");
    //premier enfant de la "balise" countries category. Celui ci correspond au premier pays de la liste de pays.
    XMLElement* river = riversCategory->FirstChildElement();
    //variable booleeene indiquant si le pays à été trouvé (boucle while)
    bool countryTrouve = false;
    //parcours de tous les pays. S'arrete si le pays est trouvé, auquel cas country contiendra le bon pays
    while (river != nullptr && !countryTrouve) {
        if (strcmp(river->FirstChildElement("name")->GetText(), riverName.c_str())==0) {
            countryTrouve = true;
        } else {
            river = river->NextSiblingElement();
        }
    }
    return river;
}

void Mondial::printAllCountriesCrossedByRiver(string riverName) const {
    //recuperer la riviere de nom countryname
    XMLElement* river = getRiverXmlelementFromNameIter(riverName);
    //si la riviere est trouvée (nous partons du principe qu'elle traverse forcement des pays)
    if (river != nullptr) {
        //affichage d'un message approprié
        cout << "Pays traversés par : " << riverName << endl;
        string pays = river->Attribute("country");

        //traiter la liste de pays comme un flux d'entrée
        istringstream paysFlow(pays);
        //variable qui va stocker chaque mot du flux paysFlow, qui correspond au code de chaque pays
        string paysName;
        //extraction de chaque code de pays du flux pour le traiter
        while (paysFlow >> paysName) {
            //on recupere l'element XML du pays ayant pour code paysname
            XMLElement* paysElem = getCountryXmlelementFromCode(paysName);
            //affichage du pays
            cout << "\t- " << paysElem->FirstChildElement("name")->GetText() << endl;
        }
        //affichage de la longueur du fleuve:
        cout << "sa longueur est de " << river->FirstChildElement("length")->GetText() << " km" << endl;
    //affichage du message si le fleuve n'est pas trouvé
    } else {
        cout << "le fleuve " << riverName << " existe pas !!" << endl;
    }
}


void Mondial::printCountriesWithProvincesCrossedByRiver(string riverName) const {
    //recuperer la riviere de nom countryname
    XMLElement* river = getRiverXmlelementFromNameIter(riverName);
    //si la riviere est trouvée
    if (river != nullptr) {
        //affichage d'un message approprié
        cout << "Pays et provinces traversés par : " << riverName << endl;
        //on recupere le premier fils de river ayant pour nom located
        XMLElement* riverElem = river->FirstChildElement("located");
        //si cet element existe, et que son nom est "located", affichage du pays
        while (riverElem != nullptr && strcmp(riverElem->Name(), "located")==0) {
            //affichage du pays correspondant au code pays contenu dans l'element located
            //pour cela, on recupere le pays correspondant à ce code, et on affiche le texte contenu dans son premier element, qui corresponds à son nom
            cout << "\t- " << getCountryXmlelementFromCode(riverElem->Attribute("country"))->FirstChildElement("name")->GetText() << endl;
            //passage a l'element suivant
            riverElem = riverElem->NextSiblingElement();
        }
        //affichage de la longueur du fleuve:
        cout << "sa longueur est de " << river->FirstChildElement("length")->GetText() << " km" << endl;
        //affichage du message si le fleuve n'est pas trouvé
    } else {
        cout << "le fleuve " << riverName << " existe pas !!" << endl;
    }
}

/*
 * A COMPLETER
 */
void Mondial::printCountriesAndProvincesCrossedByRiver(string riverName) const {
    //recuperer la riviere de nom countryname
    XMLElement* river = getRiverXmlelementFromNameIter(riverName);
    //si la riviere est trouvée
    if (river != nullptr) {



        //affichage d'un message approprié
        cout << "Pays et provinces traversés par : " << riverName << endl;
        //on recupere le premier fils de river ayant pour nom located
        XMLElement* riverElem = river->FirstChildElement("located");

        //si cet element existe, et que son nom est "located", affichage du pays
        while (riverElem != nullptr && strcmp(riverElem->Name(), "located")==0) {

            //On recupere une liste de toutes les provinces traversées :
            vector<string> provLst;
            //traiter la liste de provinces comme un flux d'entrée
            istringstream provFlow(riverElem->Attribute("province"));
            //variable qui va stocker chaque mot du flux provLst, qui correspond au code de chaque province
            string provName;
            //extraction de chaque code de pays du flux pour l'ajouter a la liste
            while (provFlow >> provName) {
                provLst.push_back(provName);
            }

            //on recupere l'element XML du pays traversé
            XMLElement* countryCrossed = getCountryXmlelementFromCode(riverElem->Attribute("country"));
            //affichage du pays
            cout << "\t- " << countryCrossed->FirstChildElement("name")->GetText() << endl;

            //Affichage des provinces traversées.
            //Pour cela, parcours de toutes les provinces du pays courant. Pour chacunes de ces provinces, on parcours la liste
            //des provinces de la riviere (provLst) et on regarde si il y a correspondance.
            //element enfant du pays courant, a partir de l'element province
            XMLElement* countryElem = countryCrossed->FirstChildElement("province");
            //parcours des provinces du pays courant
            while (countryElem != nullptr && strcmp(countryElem->Name(), "province")==0) {
                //parcours des provinces de la riviere
                for (auto provCode : provLst) {
                    //si les deux correspondent, alors afficher la province
                    if (strcmp(provCode.c_str(), countryElem->Attribute("id")) == 0) {
                        cout << "\t\t* " << countryElem->FirstChildElement("name")->GetText() << endl;
                    }
                }
                //passage a l'element suivant du pays
                countryElem = countryElem->NextSiblingElement();
            }

            //passage a l'element suivant de la riviere
            riverElem = riverElem->NextSiblingElement();
        }
        //affichage de la longueur du fleuve:
        cout << "sa longueur est de " << river->FirstChildElement("length")->GetText() << " km" << endl;
        //affichage du message si le fleuve n'est pas trouvé
    } else {
        cout << "le fleuve " << riverName << " existe pas !!" << endl;
    }
}

/*
 * A COMPLETER
 */
void Mondial::printCityInformation(string cityName) const {
    /*
     * A COMPLETER
     */
}

/**
 * Exemple de question additionnelle pour l'exercice 9 afficher toutes les informations disponibles
 * dans Mondial concernant toutes les îles.
 * On peut commencer par une île en particulier à partir de son nom
 */
void Mondial::printIslandsInformations() const {
    /*
     * A COMPLETER
     */
}

/*
 * Méthodes de service fournies
 */

template<typename Out>
void Mondial::split(string& s, char delim, Out result) const {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        *(result++) = item;
    }
}

/**
 * Méthode à utiliser pour découper les mots d'une chaîne dans un vecteur
 * Utile pour la gestion des attributs à valeurs multiples
 * @param s chaîne à découper
 * @param delim délimiteur des mots de la chaîne à découper
 * @return vecteur contenant les mots de la chaîne découpée
 */
vector<std::string> Mondial::split(string& s, char delim) const {
    vector<std::string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}

Mondial::~Mondial() {
}