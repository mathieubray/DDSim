#ifndef MATCH_H
#define MATCH_H

#include "KPD-Enums-Functions.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

class KPDMatch {

private:

	bool incidence;

	double fiveYearSurvival;
	double tenYearSurvival;
	double transplantDifficultyScore;
	double randomUtility;

	double assumedSuccessProbability;
	double actualSuccessProbability;

	KPDCrossmatchResult virtualCrossmatchResult;
	bool labCrossmatchResult;

public:

	KPDMatch();
	KPDMatch(bool isIncident, double fiveYearSurv, double tenYearSurv, double score, double util, double assumedProb, double actualProb, KPDCrossmatchResult vResult, bool lResult);
	~KPDMatch();

	//Getters
	bool getIncidence();

	double getFiveYearSurvival();
	double getTenYearSurvival();
	double getTransplantDifficultyScore();
	double getRandomUtility();

	double getUtility(KPDUtilityScheme scheme);

	double getAssumedSuccessProbability();
	double getActualSuccessProbability();

	KPDCrossmatchResult getVirtualCrossmatchResult();
	bool getLabCrossmatchResult();


	//Setters
	void setIncidence(bool isIncident);

	void setFiveYearSurvival(double surv);
	void setTenYearSurvival(double surv);
	void setTransplantDifficultyScore(double score);
	void setRandomUtility(double util);

	void setUtility(KPDUtilityScheme scheme, double util);

	void setAssumedSuccessProbability(double prob);
	void setActualSuccessProbability(double prob);

	void setVirtualCrossmatchResult(KPDCrossmatchResult result);
	void setLabCrossmatchResult(bool result);
	
	void setMatchProperties(bool isIncident, double fiveYearSurv, double tenYearSurv, double score, double util, double assumedProb, double actualProb, KPDCrossmatchResult vResult, bool lResult);

	//Strings
	std::string matchOutput(int space);
	std::string matchShortOutput();
	std::string matchString();

	//Copy Constructor
	KPDMatch * copy();

};

KPDMatch::KPDMatch(){	
	setMatchProperties(false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, FAILED_CROSSMATCH_LAB, false);	
}

KPDMatch::KPDMatch(bool inc, double fiveYearSurv, double tenYearSurv, double score, double util, double assumedProb, double actualProb, KPDCrossmatchResult vResult, bool lResult){
	setMatchProperties(inc, fiveYearSurv, tenYearSurv, score, util, assumedProb, actualProb, vResult, lResult);
}

KPDMatch::~KPDMatch(){

}

bool KPDMatch::getIncidence(){
	return incidence;
}

double KPDMatch::getFiveYearSurvival(){
	return fiveYearSurvival;
}

double KPDMatch::getTenYearSurvival(){
	return tenYearSurvival;
}

double KPDMatch::getTransplantDifficultyScore(){
	return transplantDifficultyScore;
}

double KPDMatch::getRandomUtility(){
	return randomUtility;
}

double KPDMatch::getUtility(KPDUtilityScheme scheme){
	if (scheme == FIVE_YEAR_SURVIVAL) {
		return fiveYearSurvival;
	}
	else if (scheme == TEN_YEAR_SURVIVAL) {
		return tenYearSurvival;
	}
	else if (scheme == TRANSPLANT_DIFFICULTY) {
		return transplantDifficultyScore;
	}
	else if (scheme == RANDOM_UTILITY) {
		return randomUtility;
	}
	else {
		return 1.0;
	}
}

double KPDMatch::getAssumedSuccessProbability(){
	return assumedSuccessProbability;
}

double KPDMatch::getActualSuccessProbability(){
	return actualSuccessProbability;
}

KPDCrossmatchResult KPDMatch::getVirtualCrossmatchResult(){
	return virtualCrossmatchResult;
}

bool KPDMatch::getLabCrossmatchResult(){
	return labCrossmatchResult;
}

void KPDMatch::setIncidence(bool isIncident){
	incidence = isIncident;
}

void KPDMatch::setFiveYearSurvival(double surv){
	fiveYearSurvival = surv;
}

void KPDMatch::setTenYearSurvival(double surv){
	tenYearSurvival = surv;
}

void KPDMatch::setTransplantDifficultyScore(double score){
	transplantDifficultyScore = score;
}

