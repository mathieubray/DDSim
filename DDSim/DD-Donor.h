/* ---------------------------------------------
DD-Donor.h
Stores information about donors (KPD and deceased)
---------------------------------------------- */

#ifndef DONOR_H
#define DONOR_H

#include "DD-Enums-Functions.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

class KPDDonor {

private:

	//IDs
	int donorID;
	int donorKPDMatchingID;
			
	//Crossmatch Information	
	KPDBloodType donorBT;
	bool donorMinorA;
	std::vector<std::string> donorHLA;

	//Characteristics
	KPDRelation donorRelation;	
	int donorAge;
	bool donorMale;
	KPDRace donorRace;
	double donorHeight;
	double donorWeight;
	bool donorCigaretteUse;

	bool donorDeceased;
	int donorRecoveryTime;
	bool donorBothKidneysAvailable;
	int donorOPO;
	double donorKDPI;

public:

	//Constructors

	//Constructs a dummy donor
	KPDDonor();
	//Constructs a KPD donor from APD data
	KPDDonor(int id, int matchingID, KPDBloodType bt, 
		KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse);
	//Constructs a deceased donor from SRTR data
	KPDDonor(int id, KPDBloodType bt, bool minorA, 
		int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse,
		int recoveryTime, int opo, double kdpi);
	//Constructs a donor with all defined donor characteristics
	KPDDonor(int id, int matchingID, KPDBloodType bt, bool minorA, 
		KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse,
		bool dd, int recoveryTime,  bool bothKidneys, int opo, double kdpi);
	~KPDDonor();	
	
	//Getters
	int getDonorID();
	int getKPDMatchingID();
		
	KPDBloodType getBT();
	bool getMinorA();
	std::vector<std::string> getHLA();

	KPDRelation getRelation();
	int getAge();
	bool getMale();
	KPDRace getRace();
	double getHeight();
	double getWeight();
	double getBMI();
	bool getCigaretteUse();

	bool isDeceasedDonor();
	int getRecoveryTime();
	bool getBothKidneysAvailable();
	int getOPO();

	double getKDPI();

	//Setters
	void setDonorID(int id);
	void setKPDMatchingID(int id);
		
	void setBT(KPDBloodType bt);
	void setMinorA(bool minorA);
	void setHLA(std::vector<std::string> hla);

	void setRelation(KPDRelation relation);
	void setAge(int age);
	void setMale(bool genderMale);
	void setRace(KPDRace race);
	void setHeight(double height);
	void setWeight(double weight);
	void setCigaretteUse(bool cigaretteUse);

	void setDeceasedDonor(bool dd);
	void setRecoveryTime(int time);
	void setBothKidneysAvailable(bool both);
	void setOPO(int opo);
	void setKDPI(double kdpi);

	//Strings
	std::string donorString(); // Returns a comma-separated string
	std::string donorOutput(); // Returns detailed donor information

	//Copy Constructor
	KPDDonor * copy();
};

KPDDonor::KPDDonor() {

	donorID = -1;
	donorKPDMatchingID = -1;
	
	donorBT = BT_UNSPECIFIED;
	donorMinorA = false;

	donorRelation = RELATION_UNSPECIFIED;
	donorAge = 40;
	donorMale = true;
	donorRace = RACE_UNSPECIFIED;
	donorHeight = 1.75;
	donorWeight = 80;
	donorCigaretteUse = false;

	donorDeceased = false;
	donorRecoveryTime = 0;
	donorBothKidneysAvailable = false;
	donorOPO = -1;
	donorKDPI = 0.0;
}

KPDDonor::KPDDonor(int id, int matchingID, KPDBloodType bt, 
	KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse) {

	donorID = id;
	donorKPDMatchingID = matchingID;
		
	donorBT = bt;
	donorMinorA = false;

	donorRelation = relation;
	donorAge = age;
	donorMale = male;
	donorRace = race;
	donorHeight = height;
	donorWeight = weight;
	donorCigaretteUse = cigaretteUse;

	donorDeceased = false;
	donorRecoveryTime = 0;
	donorBothKidneysAvailable = false;
	donorOPO = -1;
	donorKDPI = 0.0;
}

