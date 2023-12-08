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


/**
 *
 * @return le nombre d'element appartenant à la catégorie desert
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

/**
 * @param categoryName le nom de la catégorie dont on cherche des informations
 * @return le nombre d'elements par catéories
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

/**
 * boucle récursive, permettant de récuperer l'element XML pays ayant pour nom countryName
 * @param countryName le nom du pays que l'ont recherche
 * @return un pointeur sur un element XML, correspondant au pays recherché.
 */
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

/**
 * @param countryName le nom du pays dont on cherche les informations
 * @return le code du pays, dont le nom a été passé en parametre.
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

/**
 * élément <country> d'un pays identifié par son nom countryName
 * @param countryName
 * @return pointeur sur l'élément <country> dont la valeur du fils <name> est égal à countryName, nullprt sinon
 */
XMLElement* Mondial::getCountryXmlelementFromNameIter(string countryName) const {
    //on recupere l'element (balise) "countriescategory". Celle contient tous les pays.
    XMLElement* countriesCategory = racineMondial->FirstChildElement("countriescategory");
    //premier enfant de la "balise" countries category. Celui ci correspond au premier pays de la liste de pays.
    return getXmlelementFromName(countriesCategory, countryName);
}

/**
 *
 * @param countryName le nom du pays dont on veux afficher des informations
 * @return la dernière donnée concernant la population d'un pays. retourne -1 si le pays ne dispose d'aucune donnée.
 */
int Mondial::getCountryPopulationFromName(string countryName) const {
    //on recupere l'element xml correspondant au pays countryName
    XMLElement* country = getCountryXmlelementFromNameRec(countryName);
    //si le pays n'existe pas, on retourne -1
    if (country == nullptr) {
        return -1;
    } else {
        //On recupere les données contenues dans la balises du dernier element de nom population.
        //Les balises populations sont classé par ordre croissant, ainsi, la derniere balise étudiée sera forcement la plus recente.
        XMLElement* countryElem = country->LastChildElement("population");
        //initialisation de la variable contenant la population
        int pop = stoi(countryElem->GetText());
        return pop;
    }
}

/**
 * @param countryCode le code d'un pays dont on veux retrouver l'element XML
 * @return l'élément XML dont le code correspond à celui en paramètre.
 */
XMLElement* Mondial::getCountryXmlelementFromCode(string countryCode) const {
    //on recupere l'element (balise) "countriescategory". Celle contient tous les pays.
    XMLElement* countriesCategory = racineMondial->FirstChildElement("countriescategory");
    return getXmlelementFromAttribute(countriesCategory, "car_code", countryCode);
}

/**
 * affichage des frontière d'un pays
 * @param countryName le nom du pays dont on veux afficher les frontières
 */
