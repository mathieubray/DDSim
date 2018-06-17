
#ifndef CANDIDATE_H
#define CANDIDATE_H

#include "DD-Enums-Functions.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

class KPDCandidate {

private:

	//IDs
	int matchingID;
	int uniqueID;
	
	//Crossmatch Information
	int candidatePRA;
	KPDBloodType candidateBT;

	std::vector<std::string> candidateUnacceptableHLA;
	std::vector<std::string> candidateDesensitizableHLA;

	//Characteristics
	int candidateAge;
	bool candidateMale;
	KPDRace candidateRace;
	bool candidateDiabetes;
	double candidateHeight;
	double candidateWeight;
	bool candidatePrevTrans;
	double candidateTOD;
	bool candidateHepC;
	KPDInsurance candidateInsurance;

public:

	KPDCandidate(int pra, KPDBloodType bt, std::vector<std::string> unacceptableHLA, std::vector<std::string> desensitizableHLA,
		int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance);
	KPDCandidate(int matchingID, int uniqueID, 
		int pra, KPDBloodType bt, std::vector<std::string> unacceptableHLA, std::vector<std::string> desensitizableHLA,
		int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance);
	~KPDCandidate();
	
	//Getters
	int getMatchingID();
	int getUniqueID();

	int getPRA();
	KPDBloodType getBT();

	std::vector<std::string> getUnacceptableHLA();
	std::vector<std::string> getDesensitizableHLA();

	int getAge();
	bool getMale();
	KPDRace getRace();
	bool getDiabetes();
	double getHeight();
	double getWeight();
	double getBMI();
	bool getPrevTrans();
	double getTOD();
	bool getHepC();
	KPDInsurance getInsurance();

	//Setters
	void setMatchingID(int id);
	void setUniqueID(int id);

	void setPRA(int pra);
	void setBT(KPDBloodType bt);

	void setUnacceptableHLA(std::vector<std::string> hla);
	void setDesensitizableHLA(std::vector<std::string> hla);

	void setAge(int age);
	void setMale(bool genderMale);
	void setRace(KPDRace race);
	void setDiabetes(bool diabetes);
	void setHeight(double height);
	void setWeight(double weight);
	void setPrevTrans(bool prevTrans);
	void setTOD(double tod);
	void setHepC(bool hepC);
	void setInsurance(KPDInsurance insurance);

	//Strings
	std::string candidateOutput(int space);
	std::string candidateString();

	//Copy Constructor
	KPDCandidate * copy();

};

KPDCandidate::KPDCandidate(int pra, KPDBloodType bt, std::vector<std::string> unacceptableHLA, std::vector<std::string> desensitizableHLA,
	int age, bool male,	KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance){
	
	matchingID = 0;
	uniqueID = 0;

	setPRA(pra);
	setBT(bt);

	setUnacceptableHLA(unacceptableHLA);
	setDesensitizableHLA(desensitizableHLA);

	setAge(age);
	setMale(male);
	setRace(race);
	setDiabetes(diabetes);
	setHeight(height);
	setWeight(weight);
	setPrevTrans(prevTrans);
	setTOD(tod);
	setHepC(hepC);
	setInsurance(insurance);
}

KPDCandidate::KPDCandidate(int mID, int cID, 
	int pra, KPDBloodType bt, std::vector<std::string> unacceptableHLA, std::vector<std::string> desensitizableHLA,
	int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance) {

	setMatchingID(mID);
	setUniqueID(cID);

	setPRA(pra);
	setBT(bt);

	setUnacceptableHLA(unacceptableHLA);
	setDesensitizableHLA(desensitizableHLA);

	setAge(age);
	setMale(male);
	setRace(race);
	setDiabetes(diabetes);
	setHeight(height);
	setWeight(weight);
	setPrevTrans(prevTrans);
	setTOD(tod);
	setHepC(hepC);
	setInsurance(insurance);
}

KPDCandidate::~KPDCandidate(){

}

int KPDCandidate::getMatchingID() {
	return matchingID;
}

int KPDCandidate::getUniqueID() {
	return uniqueID;
}

int KPDCandidate::getPRA() {
	return candidatePRA;
}

KPDBloodType KPDCandidate::getBT() {
	return candidateBT;
}

std::vector<std::string> KPDCandidate::getUnacceptableHLA() {

	std::vector<std::string> hla;

	for (std::vector<std::string>::iterator it = candidateUnacceptableHLA.begin(); it != candidateUnacceptableHLA.end(); it++) {
		hla.push_back(*it);
	}

	return hla;
}

std::vector<std::string> KPDCandidate::getDesensitizableHLA() {

	std::vector<std::string> hla;

	for (std::vector<std::string>::iterator it = candidateDesensitizableHLA.begin(); it != candidateDesensitizableHLA.end(); it++) {
		hla.push_back(*it);
	}

	return hla;
}

int KPDCandidate::getAge() {
	return candidateAge;
}

bool KPDCandidate::getMale(){
	return candidateMale;
}

KPDRace KPDCandidate::getRace(){
	return candidateRace;
}

double KPDCandidate::getHeight() {
	return candidateHeight;
}

double KPDCandidate::getWeight(){
	return candidateWeight;
}

double KPDCandidate::getBMI(){
	return (candidateWeight/candidateHeight)/candidateHeight;
}

bool KPDCandidate::getDiabetes(){
	return candidateDiabetes;
}

bool KPDCandidate::getPrevTrans(){
	return candidatePrevTrans;
}

double KPDCandidate::getTOD(){
	return candidateTOD;
}

bool KPDCandidate::getHepC(){
	return candidateHepC;
}

KPDInsurance KPDCandidate::getInsurance() {
	return candidateInsurance;
}