KPDDonor::KPDDonor(int id, KPDBloodType bt, bool minorA,
	int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse,
	int recoveryTime, int opo, double kdpi){

	donorID = id;
	donorKPDMatchingID = -1;

	donorBT = bt;
	donorMinorA = minorA;

	donorRelation = RELATION_UNSPECIFIED;
	donorAge = age;
	donorMale = male;
	donorRace = race;
	donorHeight = height;
	donorWeight = weight;
	donorCigaretteUse = cigaretteUse;

	donorDeceased = true;
	donorRecoveryTime = recoveryTime;
	donorBothKidneysAvailable = false;
	donorOPO = opo;
	donorKDPI = kdpi;
}

KPDDonor::KPDDonor(int id, int matchingID, KPDBloodType bt, bool minorA,
	KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse,
	bool dd, int recoveryTime, bool bothKidneys, int opo, double kdpi) {

	donorID = id;
	donorKPDMatchingID = matchingID;

	donorBT = bt;
	donorMinorA = minorA;

	donorRelation = relation;
	donorAge = age;
	donorMale = male;
	donorRace = race;
	donorHeight = height;
	donorWeight = weight;
	donorCigaretteUse = cigaretteUse;

	donorDeceased = dd;
	donorRecoveryTime = recoveryTime;
	donorBothKidneysAvailable = bothKidneys;
	donorOPO = opo;

	donorKDPI = kdpi;
}

KPDDonor::~KPDDonor() {

}

int KPDDonor::getDonorID() {
	return donorID;
}

int KPDDonor::getKPDMatchingID() {
	return donorKPDMatchingID;
}

KPDBloodType KPDDonor::getBT() {
	return donorBT;
}

bool KPDDonor::getMinorA() {
	return donorMinorA;
}

std::vector<std::string> KPDDonor::getHLA() {

	std::vector<std::string> hla;

	for (std::vector<std::string>::iterator it = donorHLA.begin(); it != donorHLA.end(); it++) {
		hla.push_back(*it);
	}

	return hla;
}

KPDRelation KPDDonor::getRelation() {
	return donorRelation;
}

int KPDDonor::getAge() {
	return donorAge;
}

bool KPDDonor::getMale() {
	return donorMale;
}

KPDRace KPDDonor::getRace() {
	return donorRace;
}

double KPDDonor::getHeight() {
	return donorHeight;
}

double KPDDonor::getWeight() {
	return donorWeight;
}

double KPDDonor::getBMI() {
	return donorWeight/donorHeight/donorHeight;
}

bool KPDDonor::getCigaretteUse() {
	return donorCigaretteUse;
}

bool KPDDonor::isDeceasedDonor(){
	return donorDeceased;
}

int KPDDonor::getRecoveryTime(){
	return donorRecoveryTime;
}

bool KPDDonor::getBothKidneysAvailable(){
	return donorBothKidneysAvailable;
}

int KPDDonor::getOPO() {
	return donorOPO;
}

double KPDDonor::getKDPI() {
	return donorKDPI;
}

void KPDDonor::setDonorID(int id) {
	donorID = id;
}

void KPDDonor::setKPDMatchingID(int id) {
	donorKPDMatchingID = id;
}

void KPDDonor::setBT(KPDBloodType bt) {
	donorBT = bt;
}

void KPDDonor::setMinorA(bool minorA){
	donorMinorA = minorA;
}

void KPDDonor::setHLA(std::vector<std::string> hla) {

	donorHLA.clear();
	for (std::vector<std::string>::iterator hlaIt = hla.begin(); hlaIt != hla.end(); hlaIt++) {
		donorHLA.push_back(*hlaIt);
	}
}

void KPDDonor::setRelation(KPDRelation relation) {
	donorRelation = relation;
}

void KPDDonor::setAge(int age) {
	donorAge = age;
}

void KPDDonor::setMale(bool genderMale) {
	donorMale = genderMale;
}

void KPDDonor::setRace(KPDRace race) {
	donorRace = race;
}

void KPDDonor::setHeight(double height) {
	donorHeight = height;
}

void KPDDonor::setWeight(double weight) {
	donorWeight = weight;
}

void KPDDonor::setCigaretteUse(bool cigaretteUse) {
	donorCigaretteUse = cigaretteUse;
}

void KPDDonor::setDeceasedDonor(bool dd) {
	donorDeceased = dd;
}

void KPDDonor::setRecoveryTime(int time) {
	donorRecoveryTime = time;
}

void KPDDonor::setBothKidneysAvailable(bool both) {
	donorBothKidneysAvailable = both;
}

void KPDDonor::setOPO(int opo) {
	donorOPO = opo;
}

void KPDDonor::setKDPI(double kdpi) {
	donorKDPI = kdpi;
}