void Mondial::printCountryBorders(string countryName) const {
    //On recupere le pays de nom countryName
    XMLElement* country = getCountryXmlelementFromNameIter(countryName);
    //si le pays n'existe pas, affichage d'un message approprié
    if (country == nullptr) {
        cout << "LE PAYS " + countryName + " EXISTE PAS !" << endl;
    //si le pays existe
    } else {
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

/**
 *
 * @param riverName le nom de la rivière, dont on veux retrouver l'element XML.
 * @return l'élément XML dont le nom correspond à celui de la rivière.
 */
XMLElement* Mondial::getRiverXmlelementFromNameIter(string riverName) const {
    //on recupere l'element (balise) "riverscategory". Celle contient tous les pays.
    XMLElement* riversCategory = racineMondial->FirstChildElement("riverscategory");
    return getXmlelementFromName(riversCategory, riverName);
}

/**
 * afficher le nom des pays traversés par une rivière. Utilisation de l'attribut country.
 * @param riverName le nom de la rivière dont on veux afficher les informations
 */
void Mondial::printAllCountriesCrossedByRiver(string riverName) const {
    //recuperer la riviere de nom riverName
    XMLElement* river = getRiverXmlelementFromNameIter(riverName);
    //si la riviere est trouvée
    if (river != nullptr) {
        //affichage d'un message approprié
        cout << "Pays traversés par : " << riverName << endl;
        //recuperer les codes de pays
        string pays = river->Attribute("country");
        vector<string> paysLst;
        split(pays, ' ', back_inserter(paysLst));
        //affichage des pays
        for (auto elem : paysLst) {
            cout << "\t- " << getCountryXmlelementFromCode(elem)->FirstChildElement("name")->GetText() << endl;
        }
        //affichage de la longueur du fleuve:
        cout << "sa longueur est de " << river->FirstChildElement("length")->GetText() << " km" << endl;
    //affichage du message si le fleuve n'est pas trouvé
    } else {
        cout << "le fleuve " << riverName << " existe pas !!" << endl;
    }
}

/**
 * afficher le nom des pays traversés par une rivière. Utilisation des balises located.
 * @param riverName le nom de la rivière dont on veux afficher les informations
 */
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

/**
 * Permet d'afficher tout les pays traversés par une riviere, ainsi que les provinces des pays traversées par la rivière
 * @param riverName le nom de la rivière dont on veux afficher les informations
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
            //recuperer les provinces traversées
            string provinces = riverElem->Attribute("province");
            vector<string> provLst;
            split(provinces, ' ', back_inserter(provLst));
            //on recupere l'element XML du pays traversé
            XMLElement* countryCrossed = getCountryXmlelementFromCode(riverElem->Attribute("country"));
            //affichage du pays
            cout << "\t- " << countryCrossed->FirstChildElement("name")->GetText() << endl;
            //affichage des provinces
            printProvinceFromCountryInProvinceList(countryCrossed, provLst);
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
 * permet de retrouver une ville parmis tous les pays
 * @param cityName le nom de la ville recherchée
 * @return un pointeur vers un élément XML de la ville
 */
XMLElement *Mondial::getCityXmlelementFromName(std::string cityName) const {
    //variable pour arreter la boucle lorsque l'element est trouve
    bool elementTrouve = false;
    //on recupere l'element (balise) "countriescategory". Celle contient tous les pays.
    XMLElement* countriesCategory = racineMondial->FirstChildElement("countriescategory");
    XMLElement* country = countriesCategory->FirstChildElement();
    //parcours des pays, tant qu'on a pas trouvé ce qu'on cherche
    while (country != nullptr && !elementTrouve) {

    }
}

/**
 * parcours de tous les enfant de XMLLevel
 * @param xmlLevel l'element XML dont on veux parcourir les enfants
 * @param name le nom de l'element que l'ont recherche
 * @return  renvoie l'element XML ayant pour nom name, nullptr sinon
 */
XMLElement *Mondial::getXmlelementFromName(tinyxml2::XMLElement *xmlLevel, std::string name) const {
    //variable pour arreter la boucle lorsque l'element est trouve
    bool elementTrouve = false;
    //premier element enfant de l'element XMl en parametre
    XMLElement* child = xmlLevel->FirstChildElement();
    //parcours des elements enfant, jusqu'a trouver ce qu'on cherche
    while (child != nullptr && !elementTrouve) {
        //si l'element est trouvé, on arrete la boucle
        if (child->FirstChildElement("name")->GetText() == name) {
            elementTrouve = true;
        } else {
            //sinon on continue
            child = child->NextSiblingElement();
        }
    }
    return child;
}

/**
 * @param xmlLevel l'element xml dont on veut parcourir les enfants
 * @param attribute l'attribut concerné par la recherche
 * @param id la valeur de l'attribut que l'ont recherche
 * @return le premier element XML dont attribut correspond a la valeur id
 */
XMLElement *Mondial::getXmlelementFromAttribute(tinyxml2::XMLElement *xmlLevel, std::string attribute, string id) const {
    //variable pour arreter la boucle lorsque l'element est trouve
    bool elementTrouve = false;
    //premier element enfant de l'element XMl en parametre
    XMLElement* child = xmlLevel->FirstChildElement();
    //parcours des elements enfant, jusqu'a trouver ce qu'on cherche
    while (child != nullptr && !elementTrouve) {
        //si l'element est trouvé, on arrete la boucle
        if (child->Attribute(attribute.c_str()) != NULL && child->Attribute(attribute.c_str()) == id) {
            elementTrouve = true;
        } else {
            //sinon on continue
            child = child->NextSiblingElement();
        }
    }
    return child;
}

/**
 * affiche le nom des provinces d'un pays, donc le code se trouve dans la liste provinceCode
 * @param country le pays dont on veux afficher les provinces
 * @param provinceCode la liste des provinces qu'on veux afficher
 */
void Mondial::printProvinceFromCountryInProvinceList(tinyxml2::XMLElement *country, vector<std::string> provinceCode) const {
    //boucle affichage. parcours chaque provinces traversée
    for (const auto& provCode : provinceCode) {
        //on recupere l'element xml de la province du pays, correspondant à la province traversée
        XMLElement* countryProv = getXmlelementFromAttribute(country, "id", provCode);
        //si la province est trouvée, alors afficher son nom
        if (countryProv != nullptr) {
            cout << "\t\t* " << countryProv->FirstChildElement("name")->GetText() << endl;
        }
    }
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