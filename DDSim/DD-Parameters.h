/* ---------------------------------------------
DD-Parameters.h
Stores simulation parameters
---------------------------------------------- */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "DD-Enums-Functions.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>

class KPDParameters {

private:

	//Output Folder
	std::string outputFolder;
	std::string subFolder;
	
	//Simulation Settings
	KPDUtilityScheme utilityScheme;	
		
	//Numerical Parameters	
	int numberOfIterations;
	int startingIterationID;
	
	int initKPDSize;
	double pairArrivals;
	double nddArrivals;		

	int timeSimulation;
	int timeBetweenMatchRuns;
	int timeBetweenSelectionAndTransplantation;

	int maxCycleSize;
	int maxChainLength;

	double matchUtilityLowerBound;
	double matchUtilityUpperBound;

	double probPairActiveToInactive;
	double probPairInactiveToActive;

	double probInitKPDAttrition;
	double probPairAttrition;
	double probBridgeDonorAttrition;

	int praEligibilityMin;
	int praEligibilityMax;

	//Additional Options
	bool estimateExpectedUtility;
	int numberOfExpectedUtilityIterations;

	bool reserveODonorsForOCandidates;	
	bool allowABBridgeDonors;
	bool allowDesensitization;

	//Files
	std::string fileKPDData;
	std::string fileHLAFrequency;
	std::string fileHLADictionary;
	std::string fileSurvivalParameters;
	std::string fileDeceasedDonors;
	std::string fileWaitingListCandidates;
	
	//Random Number Generator Seeds
	int rngSeedSelection;
	int rngSeedAttrition;
	int rngSeedArrival;
	int rngSeedMatch;
	int rngSeedDonor;
	int rngSeedStatus;
	int rngSeedExpectedUtility;

	//Output Suppression
	bool suppressExchangeInformation;
	bool suppressPopulationInformation;
	bool suppressSimulationInformation;

	//Log
	std::stringstream parametersLog;


public:

	KPDParameters(); // Constructs new parameters object with default values
	~KPDParameters();

	bool processParameters(std::string & fileName); // Collect parameters from file
	
	//Output Folder
	std::string getOutputFolder();
	std::string getSubFolder();

	//Simulation Settings
	KPDUtilityScheme getUtilityScheme();
	
	//Numerical Parameters
	int getNumberOfIterations();
	int getStartingIterationID();
		
	int getInitKPDSize();
	double getPairArrivals();
	double getNDDArrivals();	

	int getTimeSimulation();
	int getTimeBetweenMatchRuns();
	int getTimeBetweenSelectionAndTransplantation();

	int getMaxCycleSize();
	int getMaxChainLength();

	double getMatchUtilityLowerBound();
	double getMatchUtilityUpperBound();

	double getProbPairActiveToInactive();
	double getProbPairInactiveToActive();

	double getProbInitKPDAttrition();
	double getProbPairAttrition();
	double getProbBridgeDonorAttrition();

	double getPRAEligibilityMin();
	double getPRAEligibilityMax();
	
	//Additional Options
	bool getEstimateExpectedUtility();
	int getNumberOfExpectedUtilityIterations();

	bool getReserveODonorsForOCandidates();
	bool getAllowABBridgeDonors();	
	bool getAllowDesensitization();
	
	//Files and Folders
	std::string getFileKPDData();
	std::string getFileHLAFrequency();
	std::string getFileHLADictionary();
	std::string getFileSurvivalParameters();
	std::string getFileDeceasedDonors();
	std::string getFileWaitingListCandidates();

	//Random Number Generator Seeds
	int getRNGSeedSelection();
	int getRNGSeedAttrition();
	int getRNGSeedArrival();
	int getRNGSeedMatch();
	int getRNGSeedDonor();
	int getRNGSeedStatus();
	int getRNGSeedExpectedUtility();

