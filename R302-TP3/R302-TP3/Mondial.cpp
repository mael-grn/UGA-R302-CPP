/* 
 * File:   Mondial.cpp
 * Author: hb
 * 
 * Created on 22 novembre 2018, 16:05
 */

#include "Mondial.h"
#include "chrono"

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
 * ----------------------------------------------------------------------------------------------------------------print
 */
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
 * Affichage des frontière d'un pays
 * @param countryName corresponds au nom du pays dont on veux afficher les frontières
 */
void Mondial::printCountryBorders(string countryName) const {
    //On recupere le pays de nom countryName
    XMLElement* country = getXmlelementFromNameRec(countryName);
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
            //si le pays n'a pas de pays frontaliers
        } else {
            //affichage d'un message approprié
            cout << "Le pays " + countryName + " ne possede aucun pays frontaliers...";
        }
    }
}

/**
 * Affichage du nom des pays traversés par une rivière. Utilisation de l'attribut country
 * @param riverName corresponds au nom de la rivière dont on veux afficher les informations
 */
void Mondial::printAllCountriesCrossedByRiver(string riverName) const {
    //recuperer la riviere de nom riverName
    XMLElement* river = getXmlelementFromNameRec(riverName);
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
            XMLElement* country = getCountryXmlelementFromCode(elem);
            if (country != nullptr) {
                cout << "\t- " << country->FirstChildElement("name")->GetText() << endl;
            }
        }
        //affichage de la longueur du fleuve:
        cout << "sa longueur est de " << river->FirstChildElement("length")->GetText() << " km" << endl;
        //affichage du message si le fleuve n'est pas trouvé
    } else {
        cout << "le fleuve " << riverName << " existe pas !!" << endl;
    }
}

/**
 * Afficher le nom des pays traversés par une rivière. Utilisation des balises located
 * @param riverName corresponds au nom de la rivière dont on veux afficher les informations
 */
