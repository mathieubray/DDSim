/* ---------------------------------------------
DD-Candidate.h
Stores Information on KPD and Waitlist Candidates
---------------------------------------------- */

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
	KPDBloodTypeEnhanced candidateEnhancedBT;

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
	bool candidateAcceptsHCVPositive;
	double candidateEPTS;
	bool candidateEPTSPriority;
	std::vector<std::string> candidateHLA;

	//Waitlist Candidate Characteristics
	bool candidateWaitlist;
	int candidateListingYear;
	int candidateListingMonth;
	int candidateListingDay;
	int candidateRemovalYear;
	int candidateRemovalMonth;
	int candidateRemovalDay;
	std::vector<int> candidateStatusChangeYear;
	std::vector<int> candidateStatusChangeMonth;
	std::vector<int> candidateStatusChangeDay;
	std::vector<bool> candidateStatusActive;
	int candidateCenterID;
	int candidateOPO;

	//Transplant Characteristics (Remove?)
	bool candidateTransplanted;
	int transplantDonorID;
	bool transplantLivingDonor;
	bool transplantDeceasedDonor;
	KPDRelation transplantRelation;
	bool transplantBiological;
	bool transplantNonBiological;
	bool transplantPairedDonation;
	bool transplantNonDirectedDonation;


public:

	//Constructors

	//Constructs a dummy candidate
	KPDCandidate();
	//Constructs a KPD candidate from APD data
	KPDCandidate(int mID, int uID, int pra, KPDBloodType bt,
		int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance);
	//Constructs a waitlist candidate from SRTR data
	KPDCandidate(int uID, int pra, KPDBloodType bt, KPDBloodTypeEnhanced bte,
		KPDAgeCategory ageCat, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, KPDInsurance insurance, bool acceptsHCV, double epts, bool eptsPriority,
		int listingYear, int listingMonth, int listingDay, int removalYear, int removalMonth, int removalDay, int centerID, int opoID,
		bool transplanted, int dID, bool living, bool deceased, KPDRelation relation, bool biological, bool nonBiological, bool pairedDonation, bool nonDirectedDonation);
	//Constructs a candidate with all defined candidate characteristics
	KPDCandidate(int mID, int uID, int pra, KPDBloodType bt, KPDBloodTypeEnhanced bte,
		int age, KPDAgeCategory ageCat, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance, bool acceptsHCV, double epts, bool eptsPriority,
		bool waitlist, int listingYear, int listingMonth, int listingDay, int removalYear, int removalMonth, int removalDay, int centerID, int opoID,
		bool transplanted, int dID, bool living, bool deceased, KPDRelation relation, bool biological, bool nonBiological, bool pairedDonation, bool nonDirectedDonation);
	~KPDCandidate();
	
	//Getters
	int getMatchingID();
	int getUniqueID();

	int getPRA();
	KPDBloodType getBT();
	KPDBloodTypeEnhanced getEnhancedBT();

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
	bool getAcceptsHCVPostiive();
	double getEPTS();
	bool getEPTSPriority();
	std::vector<std::string> getHLA();

	bool getWaitlist();
	int getListingYear();
	int getListingMonth();
	int getListingDay();
	int getRemovalYear();
	int getRemovalMonth();
	int getRemovalDay();
	std::vector<int> getStatusChangeYears();
	std::vector<int> getStatusChangeMonths();
	std::vector<int> getStatusChangeDays();
	std::vector<bool> getStatuses();
	int getCenterID();
	int getOPO();

	bool getCandidateTransplanted();
	int getTransplantDonorID();
	bool getTransplantLivingDonor();
	bool getTransplantDeceasedDonor();
	KPDRelation getTransplantRelation();
	bool getTransplantBiological();
	bool getTransplantNonBiological();
	bool getTransplantPairedDonation();
	bool getTransplantNonDirectedDonation();

	//Setters
	void setMatchingID(int id);
	void setUniqueID(int id);

	void setPRA(int pra);
	void setBT(KPDBloodType bt);
	void setEnhancedBT(KPDBloodTypeEnhanced bte);

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
	void setAccpetsHCVPositive(bool accepts);
	void setEPTS(double epts);
	void setEPTSPriority(bool priority);
	void setHLA(std::vector<std::string> hla);

	void setWaitlist(bool waitlist);
	void setListingYear(int year);
	void setListingMonth(int month);
	void setListingDay(int day);
	void setRemovalYear(int year);
	void setRemovalMonth(int month);
	void setRemovalDay(int day);
	void setStatusChangeYear(std::vector<int> years);
	void setStatusChangeMonth(std::vector<int> months);
	void setStatusChangeDay(std::vector<int> days);
	void setStatusChange(std::vector<bool> statuses);
	void addStatusChangeYear(int year);
	void addStatusChangeMonth(int month);
	void addStatusChangeDay(int day);
	void addStatusChange(bool status);
	void setCenterID(int id);
	void setOPO(int opo);

	void setCandidateTransplanted(bool transplant);
	void setTransplantDonorID(int id);
	void setTransplantLivingDonor(bool living);
	void setTransplantDeceasedDonor(bool deceased);
	void setTransplantRelation(KPDRelation relation);
	void setTransplantBiological(bool biological);
	void setTransplantNonBiological(bool nonbiological);
	void setTransplantPairedDonation(bool pairedDonation);
	void setTransplantNonDirectedDonation(bool ndd);

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
	candidateEnhancedBT = BTE_UNSPECIFIED;

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
	candidateAcceptsHCVPositive = false;
	candidateEPTS = 0;
	candidateEPTSPriority = false;
	
	candidateWaitlist = false;
	candidateListingYear = 0;
	candidateListingMonth = 0;
	candidateListingDay = 0;
	candidateRemovalYear = 0;
	candidateRemovalMonth = 0;
	candidateRemovalDay = 0;
	candidateCenterID = -1;
	candidateOPO = -1;

	candidateTransplanted = false;
	transplantDonorID = -1;
	transplantLivingDonor = false;
	transplantDeceasedDonor = false;
	transplantRelation = RELATION_UNSPECIFIED;
	transplantBiological = false;
	transplantNonBiological = false;
	transplantPairedDonation = false;
	transplantNonDirectedDonation = false;
}

