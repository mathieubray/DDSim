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
	int candidateID;
	int candidateKPDMatchingID;
	
	//Crossmatch Information
	int candidatePRA;
	KPDBloodType candidateBT;
	bool candidateMinorA;

	std::vector<std::string> candidateHLA;
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
	double candidateEPTS;
	bool candidateEPTSPriority;

	//Waitlist Information
	bool candidateWaitlist;
	int candidateListingTime;
	std::vector<int> candidateStatusChangeTimes;
	std::vector<KPDStatus> candidateStatuses;
	int candidateOPO;

	bool candidateWithdraws;
	int candidateWithdrawalTime;


public:

	//Constructors

	//Constructs a dummy candidate
	KPDCandidate();
	//Constructs a KPD candidate from APD data
	KPDCandidate(int id, int matchingID, int pra, KPDBloodType bt,
		int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance);
	//Constructs a waitlist candidate from SRTR data
	KPDCandidate(int id, int pra, KPDBloodType bt, bool minorA,
		int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance,  double epts, bool eptsPriority,
		int listingTime, int opo, bool withdraw, int withdrawTime);
	//Constructs a candidate with all defined candidate characteristics
	KPDCandidate(int id, int matchingID, int pra, KPDBloodType bt, bool minorA,
		int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance, double epts, bool eptsPriority,
		bool waitlist, int listingTime, int opo, bool withdraw, int withdrawTime);
	~KPDCandidate();
	
	//Getters
	int getCandidateID();
	int getKPDMatchingID();

	int getPRA();
	KPDBloodType getBT();
	bool getMinorA();

	std::vector<std::string> getHLA();
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
	double getEPTS();
	bool getEPTSPriority();

	bool getWaitlist();
	int getListingTime();
	std::deque<int> getStatusChangeTimes();
	std::deque<KPDStatus> getStatuses();
	int getOPO();

	bool getWithdraws();
	int getWithdrawalTime();

	//Setters
	void setCandidateID(int id);
	void setKPDMatchingID(int id);

	void setPRA(int pra);
	void setBT(KPDBloodType bt);
	void setMinorA(bool minorA);

	void setHLA(std::vector<std::string> hla);
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
	void setEPTS(double epts);
	void setEPTSPriority(bool priority);

	void setWaitlist(bool waitlist);
	void setListingTime(int time);
	void setStatusChangeTime(std::vector<int> time);
	void setStatusChange(std::vector<bool> statuses);
	void setStatusChanges(std::vector<KPDStatus> statuses);
	void addStatusChangeTime(int time);
	void addStatusChange(bool status);
	void setOPO(int opo);

	void setWithdraws(bool withdraw);
	void setWithdrawalTime(int withdrawalTime);

	//Strings
	std::string candidateString(); // Returns a comma-separated string
	std::string candidateOutput(); // Returns detailed candidate information

	//Copy Constructor
	KPDCandidate * copy();

};

KPDCandidate::KPDCandidate() {

	candidateID = -1;
	candidateKPDMatchingID = -1;
	
	candidatePRA = 0;
	candidateBT = BT_UNSPECIFIED;
	candidateMinorA = false;

	candidateAge = 40;
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
	candidateOPO = -1;

	candidateWithdraws = false;
	candidateWithdrawalTime = -1;
}

KPDCandidate::KPDCandidate(int id, int matchingID, int pra, KPDBloodType bt,
	int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance) {

	candidateID = id;
	candidateKPDMatchingID = matchingID;

	candidatePRA = pra;
	candidateBT = bt;
	candidateMinorA = false;

	candidateAge = age;
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
	candidateOPO = -1;

	candidateWithdraws = false;
	candidateWithdrawalTime = -1;
}

KPDCandidate::KPDCandidate(int id, int pra, KPDBloodType bt, bool minorA, 
	int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance,  double epts, bool eptsPriority,
	int listingTime, int opo, bool withdraw, int withdrawalTime) {

	candidateID = id;
	candidateKPDMatchingID = -1;

	candidatePRA = pra;
	candidateBT = bt;
	candidateMinorA = minorA;

	candidateAge = age;
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
	candidateOPO = opo;

	candidateWithdraws = withdraw;
	candidateWithdrawalTime = withdrawalTime;
}