void KPDMatch::setRandomUtility(double util){
	randomUtility = util;
}

void KPDMatch::setUtility(KPDUtilityScheme scheme, double util){
	if (scheme == FIVE_YEAR_SURVIVAL) {
		fiveYearSurvival = util;
	}
	else if (scheme == TEN_YEAR_SURVIVAL) {
		tenYearSurvival = util;
	}
	else if (scheme == TRANSPLANT_DIFFICULTY) {
		transplantDifficultyScore = util;
	}
	else if (scheme == RANDOM_UTILITY) {
		randomUtility = util;
	}
}

void KPDMatch::setAssumedSuccessProbability(double prob){
	assumedSuccessProbability = prob;
}

void KPDMatch::setActualSuccessProbability(double prob){
	actualSuccessProbability = prob;
}

void KPDMatch::setVirtualCrossmatchResult(KPDCrossmatchResult result){
	virtualCrossmatchResult = result;
}

void KPDMatch::setLabCrossmatchResult(bool result){
	labCrossmatchResult = result;
}

void KPDMatch::setMatchProperties(bool isIncident, double fiveYearSurv, double tenYearSurv, double score, double util, double assumedProb, double actualProb, KPDCrossmatchResult vResult, bool lResult){
	
	incidence = isIncident;

	fiveYearSurvival = fiveYearSurv;
	tenYearSurvival = tenYearSurv;
	transplantDifficultyScore = score;
	randomUtility = util;

	assumedSuccessProbability = assumedProb;
	actualSuccessProbability = actualProb;

	virtualCrossmatchResult = vResult;
	labCrossmatchResult = lResult;

}

std::string KPDMatch::matchString(){

	std::stringstream matchInfo;

	matchInfo << fiveYearSurvival << ",";									// 5-Year Survival
	matchInfo << tenYearSurvival << ",";									// 10-Year Survival
	matchInfo << transplantDifficultyScore << ",";							// Transplant Difficulty Score
	matchInfo << randomUtility << ",";										// Random Utility
	matchInfo << assumedSuccessProbability << ",";							// Assumed Success Probability
	matchInfo << actualSuccessProbability << ",";							// Actual Success Probability
	matchInfo << KPDFunctions::toString(virtualCrossmatchResult) << ",";	// Virtual Crossmatch Result
	matchInfo << KPDFunctions::labCrossmatchToString(labCrossmatchResult);	// Lab Crossmatch Result

	return matchInfo.str();
}

std::string KPDMatch::matchOutput(int space){

	std::stringstream indent;

	for (int i = 1; i <= space; i++) {
		indent << " ";
	}

	std::stringstream matchInfo;

	matchInfo << indent.str() << "5-Year Survival: " << fiveYearSurvival << ", 10-Year Survival: " << tenYearSurvival << ", Score: " << transplantDifficultyScore << ", Random Utility: " << randomUtility << std::endl;
	matchInfo << indent.str() << "Assumed Success Probability: " << assumedSuccessProbability << ", Actual Success Probability: " << actualSuccessProbability << std::endl;
	matchInfo << indent.str() << "Virtual Crossmatch Result: " << KPDFunctions::toString(virtualCrossmatchResult) << ", Lab Crossmatch Result: " << KPDFunctions::labCrossmatchToString(labCrossmatchResult) << std::endl;

	return matchInfo.str();

}

std::string KPDMatch::matchShortOutput() {

	std::stringstream matchInfo;

	matchInfo << "[" << fiveYearSurvival << "," << tenYearSurvival << "," << transplantDifficultyScore << "," << randomUtility << ",";
	matchInfo << assumedSuccessProbability << "," << actualSuccessProbability << ",";
	matchInfo << KPDFunctions::toShortString(virtualCrossmatchResult) << "," << KPDFunctions::labCrossmatchToShortString(labCrossmatchResult) << "]";

	return matchInfo.str();

}

KPDMatch * KPDMatch::copy(){

	KPDMatch * copyMatch = new KPDMatch();

	copyMatch->setMatchProperties(incidence, fiveYearSurvival, tenYearSurvival, transplantDifficultyScore, randomUtility, assumedSuccessProbability, actualSuccessProbability, virtualCrossmatchResult, labCrossmatchResult);

	return copyMatch;
}


#endif
