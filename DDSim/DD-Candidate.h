/* ---------------------------------------------
DD-Candidate.h
Stores information about candidates (KPD and waitlist)
---------------------------------------------- */

#ifndef CANDIDATE_H
#define CANDIDATE_H

#include "DD-Enums-Functions.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <deque>

class KPDCandidate {

private:

	//IDs
	int matchingID;
	int uniqueID;
	
	//Crossmatch Information
	int candidatePRA;
	KPDBloodType candidateBT;
	bool candidateMinorA;

	std::vector<std::string> candidateUnacceptableHLA;
	std::vector<std::string> candidateDesensitizableHLA;

	//Characteristics
	int candidateAge;
	KPDAgeCategory candidateAgeCategory;
	bool candidateMale;
	KPDRace candidateRace;
	bool candidateDiabetes;
	double candidateHeight;
	double candidateWeight;
	bool candidatePrevTrans;
	double candidateTOD;
	bool candidateHepC;
	KPDInsurance candidateInsurance;
	double candidateEPTS;
	bool candidateEPTSPriority;
	std::vector<std::string> candidateHLA;

	//Waitlist Information
	bool candidateWaitlist;
	int candidateListingTime;
	int candidateListingYear;
	int candidateRemovalTime;
	bool candidateWithdrawn;
	std::vector<int> candidateStatusChangeTimes;
	std::vector<KPDStatus> candidateStatuses;
	int candidateCenterID;
	int candidateOPO;


public:

	//Constructors

	//Constructs a dummy candidate
	KPDCandidate();
	//Constructs a KPD candidate from APD data
	KPDCandidate(int mID, int uID, int pra, KPDBloodType bt,
		int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance);
	//Constructs a waitlist candidate from SRTR data
	KPDCandidate(int uID, int pra, KPDBloodType bt, bool minorA,
		KPDAgeCategory ageCat, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance,  double epts, bool eptsPriority,
		int listingTime, int listingYear, int removalTime, bool withdrawn, int centerID, int opoID);
	//Constructs a candidate with all defined candidate characteristics
	KPDCandidate(int mID, int uID, int pra, KPDBloodType bt, bool minorA,
		int age, KPDAgeCategory ageCat, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance, double epts, bool eptsPriority,
		bool waitlist, int listingTime, int listingYear, int removalTime, bool withdrawn, int centerID, int opoID);
	~KPDCandidate();
	
	//Getters
	int getMatchingID();
	int getUniqueID();

	int getPRA();
	KPDBloodType getBT();
	bool getMinorA();

	std::vector<std::string> getUnacceptableHLA();
	std::vector<std::string> getDesensitizableHLA();

	int getAge();
	KPDAgeCategory getAgeCategory();
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
	double getEPTS();
	bool getEPTSPriority();
	std::vector<std::string> getHLA();

	bool getWaitlist();
	int getListingTime();
	int getListingYear();
	int getRemovalTime();
	bool getWithdrawn();
	std::deque<int> getStatusChangeTimes();
	std::deque<KPDStatus> getStatuses();
	int getCenterID();
	int getOPO();

	//Setters
	void setMatchingID(int id);
	void setUniqueID(int id);

	void setPRA(int pra);
	void setBT(KPDBloodType bt);
	void setMinorA(bool minorA);

	void setUnacceptableHLA(std::vector<std::string> hla);
	void setDesensitizableHLA(std::vector<std::string> hla);

	void setAge(int age);
	void setAgeCategory(KPDAgeCategory ageCategory);
	void setMale(bool genderMale);
	void setRace(KPDRace race);
	void setDiabetes(bool diabetes);
	void setHeight(double height);
	void setWeight(double weight);
	void setPrevTrans(bool prevTrans);
	void setTOD(double tod);
	void setHepC(bool hepC);
	void setInsurance(KPDInsurance insurance);
	void setEPTS(double epts);
	void setEPTSPriority(bool priority);
	void setHLA(std::vector<std::string> hla);

