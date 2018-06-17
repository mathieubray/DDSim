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

	//Folder Names
	std::string outputFolder;
	std::string subFolder;
	
	//Simulation Settings
	KPDUtilityScheme utilityScheme;	
	KPDDonorAssignmentScheme donorAssignmentScheme;
	
	KPDTimeline timeline;

	KPDMatchFailureScheme assumedMatchFailureScheme;
	KPDMatchFailureScheme actualMatchFailureScheme;

	//Optimization Settings
	bool cyclesAndChains;
	bool cyclesAndChainsWithFallbacks;
	bool locallyRelevantSubsets;
	
	//Numerical Parameters	
	int numberOfIterations;
	int startingIterationID;
	
	double arrivals;
	double ndds;		

	int timeSpan;
	int initMatchRun;
	int freqMatchRun;

	int processingTime;

	int maxCycleSize;
	int maxChainLength;
	int maxLRSSize;

	double matchUtilityLowerBound;
	double matchUtilityUpperBound;

	//NDD Failure
	double assumedNDDActiveToInactive;
	double assumedNDDActiveToWithdrawn;
	double assumedNDDInactiveToActive;
	double assumedNDDInactiveToWithdrawn;

	double actualNDDActiveToInactive;
	double actualNDDActiveToWithdrawn;
	double actualNDDInactiveToActive;
	double actualNDDInactiveToWithdrawn;

	//Donor Failure
	double assumedDonorActiveToInactive;
	double assumedDonorActiveToWithdrawn;
	double assumedDonorInactiveToActive;
	double assumedDonorInactiveToWithdrawn;

	double actualDonorActiveToInactive;
	double actualDonorActiveToWithdrawn;
	double actualDonorInactiveToActive;
	double actualDonorInactiveToWithdrawn;

	//Candidate Failure
	double assumedCandidateActiveToInactive;
	double assumedCandidateActiveToWithdrawn;
	double assumedCandidateInactiveToActive;
	double assumedCandidateInactiveToWithdrawn;

	double actualCandidateActiveToInactive;
	double actualCandidateActiveToWithdrawn;
	double actualCandidateInactiveToActive;
	double actualCandidateInactiveToWithdrawn;

	//Match Failure
	double assumedMatchFailureLowerBound;
	double assumedMatchFailureUpperBound;
	double assumedMatchFailureAdjustment;
	double actualMatchFailureLowerBound;
	double actualMatchFailureUpperBound;
	double actualMatchFailureAdjustment;

	//Additional Options
	bool estimateEU;
	int numberOfEUIterations;

	double twoDonorsProbability;
	double threeDonorsProbability;

	bool addAdvantage;
	int advantageCutoff;
	double advantageValue;

	bool reserveOForO;	
	bool includeCompatible;
	bool excludeABDonors;
	bool allowABBridge;	
	bool desensitize;

	//Defaults
	std::string fileKPD;
	std::string fileHLAFrequency;
	std::string fileHLADictionary;
	std::string fileCharacteristics;

	bool printArrangementList;
	bool printTransplantList;
	bool printSimulationLog;
	bool printPopulationStats;

	int rngSeedCharacteristics;
	int rngSeedNDD;
	int rngSeedSelection;
	int rngSeedAdditionalDonor;
	int rngSeedArrival;
	int rngSeedUtility;
	int rngSeedCrossmatch;
	int rngSeedPairFailure;
	int rngSeedMatchFailure;
	int rngSeedDonorGenerator;
	int rngSeedTransition;
	int rngSeedExpectedUtilityEstimation;

	std::stringstream kpdParametersLog;

	std::string logFile;

public:

	KPDParameters();
	~KPDParameters();

	bool processParameters(std::string & fileName);
	
	//Folder Names
	std::string getOutputFolder();
	std::string getSubFolder();

	//Simulation Settings
	KPDUtilityScheme getUtilityScheme();
	KPDDonorAssignmentScheme getDonorAssignmentScheme();
	
	KPDTimeline getTimeline();

	KPDMatchFailureScheme getAssumedMatchFailureScheme();
	KPDMatchFailureScheme getActualMatchFailureScheme();
	
	//Optimization Settings
	bool runCyclesAndChains();
	bool runCyclesAndChainsWithFallbacks();
	bool runLocallyRelevantSubsets();

	//Numerical Parameters
	int getNumberOfIterations();
	int getStartingIterationID();
		
	double getArrivals();
	double getNDDs();	

	int getTimeSpan();
	int getInitMatchRun();
	int getFreqMatchRun();

	int getProcessingTime();

	int getMaxCycleSize();
	int getMaxChainLength();
	int getMaxLRSSize();

	double getMatchUtilityLowerBound();
	double getMatchUtilityUpperBound();

	//NDD Failure
	double getAssumedNDDActiveToInactive();
	double getAssumedNDDActiveToWithdrawn();
	double getAssumedNDDInactiveToActive();
	double getAssumedNDDInactiveToWithdrawn();

	double getActualNDDActiveToInactive();
	double getActualNDDActiveToWithdrawn();
	double getActualNDDInactiveToActive();
	double getActualNDDInactiveToWithdrawn();

	//Donor Failure
	double getAssumedDonorActiveToInactive();
	double getAssumedDonorActiveToWithdrawn();
	double getAssumedDonorInactiveToActive();
	double getAssumedDonorInactiveToWithdrawn();

	double getActualDonorActiveToInactive();
	double getActualDonorActiveToWithdrawn();
	double getActualDonorInactiveToActive();
	double getActualDonorInactiveToWithdrawn();

	//Candidate Failure
	double getAssumedCandidateActiveToInactive();
	double getAssumedCandidateActiveToWithdrawn();
	double getAssumedCandidateInactiveToActive();
	double getAssumedCandidateInactiveToWithdrawn();

	double getActualCandidateActiveToInactive();
	double getActualCandidateActiveToWithdrawn();
	double getActualCandidateInactiveToActive();
	double getActualCandidateInactiveToWithdrawn();

	//Match Failure
	double getAssumedMatchFailureLowerBound();
	double getAssumedMatchFailureUpperBound();
	double getAssumedMatchFailureAdjustment();
	double getActualMatchFailureLowerBound();
	double getActualMatchFailureUpperBound();
	double getActualMatchFailureAdjustment();
	
	//Additional Options
	bool estimateExpectedUtility();
	int getNumberOfExpectedUtilityIterations();

	double getTwoDonorsProbability();
	double getThreeDonorsProbability();

	bool addAdvantageToHighPRACandidates();
	int getPRAAdvantageCutoff();
	double getPRAAdvantageValue();

	bool reserveODonorsForOCandidates();	
	bool includeCompatiblePairs();
	bool excludeABDonorsFromSimulation();
	bool allowABBridgeDonors();	
	bool allowDesensitization();
	
	//Defaults
	std::string getFileKPD();
	std::string getFileHLAFrequency();
	std::string getFileHLADictionary();
	std::string getFileCharacteristics();

	bool outputArrangementList();
	bool outputTransplantList();
	bool outputSimulationLog();
	bool outputPopulationStats();
	
	int getRNGSeedCharacteristics();
	int getRNGSeedNDD();
	int getRNGSeedSelection();
	int getRNGSeedAdditionalDonor();
	int getRNGSeedArrival();
	int getRNGSeedUtility();
	int getRNGSeedCrossmatch();
	int getRNGSeedPairFailure();
	int getRNGSeedMatchFailure();
	int getRNGSeedDonorGenerator();
	int getRNGSeedTransition();
	int getRNGSeedExpectedUtilityEstimation();

	void printLog();
};