KPDCandidate::KPDCandidate(int id, int matchingID, int pra, KPDBloodType bt, bool minorA,
	int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance, double epts, bool eptsPriority,
	bool waitlist, int listingTime, int opo, bool withdraw, int withdrawalTime) {

	candidateID = id;
	candidateKPDMatchingID = matchingID;

	candidatePRA = pra;
	candidateBT = bt;
	candidateMinorA = minorA;

	candidateAge = age;
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
	candidateOPO = opo;

	candidateWithdraws = withdraw;
	candidateWithdrawalTime = withdrawalTime;

}

KPDCandidate::~KPDCandidate(){

}

int KPDCandidate::getCandidateID() {
	return candidateID;
}

int KPDCandidate::getKPDMatchingID() {
	return candidateKPDMatchingID;
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

std::vector<std::string> KPDCandidate::getHLA() {
	
	std::vector<std::string> hla;

	for (std::vector<std::string>::iterator it = candidateHLA.begin(); it != candidateHLA.end(); it++) {
		hla.push_back(*it);
	}

	return hla;
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

double KPDCandidate::getEPTS(){
	return candidateEPTS;
}

bool KPDCandidate::getEPTSPriority(){
	return candidateEPTSPriority;
}

bool KPDCandidate::getWaitlist() {
	return candidateWaitlist;
}

int KPDCandidate::getListingTime() {
	return candidateListingTime;
}

std::deque<int> KPDCandidate::getStatusChangeTimes() {

	std::deque<int> statusChangeTimes;
	for (std::vector<int>::iterator it = candidateStatusChangeTimes.begin(); it != candidateStatusChangeTimes.end(); it++) {
		statusChangeTimes.push_back(*it);
	}

	if (candidateWithdraws) {
		statusChangeTimes.push_back(candidateWithdrawalTime);
	}

	return statusChangeTimes;
}

std::deque<KPDStatus> KPDCandidate::getStatuses() {

	std::deque<KPDStatus> statuses;
	for (std::vector<KPDStatus>::iterator it = candidateStatuses.begin(); it != candidateStatuses.end(); it++) {
		statuses.push_back(*it);
	}

	if (candidateWithdraws) {
		statuses.push_back(STATUS_WITHDRAWN);
	}

	return statuses;
}

int KPDCandidate::getOPO() {
	return candidateOPO;
}

bool KPDCandidate::getWithdraws() {
	return candidateWithdraws;
}

int KPDCandidate::getWithdrawalTime() {
	return candidateWithdrawalTime;
}

void KPDCandidate::setCandidateID(int id) {
	candidateID = id;
}

void KPDCandidate::setKPDMatchingID(int id) {
	candidateKPDMatchingID = id;
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

void KPDCandidate::setHLA(std::vector<std::string> hla) {

	candidateHLA.clear();
	for (std::vector<std::string>::iterator it = hla.begin(); it != hla.end(); it++) {
		candidateHLA.push_back(*it);
	}
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

void KPDCandidate::setEPTS(double epts){
	candidateEPTS = epts;
}

void KPDCandidate::setEPTSPriority(bool priority){
	candidateEPTSPriority = priority;
}

void KPDCandidate::setWaitlist(bool waitlist) {
	candidateWaitlist = waitlist;
}

void KPDCandidate::setListingTime(int time) {
	candidateListingTime = time;
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

void KPDCandidate::setOPO(int opo) {
	candidateOPO = opo;
}

void KPDCandidate::setWithdraws(bool withdraw) {
	candidateWithdraws = withdraw;
}

void KPDCandidate::setWithdrawalTime(int withdrawalTime) {
	candidateWithdrawalTime = withdrawalTime;
}

std::string KPDCandidate::candidateString() {

	std::stringstream candidateInfo;

	if (candidateWaitlist) {
		candidateInfo << KPDFunctions::bloodTypeToString(candidateBT) << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidateMinorA) << ",";
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
		candidateInfo << candidateEPTS << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidateEPTSPriority) << ",";
		candidateInfo << candidateListingTime << ",";
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

		candidateInfo << KPDFunctions::indent(tab) << "Candidate: " << candidateID << std::endl;
		candidateInfo << KPDFunctions::indent(tab) << "Listing Time: " << candidateListingTime << std::endl;
				
		for (int i = 1; i <= candidateStatusChangeTimes.size(); i++) {
			candidateInfo << KPDFunctions::indent(tab) << "Status at " << candidateStatusChangeTimes[i - 1] << ": " << KPDFunctions::statusToString(candidateStatuses[i - 1]);			
			candidateInfo << std::endl;
		}
		if (candidateWithdraws) {
			candidateInfo << KPDFunctions::indent(tab) << "Status at " << candidateWithdrawalTime << ": Withdrawn";
		}

		candidateInfo << KPDFunctions::indent(tab) << "OPO: " << candidateOPO << std::endl;

	}
	else {
		candidateInfo << KPDFunctions::indent(tab) << "Candidate: " << candidateID << " (Matching ID: " << candidateKPDMatchingID << ")" << std::endl;
	}

	candidateInfo << KPDFunctions::indent(tab) << "PRA: " << candidatePRA <<  ", BT: " << KPDFunctions::bloodTypeToString(candidateBT) << ", Minor A2 Subtype: " << KPDFunctions::boolToYesNo(candidateMinorA) << std::endl;
	
	if (candidateWaitlist) {
		candidateInfo << KPDFunctions::indent(tab) << "HLA:";
		for (std::vector<std::string>::iterator hla = candidateHLA.begin(); hla != candidateHLA.end(); hla++) {
			candidateInfo << " " << *hla;
		}
	}
	else {
		candidateInfo << KPDFunctions::indent(tab) << "Unacceptable HLA:";
		for (std::vector<std::string>::iterator hla = candidateUnacceptableHLA.begin(); hla != candidateUnacceptableHLA.end(); hla++) {
			candidateInfo << " " << *hla;
		}
		candidateInfo << std::endl;

		candidateInfo << KPDFunctions::indent(tab) << "Desensitizable HLA:";
		for (std::vector<std::string>::iterator hla = candidateDesensitizableHLA.begin(); hla != candidateDesensitizableHLA.end(); hla++) {
			candidateInfo << " " << *hla;
		}
	}
	candidateInfo << std::endl;
	
	candidateInfo << KPDFunctions::indent(tab) << "Age: " << candidateAge << ", Sex: " << KPDFunctions::boolToMaleFemale(candidateMale) << ", Race: " << KPDFunctions::raceToString(candidateRace) << ", Diabetes: " << KPDFunctions::boolToYesNo(candidateDiabetes) << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Height: " << candidateHeight << ", Weight: " << candidateWeight << ", BMI: " << (candidateWeight/candidateHeight)/candidateHeight  << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Previous Transplant: " << KPDFunctions::boolToYesNo(candidatePrevTrans) << ", Time on Dialysis: " << candidateTOD << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "Hep C: " << KPDFunctions::boolToYesNo(candidateHepC) << ", Inusrance: " << KPDFunctions::insuranceToString(candidateInsurance) << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "EPTS: " << candidateEPTS << ", EPTS Priority: " << KPDFunctions::boolToYesNo(candidateEPTSPriority) << std::endl;

	candidateInfo << std::endl;
	
	return candidateInfo.str();

}

KPDCandidate * KPDCandidate::copy(){
	
	KPDCandidate * copyCandidate = new KPDCandidate(candidateID, candidateKPDMatchingID, candidatePRA, candidateBT, candidateMinorA,
		candidateAge, candidateMale, candidateRace, candidateDiabetes, candidateHeight, candidateWeight, candidatePrevTrans, candidateTOD, candidateHepC, candidateInsurance, candidateEPTS, candidateEPTSPriority,
		candidateWaitlist, candidateListingTime, candidateOPO, candidateWithdraws, candidateWithdrawalTime);
	
	copyCandidate->setUnacceptableHLA(candidateUnacceptableHLA);
	copyCandidate->setDesensitizableHLA(candidateDesensitizableHLA);
	copyCandidate->setHLA(candidateHLA);
	copyCandidate->setStatusChangeTime(candidateStatusChangeTimes);
	copyCandidate->setStatusChanges(candidateStatuses);

	return copyCandidate;
}


#endif