	void setWaitlist(bool waitlist);
	void setListingTime(int time);
	void setListingYear(int year);
	void setRemovalTime(int time);
	void setWithdrawn(bool withdrawn);
	void setStatusChangeTime(std::vector<int> time);
	void setStatusChange(std::vector<bool> statuses);
	void setStatusChanges(std::vector<KPDStatus> statuses);
	void addStatusChangeTime(int time);
	void addStatusChange(bool status);
	void setCenterID(int id);
	void setOPO(int opo);

	//Strings
	std::string candidateString(); // Returns a comma-separated string
	std::string candidateOutput(); // Returns detailed candidate information

	//Copy Constructor
	KPDCandidate * copy();

};

KPDCandidate::KPDCandidate() {

	matchingID = -1;
	uniqueID = -1;
	
	candidatePRA = 0;
	candidateBT = BT_UNSPECIFIED;
	candidateMinorA = false;

	candidateAge = 40;
	candidateAgeCategory = AGE_UNSPECIFIED;
	candidateMale = true;
	candidateRace = RACE_UNSPECIFIED;
	candidateDiabetes = false;
	candidateHeight = 1.75;
	candidateWeight = 80;
	candidatePrevTrans = false;
	candidateTOD = 0;
	candidateHepC = false;
	candidateInsurance = INSURANCE_UNSPECIFIED;
	candidateEPTS = 0;
	candidateEPTSPriority = false;
	
	candidateWaitlist = false;
	candidateListingTime = -1;
	candidateListingYear = -1;
	candidateRemovalTime = -1;
	candidateWithdrawn = true;
	candidateCenterID = -1;
	candidateOPO = -1;
}

KPDCandidate::KPDCandidate(int mID, int uID, int pra, KPDBloodType bt,
	int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance) {

	matchingID = mID;
	uniqueID = uID;

	candidatePRA = pra;
	candidateBT = bt;
	candidateMinorA = false;

	candidateAge = age;
	candidateAgeCategory = AGE_UNSPECIFIED;
	candidateMale = male;
	candidateRace = race;
	candidateDiabetes = diabetes;
	candidateHeight = height;
	candidateWeight = weight;
	candidatePrevTrans = prevTrans;
	candidateTOD = tod;
	candidateHepC = hepC;
	candidateInsurance = insurance;
	candidateEPTS = 0;
	candidateEPTSPriority = false;

	candidateWaitlist = false;
	candidateListingTime = -1;
	candidateListingYear = -1;
	candidateRemovalTime = -1;
	candidateWithdrawn = true;
	candidateCenterID = -1;
	candidateOPO = -1;
}

KPDCandidate::KPDCandidate(int uID, int pra, KPDBloodType bt, bool minorA, 
	KPDAgeCategory ageCat, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance,  
	double epts, bool eptsPriority,
	int listingTime, int listingYear, int removalTime, bool withdrawn, int centerID, int opoID) {

	matchingID = -1;
	uniqueID = uID;

	candidatePRA = pra;
	candidateBT = bt;
	candidateMinorA = minorA;

	candidateAge = -1;
	candidateAgeCategory = ageCat;
	candidateMale = male;
	candidateRace = race;
	candidateDiabetes = diabetes;
	candidateHeight = height;
	candidateWeight = weight;
	candidatePrevTrans = prevTrans;
	candidateTOD = tod;
	candidateHepC = hepC;
	candidateInsurance = insurance;
	candidateEPTS = epts;
	candidateEPTSPriority = eptsPriority;

	candidateWaitlist = true;
	candidateListingTime = listingTime;
	candidateListingYear = listingYear;
	candidateRemovalTime = removalTime;
	candidateWithdrawn = withdrawn;
	candidateCenterID = centerID;
	candidateOPO = opoID;
}