std::string KPDDonor::donorString() {
	
	std::stringstream donorInfo;
	
	if (donorDeceased) {
		donorInfo << KPDFunctions::bloodTypeToString(donorBT) << ",";
		donorInfo << KPDFunctions::boolToYesNo(donorMinorA) << ",";
		donorInfo << donorAge << ",";
		donorInfo << KPDFunctions::boolToMaleFemale(donorMale) << ",";
		donorInfo << KPDFunctions::raceToString(donorRace) << ",";
		donorInfo << donorHeight << ",";
		donorInfo << donorWeight << ",";
		donorInfo << (donorWeight / donorHeight) / donorHeight << ",";
		donorInfo << KPDFunctions::boolToYesNo(donorCigaretteUse) << ",";
		donorInfo << donorRecoveryTime << ",";
		donorInfo << KPDFunctions::boolToYesNo(donorBothKidneysAvailable) << ",";
		donorInfo << donorOPO << ",";
		donorInfo << donorKDPI;
	}
	else {
		donorInfo << KPDFunctions::bloodTypeToString(donorBT) << ",";
		donorInfo << KPDFunctions::relationToString(donorRelation) << ",";
		donorInfo << donorAge << ",";
		donorInfo << KPDFunctions::boolToMaleFemale(donorMale) << ",";
		donorInfo << KPDFunctions::raceToString(donorRace) << ",";
		donorInfo << donorHeight << ",";
		donorInfo << donorWeight << ",";
		donorInfo << (donorWeight / donorHeight) / donorHeight << ",";
		donorInfo << KPDFunctions::boolToYesNo(donorCigaretteUse);
	}

	return donorInfo.str();
}

std::string KPDDonor::donorOutput() {

	int tab = 3;
	
	std::stringstream donorInfo;

	if (donorDeceased) {
		donorInfo << KPDFunctions::indent(tab) << "Donor: " << donorID << std::endl;
		donorInfo << KPDFunctions::indent(tab) << "Recovery Date: " << donorRecoveryTime << std::endl;
		if (donorBothKidneysAvailable) {
			donorInfo << KPDFunctions::indent(tab) << "Both Kidneys Available" << std::endl;
		}
		else {
			donorInfo << KPDFunctions::indent(tab) << "One Kidney Available" << std::endl;
		}
		donorInfo << KPDFunctions::indent(tab) << "OPO: " << donorOPO << std::endl;
	}
	else {
		donorInfo << KPDFunctions::indent(tab) << "Donor " << donorID << " (Matching ID: " << donorKPDMatchingID << ")" << std::endl;
	}

	donorInfo << KPDFunctions::indent(tab) << "BT: " << KPDFunctions::bloodTypeToString(donorBT) << std::endl;
	if (donorMinorA) {
		donorInfo << KPDFunctions::indent(tab) << "Minor A2 Subtype" << std::endl;
	}
	
	donorInfo << KPDFunctions::indent(tab) << "HLA:";
	for (std::vector<std::string>::iterator hla = donorHLA.begin(); hla != donorHLA.end(); hla++) {
		donorInfo << " " << *hla;
	}
	donorInfo << std::endl;

	if (!donorDeceased) {
		donorInfo << KPDFunctions::indent(tab) << "Relation to Candidate: " << KPDFunctions::relationToString(donorRelation) << std::endl;
	}
	donorInfo << KPDFunctions::indent(tab) << "Age: " << donorAge << ", Sex: " << KPDFunctions::boolToMaleFemale(donorMale) << ", Race: " << KPDFunctions::raceToString(donorRace) << std::endl;
	donorInfo << KPDFunctions::indent(tab) << "Height: " << donorHeight << ", Weight: " << donorWeight << ", BMI: " << (donorWeight / donorHeight) / donorHeight << std::endl;
	donorInfo << KPDFunctions::indent(tab) << "Cigarette Use: " << KPDFunctions::boolToYesNo(donorCigaretteUse) << std::endl;
	donorInfo << KPDFunctions::indent(tab) << "KDPI: " << donorKDPI << std::endl;
	return donorInfo.str();
}

KPDDonor * KPDDonor::copy() {

	KPDDonor * copyDonor = new KPDDonor(donorID, donorKPDMatchingID, donorBT,donorMinorA,
		donorRelation,donorAge,donorMale,donorRace,donorHeight,donorWeight,donorCigaretteUse,
		donorDeceased, donorRecoveryTime, donorBothKidneysAvailable, donorOPO, donorKDPI);

	copyDonor->setHLA(donorHLA);

	return copyDonor;
}

#endif