void Mondial::printCountriesWithProvincesCrossedByRiver(string riverName) const {
    //recuperer la riviere de nom riverName
    XMLElement* river = getXmlelementFromNameRec(riverName);
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
            XMLElement* country = getCountryXmlelementFromCode(riverElem->Attribute("country"));
            if (country != nullptr) {
                cout << "\t- " << country->FirstChildElement("name")->GetText() << endl;
            }
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
 * Affichage de tout les pays traversés par une riviere, ainsi que les provinces des pays traversées par la rivière
 * @param riverName corresponds au nom de la rivière dont on veux afficher les informations
 */
void Mondial::printCountriesAndProvincesCrossedByRiver(string riverName) const {
    //recuperer la riviere de nom countryname
    XMLElement* riversCat = racineMondial->FirstChildElement("riverscategory");
    XMLElement* river = getXmlelementFromNameRec(riverName, riversCat);
    //si la riviere est trouvée
    if (river != nullptr) {
        //affichage d'un message approprié
        cout << "Pays et provinces traversés par : " << riverName << endl;
        //on recupere les pays traversés depuis l'attribut country
        //il est necessaire de les recuperer depuis cet attribut, car certains pays peuvent ne pas apparaitre
        //dans la balise located
        vector<string> countryLst;
        string countries = river->Attribute("country");
        split(countries, ' ', back_inserter(countryLst));
        //pour chaque code pays récupérés depuis l'attribut country
        for (auto country : countryLst) {
            //on recupere l'element XML du pays
            XMLElement* countryElem = getCountryXmlelementFromCode(country);
            //si celui ci a été trouvé
            if (countryElem != nullptr) {
                //on affiche le nom du pays
                cout << "\t- " << countryElem->FirstChildElement("name")->GetText() << endl;
                //on recupere la balise located de la rivere correspondant au pays.
                //cette balise nous fournis les provinces de ce pays traversées par la rivière.
                //Pour cela, on cherche, dans les balises de la rivière, celle ayant le code du pays pour son attribut country
                XMLElement* locatedElem = getXmlelementFromAttributeRec("country", country, river);
                //si la balise est trouvée, et que celle ci contient des provinces
                if (locatedElem != nullptr && locatedElem->Attribute("province") != NULL) {
                    //on recupere les provinces, et on les ajoutes dans une listes
                    string provinces = locatedElem->Attribute("province");
                    vector<string> provLst;
                    split(provinces, ' ', back_inserter(provLst));
                    //on affiche les provinces
                    printProvinceFromCountryInProvinceList(provLst);
                } else {
                    cout << "\t\t* Aucune provinces traversées..." << endl;
                }

            } else {
                cout << "\tERREUR : pays " << country << " non trouvé" << endl;
            }
        }
        //affichage de la longueur du fleuve:
        cout << "sa longueur est de " << river->FirstChildElement("length")->GetText() << " km" << endl;
        //affichage du message si le fleuve n'est pas trouvé
    } else {
        cout << "le fleuve " << riverName << " existe pas !!" << endl;
    }
}

/**
 * Affiche certaines informations sur une ville
 * @param cityName corresponds au nom de la ville dont on souhaite afficher les informations
 */
void Mondial::printCityInformation(string cityName) const {
    XMLElement * countryCat = racineMondial->FirstChildElement("countriescategory");
    XMLElement* city = getXmlelementFromNameRec(cityName, countryCat);
    if (city == nullptr) {
        cout << "la ville " << cityName << " existe pas..." << endl;
    } else {
        cout << cityName << " : " << endl;

        XMLElement* country = nullptr;
        if (city->Attribute("country") != NULL){
            country = getCountryXmlelementFromCode(city->Attribute("country"));
        }
        XMLElement* province = nullptr;
        if (city->Attribute("province") != NULL) {
            province = getXmlelementFromAttributeRec("id", city->Attribute("province"));
        }
        //affichage du pays
        if (country != nullptr && country->FirstChildElement("name") != nullptr) {
            cout << "\t- Country : " << country->FirstChildElement("name")->GetText() << endl;
        } else {
            cout << "\t- Cette ville ne possede pas de pays..." << endl;
        }
        //affichage de la province
        if (province != nullptr && province->FirstChildElement("name") != nullptr) {
            cout << "\t- Province : " << province->FirstChildElement("name")->GetText() << endl;
        } else {
            cout << "\t- Cette ville ne possede pas de province..." << endl;
        }
        //affichage de la latitude
        if (city->FirstChildElement("latitude") != nullptr) {
            cout << "\t- latitude : " << city->FirstChildElement("latitude")->GetText() << endl;
        } else {
            cout << "\t- Cette ville ne possede pas de latitude..." << endl;
        }
        //affichage de la longitude
        if (city->FirstChildElement("longitude") != nullptr) {
            cout << "\t- longitude : " << city->FirstChildElement("longitude")->GetText() << endl;
        } else {
            cout << "\t- Cette ville ne possede pas de longitude..." << endl;
        }
        //affichage de la derniere population renseignée
        if (city->LastChildElement("population") != nullptr) {
            cout << "\t- Population : " << city->LastChildElement("population")->GetText() << endl;
        } else {
            cout << "\t- Cette ville n'a pas de population renseignée population..." << endl;
        }
    }

}

/**
 * Question additionelle de l'exercice 9. Affichage des informations d'une île à partir de son nom
 * @param islandName corresponds au nom de l'île
 */
void Mondial::printIslandsInformations(string islandName) const {
    XMLElement* islandCat = racineMondial->FirstChildElement("islandscategory");
    XMLElement* island = getXmlelementFromNameRec(islandName, islandCat);
    if (island == nullptr) {
        cout << "la ville " << islandName << " existe pas..." << endl;
    } else {
        cout << islandName << " : " << endl;

        XMLElement* country = nullptr;
        if (island->Attribute("country") != NULL){
            country = getCountryXmlelementFromCode(island->Attribute("country"));
        }
        XMLElement* sea = nullptr;
        if (island->Attribute("sea") != NULL) {
            sea = getXmlelementFromAttributeRec("id", island->Attribute("sea"));
        }
        //affichage du pays
        if (country != nullptr && country->FirstChildElement("name") != nullptr) {
            cout << "\t- Country : " << country->FirstChildElement("name")->GetText() << endl;
        } else {
            cout << "\t- Cette ile ne possede pas de pays..." << endl;
        }
        //affichage de la province
        if (sea != nullptr && sea->FirstChildElement("name") != nullptr) {
            cout << "\t- Sea : " << sea->FirstChildElement("name")->GetText() << endl;
        } else {
            cout << "\t- Cette ile ne possede pas de mer..." << endl;
        }
    }
}

/**
 * Question additionelle de l'exercice 9. Affichage des statistiques sur les religions d'un pays
 * @param countryName corresponds au nom du pays dont on veut afficher les religions
 */
void Mondial::printReligionForCountry(string countryName) const {
    XMLElement* country = getCountryXmlelementFromNameRec(countryName);
    XMLElement* religionElement = nullptr;
    if (country == nullptr) {
        cout << "Le pays " << countryName << " existe pas...";
    } else {
        religionElement = country->FirstChildElement("religion");
    }
    if (religionElement == nullptr) {
        cout << "Le pays " + countryName + " n'a pas de religions..." << endl;
    } else {
        cout << "Religions pour " << countryName << " : " << endl;
    }
    while (religionElement != nullptr && strcmp(religionElement->Name(),"religion")==0) {
        cout << "\t" << religionElement->GetText() << endl << "\t";
        int percentage = stoi(religionElement->Attribute("percentage"));
        printPercentage(percentage);
        cout << " " << percentage << "%" << endl;
        religionElement = religionElement->NextSiblingElement();
    }
}

/**
 * Question additionelle de l'exercice 9. Affichage de toutes les montagnes existantes, dont l'altitude est supérieur
 * ou egal à une valeur définit
 * @param minHeight corresponds à l'altitude minimum des montagnes à afficher
 */
void Mondial::printHighestMountains(int minHeight) const {
    //on recupere l'element correspondant à mountainscategory
    XMLElement* mountainCat = racineMondial->FirstChildElement("mountainscategory");
    //on recupere le premier element enfant de mountaincategory (soit le premiere montagne)
    XMLElement* mountainElem = mountainCat->FirstChildElement();
    //variable indiquant si des montagnes à l'elevations en parametre existe
    bool existMountain = false;
    //boucle de parcours des montagnes
    while (mountainElem != nullptr) {
        //si la montagne a un attribut elevation, et si celui ci est superieur ou egal à minHeight
        if (mountainElem->FirstChildElement("elevation") != nullptr &&
            stoi(mountainElem->FirstChildElement("elevation")->GetText()) >= minHeight) {
            //On affiche le nom de la montagne
            cout << mountainElem->FirstChildElement("name")->GetText() << " : " << endl;
            //si la montagne a un element located
            if (mountainElem->FirstChildElement("located") != nullptr) {
                //si located a un attribut country, on recherche l'element XML du pays, et on affiche son nom
                XMLElement *country = getCountryXmlelementFromCode(
                        mountainElem->FirstChildElement("located")->Attribute("country"));
                if (country != nullptr) {
                    cout << "\t- Country : " << country->FirstChildElement("name")->GetText() << endl;
                }
                //si located a un attribut province, on recherche l'element XML de la province, et on affiche son nom
                XMLElement *province = getXmlelementFromAttributeRec("id", mountainElem->FirstChildElement(
                        "located")->Attribute("province"));
                if (province != nullptr) {
                    cout << "\t- Province : " << province->FirstChildElement("name")->GetText() << endl;
                }
            } else {
                //affichage d'un message si la balise located n'est pas trouvée
                cout << "\tLa montagne n'a pas de localisation" << endl;
            }
            //on recupere l'elevation de la montagne, et on l'affiche
            string elevation = mountainElem->FirstChildElement("elevation")->GetText();
            cout << "\t- Elevation : " << stoi(elevation) << endl;
            //on indique qu'on a bien trouvé des montagnes a cette altitude
            existMountain = true;
        }
        //passage a la montagne suivante
        mountainElem = mountainElem->NextSiblingElement();
    }
    //si aucune montagne n'a été trouvée, affichage d'un message
    if (!existMountain) {
        cout << "Il n'existe aucune montagne au dessus de " << minHeight << "..." << endl;
    }
}

/**
 * Affichage du temps d'executions des differentes methodes, à des fin d'optimisation
 */
void Mondial::printTempsExec() const {
    //temps de début
    auto start_time = std::chrono::high_resolution_clock::now();
    //recherche
    getCountryXmlelementFromNameIter("United States");
    //temps de fin
    auto end_time = std::chrono::high_resolution_clock::now();
    //durée d'exécution
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "\t- Recherche d'un pays iteratif par son nom : " << duration.count() << " microseconds" << endl;

    start_time = std::chrono::high_resolution_clock::now();
    getCountryXmlelementFromNameRec("United States");
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "\t- Recherche d'un pays recursif par son nom : " << duration.count() << " microseconds" << endl;

    start_time = std::chrono::high_resolution_clock::now();
    getXmlelementFromNameRec("United States");
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "\t- Recherche d'un element XML recursif par son nom (recherche dans tout le fichier) : " << duration.count() << " microseconds" << endl;

    start_time = std::chrono::high_resolution_clock::now();
    XMLElement* countryCat = racineMondial->FirstChildElement("countriescategory");
    getXmlelementFromNameRec("United States", countryCat);
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "\t- Recherche d'un element XML recursif par son nom (recherche dans une categorie) : " << duration.count() << " microseconds" << endl;

    start_time = std::chrono::high_resolution_clock::now();
    getCountryXmlelementFromCode("USA");
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "\t- Recherche d'un pays iteratif par son code : " << duration.count() << " microseconds" << endl;

    start_time = std::chrono::high_resolution_clock::now();
    getXmlelementFromAttributeRec("car_code", "USA");
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << "\t- Recherche d'un element XML recursif par son code : " << duration.count() << " microseconds" << endl;
}

/*
 * ---------------------------------------------------------------------------------------------------------------Getter
 */
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

/**
 * @return le nombre d'element appartenant à la catégorie desert
 */
int Mondial::getNbDeserts() const {
    XMLElement* debug = racineMondial->FirstChildElement("riverscategory")->FirstChildElement();
    cout << "debug : " << debug->Name() << " : " << debug->Attribute("id") << endl;
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
 * @param categoryName corresponds au nom de la catégorie dont on cherche des informations
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
 * @param countryName corresponds au nom du pays dont on veux afficher des informations
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

/*
 * ----------------------------------------------------------------------------------------------------Getter XMLElement
 */
/**
 * Boucle récursive, permettant de récuperer l'element XML pays ayant pour nom countryName
 * @param countryName corresponds au nom du pays que l'ont recherche
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
 * Recherche du code d'un pays à partir de son nom. Recherche recursive à partir de getCountryXmlelementFromNameRec
 * @param countryName corresponds au nom du pays dont on cherche les informations
 * @return le code du pays, dont le nom a été passé en parametre.
 */
string Mondial::getCountryCodeFromName(string countryName) const throw (PrecondVioleeExcep) {
    //recuperer l'objet XMLElement correspondant au pays countryName
    XMLElement* country = getCountryXmlelementFromNameRec(countryName);
    //si le pays n'existe pas, throw une exception
    if (country == nullptr) {
        throw PrecondVioleeExcep("Le pays \"" + countryName + "\" n'existe pas...");
    }
    //retourne le code du pays
    return country->Attribute("car_code");
}

/**
 * élément <country> d'un pays identifié par son nom countryName
 * @param countryName corresponds au nom du pays dont on cherche les informations
 * @return pointeur sur l'élément <country> dont la valeur du fils <name> est égal à countryName, nullprt sinon
 */
XMLElement* Mondial::getCountryXmlelementFromNameIter(string countryName) const {
    //on recupere l'element racine
    XMLElement* elemCat = racineMondial->FirstChildElement("countriescategory");
    //on recupere le premier element de la categorie
    XMLElement* currentElemCat = elemCat->FirstChildElement();
    //variable permettant d'arreter la boucle quand l'element est trouve
    bool elemTrouve = false;
    //tant qu'il y a un prochain element, et que l'element n'est pas trouvé
    while (currentElemCat != nullptr && !elemTrouve) {
        //si l'element correspond au critere, alors on arrete la boucle
        if ( strcmp(currentElemCat->FirstChildElement("name")->GetText(), countryName.c_str())==0){
            elemTrouve = true;
            //sinon, on passe a l'element
        } else {
            currentElemCat = currentElemCat->NextSiblingElement();
        }
    }
    return currentElemCat;
}

/**
 * @param countryCode corresponds au code d'un pays dont on veux retrouver l'element XML
 * @return l'élément XML dont le code correspond à celui en paramètre.
 */
XMLElement* Mondial::getCountryXmlelementFromCode(string countryCode) const {
    //on recupere l'element racine
    XMLElement* elemCat = racineMondial->FirstChildElement("countriescategory");
    //on recupere le premier element de la categorie
    XMLElement* currentElemCat = elemCat->FirstChildElement();
    //variable permettant d'arreter la boucle quand l'element est trouve
    bool elemTrouve = false;
    //tant qu'il y a un prochain element, et que l'element n'est pas trouvé
    while (currentElemCat != nullptr && !elemTrouve) {
        //si l'element correspond au critere, alors on arrete la boucle
        if ( strcmp(currentElemCat->Attribute("car_code"), countryCode.c_str())==0){
            elemTrouve = true;
            //sinon, on passe a l'element
        } else {
            currentElemCat = currentElemCat->NextSiblingElement();
        }
    }
    return currentElemCat;
}

/**
 *
 * @param riverName corresponds au nom de la rivière, dont on veux retrouver l'element XML.
 * @return l'élément XML dont le nom correspond à celui de la rivière.
 */
XMLElement* Mondial::getRiverXmlelementFromNameIter(string riverName) const {
    //on recupere l'element racine
    XMLElement* elemCat = racineMondial->FirstChildElement("riverscategory");
    //on recupere le premier element de la categorie
    XMLElement* currentElemCat = elemCat->FirstChildElement();
    //variable permettant d'arreter la boucle quand l'element est trouve
    bool elemTrouve = false;
    //tant qu'il y a un prochain element, et que l'element n'est pas trouvé
    while (currentElemCat != nullptr && !elemTrouve) {
        //si l'element correspond au critere, alors on arrete la boucle
        if ( strcmp(currentElemCat->FirstChildElement("name")->GetText(), riverName.c_str())==0){
            elemTrouve = true;
            //sinon, on passe a l'element
        } else {
            currentElemCat = currentElemCat->NextSiblingElement();
        }
    }
    return currentElemCat;
}

/**
 * programme recursif, permettant de rechercher dans le fichier xml n'importe quel attribut a n'importe quel niveau ayant
 * pour nom "name".
 * @param name le nom de ce que l'ont recherche
 * @param category facultatif. Correspond à une categorie specifique dans laquelle on veux chercher. Utile si l'element apparait dans plusieurs catégories.
 * @return l'element xml dont le nom est name, nullptr sinon
 */
XMLElement* Mondial::getXmlelementFromNameRec(string name, XMLElement* begin) const {
    //on recupere la racine du document
    if (begin == nullptr) {
        begin = racineMondial->FirstChildElement();
    }
    //demarrage de la recherche recursive
    return getXmlelementFromNameRecWorker(begin, name);
}
XMLElement *Mondial::getXmlelementFromNameRecWorker(XMLElement *xmlLevel, string name) const {

    //si l'element courant a un nom, et si celui ci correspond a celui que nous cherchons, alors on s'arrete
    if (xmlLevel->FirstChildElement("name") != nullptr && strcmp(xmlLevel->FirstChildElement("name")->GetText(), name.c_str()) == 0 ) {
        return xmlLevel;
    } else {
        //sinon, declaration de l'element suivant
        XMLElement *next= nullptr;
        //si l'element suivant existe, alors on descend d'un niveau.
        if (xmlLevel->FirstChildElement() != nullptr) {
            //On descend d'un niveau
            next = getXmlelementFromNameRecWorker(xmlLevel->FirstChildElement(), name);
        }
        //si l'element voisin existe, et si le fait de descendre n'a rien rapporté
        if (xmlLevel->NextSiblingElement() != nullptr && next == nullptr) {
            //on passe à l'element suivant
            next = getXmlelementFromNameRecWorker(xmlLevel->NextSiblingElement(), name);
        }
        return next;
    }
}

/**
 * programme recursif, permettant de rechercher dans le fichier xml n'importe quel element a n'importe quel niveau ayant
 * pour identifiant id à l'attribut de nom attribute
 * @param xmlLevel la racine du document
 * @param name le nom de ce que l'ont recherche
 * @return l'element xml dont le nom est name, nullptr sinon
 */
XMLElement *Mondial::getXmlelementFromAttributeRec(std::string attribute, std::string id, XMLElement* begin) const {
    //on recupere la racine du document
    if (begin == nullptr) {
        begin = racineMondial->FirstChildElement();
    }
    //demarrage de la recherche recursive
    return getXmlelementFromAttributeRecWorker(begin,attribute, id);
}
XMLElement *Mondial::getXmlelementFromAttributeRecWorker(tinyxml2::XMLElement *xmlLevel, std::string attribute,
                                                         std::string id) const {
    //si l'element courant a un attribut attribute, et si celui ci correspond a celui que nous cherchons, alors on s'arrete
    if (xmlLevel->Attribute(attribute.c_str()) != NULL && strcmp(xmlLevel->Attribute(attribute.c_str()),id.c_str())==0) {
        return xmlLevel;
    } else {
        //sinon, declaration de l'element suivant
        XMLElement *next = nullptr;
        //Recherche dans les niveaux enfants.
        if (xmlLevel->FirstChildElement() != nullptr) {
            //On descend d'un niveau
            next = getXmlelementFromAttributeRecWorker(xmlLevel->FirstChildElement(), attribute, id);
        }
        //si l'element voisin existe, et si le fait de descendre n'a rien rapporté
        if (xmlLevel->NextSiblingElement() != nullptr && next == nullptr) {
            //on passe à l'element suivant
            next = getXmlelementFromAttributeRecWorker(xmlLevel->NextSiblingElement(), attribute, id);
        }
        return next;
    }
}

/*
 * ---------------------------------------------------------------------------------------------------------------Outils
 */
/**
 * affiche le nom des provinces d'un pays, donc le code se trouve dans la liste provinceCode
 * @param country le pays dont on veux afficher les provinces
 * @param provinceCode la liste des provinces qu'on veux afficher
 */
void Mondial::printProvinceFromCountryInProvinceList(vector<std::string> provinceCode) const {
    //boucle affichage. parcours chaque provinces traversée
    for (const auto& provCode : provinceCode) {
        //on recupere l'element xml de la province du pays, correspondant à la province traversée
        XMLElement* countryProv = getXmlelementFromAttributeRec("id", provCode);
        //si la province est trouvée, alors afficher son nom
        if (countryProv != nullptr) {
            cout << "\t\t* " << countryProv->FirstChildElement("name")->GetText() << endl;
        }
    }
}


/**
 * affiche une barre, correspondant à un certain pourcentage
 * @param percentage le pourcentage à afficher
 */
void Mondial::printPercentage(int percentage) const {
    cout << "[";
    for (int i=0; i<percentage; i++) {
        cout << "|";
    }
    cout << "]";
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