KPDParameters::KPDParameters(){

	/* Default Parameters */

	//Folder Names
	outputFolder = "Test";
	subFolder = "DefaultTest";

	//Simulation Settings
	utilityScheme = TRANSPLANTS;
	donorAssignmentScheme = PAIRED_DONORS;

	timeline = FIXED_MATCH_RUNS;
	
	assumedMatchFailureScheme = RANDOM_MATCH_FAILURE;
	actualMatchFailureScheme = RANDOM_MATCH_FAILURE;

	//Optimization Settings	
	cyclesAndChains = true;
	cyclesAndChainsWithFallbacks = false;
	locallyRelevantSubsets = false;

	//Numerical Parameters
	numberOfIterations= 200;
	startingIterationID = 1;

	arrivals = 30;
	ndds = 1;

	timeSpan = 8;
	initMatchRun = 1;
	freqMatchRun = 1;

	processingTime = 0;

	maxCycleSize = 3;
	maxChainLength = 3;
	maxLRSSize = 4;

	matchUtilityLowerBound = 1.0;
	matchUtilityUpperBound = 1.0;

	//NDD Failure
	assumedNDDActiveToInactive = 0.0;
	assumedNDDActiveToWithdrawn = 0.0;
	assumedNDDInactiveToActive = 0.0;
	assumedNDDInactiveToWithdrawn = 0.0;

	actualNDDActiveToInactive = 0.0;
	actualNDDActiveToWithdrawn = 0.0;
	actualNDDInactiveToActive = 0.0;
	actualNDDInactiveToWithdrawn = 0.0;

	//Donor Failure
	assumedDonorActiveToInactive = 0.0;
	assumedDonorActiveToWithdrawn = 0.0;
	assumedDonorInactiveToActive = 0.0;
	assumedDonorInactiveToWithdrawn = 0.0;

	actualDonorActiveToInactive = 0.0;
	actualDonorActiveToWithdrawn = 0.0;
	actualDonorInactiveToActive = 0.0;
	actualDonorInactiveToWithdrawn = 0.0;

	//Candidate Failure
	assumedCandidateActiveToInactive = 0.0;
	assumedCandidateActiveToWithdrawn = 0.0;
	assumedCandidateInactiveToActive = 0.0;
	assumedCandidateInactiveToWithdrawn = 0.0;

	actualCandidateActiveToInactive = 0.0;
	actualCandidateActiveToWithdrawn = 0.0;
	actualCandidateInactiveToActive = 0.0;
	actualCandidateInactiveToWithdrawn = 0.0;
	
	//Match Failure
	assumedMatchFailureLowerBound = 0.1;
	assumedMatchFailureUpperBound = 0.1;
	assumedMatchFailureAdjustment = 0.0;
	actualMatchFailureLowerBound = 0.1;
	actualMatchFailureUpperBound = 0.1;
	actualMatchFailureAdjustment = 0.0;
	
	//Additional Options
	estimateEU = false;
	numberOfEUIterations = 100;

	twoDonorsProbability = 0.0;
	threeDonorsProbability = 0.0;

	addAdvantage = false;
	advantageCutoff = 100;
	advantageValue = 0.0;

	reserveOForO = false;
	includeCompatible = false;
	excludeABDonors = false;
	allowABBridge = false;
	desensitize = true;

	//Defaults
	fileKPD = "APD Data - KPD Simulations.csv";
	fileHLAFrequency = "Bone Marrow Registry - KPD Simulations.csv";
	fileHLADictionary = "HLA Dictionary - KPD Simulations.csv";
	fileCharacteristics = "Survival Parameters - KPD Simulations.csv";

	printArrangementList = true;
	printTransplantList = true;
	printSimulationLog = true;
	printPopulationStats = true;

	rngSeedCharacteristics = 3030303;
	rngSeedNDD = 23958126;
	rngSeedSelection = 9007900;
	rngSeedAdditionalDonor = 2531530;
	rngSeedArrival = 5416162;
	rngSeedUtility = 328749;
	rngSeedCrossmatch = 3102156;
	rngSeedPairFailure = 326346;
	rngSeedMatchFailure = 2349412;
	rngSeedDonorGenerator = 3942252;
	rngSeedTransition = 7156372;
	rngSeedExpectedUtilityEstimation = 1923323;
	
	//activeToInactive = 0.008333;
	//activeToWithdrawn = 0.000476;
	//inactiveToActive = 0.033333;
	//inactiveToWithdrawn = 0.001428;

	//activetoinactive = 0.25
	//activetowithdrawn = 0.01429
	//inactivetoactive = 1
	//inactivetowithdrawn = 0.042486
	
}

