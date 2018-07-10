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
	KPDBloodTypeEnhanced donorEnhancedBT;
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
	int donorRecoveryYear;
	int donorRecoveryMonth;
	int donorRecoveryDay;
	int donorCenterID;
	bool donorBothKidneysAvailable;

public:

	//Constructors

	//Constructs a dummy donor
	KPDDonor();
	//Constructs a KPD donor from APD data
	KPDDonor(int mID, int uID, KPDBloodType bt, 
		KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse);
	//Constructs a deceased donor from SRTR data
	KPDDonor(int uID, KPDBloodType bt, KPDBloodTypeEnhanced bte, 
		int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse, 
		int year, int month, int day, int cID);
	//Constructs a donor with all defined donor characteristics
	KPDDonor(int mID, int uID, KPDBloodType bt, KPDBloodTypeEnhanced bte, 
		KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse, 
		bool deceased, int year, int month, int day, int cID, bool bothKidneys);
	~KPDDonor();	
	
	//Getters
	int getMatchingID();
	int getUniqueID();
	
	KPDBloodType getBT();
	KPDBloodTypeEnhanced getEnhancedBT();
	std::vector<std::string> getHLA();

	KPDRelation getRelation();
	int getAge();
	bool getMale();
	KPDRace getRace();
	double getHeight();
	double getWeight();
	double getBMI();
	bool getCigaretteUse();

	bool getDeceased();
	int getRecoveryYear();
	int getRecoveryMonth();
	int getRecoveryDay();
	int getCenterID();
	bool getBothKidneysAvailable();

	//Setters
	void setMatchingID(int id);
	void setUniqueID(int id);
	
	void setBT(KPDBloodType bt);
	void setEnhancedBT(KPDBloodTypeEnhanced bte);
	void setHLA(std::vector<std::string> hla);

	void setRelation(KPDRelation relation);
	void setAge(int age);
	void setMale(bool genderMale);
	void setRace(KPDRace race);
	void setHeight(double height);
	void setWeight(double weight);
	void setCigaretteUse(bool cigaretteUse);

	void setDeceased(bool deceased);
	void setRecoveryYear(int year);
	void setRecoveryMonth(int month);
	void setRecoveryDay(int day);
	void setCenterID(int id);
	void setBothKidneysAvailable(bool bothAvailable);

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
	donorEnhancedBT = BTE_UNSPECIFIED;

	donorRelation = RELATION_UNSPECIFIED;
	donorAge = 40;
	donorMale = true;
	donorRace = RACE_UNSPECIFIED;
	donorHeight = 1.75;
	donorWeight = 80;
	donorCigaretteUse = false;

	donorDeceased = false;
	donorRecoveryYear = 2013;
	donorRecoveryMonth = 1;
	donorRecoveryDay = 1;
	donorCenterID = 0;
	donorBothKidneysAvailable = false;
}

KPDDonor::KPDDonor(int mID, int uID, KPDBloodType bt, 
	KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse) {

	matchingID = mID;
	uniqueID = uID;
	
	donorBT = bt;
	donorEnhancedBT = BTE_UNSPECIFIED;

	donorRelation = relation;
	donorAge = age;
	donorMale = male;
	donorRace = race;
	donorHeight = height;
	donorWeight = weight;
	donorCigaretteUse = cigaretteUse;

	donorDeceased = false;
	donorRecoveryYear = 2013;
	donorRecoveryMonth = 1;
	donorRecoveryDay = 1;
	donorCenterID = 0;
	donorBothKidneysAvailable = false;
}

KPDDonor::KPDDonor(int uID, KPDBloodType bt, KPDBloodTypeEnhanced bte, 
	int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse, 
	int year, int month, int day, int cID){
	
	matchingID = -1;
	uniqueID = uID;

	donorBT = bt;
	donorEnhancedBT = bte;

	donorRelation = RELATION_UNSPECIFIED;
	donorAge = age;
	donorMale = male;
	donorRace = race;
	donorHeight = height;
	donorWeight = weight;
	donorCigaretteUse = cigaretteUse;

	donorDeceased = true;
	donorRecoveryYear = year;
	donorRecoveryMonth = month;
	donorRecoveryDay = day;
	donorCenterID = cID;
	donorBothKidneysAvailable = false;
}