KPDCandidate::KPDCandidate(int mID, int uID, int pra, KPDBloodType bt,
	int age, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance) {

	matchingID = mID;
	uniqueID = uID;

	candidatePRA = pra;
	candidateBT = bt;
	candidateEnhancedBT = BTE_UNSPECIFIED;

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
	candidateAcceptsHCVPositive = false;
	candidateEPTS = 0;
	candidateEPTSPriority = false;

	candidateWaitlist = false;
	candidateListingYear = 0;
	candidateListingMonth = 0;
	candidateListingDay = 0;
	candidateRemovalYear = 0;
	candidateRemovalMonth = 0;
	candidateRemovalDay = 0;
	candidateCenterID = -1;
	candidateOPO = -1;

	candidateTransplanted = false;
	transplantDonorID = -1;
	transplantLivingDonor = false;
	transplantDeceasedDonor = false;
	transplantRelation = RELATION_UNSPECIFIED;
	transplantBiological = false;
	transplantNonBiological = false;
	transplantPairedDonation = false;
	transplantNonDirectedDonation = false;
}

KPDCandidate::KPDCandidate(int uID, int pra, KPDBloodType bt, KPDBloodTypeEnhanced bte, 
	KPDAgeCategory ageCat, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, KPDInsurance insurance, bool acceptsHCV, double epts, bool eptsPriority, 
	int listingYear, int listingMonth, int listingDay, int removalYear, int removalMonth, int removalDay, int centerID, int opoID, 
	bool transplanted, int dID, bool living, bool deceased, KPDRelation relation, bool biological, bool nonBiological, bool pairedDonation, bool nonDirectedDonation) {

	matchingID = -1;
	uniqueID = uID;

	candidatePRA = pra;
	candidateBT = bt;
	candidateEnhancedBT = bte;

	candidateAge = -1;
	candidateAgeCategory = ageCat;
	candidateMale = male;
	candidateRace = race;
	candidateDiabetes = diabetes;
	candidateHeight = height;
	candidateWeight = weight;
	candidatePrevTrans = prevTrans;
	candidateTOD = tod;
	candidateHepC = false;
	candidateInsurance = insurance;
	candidateAcceptsHCVPositive = acceptsHCV;
	candidateEPTS = epts;
	candidateEPTSPriority = eptsPriority;

	candidateWaitlist = true;
	candidateListingYear = listingYear;
	candidateListingMonth = listingMonth;
	candidateListingDay = listingDay;
	candidateRemovalYear = removalYear;
	candidateRemovalMonth = removalMonth;
	candidateRemovalDay = removalDay;
	candidateCenterID = centerID;
	candidateOPO = opoID;

	candidateTransplanted = transplanted;
	transplantDonorID = dID;
	transplantLivingDonor = living;
	transplantDeceasedDonor = deceased;
	transplantRelation = relation;
	transplantBiological = biological;
	transplantNonBiological = nonBiological;
	transplantPairedDonation = pairedDonation;
	transplantNonDirectedDonation = nonDirectedDonation;
}