	//Output Suppression
	bool suppressExchangeOutput();
	bool suppressSimulationOutput();
	bool suppressPopulationOutput();
	
	//Log
	void printLog(); // Prints out parameter information to file
};

KPDParameters::KPDParameters(){

	/* Default Values */
	
	//Output Folder
	outputFolder = "Test";
	subFolder = "Test";

	//Simulation Settings
	utilityScheme = UTILITY_TRANSPLANTS;
	
	//Numerical Parameters
	numberOfIterations= 200;
	startingIterationID = 1;

	initKPDSize = 200;
	pairArrivals = 500.0;
	nddArrivals = 10.0;

	timeSimulation = 1095; // 3 Years = 365 * 3 Days
	timeBetweenMatchRuns = 1;
	timeBetweenSelectionAndTransplantation = 30;
	
	maxCycleSize = 3;
	maxChainLength = 3;

	matchUtilityLowerBound = 1.0;
	matchUtilityUpperBound = 1.0;

	probPairActiveToInactive = 0.01;
	probPairInactiveToActive = 0.02;

	probInitKPDAttrition = 0.02;
	probPairAttrition = 0.005;
	probBridgeDonorAttrition = 0.05;

	praEligibilityMin = 58;
	praEligibilityMax = 98;
	
	//Additional Options
	estimateExpectedUtility = false;
	numberOfExpectedUtilityIterations = 100;

	reserveODonorsForOCandidates = false;
	allowABBridgeDonors = false;
	allowDesensitization = true;
	
	//Files and Folders
	fileKPDData = "APDData.csv";
	fileHLAFrequency = "HLAFrequency.csv";
	fileHLADictionary = "HLADictionary.csv";
	fileSurvivalParameters = "SurvivalParameters.csv";
	fileDeceasedDonors = "DeceasedDonors.csv";
	fileWaitingListCandidates = "CandidateWaitlist.csv";
	
	//Random Number Generators Seeds
	rngSeedSelection = 9007900;
	rngSeedAttrition = 52531;
	rngSeedArrival = 5416162;
	rngSeedMatch = 3102156;
	rngSeedDonor = 3942252;
	rngSeedStatus = 7156372;
	rngSeedExpectedUtility = 1923323;
	
	//Output Suppression
	suppressExchangeInformation = true;
	suppressSimulationInformation = true;
	suppressPopulationInformation = false;
		
}

KPDParameters::~KPDParameters(){
	printLog();
}