KPDDonor::KPDDonor(int mID, int uID, KPDBloodType bt, KPDBloodTypeEnhanced bte, 
	KPDRelation relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse, 
	bool deceased, int year, int month, int day, int cID, bool bothKidneys) {

	matchingID = mID;
	uniqueID = uID;

	donorBT = bt;
	donorEnhancedBT = bte;

	donorRelation = relation;
	donorAge = age;
	donorMale = male;
	donorRace = race;
	donorHeight = height;
	donorWeight = weight;
	donorCigaretteUse = cigaretteUse;

	donorDeceased = deceased;
	donorRecoveryYear = year;
	donorRecoveryMonth = month;
	donorRecoveryDay = day;
	donorCenterID = cID;
	donorBothKidneysAvailable = bothKidneys;
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

KPDBloodTypeEnhanced KPDDonor::getEnhancedBT() {
	return donorEnhancedBT;
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

bool KPDDonor::getDeceased() {
	return donorDeceased;
}

int KPDDonor::getRecoveryYear() {
	return donorRecoveryYear;
}

int KPDDonor::getRecoveryMonth() {
	return donorRecoveryMonth;
}

int KPDDonor::getRecoveryDay() {
	return donorRecoveryDay;
}

int KPDDonor::getCenterID() {
	return donorCenterID;
}

bool KPDDonor::getBothKidneysAvailable() {
	return donorBothKidneysAvailable;
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

void KPDDonor::setEnhancedBT(KPDBloodTypeEnhanced bte){
	donorEnhancedBT = bte;
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

void KPDDonor::setDeceased(bool deceased) {
	donorDeceased = deceased;
}

void KPDDonor::setRecoveryYear(int year) {
	donorRecoveryYear = year;
}

void KPDDonor::setRecoveryMonth(int month) {
	donorRecoveryMonth = month;
}

void KPDDonor::setRecoveryDay(int day) {
	donorRecoveryDay = day;
}

void KPDDonor::setCenterID(int id) {
	donorCenterID = id;
}

void KPDDonor::setBothKidneysAvailable(bool bothAvailable) {
	donorBothKidneysAvailable = bothAvailable;
}

std::string KPDDonor::donorString() {
	
	std::stringstream donorInfo;
	
	if (donorDeceased) {
		donorInfo << uniqueID << ",";
		donorInfo << KPDFunctions::bloodTypeToString(donorBT) << ",";
		donorInfo << KPDFunctions::bloodTypeEnhancedToString(donorEnhancedBT) << ",";
		donorInfo << KPDFunctions::relationToString(donorRelation) << ",";
		donorInfo << donorAge << ",";
		donorInfo << KPDFunctions::boolToMaleFemale(donorMale) << ",";
		donorInfo << KPDFunctions::raceToString(donorRace) << ",";
		donorInfo << donorHeight << ",";
		donorInfo << donorWeight << ",";
		donorInfo << (donorWeight / donorHeight) / donorHeight << ",";
		donorInfo << KPDFunctions::boolToYesNo(donorCigaretteUse) << ",";
		donorInfo << donorRecoveryYear << "-" << donorRecoveryMonth << "-" << donorRecoveryDay << ",";
		donorInfo << donorCenterID << ",";
		donorInfo << KPDFunctions::boolToYesNo(donorBothKidneysAvailable);
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
		donorInfo << KPDFunctions::indent(tab) << "Recovery Date: " << donorRecoveryYear << "-" << donorRecoveryMonth << "-" << donorRecoveryDay << std::endl;
		donorInfo << KPDFunctions::indent(tab) << "Center: " << donorCenterID << std::endl;
		if (donorBothKidneysAvailable) {
			donorInfo << KPDFunctions::indent(tab) << "Both Kidneys Available" << std::endl;
		}
		else {
			donorInfo << KPDFunctions::indent(tab) << "One Kidney Available" << std::endl;
		}
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
	donorInfo << KPDFunctions::indent(tab) << "Enhanced BT: " << KPDFunctions::bloodTypeEnhancedToString(donorEnhancedBT) << std::endl;
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

	KPDDonor * copyDonor = new KPDDonor(matchingID,uniqueID,
		donorBT,donorEnhancedBT,
		donorRelation,donorAge,donorMale,donorRace,donorHeight,donorWeight,donorCigaretteUse,
		donorDeceased,donorRecoveryYear,donorRecoveryMonth,donorRecoveryDay,donorCenterID,donorBothKidneysAvailable);

	copyDonor->setHLA(donorHLA);

	return copyDonor;
}

#endif