KPDCandidate::KPDCandidate(int mID, int uID, int pra, KPDBloodType bt, bool minorA,
	int age, KPDAgeCategory ageCat, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance, 
	double epts, bool eptsPriority,
	bool waitlist, int listingTime, int listingYear, int removalTime, bool withdrawn, int centerID, int opoID) {

	matchingID = mID;
	uniqueID = uID;

	candidatePRA = pra;
	candidateBT = bt;
	candidateMinorA = minorA;

	candidateAge = age;
	candidateAgeCategory = ageCat;
	candidateMale = male;
	candidateRace = race;
	candidateDiabetes = diabetes;
	candidateHeight = height;
	candidateWeight = weight;
	candidatePrevTrans = prevTrans;
	candidateTOD = tod;
	candidateHepC = hepC;
	candidateInsurance = insurance;
	candidateEPTS = epts;
	candidateEPTSPriority = eptsPriority;

	candidateWaitlist = waitlist;
	candidateListingTime = listingTime;
	candidateListingYear = listingYear;
	candidateRemovalTime = removalTime;
	candidateWithdrawn = withdrawn;
	candidateCenterID = centerID;
	candidateOPO = opoID;

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

bool KPDCandidate::getMinorA(){
	return candidateMinorA;
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

KPDAgeCategory KPDCandidate::getAgeCategory(){
	return candidateAgeCategory;
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

double KPDCandidate::getEPTS(){
	return candidateEPTS;
}

bool KPDCandidate::getEPTSPriority(){
	return candidateEPTSPriority;
}

std::vector<std::string> KPDCandidate::getHLA(){
	return candidateHLA;
}

bool KPDCandidate::getWaitlist() {
	return candidateWaitlist;
}

int KPDCandidate::getListingTime() {
	return candidateListingTime;
}

int KPDCandidate::getListingYear() {
	return candidateListingYear;
}

int KPDCandidate::getRemovalTime() {
	return candidateRemovalTime;
}

bool KPDCandidate::getWithdrawn() {
	return candidateWithdrawn;
}

std::deque<int> KPDCandidate::getStatusChangeTimes() {

	std::deque<int> statusChangeTimes;
	for (std::vector<int>::iterator it = candidateStatusChangeTimes.begin(); it != candidateStatusChangeTimes.end(); it++) {
		statusChangeTimes.push_back(*it);
	}
	return statusChangeTimes;
}

std::deque<KPDStatus> KPDCandidate::getStatuses() {

	std::deque<KPDStatus> statuses;
	for (std::vector<KPDStatus>::iterator it = candidateStatuses.begin(); it != candidateStatuses.end(); it++) {
		statuses.push_back(*it);
	}
	return statuses;
}

int KPDCandidate::getCenterID() {
	return candidateCenterID;
}

int KPDCandidate::getOPO() {
	return candidateOPO;
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

void KPDCandidate::setMinorA(bool minorA){
	candidateMinorA = minorA;
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

void KPDCandidate::setAgeCategory(KPDAgeCategory ageCategory){
	candidateAgeCategory = ageCategory;
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

void KPDCandidate::setEPTS(double epts){
	candidateEPTS = epts;
}

void KPDCandidate::setEPTSPriority(bool priority){
	candidateEPTSPriority = priority;
}

void KPDCandidate::setHLA(std::vector<std::string> hla){

	candidateHLA.clear();
	for (std::vector<std::string>::iterator it = hla.begin(); it != hla.end(); it++) {
		candidateHLA.push_back(*it);
	}
}


void KPDCandidate::setWaitlist(bool waitlist) {
	candidateWaitlist = waitlist;
}

void KPDCandidate::setListingTime(int time) {
	candidateListingTime = time;
}

void KPDCandidate::setListingYear(int year) {
	candidateListingYear = year;
}

void KPDCandidate::setRemovalTime(int time) {
	candidateRemovalTime = time;
}

void KPDCandidate::setWithdrawn(bool withdrawn) {
	candidateWithdrawn = withdrawn;
}

void KPDCandidate::setStatusChangeTime(std::vector<int> times) {

	candidateStatusChangeTimes.clear();
	for (std::vector<int>::iterator it = times.begin(); it != times.end(); it++) {
		candidateStatusChangeTimes.push_back(*it);
	}
}

void KPDCandidate::setStatusChange(std::vector<bool> statuses) {

	candidateStatuses.clear();
	for (std::vector<bool>::iterator it = statuses.begin(); it != statuses.end(); it++) {

		if (*it) {
			candidateStatuses.push_back(STATUS_ACTIVE);
		}
		else {
			candidateStatuses.push_back(STATUS_INACTIVE);
		}
		
	}
}

void KPDCandidate::setStatusChanges(std::vector<KPDStatus> statuses) {

	candidateStatuses.clear();
	for (std::vector<KPDStatus>::iterator it = statuses.begin(); it != statuses.end(); it++) {
		candidateStatuses.push_back(*it);
	}
}

void KPDCandidate::addStatusChangeTime(int time) {
	candidateStatusChangeTimes.push_back(time);
}

void KPDCandidate::addStatusChange(bool status) {

	if (status) {
		candidateStatuses.push_back(STATUS_ACTIVE);
	}
	else {
		candidateStatuses.push_back(STATUS_INACTIVE);
	}
}

void KPDCandidate::setCenterID(int id) {
	candidateCenterID = id;
}

void KPDCandidate::setOPO(int opo) {
	candidateOPO = opo;
}

std::string KPDCandidate::candidateString() {

	std::stringstream candidateInfo;

	if (candidateWaitlist) {
		candidateInfo << KPDFunctions::bloodTypeToString(candidateBT) << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidateMinorA) << ",";
		candidateInfo << candidatePRA << ",";
		candidateInfo << candidateAge << ",";
		candidateInfo << KPDFunctions::ageCategoryToString(candidateAgeCategory) << ",";
		candidateInfo << KPDFunctions::boolToMaleFemale(candidateMale) << ",";
		candidateInfo << KPDFunctions::raceToString(candidateRace) << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidateDiabetes) << ",";
		candidateInfo << candidateHeight << ",";
		candidateInfo << candidateWeight << ",";
		candidateInfo << candidateWeight / candidateHeight / candidateHeight << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidatePrevTrans) << ",";
		candidateInfo << candidateTOD << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidateHepC) << ",";
		candidateInfo << KPDFunctions::insuranceToString(candidateInsurance);
		candidateInfo << candidateEPTS << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidateEPTSPriority) << ",";
		candidateInfo << candidateListingTime << ",";
		candidateInfo << candidateListingYear << ",";
		candidateInfo << candidateRemovalTime << ",";
		candidateInfo << KPDFunctions::boolToTF(candidateWithdrawn) << ",";
		candidateInfo << candidateCenterID << ",";
		candidateInfo << candidateOPO;
	
	}
	else {
		candidateInfo << KPDFunctions::bloodTypeToString(candidateBT) << ",";
		candidateInfo << candidatePRA << ",";
		candidateInfo << candidateAge << ",";
		candidateInfo << KPDFunctions::boolToMaleFemale(candidateMale) << ",";
		candidateInfo << KPDFunctions::raceToString(candidateRace) << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidateDiabetes) << ",";
		candidateInfo << candidateHeight << ",";
		candidateInfo << candidateWeight << ",";
		candidateInfo << candidateWeight / candidateHeight / candidateHeight << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidatePrevTrans) << ",";
		candidateInfo << candidateTOD << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidateHepC) << ",";
		candidateInfo << KPDFunctions::insuranceToString(candidateInsurance);
	}
	return candidateInfo.str();
}

