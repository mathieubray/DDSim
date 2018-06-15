
#ifndef DONOR_H
#define DONOR_H

#include "KPD-Enums-Functions.h"

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
	std::vector<std::string> donorHLA;

	//Characteristics
	KPDRelationToCandidate donorRelation;
	
	int donorAge;
	bool donorMale;
	KPDRace donorRace;
	double donorHeight;
	double donorWeight;
	bool donorCigaretteUse;

public:

	KPDDonor();
	KPDDonor(KPDBloodType bt, std::vector<std::string> hla, 
		KPDRelationToCandidate relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse);
	KPDDonor(int matchingID, int uniqueID, KPDBloodType bt, std::vector<std::string> hla, 
		KPDRelationToCandidate relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse);
	~KPDDonor();	

	//Getters
	int getMatchingID();
	int getUniqueID();

	KPDBloodType getBT();
	std::vector<std::string> getHLA();

	KPDRelationToCandidate getRelationToCandidate();
	int getAge();
	bool getMale();
	KPDRace getRace();
	double getHeight();
	double getWeight();
	double getBMI();
	bool getCigaretteUse();

	//Setters
	void setMatchingID(int id);
	void setUniqueID(int id);

	void setBT(KPDBloodType bt);
	void setHLA(std::vector<std::string> hla);

	void setRelationToCandidate(KPDRelationToCandidate relation);
	void setAge(int age);
	void setMale(bool genderMale);
	void setRace(KPDRace race);
	void setHeight(double height);
	void setWeight(double weight);
	void setCigaretteUse(bool cigaretteUse);

	//Strings
	std::string donorOutput(int space);
	std::string donorString();

	//Copy Constructor
	KPDDonor * copy();

};

//Default Donors
KPDDonor::KPDDonor() {

	matchingID = 0;
	uniqueID = 0;

	setBT(BT_O);

	setRelationToCandidate(UNRELATED);
	setAge(40);
	setMale(true);
	setRace(WHITE);
	setHeight(72);
	setWeight(150);
	setCigaretteUse(false);
}

KPDDonor::KPDDonor(KPDBloodType bt, std::vector<std::string> hla, KPDRelationToCandidate relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse) {

	matchingID = 0;
	uniqueID = 0;

	setBT(bt);
	setHLA(hla);

	setRelationToCandidate(relation);
	setAge(age);
	setMale(male);
	setRace(race);
	setHeight(height);
	setWeight(weight);
	setCigaretteUse(cigaretteUse);
}

KPDDonor::KPDDonor(int mID, int dID, KPDBloodType bt, std::vector<std::string> hla, KPDRelationToCandidate relation, int age, bool male, KPDRace race, double height, double weight, bool cigaretteUse) {

	setMatchingID(mID);
	setUniqueID(dID);

	setBT(bt);
	setHLA(hla);

	setRelationToCandidate(relation);
	setAge(age);
	setMale(male);
	setRace(race);
	setHeight(height);
	setWeight(weight);
	setCigaretteUse(cigaretteUse);
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

std::vector<std::string> KPDDonor::getHLA() {

	std::vector<std::string> hla;

	for (std::vector<std::string>::iterator it = donorHLA.begin(); it != donorHLA.end(); it++) {
		hla.push_back(*it);
	}

	return hla;
}

KPDRelationToCandidate KPDDonor::getRelationToCandidate() {
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

void KPDDonor::setMatchingID(int id) {
	matchingID = id;
}

void KPDDonor::setUniqueID(int id) {
	uniqueID = id;
}

void KPDDonor::setBT(KPDBloodType bt) {
	donorBT = bt;
}

void KPDDonor::setHLA(std::vector<std::string> hla) {

	donorHLA.clear();
	for (std::vector<std::string>::iterator hlaIt = hla.begin(); hlaIt != hla.end(); hlaIt++) {
		donorHLA.push_back(*hlaIt);
	}
}

void KPDDonor::setRelationToCandidate(KPDRelationToCandidate relation) {
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

std::string KPDDonor::donorString() {
	
	std::stringstream donorInfo;
	
	donorInfo << KPDFunctions::toString(donorBT) << ",";		// Blood Type
	donorInfo << KPDFunctions::toString(donorRelation) << ",";	// Relation to Candidate
	donorInfo << donorAge << ",";								// Age
	donorInfo << KPDFunctions::genderToString(donorMale) << ",";// Gender
	donorInfo << KPDFunctions::toString(donorRace) << ",";		// Race
	donorInfo << donorHeight << ",";							// Height
	donorInfo << donorWeight << ",";							// Weight
	donorInfo << (donorWeight/donorHeight)/donorHeight << ",";	// BMI
	donorInfo << KPDFunctions::boolToString(donorCigaretteUse);	// Cigarette Use
	
	return donorInfo.str();
}

std::string KPDDonor::donorOutput(int space) {

	std::stringstream indent;

	for (int i = 1; i <= space; i++) {
		indent << " ";
	}

	std::stringstream donorInfo;

	if (!(matchingID == 0 && uniqueID == 0)) {
		donorInfo << indent.str() << "Donor " << uniqueID << " (Matching ID: " << matchingID << ")" << std::endl;
	}
	else {
		donorInfo << indent.str() << "Donor:" << std::endl;
	}
	donorInfo << indent.str() << "BT: " << KPDFunctions::toString(donorBT) << std::endl;
	donorInfo << indent.str() << "HLA:";
	for (std::vector<std::string>::iterator hla = donorHLA.begin(); hla != donorHLA.end(); hla++) {
		donorInfo << " " << *hla;
	}
	donorInfo << std::endl;

	donorInfo << indent.str() << "Relation to Candidate: " << KPDFunctions::toString(donorRelation) << std::endl;
	donorInfo << indent.str() << "Age: " << donorAge << ", Sex: " << KPDFunctions::genderToString(donorMale) << ", Race: " << KPDFunctions::toString(donorRace) << std::endl;
	donorInfo << indent.str() << "Height: " << donorHeight << ", Weight: " << donorWeight << ", BMI: " << (donorWeight / donorHeight) / donorHeight << std::endl;
	donorInfo << indent.str() << "Cigarette Use: " << KPDFunctions::boolToString(donorCigaretteUse) << std::endl;

	return donorInfo.str();
}

KPDDonor * KPDDonor::copy() {

	KPDDonor * copyDonor = new KPDDonor(getMatchingID(), getUniqueID(), getBT(), getHLA(), 
		getRelationToCandidate(), getAge(), getMale(), getRace(), getHeight(), getWeight(), getCigaretteUse());

	return copyDonor;
}

#endif