KPDParameters::~KPDParameters(){
	printLog();
}


bool KPDParameters::processParameters(std::string & fileName){

	std::ifstream iFile(fileName.c_str());

	if (!iFile.is_open()){
		kpdParametersLog << "Cannot open file: " << fileName << std::endl;
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

			//Folder Names
			if (tokenOne.compare("#outputfolder") == 0){ outputFolder = tokenTwo; }
			if (tokenOne.compare("#subfolder") == 0){ subFolder = tokenTwo; }

			//Simulation Settings
			if (tokenOne.compare("#utilityscheme") == 0){
				utilityScheme = KPDFunctions::stringToUtilityScheme(tokenTwo); //TRANSPLANTS, FIVE_YEAR_SURVIVAL, TEN_YEAR_SURVIVAL, TRANSPLANT_DIFFICULTY, RANDOM_UTILITY
			}
			if (tokenOne.compare("#donorassignmentscheme") == 0){
				donorAssignmentScheme = KPDFunctions::stringToDonorAssignmentScheme(tokenTwo); //PAIRED_DONORS, RANDOM_DONORS
			}
			
			if (tokenOne.compare("#timeline") == 0) {
				timeline = KPDFunctions::stringToTimeline(tokenTwo); //FIXED_MATCH_RUNS, CONTINUOUS_TIMELINE
			}
			
			if (tokenOne.compare("#assumedmatchfailurescheme") == 0){
				assumedMatchFailureScheme = KPDFunctions::stringToMatchFailureScheme(tokenTwo); // PRA_BASED_MATCH_FAILURE, RANDOM_MATCH_FAILURE
			}
			if (tokenOne.compare("#actualmatchfailurescheme") == 0) {
				actualMatchFailureScheme = KPDFunctions::stringToMatchFailureScheme(tokenTwo);
			}
			
			//Optimization Settings	
			if (tokenOne.compare("#cyclesandchains") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { cyclesAndChains = true; }
				else if (tokenTwo.compare("FALSE") == 0) { cyclesAndChains = false; }
			}
			if (tokenOne.compare("#cyclesandchainswithfallbacks") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { cyclesAndChainsWithFallbacks = true; }
				else if (tokenTwo.compare("FALSE") == 0) { cyclesAndChainsWithFallbacks = false; }
			}
			if (tokenOne.compare("#locallyrelevantsubsets") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { locallyRelevantSubsets = true; }
				else if (tokenTwo.compare("FALSE") == 0) { locallyRelevantSubsets = false; }
			}

			//Numerical Parameters
			if (tokenOne.compare("#numberofiterations") == 0){ numberOfIterations = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#startingiterationid") == 0){ startingIterationID = atoi(tokenTwo.c_str()); }

			if (tokenOne.compare("#arrivals") == 0){ arrivals = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#ndds") == 0){ ndds = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#timespan") == 0){ timeSpan = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#initmatchrun") == 0){ initMatchRun = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#freqmatchrun") == 0){ freqMatchRun = atoi(tokenTwo.c_str()); }

			if (tokenOne.compare("#processingtime") == 0){ processingTime = atoi(tokenTwo.c_str()); }

			if (tokenOne.compare("#maxcyclesize") == 0) { maxCycleSize = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#maxchainlength") == 0) { maxChainLength = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#maxlrssize") == 0) { maxLRSSize = atoi(tokenTwo.c_str()); }

			if (tokenOne.compare("#matchutilitylowerbound") == 0) { matchUtilityLowerBound = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#matchutilityupperbound") == 0) { matchUtilityUpperBound = atof(tokenTwo.c_str()); }

			//NDD Failure
			if (tokenOne.compare("#assumednddactivetoinactive") == 0) { assumedNDDActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumednddactivetowithdrawn") == 0) { assumedNDDActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumednddinactivetoactive") == 0) { assumedNDDInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumednddinactivetowithdrawn") == 0) { assumedNDDInactiveToWithdrawn = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#actualnddactivetoinactive") == 0) { actualNDDActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualnddactivetowithdrawn") == 0) { actualNDDActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualnddinactivetoactive") == 0) { actualNDDInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualnddinactivetowithdrawn") == 0) { actualNDDInactiveToWithdrawn = atof(tokenTwo.c_str()); }

			//Donor Failure
			if (tokenOne.compare("#assumeddonoractivetoinactive") == 0) { assumedDonorActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumeddonoractivetowithdrawn") == 0) { assumedDonorActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumeddonorinactivetoactive") == 0) { assumedDonorInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumeddonorinactivetowithdrawn") == 0) { assumedDonorInactiveToWithdrawn = atof(tokenTwo.c_str()); }
			
			if (tokenOne.compare("#actualdonoractivetoinactive") == 0) { actualDonorActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualdonoractivetowithdrawn") == 0) { actualDonorActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualdonorinactivetoactive") == 0) { actualDonorInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualdonorinactivetowithdrawn") == 0) { actualDonorInactiveToWithdrawn = atof(tokenTwo.c_str()); }

			//Candidate Failure
			if (tokenOne.compare("#assumedcandidateactivetoinactive") == 0) { assumedCandidateActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumedcandidateactivetowithdrawn") == 0) { assumedCandidateActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumedcandidateinactivetoactive") == 0) { assumedCandidateInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumedcandidateinactivetowithdrawn") == 0) { assumedCandidateInactiveToWithdrawn = atof(tokenTwo.c_str()); }
			
			if (tokenOne.compare("#actualcandidateactivetoinactive") == 0) { actualCandidateActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualcandidateactivetowithdrawn") == 0) { actualCandidateActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualcandidateinactivetoactive") == 0) { actualCandidateInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualcandidateinactivetowithdrawn") == 0) { actualCandidateInactiveToWithdrawn = atof(tokenTwo.c_str()); }

			//Match Failure
			if (tokenOne.compare("#assumedmatchfailurelowerbound") == 0){ assumedMatchFailureLowerBound = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumedmatchfailureupperbound") == 0) { assumedMatchFailureUpperBound = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumedmatchfailureadjustment") == 0) { assumedMatchFailureAdjustment = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualmatchfailurelowerbound") == 0) { actualMatchFailureLowerBound = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualmatchfailureupperbound") == 0) { actualMatchFailureUpperBound = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualmatchfailureadjustment") == 0) { actualMatchFailureAdjustment = atof(tokenTwo.c_str()); }

			//Additional Options
			if (tokenOne.compare("#estimateexpectedutility") == 0){
				if (tokenTwo.compare("TRUE") == 0){ estimateEU = true; }
				else if (tokenTwo.compare("FALSE") == 0){ estimateEU = false; }
			}
			if (tokenOne.compare("#numberofexpectedutilityiterations") == 0){ numberOfEUIterations = atoi(tokenTwo.c_str()); }

			if (tokenOne.compare("#twodonorsprobability") == 0){ twoDonorsProbability = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#threedonorsprobability") == 0) { threeDonorsProbability = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#addadvantagetohighpracandidates") == 0){
				if (tokenTwo.compare("TRUE") == 0){ addAdvantage = true; }
				else if (tokenTwo.compare("FALSE") == 0){ addAdvantage = false; }
			}
			if (tokenOne.compare("#praadvantagecutoff") == 0){
				advantageCutoff = atoi(tokenTwo.c_str());
			}
			if (tokenOne.compare("#praadvantagevalue") == 0){
				advantageValue = atof(tokenTwo.c_str());
			}

			if (tokenOne.compare("#reserveodonorsforocandidates") == 0){
				if (tokenTwo.compare("TRUE") == 0){ reserveOForO = true; }
				else if (tokenTwo.compare("FALSE") == 0){ reserveOForO = false; }
			}
			if (tokenOne.compare("#includecompatiblepairs") == 0){
				if (tokenTwo.compare("TRUE") == 0){ includeCompatible = true; }
				else if (tokenTwo.compare("FALSE") == 0){ includeCompatible = false; }
			}
			if (tokenOne.compare("#excludeabdonorsfromsimulation") == 0){
				if (tokenTwo.compare("TRUE") == 0){ excludeABDonors = true; }
				else if (tokenTwo.compare("FALSE") == 0){ excludeABDonors = false; }
			}
			if (tokenOne.compare("#allowabbridgedonors") == 0){
				if (tokenTwo.compare("TRUE") == 0){ allowABBridge = true; }
				else if (tokenTwo.compare("FALSE") == 0){ allowABBridge = false; }
			}
			if (tokenOne.compare("#allowdesensitization") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { desensitize = true; }
				else if (tokenTwo.compare("FALSE") == 0) { desensitize = false; }
			}

			//Default
			if (tokenOne.compare("#filekpd") == 0) { fileKPD = tokenTwo; }
			if (tokenOne.compare("#filehlafrequency") == 0) { fileHLAFrequency = tokenTwo; }
			if (tokenOne.compare("#filehladictionary") == 0) { fileHLADictionary = tokenTwo; }
			if (tokenOne.compare("#filecharacteristics") == 0) { fileCharacteristics = tokenTwo; }

			if (tokenOne.compare("#printarrangementlist") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { printArrangementList = true; }
				else if (tokenTwo.compare("FALSE") == 0) { printArrangementList = false; }
			}
			if (tokenOne.compare("#printtransplantlist") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { printTransplantList = true; }
				else if (tokenTwo.compare("FALSE") == 0) { printTransplantList = false; }
			}
			if (tokenOne.compare("#printsimulationlog") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { printSimulationLog = true; }
				else if (tokenTwo.compare("FALSE") == 0) { printSimulationLog = false; }
			}
			if (tokenOne.compare("#printpopulationstats") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { printPopulationStats = true; }
				else if (tokenTwo.compare("FALSE") == 0) { printPopulationStats = false; }
			}

			if (tokenOne.compare("#rngseedcharacteristics") == 0) { rngSeedCharacteristics = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedndd") == 0) { rngSeedNDD = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedselection") == 0) { rngSeedSelection = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedadditionaldonor") == 0) { rngSeedAdditionalDonor = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedarrival") == 0) { rngSeedArrival = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedutility") == 0) { rngSeedUtility = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedcrossmatch") == 0) { rngSeedCrossmatch = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedpairfailure") == 0) { rngSeedPairFailure = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedmatchfailure") == 0) { rngSeedMatchFailure = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseeddonorgenerator") == 0) { rngSeedDonorGenerator = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedtransition") == 0) { rngSeedTransition = atoi(tokenTwo.c_str()); }
			if (tokenOne.compare("#rngseedexpectedutilityestimation") == 0) { rngSeedExpectedUtilityEstimation = atoi(tokenTwo.c_str()); }

		}
	}
	
	return true;
}