KPDCandidate::KPDCandidate(int mID, int uID, int pra, KPDBloodType bt, KPDBloodTypeEnhanced bte,
	int age, KPDAgeCategory ageCat, bool male, KPDRace race, bool diabetes, double height, double weight, bool prevTrans, double tod, bool hepC, KPDInsurance insurance, bool acceptsHCV, double epts, bool eptsPriority,
	bool waitlist, int listingYear, int listingMonth, int listingDay, int removalYear, int removalMonth, int removalDay, int centerID, int opoID,
	bool transplanted, int dID, bool living, bool deceased, KPDRelation relation, bool biological, bool nonBiological, bool pairedDonation, bool nonDirectedDonation) {

	matchingID = mID;
	uniqueID = uID;

	candidatePRA = pra;
	candidateBT = bt;
	candidateEnhancedBT = bte;

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
	candidateAcceptsHCVPositive = acceptsHCV;
	candidateEPTS = epts;
	candidateEPTSPriority = eptsPriority;

	candidateWaitlist = waitlist;
	candidateListingYear = listingYear;
	candidateListingMonth = listingMonth;
	candidateListingDay = listingDay;
	candidateRemovalYear = removalYear;
	candidateRemovalMonth = removalMonth;
	candidateRemovalDay = removalDay;
	candidateCenterID = centerID;
	candidateOPO = opoID;

	candidateTransplanted = transplanted;
	transplantDonorID = dID;
	transplantLivingDonor = living;
	transplantDeceasedDonor = deceased;
	transplantRelation = relation;
	transplantBiological = biological;
	transplantNonBiological = nonBiological;
	transplantPairedDonation = pairedDonation;
	transplantNonDirectedDonation = nonDirectedDonation;

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

KPDBloodTypeEnhanced KPDCandidate::getEnhancedBT(){
	return candidateEnhancedBT;
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

bool KPDCandidate::getAcceptsHCVPostiive(){
	return candidateAcceptsHCVPositive;
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

int KPDCandidate::getListingYear() {
	return candidateListingYear;
}

int KPDCandidate::getListingMonth() {
	return candidateListingMonth;
}

int KPDCandidate::getListingDay() {
	return candidateListingDay;
}

int KPDCandidate::getRemovalYear() {
	return candidateRemovalYear;
}

int KPDCandidate::getRemovalMonth() {
	return candidateRemovalMonth;
}

int KPDCandidate::getRemovalDay() {
	return candidateRemovalDay;
}

std::vector<int> KPDCandidate::getStatusChangeYears() {
	return candidateStatusChangeYear;
}

std::vector<int> KPDCandidate::getStatusChangeMonths() {
	return candidateStatusChangeMonth;
}

std::vector<int> KPDCandidate::getStatusChangeDays() {
	return candidateStatusChangeDay;
}

std::vector<bool> KPDCandidate::getStatuses() {
	return candidateStatusActive;
}

int KPDCandidate::getCenterID() {
	return candidateCenterID;
}

int KPDCandidate::getOPO() {
	return candidateOPO;
}

bool KPDCandidate::getCandidateTransplanted() {
	return candidateTransplanted;
}

int KPDCandidate::getTransplantDonorID(){
	return transplantDonorID;
}

bool KPDCandidate::getTransplantLivingDonor(){
	return transplantLivingDonor;
}

bool KPDCandidate::getTransplantDeceasedDonor(){
	return transplantDeceasedDonor;
}

KPDRelation KPDCandidate::getTransplantRelation(){
	return transplantRelation;
}

bool KPDCandidate::getTransplantBiological(){
	return transplantBiological;
}

bool KPDCandidate::getTransplantNonBiological(){
	return transplantNonBiological;
}

bool KPDCandidate::getTransplantPairedDonation(){
	return transplantPairedDonation;
}

bool KPDCandidate::getTransplantNonDirectedDonation(){
	return transplantNonDirectedDonation;
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

void KPDCandidate::setEnhancedBT(KPDBloodTypeEnhanced bte){
	candidateEnhancedBT = bte;
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

void KPDCandidate::setAccpetsHCVPositive(bool accepts){
	candidateAcceptsHCVPositive = accepts;
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

void KPDCandidate::setListingYear(int year) {
	candidateListingYear = year;
}

void KPDCandidate::setListingMonth(int month) {
	candidateListingMonth = month;
}

void KPDCandidate::setListingDay(int day) {
	candidateListingDay = day;
}

void KPDCandidate::setRemovalYear(int year) {
	candidateRemovalYear = year;
}

void KPDCandidate::setRemovalMonth(int month) {
	candidateRemovalMonth = month;
}

void KPDCandidate::setRemovalDay(int day) {
	candidateRemovalDay = day;
}

void KPDCandidate::setStatusChangeYear(std::vector<int> years) {
	candidateStatusChangeYear.clear();
	for (std::vector<int>::iterator it = years.begin(); it != years.end(); it++) {
		candidateStatusChangeYear.push_back(*it);
	}
}

void KPDCandidate::setStatusChangeMonth(std::vector<int> months) {
	candidateStatusChangeMonth.clear();
	for (std::vector<int>::iterator it = months.begin(); it != months.end(); it++) {
		candidateStatusChangeMonth.push_back(*it);
	}
}

void KPDCandidate::setStatusChangeDay(std::vector<int> days) {
	candidateStatusChangeDay.clear();
	for (std::vector<int>::iterator it = days.begin(); it != days.end(); it++) {
		candidateStatusChangeDay.push_back(*it);
	}
}

void KPDCandidate::setStatusChange(std::vector<bool> statuses) {
	candidateStatusActive.clear();
	for (std::vector<bool>::iterator it = statuses.begin(); it != statuses.end(); it++) {
		candidateStatusActive.push_back(*it);
	}
}

void KPDCandidate::addStatusChangeYear(int year) {
	candidateStatusChangeYear.push_back(year);
}

void KPDCandidate::addStatusChangeMonth(int month) {
	candidateStatusChangeMonth.push_back(month);
}

void KPDCandidate::addStatusChangeDay(int day) {
	candidateStatusChangeDay.push_back(day);
}

void KPDCandidate::addStatusChange(bool status) {
	candidateStatusActive.push_back(status);
}

void KPDCandidate::setCenterID(int id) {
	candidateCenterID = id;
}

void KPDCandidate::setOPO(int opo) {
	candidateOPO = opo;
}

void KPDCandidate::setCandidateTransplanted(bool transplanted) {
	candidateTransplanted = transplanted;
}

void KPDCandidate::setTransplantDonorID(int id){
	transplantDonorID = id;
}

void KPDCandidate::setTransplantLivingDonor(bool living){
	transplantLivingDonor = living;
}

void KPDCandidate::setTransplantDeceasedDonor(bool deceased){
	transplantDeceasedDonor = deceased;
}

void KPDCandidate::setTransplantRelation(KPDRelation relation){
	transplantRelation = relation;
}

void KPDCandidate::setTransplantBiological(bool biological){
	transplantBiological = biological;
}

void KPDCandidate::setTransplantNonBiological(bool nonbiological){
	transplantNonBiological = nonbiological;
}

void KPDCandidate::setTransplantPairedDonation(bool pairedDonation){
	transplantPairedDonation = pairedDonation;
}

void KPDCandidate::setTransplantNonDirectedDonation(bool ndd){
	transplantNonDirectedDonation = ndd;
}

std::string KPDCandidate::candidateString() {

	std::stringstream candidateInfo;

	if (candidateWaitlist) {
		candidateInfo << KPDFunctions::bloodTypeToString(candidateBT) << ",";
		candidateInfo << KPDFunctions::bloodTypeEnhancedToString(candidateEnhancedBT) << ",";
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
		candidateInfo << KPDFunctions::boolToYesNo(candidateAcceptsHCVPositive) << ",";
		candidateInfo << candidateEPTS << ",";
		candidateInfo << KPDFunctions::boolToYesNo(candidateEPTSPriority) << ",";
		candidateInfo << candidateListingYear << ",";
		candidateInfo << candidateListingMonth << ",";
		candidateInfo << candidateListingDay << ",";
		candidateInfo << candidateRemovalYear << ",";
		candidateInfo << candidateRemovalMonth << ",";
		candidateInfo << candidateRemovalDay << ",";
		candidateInfo << candidateCenterID << ",";
		candidateInfo << candidateOPO << ",";
		candidateInfo << transplantDonorID << ",";
		candidateInfo << KPDFunctions::boolToYesNo(transplantLivingDonor) << ",";
		candidateInfo << KPDFunctions::boolToYesNo(transplantDeceasedDonor) << ",";
		candidateInfo << KPDFunctions::relationToString(transplantRelation) << ",";
		candidateInfo << KPDFunctions::boolToYesNo(transplantBiological) << ",";
		candidateInfo << KPDFunctions::boolToYesNo(transplantNonBiological) << ",";
		candidateInfo << KPDFunctions::boolToYesNo(transplantPairedDonation) << ",";
		candidateInfo << KPDFunctions::boolToYesNo(transplantNonDirectedDonation) << ",";
	
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
		candidateInfo << KPDFunctions::indent(tab) << "Listing Date: " << candidateListingYear << "-" << candidateListingMonth << "-" << candidateListingDay << std::endl;
		candidateInfo << KPDFunctions::indent(tab) << "Removal Date: " << candidateRemovalYear << "-" << candidateRemovalMonth << "-" << candidateRemovalDay << std::endl;
			
		for (int i = 1; i <= candidateStatusChangeYear.size(); i++) {
			candidateInfo << KPDFunctions::indent(tab) << "Status at " << candidateStatusChangeYear[i - 1] << "-" << candidateStatusChangeMonth[i - 1] << "-" << candidateStatusChangeDay[i - 1] << ": ";
			if (candidateStatusActive[i - 1]) {
				candidateInfo << "Active";
			}
			else {
				candidateInfo << "Inactive";
			}
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



	candidateInfo << KPDFunctions::indent(tab) << "PRA: " << candidatePRA <<  ", BT: " << KPDFunctions::bloodTypeToString(candidateBT) << ", Enhanced BT: " << KPDFunctions::bloodTypeEnhancedToString(candidateEnhancedBT) << std::endl;
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
	candidateInfo << KPDFunctions::indent(tab) << "Accepts HCV Positive Transplant: " << KPDFunctions::boolToYesNo(candidateAcceptsHCVPositive) << std::endl;
	candidateInfo << KPDFunctions::indent(tab) << "EPTS: " << candidateEPTS << ", EPTS Priority: " << KPDFunctions::boolToYesNo(candidateEPTSPriority) << std::endl;

	candidateInfo << KPDFunctions::indent(tab) << "HLA:";
	for (std::vector<std::string>::iterator hla = candidateHLA.begin(); hla != candidateHLA.end(); hla++) {
		candidateInfo << " " << *hla;
	}
	candidateInfo << std::endl;
	
	if (candidateTransplanted) {

		candidateInfo << KPDFunctions::indent(tab) << "Transplant Information:" << std::endl;
		candidateInfo << KPDFunctions::indent(tab) << KPDFunctions::indent(tab) << "Donor ID: " << transplantDonorID << ", Living: " << KPDFunctions::boolToYesNo(transplantLivingDonor) << ", Deceased: " << KPDFunctions::boolToYesNo(transplantDeceasedDonor) << std::endl;
		candidateInfo << KPDFunctions::indent(tab) << KPDFunctions::indent(tab) << "Relation: " << KPDFunctions::relationToString(transplantRelation) << std::endl;
		candidateInfo << KPDFunctions::indent(tab) << KPDFunctions::indent(tab) << "Biological: " << KPDFunctions::boolToYesNo(transplantBiological) << ", Non-Biological: " << KPDFunctions::boolToYesNo(transplantNonBiological) << std::endl;
		candidateInfo << KPDFunctions::indent(tab) << KPDFunctions::indent(tab) << "Paired Donation: " << KPDFunctions::boolToYesNo(transplantPairedDonation) << ", Non-Directed Donation: " << KPDFunctions::boolToYesNo(transplantNonDirectedDonation) << std::endl;

	}

	return candidateInfo.str();

}

KPDCandidate * KPDCandidate::copy(){
	
	KPDCandidate * copyCandidate = new KPDCandidate(matchingID, uniqueID, candidatePRA, candidateBT, candidateEnhancedBT,
		candidateAge, candidateAgeCategory, candidateMale, candidateRace, candidateDiabetes, candidateHeight, candidateWeight, candidatePrevTrans, candidateTOD, candidateHepC, candidateInsurance, candidateAcceptsHCVPositive,candidateEPTS, candidateEPTSPriority,
		candidateWaitlist, candidateListingYear, candidateListingMonth, candidateListingDay, candidateRemovalYear, candidateRemovalMonth, candidateRemovalDay, candidateCenterID, candidateOPO,
		candidateTransplanted, transplantDonorID, transplantLivingDonor, transplantDeceasedDonor, transplantRelation, transplantBiological, transplantNonBiological, transplantPairedDonation, transplantNonDirectedDonation);
	
	copyCandidate->setUnacceptableHLA(candidateUnacceptableHLA);
	copyCandidate->setDesensitizableHLA(candidateDesensitizableHLA);
	copyCandidate->setHLA(candidateHLA);
	copyCandidate->setStatusChangeYear(candidateStatusChangeYear);
	copyCandidate->setStatusChangeMonth(candidateStatusChangeMonth);
	copyCandidate->setStatusChangeDay(candidateStatusChangeDay);
	copyCandidate->setStatusChange(candidateStatusActive);

	return copyCandidate;
}


#endif