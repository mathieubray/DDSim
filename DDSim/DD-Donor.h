/* ---------------------------------------------
DD-Donor.h
Stores Information on KPD and Deceased Donors
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
	int matchingID;
	int uniqueID;
			
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
	int donorRecoveryDate;
	bool donorBothKidneysAvailable;
	int donorCenterID;

public:

	//Constructors

	//Constructs a dummy donor
	KPDDonor();
	//Constructs a KPD donor from APD data
	KPDDonor(int mID, int uID, KPDBloodType bt, 
		KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse);
	//Constructs a deceased donor from SRTR data
	KPDDonor(int uID, KPDBloodType bt, bool minorA, 
		int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse,
		int date, int cID);
	//Constructs a donor with all defined donor characteristics
	KPDDonor(int mID, int uID, KPDBloodType bt, bool minorA, 
		KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse,
		bool dd, int date,  bool bothKidneys, int cID);
	~KPDDonor();	
	
	//Getters
	int getMatchingID();
	int getUniqueID();
		
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
	int getRecoveryDate();
	bool getBothKidneysAvailable();
	int getCenterID();

	//Setters
	void setMatchingID(int id);
	void setUniqueID(int id);
		
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
	void setRecoveryDate(int date);
	void setBothKidneysAvailable(bool both);
	void setCenterID(int cID);


	//Strings
	std::string donorString(); // Returns a comma-separated string
	std::string donorOutput(); // Returns detailed donor information

	//Copy Constructor
	KPDDonor * copy();
};

KPDDonor::KPDDonor() {

	matchingID = -1;
	uniqueID = -1;
	
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
	donorRecoveryDate = 0;
	donorBothKidneysAvailable = false;
	donorCenterID = -1;
}

KPDDonor::KPDDonor(int mID, int uID, KPDBloodType bt, 
	KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse) {

	matchingID = mID;
	uniqueID = uID;
		
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
	donorRecoveryDate = 0;
	donorBothKidneysAvailable = false;
	donorCenterID = -1;
}

KPDDonor::KPDDonor(int uID, KPDBloodType bt, bool minorA,
	int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse,
	int date, int cID){
	
	matchingID = -1;
	uniqueID = uID;

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
	donorRecoveryDate = date;
	donorBothKidneysAvailable = false;
	donorCenterID = cID;
}

KPDDonor::KPDDonor(int mID, int uID, KPDBloodType bt, bool minorA,
	KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse,
	bool dd, int date, bool bothKidneys, int cID) {

	matchingID = mID;
	uniqueID = uID;

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
	donorRecoveryDate = date;
	donorBothKidneysAvailable = bothKidneys;
	donorCenterID = cID;
}

KPDDonor::~KPDDonor() {

}

int KPDDonor::getMatchingID() {
	return matchingID;
}

int KPDDonor::getUniqueID() {
	return uniqueID;
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

int KPDDonor::getRecoveryDate(){
	return donorRecoveryDate;
}

bool KPDDonor::getBothKidneysAvailable(){
	return donorBothKidneysAvailable;
}

int KPDDonor::getCenterID() {
	return donorCenterID;
}

void KPDDonor::setMatchingID(int id) {
	matchingID = id;
}

void KPDDonor::setUniqueID(int id) {
	uniqueID = id;
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

void KPDDonor::setRecoveryDate(int date) {
	donorRecoveryDate = date;
}

void KPDDonor::setBothKidneysAvailable(bool both) {
	donorBothKidneysAvailable = both;
}

void KPDDonor::setCenterID(int cID) {
	donorCenterID = cID;
}

std::string KPDDonor::donorString() {
	
	std::stringstream donorInfo;
	
	if (donorDeceased) {
		donorInfo << uniqueID << ",";
		donorInfo << KPDFunctions::bloodTypeToString(donorBT) << ",";
		donorInfo << KPDFunctions::boolToYesNo(donorMinorA) << ",";
		donorInfo << donorAge << ",";
		donorInfo << KPDFunctions::boolToMaleFemale(donorMale) << ",";
		donorInfo << KPDFunctions::raceToString(donorRace) << ",";
		donorInfo << donorHeight << ",";
		donorInfo << donorWeight << ",";
		donorInfo << (donorWeight / donorHeight) / donorHeight << ",";
		donorInfo << KPDFunctions::boolToYesNo(donorCigaretteUse) << ",";
		donorInfo << donorRecoveryDate << ",";
		donorInfo << KPDFunctions::boolToYesNo(donorBothKidneysAvailable) << ",";
		donorInfo << donorCenterID;
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
		donorInfo << KPDFunctions::indent(tab) << "Donor: " << uniqueID << std::endl;
		donorInfo << KPDFunctions::indent(tab) << "Recovery Date: " << donorRecoveryDate << std::endl;
		if (donorBothKidneysAvailable) {
			donorInfo << KPDFunctions::indent(tab) << "Both Kidneys Available" << std::endl;
		}
		else {
			donorInfo << KPDFunctions::indent(tab) << "One Kidney Available" << std::endl;
		}
		donorInfo << "Center ID: " << donorCenterID << std::endl;
	}
	else {
		if (!(matchingID == -1 && uniqueID == -1)) {
			donorInfo << KPDFunctions::indent(tab) << "Donor " << uniqueID << " (Matching ID: " << matchingID << ")" << std::endl;
		}
		else {
			donorInfo << KPDFunctions::indent(tab) << "Donor:" << std::endl;
		}
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

	return donorInfo.str();
}

KPDDonor * KPDDonor::copy() {

	KPDDonor * copyDonor = new KPDDonor(matchingID,uniqueID,donorBT,donorMinorA,
		donorRelation,donorAge,donorMale,donorRace,donorHeight,donorWeight,donorCigaretteUse,
		donorDeceased, donorRecoveryDate, donorBothKidneysAvailable, donorCenterID);

	copyDonor->setHLA(donorHLA);

	return copyDonor;
}

#endif