void KPDParameters::printLog(){

	//Folder Names
	kpdParametersLog << "-----------------" << std::endl;
	kpdParametersLog << "Folder Names" << std::endl;
	kpdParametersLog << "-----------------" << std::endl << std::endl;

	kpdParametersLog << "Output Folder: " << outputFolder << std::endl;
	kpdParametersLog << "Sub-Folder: " << subFolder << std::endl;
	
	kpdParametersLog << std::endl;

	//Simulation Format
	kpdParametersLog << "-----------------" << std::endl;
	kpdParametersLog << "Simulation Format" << std::endl;
	kpdParametersLog << "-----------------" << std::endl << std::endl;

	kpdParametersLog << "Utility Based On: " << KPDFunctions::toString(utilityScheme);
	if (utilityScheme == RANDOM_UTILITY) {
		if (matchUtilityLowerBound == matchUtilityUpperBound) {
			kpdParametersLog << " = " << matchUtilityLowerBound << " for all transplants";
		}
		else {
			kpdParametersLog << " ~ U(" << matchUtilityLowerBound << "," << matchUtilityUpperBound << ")";
		}
	}
	kpdParametersLog << std::endl;
		
	kpdParametersLog << "Donor Assignment Scheme: " << KPDFunctions::toString(donorAssignmentScheme) << std::endl;
	
	kpdParametersLog << "Timeline: " << KPDFunctions::toString(timeline) << std::endl;
	
	kpdParametersLog << "Assumed Match Failure Scheme: " << KPDFunctions::toString(assumedMatchFailureScheme) << std::endl;
	kpdParametersLog << "Actual Match Failure Scheme: " << KPDFunctions::toString(actualMatchFailureScheme) << std::endl;

	kpdParametersLog << std::endl;

	//Optimization Settings
	kpdParametersLog << "---------------------" << std::endl;
	kpdParametersLog << "Optimization Settings" << std::endl;
	kpdParametersLog << "---------------------" << std::endl << std::endl;

	kpdParametersLog << "Schemes Under Consideration: " << std::endl;
	if (cyclesAndChains) { kpdParametersLog << "   - Cycles/Chains  " << std::endl; }
	if (cyclesAndChainsWithFallbacks) { kpdParametersLog << "   - Cycles/Chains With Fallbacks  " << std::endl; }
	if (locallyRelevantSubsets) { kpdParametersLog << "   - LRS" << std::endl; }

	kpdParametersLog << std::endl;	
	
	//Numerical Parameters and Failures
	kpdParametersLog << "--------------------" << std::endl;
	kpdParametersLog << "Numerical Parameters" << std::endl;
	kpdParametersLog << "--------------------" << std::endl << std::endl;

	kpdParametersLog << "Simulation Iterations: " << numberOfIterations << " (From " << startingIterationID << "-" << startingIterationID + numberOfIterations - 1 << ")" << std::endl;

	kpdParametersLog << "Maximum Cycle Size: " << maxCycleSize << std::endl;
	kpdParametersLog << "Maximum Chain Length: " << maxChainLength << std::endl;
	if (locallyRelevantSubsets) {
		kpdParametersLog << "Maximum LRS Size: " << maxLRSSize << std::endl;
	}

	kpdParametersLog << std::endl;

	if (timeline == CONTINUOUS_TIMELINE){
		kpdParametersLog << "Pairs Transition States on Continuous Timeline" << std::endl;
		kpdParametersLog << "Time Frame: " << timeSpan << " Units" << std::endl;

		kpdParametersLog << "Arrival Rate: " << arrivals << std::endl;
		kpdParametersLog << "Chance of NDD Being Added: " << ndds << std::endl;

		kpdParametersLog << "Initial Match Run: " << initMatchRun << std::endl;
		kpdParametersLog << "Match Run Frequency: Every " << freqMatchRun << " Units" << std::endl;
		kpdParametersLog << "Processing Time: " << processingTime << std::endl;

		kpdParametersLog << "NDD Failure Rates:" << std::endl;
		kpdParametersLog << "   Active --> Inactive Rate: " << assumedNDDActiveToInactive << " (Assumed), " << actualNDDActiveToInactive << " (Actual)" << std::endl;
		kpdParametersLog << "   Active --> Withdrawn Rate: " << assumedNDDActiveToWithdrawn << " (Assumed), " << actualNDDActiveToWithdrawn << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Active Rate: " << assumedNDDInactiveToActive << " (Assumed), " << actualNDDInactiveToActive << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Withdrawn Rate: " << assumedNDDInactiveToWithdrawn << " (Assumed), " << actualNDDInactiveToWithdrawn << " (Actual)" << std::endl;

		kpdParametersLog << "Donor Failure Rates:" << std::endl;
		kpdParametersLog << "   Active --> Inactive Rate: " << assumedDonorActiveToInactive << " (Assumed), " << actualDonorActiveToInactive << " (Actual)" << std::endl;
		kpdParametersLog << "   Active --> Withdrawn Rate: " << assumedDonorActiveToWithdrawn << " (Assumed), " << actualDonorActiveToWithdrawn << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Active Rate: " << assumedDonorInactiveToActive << " (Assumed), " << actualDonorInactiveToActive << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Withdrawn Rate: " << assumedDonorInactiveToWithdrawn << " (Assumed), " << actualDonorInactiveToWithdrawn << " (Actual)" << std::endl;

		kpdParametersLog << "Candidate Failure Rates:" << std::endl;
		kpdParametersLog << "   Active --> Inactive Rate: " << assumedCandidateActiveToInactive << " (Assumed), " << actualCandidateActiveToInactive << " (Actual)" << std::endl;
		kpdParametersLog << "   Active --> Withdrawn Rate: " << assumedCandidateActiveToWithdrawn << " (Assumed), " << actualCandidateActiveToWithdrawn << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Active Rate: " << assumedCandidateInactiveToActive << " (Assumed), " << actualCandidateInactiveToActive << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Withdrawn Rate: " << assumedCandidateInactiveToWithdrawn << " (Assumed), " << actualCandidateInactiveToWithdrawn << " (Actual)" << std::endl;
			
		kpdParametersLog << std::endl;
	}
	else if (timeline == FIXED_MATCH_RUNS){
		kpdParametersLog << "Pairs Transition based on Fixed Match Run Intervals" << std::endl;
		kpdParametersLog << "Time Frame: " << timeSpan << " Units" << std::endl;

		kpdParametersLog << "Pair Arrival Per Match Run: " << arrivals << std::endl;
		kpdParametersLog << "NDD Arrival Per Match Run: " << ndds << std::endl;

		kpdParametersLog << "Initial Match Run: " << initMatchRun << std::endl;
		kpdParametersLog << "Match Run Frequency: Every " << freqMatchRun << " Units" << std::endl;
		kpdParametersLog << "Processing Time: " << processingTime << std::endl;

		kpdParametersLog << "NDD Failure Probabilities:" << std::endl;
		kpdParametersLog << "   Active --> Inactive Probability: " << assumedNDDActiveToInactive << " (Assumed), " << actualNDDActiveToInactive << " (Actual)" << std::endl;
		kpdParametersLog << "   Active --> Withdrawn Probability: " << assumedNDDActiveToWithdrawn << " (Assumed), " << actualNDDActiveToWithdrawn << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Active Probability: " << assumedNDDInactiveToActive << " (Assumed), " << actualNDDInactiveToActive << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Withdrawn Probability: " << assumedNDDInactiveToWithdrawn << " (Assumed), " << actualNDDInactiveToWithdrawn << " (Actual)" << std::endl;

		kpdParametersLog << "Donor Failure Probabilities:" << std::endl;
		kpdParametersLog << "   Active --> Inactive Probability: " << assumedDonorActiveToInactive << " (Assumed), " << actualDonorActiveToInactive << " (Actual)" << std::endl;
		kpdParametersLog << "   Active --> Withdrawn Probability: " << assumedDonorActiveToWithdrawn << " (Assumed), " << actualDonorActiveToWithdrawn << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Active Probability: " << assumedDonorInactiveToActive << " (Assumed), " << actualDonorInactiveToActive << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Withdrawn Probability: " << assumedDonorInactiveToWithdrawn << " (Assumed), " << actualDonorInactiveToWithdrawn << " (Actual)" << std::endl;

		kpdParametersLog << "Candidate Failure Probabilities:" << std::endl;
		kpdParametersLog << "   Active --> Inactive Probability: " << assumedCandidateActiveToInactive << " (Assumed), " << actualCandidateActiveToInactive << " (Actual)" << std::endl;
		kpdParametersLog << "   Active --> Withdrawn Probability: " << assumedCandidateActiveToWithdrawn << " (Assumed), " << actualCandidateActiveToWithdrawn << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Active Probability: " << assumedCandidateInactiveToActive << " (Assumed), " << actualCandidateInactiveToActive << " (Actual)" << std::endl;
		kpdParametersLog << "   Inactive --> Withdrawn Probability: " << assumedCandidateInactiveToWithdrawn << " (Assumed), " << actualCandidateInactiveToWithdrawn << " (Actual)" << std::endl;

		kpdParametersLog << std::endl;
	}

	if (assumedMatchFailureScheme == RANDOM_MATCH_FAILURE) {
		if (assumedMatchFailureLowerBound == assumedMatchFailureUpperBound) {
			kpdParametersLog << "Assume Match Failure Probability of " << assumedMatchFailureLowerBound + assumedMatchFailureAdjustment;
		}
		else {
			kpdParametersLog << "Assume Match Failure Probability ~ U(" << assumedMatchFailureLowerBound << "," << assumedMatchFailureUpperBound << ")";
			if (assumedMatchFailureAdjustment > 0) {
				kpdParametersLog << " + " << assumedMatchFailureAdjustment;
			}
		}
	}
	else {
		kpdParametersLog << "Assume Match Failure Probability Based on PRA";
		if (assumedMatchFailureAdjustment > 0) {
			kpdParametersLog << " + " << assumedMatchFailureAdjustment;
		}
	}
	kpdParametersLog << std::endl;

	if (actualMatchFailureScheme == RANDOM_MATCH_FAILURE) {
		if (actualMatchFailureLowerBound == actualMatchFailureUpperBound) {
			kpdParametersLog << "Actual Match Failure Probability of " << actualMatchFailureLowerBound + actualMatchFailureAdjustment;
		}
		else {
			kpdParametersLog << "Actual Match Failure Probability ~ U(" << actualMatchFailureLowerBound << "," << actualMatchFailureUpperBound << ")";
			if (actualMatchFailureAdjustment > 0) {
				kpdParametersLog << " + " << actualMatchFailureAdjustment;
			}
		}
	}
	else {
		kpdParametersLog << "Actual Match Failure Probability based on PRA";
		if (actualMatchFailureAdjustment > 0) {
			kpdParametersLog << " + " << actualMatchFailureAdjustment;
		}
	}
	kpdParametersLog << std::endl << std::endl;

	//Additional Options
	kpdParametersLog << "------------------" << std::endl;
	kpdParametersLog << "Additional Options" << std::endl;
	kpdParametersLog << "------------------" << std::endl << std::endl;
	
	if (estimateEU == true){
		kpdParametersLog << "Number of Expected Utility Iterations: " << numberOfEUIterations << std::endl;
	}

	if (donorAssignmentScheme == RANDOM_DONORS){
		kpdParametersLog << "Probability of Pair Having Two Donors: " << twoDonorsProbability << std::endl;
		kpdParametersLog << "Probability of Pair Having Three Donors: " << threeDonorsProbability << std::endl;
	}

	if (addAdvantage == true){
		kpdParametersLog << "Add Advantage to PRA Recipients Above " << advantageCutoff << std::endl;
		kpdParametersLog << "Additional Utility: " << advantageValue << std::endl;
	}
	else {
		kpdParametersLog << "Do Not Add Advantage to High PRA Recipients" << std::endl;
	}

	if (reserveOForO == false){
		kpdParametersLog << "Do Not ";
	}
	kpdParametersLog << "Reserve O Donors for O Candidates" << std::endl;	
	if (includeCompatible == false){
		kpdParametersLog << "Do Not ";
	}
	kpdParametersLog << "Include Compatible Pairs In Match Runs" << std::endl;
	if (excludeABDonors == false){
		kpdParametersLog << "Do Not ";
	}
	kpdParametersLog << "Exclude AB Donors From Entering Simulation" << std::endl;
	if (allowABBridge == false){
		kpdParametersLog << "Do Not ";
	}
	kpdParametersLog << "Include Chains Which End With AB Bridge Donor" << std::endl;
	if (desensitize == false) {
		kpdParametersLog << "Do Not ";
	}
	kpdParametersLog << "Allow Candidate Densitization for Matches That Require Desensitization" << std::endl;

	kpdParametersLog << std::endl;
	
	//Defaults
	kpdParametersLog << "------" << std::endl;
	kpdParametersLog << "Files" << std::endl;
	kpdParametersLog << "------" << std::endl << std::endl;

	kpdParametersLog << "KPD File Name: " << fileKPD << std::endl;
	kpdParametersLog << "Bone Marrow Registry File Name: " << fileHLAFrequency << std::endl;
	kpdParametersLog << "HLA Dictionary File Name: " << fileHLADictionary << std::endl;
	kpdParametersLog << "Survival Parameters File Name: " << fileCharacteristics << std::endl;

	kpdParametersLog << std::endl;
	
	kpdParametersLog << "------------------" << std::endl;
	kpdParametersLog << "Output Suppression" << std::endl;
	kpdParametersLog << "------------------" << std::endl << std::endl;

	kpdParametersLog << "Output Files Produced:" << std::endl;
	if (printArrangementList) {
		kpdParametersLog << "   - Arrangement List" << std::endl;
	}
	if (printTransplantList) {
		kpdParametersLog << "   - Transplant List" << std::endl;
	}
	if (printSimulationLog) {
		kpdParametersLog << "   - Simulation Log" << std::endl;
	}
	if (printPopulationStats) {
		kpdParametersLog << "   - Population Statistics" << std::endl;
	}

	kpdParametersLog << std::endl;
	
	kpdParametersLog << "-----------------------------" << std::endl;
	kpdParametersLog << "Random Number Generator Seeds" << std::endl;
	kpdParametersLog << "-----------------------------" << std::endl << std::endl;

	kpdParametersLog << "Characteristics Random Number Generator Seed: " << rngSeedCharacteristics << std::endl;
	kpdParametersLog << "NDD Generation Random Number Generator Seed: " << rngSeedNDD << std::endl;
	kpdParametersLog << "Pair Selection Random Number Generator Seed: " << rngSeedSelection << std::endl;
	kpdParametersLog << "Additional Donor Random Number Generator Seed: " << rngSeedAdditionalDonor << std::endl;
	kpdParametersLog << "Arrival Time Random Number Generator Seed: " << rngSeedArrival << std::endl;
	kpdParametersLog << "Match Utility Random Number Generator Seed: " << rngSeedUtility << std::endl;
	kpdParametersLog << "Lab Crossmatch Random Number Generator Seed: " << rngSeedCrossmatch << std::endl;
	kpdParametersLog << "Pair Failure Randon Number Generator Seed: " << rngSeedPairFailure << std::endl;
	kpdParametersLog << "Match Failure Random Number Generator Seed: " << rngSeedMatchFailure << std::endl;
	kpdParametersLog << "Donor Generation Random Number Generator Seed: " << rngSeedDonorGenerator << std::endl;
	kpdParametersLog << "Transitions Random Number Generator Seed: " << rngSeedTransition << std::endl;
	kpdParametersLog << "Expected Utility Estimation Random Number Generator Seed: " << rngSeedExpectedUtilityEstimation << std::endl;

	kpdParametersLog << std::endl;

	logFile = "output/DDSim/" + outputFolder + "/" + subFolder + "/SimulationLogs/Log-Parameters.txt";
	std::ofstream outputFileLog(logFile.c_str());
	outputFileLog << kpdParametersLog.str();
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

KPDDonorAssignmentScheme KPDParameters::getDonorAssignmentScheme() {
	return donorAssignmentScheme;
}

KPDTimeline KPDParameters::getTimeline(){
	return timeline;
}

KPDMatchFailureScheme KPDParameters::getAssumedMatchFailureScheme(){
	return assumedMatchFailureScheme;
}

KPDMatchFailureScheme KPDParameters::getActualMatchFailureScheme(){
	return actualMatchFailureScheme;
}

bool KPDParameters::runCyclesAndChains() {
	return cyclesAndChains;
}

bool KPDParameters::runCyclesAndChainsWithFallbacks() {
	return cyclesAndChainsWithFallbacks;
}

bool KPDParameters::runLocallyRelevantSubsets() {
	return locallyRelevantSubsets;
}

int KPDParameters::getNumberOfIterations(){
	return numberOfIterations;
}

int KPDParameters::getStartingIterationID(){
	return startingIterationID;
}

double KPDParameters::getArrivals(){
	return arrivals;
}

double KPDParameters::getNDDs(){
	return ndds;
}

int KPDParameters::getTimeSpan(){
	return timeSpan;
}

int KPDParameters::getInitMatchRun(){
	return initMatchRun;
}

int KPDParameters::getFreqMatchRun(){
	return freqMatchRun;
}

int KPDParameters::getProcessingTime(){
	return processingTime;
}

int KPDParameters::getMaxCycleSize() {
	return maxCycleSize;
}

int KPDParameters::getMaxChainLength() {
	return maxChainLength;
}

int KPDParameters::getMaxLRSSize() {
	return maxLRSSize;
}

double KPDParameters::getMatchUtilityLowerBound() {
	return matchUtilityLowerBound;
}

double KPDParameters::getMatchUtilityUpperBound() {
	return matchUtilityUpperBound;
}

double KPDParameters::getAssumedNDDActiveToInactive() {
	return assumedNDDActiveToInactive;
}

double KPDParameters::getAssumedNDDActiveToWithdrawn() {
	return assumedNDDActiveToWithdrawn;
}

double KPDParameters::getAssumedNDDInactiveToActive() {
	return assumedNDDInactiveToActive;
}

double KPDParameters::getAssumedNDDInactiveToWithdrawn() {
	return assumedNDDInactiveToWithdrawn;
}

double KPDParameters::getActualNDDActiveToInactive() {
	return actualNDDActiveToInactive;
}

double KPDParameters::getActualNDDActiveToWithdrawn() {
	return actualNDDActiveToWithdrawn;
}

double KPDParameters::getActualNDDInactiveToActive() {
	return actualNDDInactiveToActive;
}

double KPDParameters::getActualNDDInactiveToWithdrawn() {
	return actualNDDInactiveToWithdrawn;
}

double KPDParameters::getAssumedDonorActiveToInactive() {	
	return assumedDonorActiveToInactive;
}

double KPDParameters::getAssumedDonorActiveToWithdrawn() {	
	return assumedDonorActiveToWithdrawn;
}

double KPDParameters::getAssumedDonorInactiveToActive() {	
	return assumedDonorInactiveToActive;
}

double KPDParameters::getAssumedDonorInactiveToWithdrawn() {	
	return assumedDonorInactiveToWithdrawn;
}

double KPDParameters::getActualDonorActiveToInactive() {
	return actualDonorActiveToInactive;
}

double KPDParameters::getActualDonorActiveToWithdrawn() {
	return actualDonorActiveToWithdrawn;
}

double KPDParameters::getActualDonorInactiveToActive() {
	return actualDonorInactiveToActive;
}

double KPDParameters::getActualDonorInactiveToWithdrawn() {
	return actualDonorInactiveToWithdrawn;
}

double KPDParameters::getAssumedCandidateActiveToInactive() {
	return assumedCandidateActiveToInactive;
}

double KPDParameters::getAssumedCandidateActiveToWithdrawn() {
	return assumedCandidateActiveToWithdrawn;
}

double KPDParameters::getAssumedCandidateInactiveToActive() {
	return assumedCandidateInactiveToActive;
}

double KPDParameters::getAssumedCandidateInactiveToWithdrawn() {
	return assumedCandidateInactiveToWithdrawn;
}

double KPDParameters::getActualCandidateActiveToInactive() {
	return actualCandidateActiveToInactive;
}

double KPDParameters::getActualCandidateActiveToWithdrawn() {
	return actualCandidateActiveToWithdrawn;
}

double KPDParameters::getActualCandidateInactiveToActive() {
	return actualCandidateInactiveToActive;
}

double KPDParameters::getActualCandidateInactiveToWithdrawn() {
	return actualCandidateInactiveToWithdrawn;
}

double KPDParameters::getAssumedMatchFailureLowerBound() {
	return assumedMatchFailureLowerBound;
}

double KPDParameters::getAssumedMatchFailureUpperBound() {
	return assumedMatchFailureUpperBound;
}

double KPDParameters::getAssumedMatchFailureAdjustment() {
	return assumedMatchFailureAdjustment;
}

double KPDParameters::getActualMatchFailureLowerBound() {	
	return actualMatchFailureLowerBound;
}

double KPDParameters::getActualMatchFailureUpperBound() {
	return actualMatchFailureLowerBound;
}

double KPDParameters::getActualMatchFailureAdjustment() {
	return actualMatchFailureAdjustment;
}

bool KPDParameters::estimateExpectedUtility(){
	return estimateEU;
}

int KPDParameters::getNumberOfExpectedUtilityIterations(){
	return numberOfEUIterations;
}

double KPDParameters::getTwoDonorsProbability(){
	return twoDonorsProbability;
}

double KPDParameters::getThreeDonorsProbability() {
	return threeDonorsProbability;
}

bool KPDParameters::addAdvantageToHighPRACandidates(){
	return addAdvantage;
}

int KPDParameters::getPRAAdvantageCutoff(){
	return advantageCutoff;
}

double KPDParameters::getPRAAdvantageValue(){
	return advantageValue;
}

bool KPDParameters::reserveODonorsForOCandidates(){
	return reserveOForO;
}

bool KPDParameters::includeCompatiblePairs(){
	return includeCompatible;
}

bool KPDParameters::excludeABDonorsFromSimulation(){
	return excludeABDonors;
}

bool KPDParameters::allowABBridgeDonors(){
	return allowABBridge;
}

bool KPDParameters::allowDesensitization(){
	return desensitize;
}

std::string KPDParameters::getFileKPD() {
	return fileKPD;
}

std::string KPDParameters::getFileHLAFrequency() {
	return fileHLAFrequency;
}

std::string KPDParameters::getFileHLADictionary() {
	return fileHLADictionary;
}

std::string KPDParameters::getFileCharacteristics() {
	return fileCharacteristics;
}

bool KPDParameters::outputArrangementList() {
	return printArrangementList;
}

bool KPDParameters::outputTransplantList() {
	return printTransplantList;
}

bool KPDParameters::outputSimulationLog() {
	return printSimulationLog;
}

bool KPDParameters::outputPopulationStats() {
	return printPopulationStats;
}

int KPDParameters::getRNGSeedCharacteristics() {
	return rngSeedCharacteristics;
}

int KPDParameters::getRNGSeedNDD() {
	return rngSeedNDD;
}

int KPDParameters::getRNGSeedSelection() {
	return rngSeedSelection;
}

int KPDParameters::getRNGSeedAdditionalDonor() {
	return rngSeedAdditionalDonor;
}

int KPDParameters::getRNGSeedArrival() {
	return rngSeedArrival;
}

int KPDParameters::getRNGSeedUtility() {
	return rngSeedUtility;
}

int KPDParameters::getRNGSeedCrossmatch() {
	return rngSeedCrossmatch;
}

int KPDParameters::getRNGSeedPairFailure() {
	return rngSeedPairFailure;
}

int KPDParameters::getRNGSeedMatchFailure() {
	return rngSeedMatchFailure;
}

int KPDParameters::getRNGSeedDonorGenerator() {
	return rngSeedDonorGenerator;
}

int KPDParameters::getRNGSeedTransition() {
	return rngSeedTransition;
}

int KPDParameters::getRNGSeedExpectedUtilityEstimation() {
	return rngSeedExpectedUtilityEstimation;
}


#endif