void KPDCandidate::setMatchingID(int id) {
	matchingID = id;
}

void KPDCandidate::setUniqueID(int id) {
	uniqueID = id;
}

void KPDCandidate::setPRA(int pra){
	candidatePRA = pra;
}

void KPDCandidate::setBT(KPDBloodType bt){
	candidateBT = bt;
}

void KPDCandidate::setUnacceptableHLA(std::vector<std::string> unacceptableHLA) {
	
	candidateUnacceptableHLA.clear();
	for (std::vector<std::string>::iterator it = unacceptableHLA.begin(); it != unacceptableHLA.end(); it++) {
		candidateUnacceptableHLA.push_back(*it);
	}
}

void KPDCandidate::setDesensitizableHLA(std::vector<std::string> desensitizableHLA) {
	
	candidateDesensitizableHLA.clear();
	for (std::vector<std::string>::iterator it = desensitizableHLA.begin(); it != desensitizableHLA.end(); it++) {
		candidateDesensitizableHLA.push_back(*it);
	}
}

void KPDCandidate::setAge(int age){
	candidateAge = age;
}

void KPDCandidate::setMale(bool genderMale){
	candidateMale = genderMale;
}

void KPDCandidate::setRace(KPDRace race){
	candidateRace = race;
}

void KPDCandidate::setHeight(double height) {
	candidateHeight = height;
}

void KPDCandidate::setWeight(double weight){
	candidateWeight = weight;
}

void KPDCandidate::setDiabetes(bool diabetes){
	candidateDiabetes = diabetes;
}

void KPDCandidate::setPrevTrans(bool prevTrans){
	candidatePrevTrans = prevTrans;
}

void KPDCandidate::setTOD(double tod){
	candidateTOD = tod;
}

void KPDCandidate::setHepC(bool hepC){
	candidateHepC = hepC;
}

void KPDCandidate::setInsurance(KPDInsurance insurance) {
	candidateInsurance = insurance;
}

std::string KPDCandidate::candidateString() {

	std::stringstream candidateInfo;

	candidateInfo << KPDFunctions::toString(candidateBT) << ",";			// Blood Type
	candidateInfo << candidatePRA << ",";									// PRA
	candidateInfo << candidateAge << ",";									// Age
	candidateInfo << KPDFunctions::genderToString(candidateMale) << ",";	// Gender
	candidateInfo << KPDFunctions::toString(candidateRace) << ",";			// Race
	candidateInfo << KPDFunctions::boolToString(candidateDiabetes) << ",";	// Diabetes
	candidateInfo << candidateHeight << ",";								// Height
	candidateInfo << candidateWeight << ",";								// Weight
	candidateInfo << candidateWeight/candidateHeight/candidateHeight << ",";// BMI
	candidateInfo << KPDFunctions::boolToString(candidatePrevTrans) << ",";	// Previous Transplants
	candidateInfo << candidateTOD << ",";									// Time on Dialysis
	candidateInfo << KPDFunctions::boolToString(candidateHepC) << ",";		// Hep C Status
	candidateInfo << KPDFunctions::toString(candidateInsurance);			// Insurance

	return candidateInfo.str();
}

std::string KPDCandidate::candidateOutput(int space) {

	std::stringstream indent;

	for (int i = 1; i <= space; i++) {
		indent << " ";
	}

	std::stringstream candidateInfo;

	if (!(matchingID == 0 && uniqueID==0)) {
		candidateInfo << indent.str() << "Candidate: " << uniqueID << " (Matching ID: " << matchingID << ")" << std::endl;
	}
	else {
		candidateInfo << indent.str() << "Candidate:" << std::endl;
	}
	candidateInfo << indent.str() << "PRA: " << candidatePRA <<  ", BT: " << KPDFunctions::toString(candidateBT) << std::endl;
	candidateInfo << indent.str() << "Unacceptable HLA:";
	for (std::vector<std::string>::iterator hla = candidateUnacceptableHLA.begin(); hla != candidateUnacceptableHLA.end(); hla++) {
		candidateInfo << " " << *hla;		
	}
	candidateInfo << std::endl;
	candidateInfo << indent.str() << "Desensitizable HLA:";
	for (std::vector<std::string>::iterator hla = candidateDesensitizableHLA.begin(); hla != candidateDesensitizableHLA.end(); hla++) {
		candidateInfo << " " << *hla;
	}
	candidateInfo << std::endl;
	candidateInfo << indent.str() << "Age: " << candidateAge << ", Sex: " << KPDFunctions::genderToString(candidateMale) << ", Race: " << KPDFunctions::toString(candidateRace) << ", Diabetes: " << KPDFunctions::boolToString(candidateDiabetes) << std::endl;
	candidateInfo << indent.str() << "Height:" << candidateHeight << ", Weight: " << candidateWeight << ", BMI: " << (candidateWeight/candidateHeight)/candidateHeight  << std::endl;
	candidateInfo << indent.str() << "Previous Transplant: " << KPDFunctions::boolToString(candidatePrevTrans) << ", Time on Dialysis: " << candidateTOD << std::endl;
	candidateInfo << indent.str() << "Hep C: " << KPDFunctions::boolToString(candidateHepC) << ", Inusrance: " << KPDFunctions::toString(candidateInsurance) << std::endl;
	
	return candidateInfo.str();

}

KPDCandidate * KPDCandidate::copy(){
	
	KPDCandidate * copyCandidate = new KPDCandidate(getMatchingID(), getUniqueID(),
		getPRA(), getBT(), getUnacceptableHLA(), getDesensitizableHLA(),
		getAge(), getMale(), getRace(), getDiabetes(), getHeight(), getWeight(), getPrevTrans(), getTOD(), getHepC(), getInsurance());

	return copyCandidate;
}


#endif