std::string KPDCandidate::candidateOutput() {

	int tab = 3;

	std::stringstream candidateInfo;

	if (candidateWaitlist) {

		candidateInfo << KPDFunctions::indent(tab) << "Candidate: " << uniqueID << std::endl;
		candidateInfo << KPDFunctions::indent(tab) << "Listing Time: " << candidateListingTime << " (Year: " << candidateListingYear << ")" << std::endl;
		candidateInfo << KPDFunctions::indent(tab) << "Removal Time: " << candidateRemovalTime;
		if (candidateWithdrawn) {
			candidateInfo << "(Withdrawn)";
		}
		else {
			candidateInfo << "(Transplanted)";
		}
		candidateInfo << std::endl;
			
		for (int i = 1; i <= candidateStatusChangeTimes.size(); i++) {
			candidateInfo << KPDFunctions::indent(tab) << "Status at " << candidateStatusChangeTimes[i - 1] << ": " << KPDFunctions::statusToString(candidateStatuses[i - 1]);			
			candidateInfo << std::endl;
		}

		candidateInfo << KPDFunctions::indent(tab) << "Center: " << candidateCenterID << "; OPO: " << candidateOPO << std::endl;

	}
	else {

		if (!(matchingID == 0 && uniqueID == 0)) {
			candidateInfo << KPDFunctions::indent(tab) << "Candidate: " << uniqueID << " (Matching ID: " << matchingID << ")" << std::endl;
		}
		else {
			candidateInfo << KPDFunctions::indent(tab) << "Candidate:" << std::endl;
		}
	}



	candidateInfo << KPDFunctions::indent(tab) << "PRA: " << candidatePRA <<  ", BT: " << KPDFunctions::bloodTypeToString(candidateBT) << ", Minor A2 Subtype: " << KPDFunctions::boolToYesNo(candidateMinorA) << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Unacceptable HLA:";
	for (std::vector<std::string>::iterator hla = candidateUnacceptableHLA.begin(); hla != candidateUnacceptableHLA.end(); hla++) {
		candidateInfo << " " << *hla;		
	}
	candidateInfo << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Desensitizable HLA:";
	for (std::vector<std::string>::iterator hla = candidateDesensitizableHLA.begin(); hla != candidateDesensitizableHLA.end(); hla++) {
		candidateInfo << " " << *hla;
	}
	candidateInfo << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Age: " << candidateAge << ", Age Category: " << KPDFunctions::ageCategoryToString(candidateAgeCategory) << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Sex: " << KPDFunctions::boolToMaleFemale(candidateMale) << ", Race: " << KPDFunctions::raceToString(candidateRace) << ", Diabetes: " << KPDFunctions::boolToYesNo(candidateDiabetes) << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Height: " << candidateHeight << ", Weight: " << candidateWeight << ", BMI: " << (candidateWeight/candidateHeight)/candidateHeight  << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Previous Transplant: " << KPDFunctions::boolToYesNo(candidatePrevTrans) << ", Time on Dialysis: " << candidateTOD << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Hep C: " << KPDFunctions::boolToYesNo(candidateHepC) << ", Inusrance: " << KPDFunctions::insuranceToString(candidateInsurance) << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "EPTS: " << candidateEPTS << ", EPTS Priority: " << KPDFunctions::boolToYesNo(candidateEPTSPriority) << std::endl;

	candidateInfo << KPDFunctions::indent(tab) << "HLA:";
	for (std::vector<std::string>::iterator hla = candidateHLA.begin(); hla != candidateHLA.end(); hla++) {
		candidateInfo << " " << *hla;
	}
	candidateInfo << std::endl;
	
	return candidateInfo.str();

}

KPDCandidate * KPDCandidate::copy(){
	
	KPDCandidate * copyCandidate = new KPDCandidate(matchingID, uniqueID, candidatePRA, candidateBT, candidateMinorA,
		candidateAge, candidateAgeCategory, candidateMale, candidateRace, candidateDiabetes, candidateHeight, candidateWeight, candidatePrevTrans, candidateTOD, candidateHepC, candidateInsurance, candidateEPTS, candidateEPTSPriority,
		candidateWaitlist, candidateListingTime, candidateListingYear, candidateRemovalTime, candidateWithdrawn, candidateCenterID, candidateOPO);
	
	copyCandidate->setUnacceptableHLA(candidateUnacceptableHLA);
	copyCandidate->setDesensitizableHLA(candidateDesensitizableHLA);
	copyCandidate->setHLA(candidateHLA);
	copyCandidate->setStatusChangeTime(candidateStatusChangeTimes);
	copyCandidate->setStatusChanges(candidateStatuses);

	return copyCandidate;
}


#endif