bool KPDParameters::processParameters(std::string & fileName){

	std::ifstream iFile(fileName.c_str());

	if (!iFile.is_open()){
		parametersLog << "Cannot open file: " << fileName << std::endl;
		return false;
	}

	while (!iFile.eof()){

		std::stringstream lineStream;
		std::string line;
		getline(iFile, line);
		if (line.empty())
			continue;
		lineStream << line;

		while (!lineStream.eof()){
			std::string tokenOne;
			std::string tokenTwo;
			getline(lineStream, tokenOne, '=');
			getline(lineStream, tokenTwo, '=');

			//Output Folder
			if (tokenOne.compare("#outputfolder") == 0){ outputFolder = tokenTwo; }
			if (tokenOne.compare("#subfolder") == 0){ subFolder = tokenTwo; }

			//Simulation Settings

			if (tokenOne.compare("#utilityscheme") == 0){
				utilityScheme = KPDFunctions::stringToUtilityScheme(tokenTwo); //Can be any of: UTILITY_TRANSPLANTS, UTILITY_FIVE_YEAR_SURVIVAL, UTILITY_TEN_YEAR_SURVIVAL, UTILITY_TRANSPLANT_DIFFICULTY, UTILITY_RANDOM
			}

			//Numerical Parameters
			if (tokenOne.compare("#numberofiterations") == 0){ numberOfIterations = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#startingiterationid") == 0){ startingIterationID = atoi(tokenTwo.c_str()); }

			if (tokenOne.compare("#initkpdsize") == 0) { initKPDSize = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#pairarrivals") == 0){ pairArrivals = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#nddarrivals") == 0){ nddArrivals = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#timesimulation") == 0){ timeSimulation = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#timebetweenmatchruns") == 0){ timeBetweenMatchRuns = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#timebetweenselectionandtransplantation") == 0){ timeBetweenSelectionAndTransplantation = atoi(tokenTwo.c_str()); }

			if (tokenOne.compare("#maxcyclesize") == 0) { maxCycleSize = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#maxchainlength") == 0) { maxChainLength = atoi(tokenTwo.c_str()); }

			if (tokenOne.compare("#matchutilitylowerbound") == 0) { matchUtilityLowerBound = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#matchutilityupperbound") == 0) { matchUtilityUpperBound = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#probpairactivetoinactive") == 0) { probPairActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#probpairinactivetoactive") == 0) { probPairInactiveToActive = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#probinitkpdattrition") == 0) { probInitKPDAttrition = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#probpairattrition") == 0) { probPairAttrition = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#probbridgedonorattrition") == 0) { probBridgeDonorAttrition = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#praeligibilitymin") == 0) { praEligibilityMin = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#praeligibilitymax") == 0) { praEligibilityMax = atoi(tokenTwo.c_str()); }

			//Additional Options
			if (tokenOne.compare("#estimateexpectedutility") == 0){
				if (tokenTwo.compare("TRUE") == 0){ estimateExpectedUtility = true; }
				else if (tokenTwo.compare("FALSE") == 0){ estimateExpectedUtility = false; }
			}
			if (tokenOne.compare("#numberofexpectedutilityiterations") == 0){ numberOfExpectedUtilityIterations = atoi(tokenTwo.c_str()); }
			
			if (tokenOne.compare("#reserveodonorsforocandidates") == 0){
				if (tokenTwo.compare("TRUE") == 0){ reserveODonorsForOCandidates = true; }
				else if (tokenTwo.compare("FALSE") == 0){ reserveODonorsForOCandidates = false; }
			}
			if (tokenOne.compare("#allowabbridgedonors") == 0){
				if (tokenTwo.compare("TRUE") == 0){ allowABBridgeDonors = true; }
				else if (tokenTwo.compare("FALSE") == 0){ allowABBridgeDonors = false; }
			}
			if (tokenOne.compare("#allowdesensitization") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { allowDesensitization = true; }
				else if (tokenTwo.compare("FALSE") == 0) { allowDesensitization = false; }
			}

			//Files and Folders
			if (tokenOne.compare("#filekpddata") == 0) { fileKPDData = tokenTwo; }
			if (tokenOne.compare("#filehlafrequency") == 0) { fileHLAFrequency = tokenTwo; }
			if (tokenOne.compare("#filehladictionary") == 0) { fileHLADictionary = tokenTwo; }
			if (tokenOne.compare("#filesurvivalparameters") == 0) { fileSurvivalParameters = tokenTwo; }
			if (tokenOne.compare("#filedeceaseddonors") == 0) { fileDeceasedDonors = tokenTwo; }
			if (tokenOne.compare("#filewaitinglistcandidates") == 0) { fileWaitingListCandidates = tokenTwo; }

			//Random Number Generator Seeds
			if (tokenOne.compare("#rngseedselection") == 0) { rngSeedSelection = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedattrition") == 0) { rngSeedAttrition = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedarrival") == 0) { rngSeedArrival = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedmatch") == 0) { rngSeedMatch = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseeddonor") == 0) { rngSeedDonor = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedstatus") == 0) { rngSeedStatus = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedexpectedutility") == 0) { rngSeedExpectedUtility = atoi(tokenTwo.c_str()); }

			//Output Suppression
			if (tokenOne.compare("#suppressexchangeinformation") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { suppressExchangeInformation = true; }
				else if (tokenTwo.compare("FALSE") == 0) { suppressExchangeInformation = false; }
			}
			if (tokenOne.compare("#suppresssimulationinformation") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { suppressSimulationInformation = true; }
				else if (tokenTwo.compare("FALSE") == 0) { suppressSimulationInformation = false; }
			}
			if (tokenOne.compare("#suppresspopulationinformation") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { suppressPopulationInformation = true; }
				else if (tokenTwo.compare("FALSE") == 0) { suppressPopulationInformation = false; }
			}	
		}
	}
	
	return true;
}

void KPDParameters::printLog(){

	//Folder Names
	parametersLog << "-----------------" << std::endl;
	parametersLog << "Output Folder" << std::endl;
	parametersLog << "-----------------" << std::endl << std::endl;

	parametersLog << "Output Folder: " << outputFolder << std::endl;
	parametersLog << "Sub-Folder: " << subFolder << std::endl;
	
	parametersLog << std::endl;

	//Simulation Settings
	parametersLog << "-----------------" << std::endl;
	parametersLog << "Simulation Settings" << std::endl;
	parametersLog << "-----------------" << std::endl << std::endl;

	parametersLog << "Utility Based On: " << KPDFunctions::utilitySchemeToString(utilityScheme);
	if (utilityScheme == UTILITY_RANDOM) {
		if (matchUtilityLowerBound == matchUtilityUpperBound) {
			parametersLog << " = " << matchUtilityLowerBound << " for all transplants";
		}
		else {
			parametersLog << " ~ U(" << matchUtilityLowerBound << "," << matchUtilityUpperBound << ")";
		}
	}
	parametersLog << std::endl;

	parametersLog << std::endl;
		
	//Numerical Parameters
	parametersLog << "--------------------" << std::endl;
	parametersLog << "Numerical Parameters" << std::endl;
	parametersLog << "--------------------" << std::endl << std::endl;

	parametersLog << "Simulation Iterations: " << numberOfIterations << " (From " << startingIterationID << "-" << startingIterationID + numberOfIterations - 1 << ")" << std::endl;
	
	parametersLog << "Initial Expected KPD Size: " << initKPDSize << std::endl;
	parametersLog << "Expected Pair Arrivals Per Year: " << pairArrivals << std::endl;
	parametersLog << "Expected NDD Arrivals Per Year: " << nddArrivals << std::endl;
	parametersLog << std::endl;

	parametersLog << "Simulation Time: " << timeSimulation << " Days" << std::endl;
	parametersLog << "Match Run Frequency: Every " << timeBetweenMatchRuns << " Days" << std::endl;
	parametersLog << "Post-Selection: " << timeBetweenSelectionAndTransplantation << " Days After Selection Before Transplantation" << std::endl;
	parametersLog << std::endl;

	parametersLog << "Maximum Cycle Size: " << maxCycleSize << std::endl;
	parametersLog << "Maximum Chain Length: " << maxChainLength << std::endl;
	parametersLog << std::endl;

	parametersLog << "Daily Probability of Pair Active -> Inactive: " << probPairActiveToInactive << std::endl;
	parametersLog << "Daily Probability of Pair Inactive -> Active: " << probPairInactiveToActive << std::endl;
	parametersLog << std::endl;

	parametersLog << "Probability of Attrition when Generating Initial KPD: " << probInitKPDAttrition << std::endl;
	parametersLog << "Daily Probability of Pair Withdrawing: " << probPairAttrition << std::endl;
	parametersLog << "Daily Probability of Bridge Donor Withdrawing: " << probBridgeDonorAttrition << std::endl;
	parametersLog << std::endl;

	parametersLog << "Minimum PRA for Candidate Eligibility for DD Transplant in KPD: " << praEligibilityMin << std::endl;
	parametersLog << "Maximum PRA for Candidate Eligibility for DD Transplant in KPD: " << praEligibilityMax << std::endl;
	
	parametersLog << std::endl;

	//Additional Options
	parametersLog << "------------------" << std::endl;
	parametersLog << "Additional Options" << std::endl;
	parametersLog << "------------------" << std::endl << std::endl;
	
	if (estimateExpectedUtility == true){
		parametersLog << "Number of Expected Utility Iterations: " << numberOfExpectedUtilityIterations << std::endl;
	}
	
	if (reserveODonorsForOCandidates == false){
		parametersLog << "Do Not ";
	}
	parametersLog << "Reserve O Donors for O Candidates" << std::endl;	
		
	if (allowABBridgeDonors == false){
		parametersLog << "Do Not ";
	}
	parametersLog << "Include Chains Which End With AB Bridge Donor" << std::endl;

	if (allowDesensitization == false) {
		parametersLog << "Do Not ";
	}
	parametersLog << "Allow Desensitization of Candidates (Affects HLA Considered)" << std::endl;
	
	parametersLog << std::endl;
	
	//Files and Folders
	parametersLog << "-----------" << std::endl;
	parametersLog << "Files and Folders" << std::endl;
	parametersLog << "-----------" << std::endl << std::endl;

	parametersLog << "KPD Data File Name: " << fileKPDData << std::endl;
	parametersLog << "HLA Frequency File Name: " << fileHLAFrequency << std::endl;
	parametersLog << "HLA Dictionary File Name: " << fileHLADictionary << std::endl;
	parametersLog << "Survival Parameters File Name: " << fileSurvivalParameters << std::endl;
	parametersLog << "Deceased Donors File Name: " << fileDeceasedDonors << std::endl;
	parametersLog << "Waiting List Candidates File Name: " << fileWaitingListCandidates << std::endl;

	parametersLog << std::endl;

	//Random Number Generator Seeds
	parametersLog << "-----------------------------" << std::endl;
	parametersLog << "Random Number Generator Seeds" << std::endl;
	parametersLog << "-----------------------------" << std::endl << std::endl;

	parametersLog << "Pair/NDD Selection RNG Seed: " << rngSeedSelection << std::endl;
	parametersLog << "Initial Pool Generation Attrition RNG Seed: " << rngSeedAttrition << std::endl;
	parametersLog << "Pair/NDD Arrival Time RNG Seed: " << rngSeedArrival << std::endl;
	parametersLog << "Match Properties RNG Seed: " << rngSeedMatch << std::endl;
	parametersLog << "Donor Generation RNG Seed: " << rngSeedDonor << std::endl;
	parametersLog << "Status Changes RNG Seed: " << rngSeedStatus << std::endl;
	parametersLog << "Expected Utility Estimation RNG Seed: " << rngSeedExpectedUtility << std::endl;

	parametersLog << std::endl;
	
	//Output Suppression
	if (suppressExchangeInformation || suppressPopulationInformation || suppressSimulationInformation) {
		
		parametersLog << "------------------" << std::endl;
		parametersLog << "Output Suppression" << std::endl;
		parametersLog << "------------------" << std::endl << std::endl;

		if (suppressExchangeInformation) {
			parametersLog << " - Exchange Information Will Not Be Printed" << std::endl;
		}
		if (suppressSimulationInformation) {
			parametersLog << " - Simulation Information Will Not Be Printed" << std::endl;
		}
		if (suppressPopulationInformation) {
			parametersLog << " - Popualtion Information Will Not Be Printed" << std::endl;
		}

		parametersLog << std::endl;
	}

	//Print to File
	std::string logFile = "output/" + outputFolder + "/" + subFolder + "/Log-Parameters.txt";
	std::ofstream outputFileLog(logFile.c_str());
	outputFileLog << parametersLog.str();
	outputFileLog.close();
}

std::string KPDParameters::getOutputFolder(){
	return outputFolder;
}

std::string KPDParameters::getSubFolder(){
	return subFolder;
}

KPDUtilityScheme KPDParameters::getUtilityScheme(){
	return utilityScheme;
}

int KPDParameters::getNumberOfIterations(){
	return numberOfIterations;
}

int KPDParameters::getStartingIterationID(){
	return startingIterationID;
}

int KPDParameters::getInitKPDSize(){
	return initKPDSize;
}

double KPDParameters::getPairArrivals(){
	return pairArrivals;
}

double KPDParameters::getNDDArrivals(){
	return nddArrivals;
}

int KPDParameters::getTimeSimulation(){
	return timeSimulation;
}

int KPDParameters::getTimeBetweenMatchRuns(){
	return timeBetweenMatchRuns;
}

int KPDParameters::getTimeBetweenSelectionAndTransplantation(){
	return timeBetweenSelectionAndTransplantation;
}

int KPDParameters::getMaxCycleSize() {
	return maxCycleSize;
}

int KPDParameters::getMaxChainLength() {
	return maxChainLength;
}

double KPDParameters::getMatchUtilityLowerBound() {
	return matchUtilityLowerBound;
}

double KPDParameters::getMatchUtilityUpperBound() {
	return matchUtilityUpperBound;
}

double KPDParameters::getProbPairActiveToInactive(){
	return probPairActiveToInactive;
}

double KPDParameters::getProbPairInactiveToActive(){
	return probPairInactiveToActive;
}

double KPDParameters::getProbInitKPDAttrition() {
	return probInitKPDAttrition;
}

double KPDParameters::getProbPairAttrition(){
	return probPairAttrition;
}

double KPDParameters::getProbBridgeDonorAttrition(){
	return probBridgeDonorAttrition;
}

double KPDParameters::getPRAEligibilityMin(){
	return praEligibilityMin;
}

double KPDParameters::getPRAEligibilityMax(){
	return praEligibilityMax;
}

bool KPDParameters::getEstimateExpectedUtility(){
	return estimateExpectedUtility;
}

int KPDParameters::getNumberOfExpectedUtilityIterations(){
	return numberOfExpectedUtilityIterations;
}

bool KPDParameters::getReserveODonorsForOCandidates(){
	return reserveODonorsForOCandidates;
}

bool KPDParameters::getAllowABBridgeDonors(){
	return allowABBridgeDonors;
}

bool KPDParameters::getAllowDesensitization() {
	return allowDesensitization;
}

std::string KPDParameters::getFileKPDData() {
	return fileKPDData;
}

std::string KPDParameters::getFileHLAFrequency() {
	return fileHLAFrequency;
}

std::string KPDParameters::getFileHLADictionary() {
	return fileHLADictionary;
}

std::string KPDParameters::getFileSurvivalParameters() {
	return fileSurvivalParameters;
}

std::string KPDParameters::getFileDeceasedDonors() {
	return fileDeceasedDonors;
}

std::string KPDParameters::getFileWaitingListCandidates() {
	return fileWaitingListCandidates;
}

int KPDParameters::getRNGSeedSelection() {
	return rngSeedSelection;
}

int KPDParameters::getRNGSeedAttrition() {
	return rngSeedAttrition;
}

int KPDParameters::getRNGSeedArrival() {
	return rngSeedArrival;
}

int KPDParameters::getRNGSeedMatch() {
	return rngSeedMatch;
}

int KPDParameters::getRNGSeedDonor() {
	return rngSeedDonor;
}

int KPDParameters::getRNGSeedStatus() {
	return rngSeedStatus;
}

int KPDParameters::getRNGSeedExpectedUtility() {
	return rngSeedExpectedUtility;
}

bool KPDParameters::suppressExchangeOutput() {
	return suppressExchangeInformation;
}

bool KPDParameters::suppressSimulationOutput() {
	return suppressSimulationInformation;
}

bool KPDParameters::suppressPopulationOutput() {
	return suppressPopulationInformation;
}


#endif