/* ---------------------------------------------
DD-Parameters.h
Stores Simulation Parameters
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
	bool cyclesAndChains;
	bool cyclesAndChainsWithFallbacks;
	bool locallyRelevantSubgraphs;

	KPDUtilityScheme utilityScheme;	
	KPDDonorAssignment donorAssignment;	
	KPDTimeline timeline;
	KPDMatchFailure assumedMatchFailure;
	KPDMatchFailure actualMatchFailure;
		
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

	double assumedNDDActiveToInactive;
	double assumedNDDActiveToWithdrawn;
	double assumedNDDInactiveToActive;
	double assumedNDDInactiveToWithdrawn;

	double actualNDDActiveToInactive;
	double actualNDDActiveToWithdrawn;
	double actualNDDInactiveToActive;
	double actualNDDInactiveToWithdrawn;

	double assumedDonorActiveToInactive;
	double assumedDonorActiveToWithdrawn;
	double assumedDonorInactiveToActive;
	double assumedDonorInactiveToWithdrawn;

	double actualDonorActiveToInactive;
	double actualDonorActiveToWithdrawn;
	double actualDonorInactiveToActive;
	double actualDonorInactiveToWithdrawn;

	double assumedCandidateActiveToInactive;
	double assumedCandidateActiveToWithdrawn;
	double assumedCandidateInactiveToActive;
	double assumedCandidateInactiveToWithdrawn;

	double actualCandidateActiveToInactive;
	double actualCandidateActiveToWithdrawn;
	double actualCandidateInactiveToActive;
	double actualCandidateInactiveToWithdrawn;

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

	//Files
	std::string fileKPDData;
	std::string fileHLAFrequency;
	std::string fileHLADictionary;
	std::string fileSurvivalParameters;
	std::string fileDeceasedDonors;
	std::string fileWaitingListCandidates;
	
	//Random Number Generator Seeds
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
	bool runCyclesAndChains();
	bool runCyclesAndChainsWithFallbacks();
	bool runLocallyRelevantSubgraphs();

	KPDUtilityScheme getUtilityScheme();
	KPDDonorAssignment getDonorAssignment();	
	KPDTimeline getTimeline();
	KPDMatchFailure getAssumedMatchFailure();
	KPDMatchFailure getActualMatchFailure();
	
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

	double getAssumedNDDActiveToInactive();
	double getAssumedNDDActiveToWithdrawn();
	double getAssumedNDDInactiveToActive();
	double getAssumedNDDInactiveToWithdrawn();

	double getActualNDDActiveToInactive();
	double getActualNDDActiveToWithdrawn();
	double getActualNDDInactiveToActive();
	double getActualNDDInactiveToWithdrawn();

	double getAssumedDonorActiveToInactive();
	double getAssumedDonorActiveToWithdrawn();
	double getAssumedDonorInactiveToActive();
	double getAssumedDonorInactiveToWithdrawn();

	double getActualDonorActiveToInactive();
	double getActualDonorActiveToWithdrawn();
	double getActualDonorInactiveToActive();
	double getActualDonorInactiveToWithdrawn();

	double getAssumedCandidateActiveToInactive();
	double getAssumedCandidateActiveToWithdrawn();
	double getAssumedCandidateInactiveToActive();
	double getAssumedCandidateInactiveToWithdrawn();

	double getActualCandidateActiveToInactive();
	double getActualCandidateActiveToWithdrawn();
	double getActualCandidateInactiveToActive();
	double getActualCandidateInactiveToWithdrawn();

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
	
	//Files and Folders
	std::string getFileKPDData();
	std::string getFileHLAFrequency();
	std::string getFileHLADictionary();
	std::string getFileSurvivalParameters();
	std::string getFileDeceasedDonors();
	std::string getFileWaitingListCandidates();

	//Random Number Generator Seeds
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

	//Output Suppression
	bool suppressExchangeOutput();
	bool suppressSimulationOutput();
	bool suppressPopulationOutput();
	
	//Log
	void printLog(); // Prints out parameter information to file
};

KPDParameters::KPDParameters(){
	
	//Output Folder
	outputFolder = "Test";
	subFolder = "Test";

	//Simulation Settings
	cyclesAndChains = true;
	cyclesAndChainsWithFallbacks = false;
	locallyRelevantSubgraphs = false;

	utilityScheme = UTILITY_TRANSPLANTS;
	donorAssignment = DONOR_ASSIGNMENT_PAIRED;
	timeline = TIMELINE_FIXED;
	assumedMatchFailure = MATCH_FAILURE_RANDOM;
	actualMatchFailure = MATCH_FAILURE_RANDOM;
	
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

	assumedNDDActiveToInactive = 0.0;
	assumedNDDActiveToWithdrawn = 0.0;
	assumedNDDInactiveToActive = 0.0;
	assumedNDDInactiveToWithdrawn = 0.0;

	actualNDDActiveToInactive = 0.0;
	actualNDDActiveToWithdrawn = 0.0;
	actualNDDInactiveToActive = 0.0;
	actualNDDInactiveToWithdrawn = 0.0;

	assumedDonorActiveToInactive = 0.0;
	assumedDonorActiveToWithdrawn = 0.0;
	assumedDonorInactiveToActive = 0.0;
	assumedDonorInactiveToWithdrawn = 0.0;

	actualDonorActiveToInactive = 0.0;
	actualDonorActiveToWithdrawn = 0.0;
	actualDonorInactiveToActive = 0.0;
	actualDonorInactiveToWithdrawn = 0.0;

	assumedCandidateActiveToInactive = 0.0;
	assumedCandidateActiveToWithdrawn = 0.0;
	assumedCandidateInactiveToActive = 0.0;
	assumedCandidateInactiveToWithdrawn = 0.0;

	actualCandidateActiveToInactive = 0.0;
	actualCandidateActiveToWithdrawn = 0.0;
	actualCandidateInactiveToActive = 0.0;
	actualCandidateInactiveToWithdrawn = 0.0;
	
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

	//Files and Folders
	fileKPDData = "APD Data - KPD Simulations.csv";
	fileHLAFrequency = "HLAFrequency.csv";
	fileHLADictionary = "HLADictionary.csv";
	fileSurvivalParameters = "SurvivalParameters.csv";
	
	//Random Number Generators Seeds
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
			if (tokenOne.compare("#cyclesandchains") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { cyclesAndChains = true; }
				else if (tokenTwo.compare("FALSE") == 0) { cyclesAndChains = false; }
			}
			if (tokenOne.compare("#cyclesandchainswithfallbacks") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { cyclesAndChainsWithFallbacks = true; }
				else if (tokenTwo.compare("FALSE") == 0) { cyclesAndChainsWithFallbacks = false; }
			}
			if (tokenOne.compare("#locallyrelevantsubgraphs") == 0) {
				if (tokenTwo.compare("TRUE") == 0) { locallyRelevantSubgraphs = true; }
				else if (tokenTwo.compare("FALSE") == 0) { locallyRelevantSubgraphs = false; }
			}

			if (tokenOne.compare("#utilityscheme") == 0){
				utilityScheme = KPDFunctions::stringToUtilityScheme(tokenTwo); //UTILITY_TRANSPLANTS, UTILITY_FIVE_YEAR_SURVIVAL, UTILITY_TEN_YEAR_SURVIVAL, UTILITY_TRANSPLANT_DIFFICULTY, UTILITY_RANDOM
			}
			if (tokenOne.compare("#donorassignment") == 0){
				donorAssignment = KPDFunctions::stringToDonorAssignment(tokenTwo); //DONOR_ASSIGNMENT_PAIRED, DONOR_ASSIGNMENT_RANDOM
			}			
			if (tokenOne.compare("#timeline") == 0) {
				timeline = KPDFunctions::stringToTimeline(tokenTwo); //TIMELINE_FIXED, TIMELINE_CONTINUOUS
			}			
			if (tokenOne.compare("#assumedmatchfailure") == 0){
				assumedMatchFailure = KPDFunctions::stringToMatchFailure(tokenTwo); // MATCH_FAILURE_PRA_BASED, MATCH_FAILURE_RANDOM
			}
			if (tokenOne.compare("#actualmatchfailure") == 0) {
				actualMatchFailure = KPDFunctions::stringToMatchFailure(tokenTwo);
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

			if (tokenOne.compare("#assumednddactivetoinactive") == 0) { assumedNDDActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumednddactivetowithdrawn") == 0) { assumedNDDActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumednddinactivetoactive") == 0) { assumedNDDInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumednddinactivetowithdrawn") == 0) { assumedNDDInactiveToWithdrawn = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#actualnddactivetoinactive") == 0) { actualNDDActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualnddactivetowithdrawn") == 0) { actualNDDActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualnddinactivetoactive") == 0) { actualNDDInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualnddinactivetowithdrawn") == 0) { actualNDDInactiveToWithdrawn = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#assumeddonoractivetoinactive") == 0) { assumedDonorActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumeddonoractivetowithdrawn") == 0) { assumedDonorActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumeddonorinactivetoactive") == 0) { assumedDonorInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumeddonorinactivetowithdrawn") == 0) { assumedDonorInactiveToWithdrawn = atof(tokenTwo.c_str()); }
			
			if (tokenOne.compare("#actualdonoractivetoinactive") == 0) { actualDonorActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualdonoractivetowithdrawn") == 0) { actualDonorActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualdonorinactivetoactive") == 0) { actualDonorInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualdonorinactivetowithdrawn") == 0) { actualDonorInactiveToWithdrawn = atof(tokenTwo.c_str()); }

			if (tokenOne.compare("#assumedcandidateactivetoinactive") == 0) { assumedCandidateActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumedcandidateactivetowithdrawn") == 0) { assumedCandidateActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumedcandidateinactivetoactive") == 0) { assumedCandidateInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#assumedcandidateinactivetowithdrawn") == 0) { assumedCandidateInactiveToWithdrawn = atof(tokenTwo.c_str()); }
			
			if (tokenOne.compare("#actualcandidateactivetoinactive") == 0) { actualCandidateActiveToInactive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualcandidateactivetowithdrawn") == 0) { actualCandidateActiveToWithdrawn = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualcandidateinactivetoactive") == 0) { actualCandidateInactiveToActive = atof(tokenTwo.c_str()); }
			if (tokenOne.compare("#actualcandidateinactivetowithdrawn") == 0) { actualCandidateInactiveToWithdrawn = atof(tokenTwo.c_str()); }

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

			//Files and Folders
			if (tokenOne.compare("#filekpddata") == 0) { fileKPDData = tokenTwo; }
			if (tokenOne.compare("#filehlafrequency") == 0) { fileHLAFrequency = tokenTwo; }
			if (tokenOne.compare("#filehladictionary") == 0) { fileHLADictionary = tokenTwo; }
			if (tokenOne.compare("#filesurvivalparameters") == 0) { fileSurvivalParameters = tokenTwo; }
			if (tokenOne.compare("#filedeceaseddonors") == 0) { fileDeceasedDonors = tokenTwo; }
			if (tokenOne.compare("#filewaitinglistcandidates") == 0) { fileWaitingListCandidates = tokenTwo; }

			//Random Number Generator Seeds
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

	parametersLog << "Optimization Scheme: " << std::endl;
	if (cyclesAndChains) { parametersLog << "- Cycles/Chains" << std::endl; }
	if (cyclesAndChainsWithFallbacks) { parametersLog << "- Cycles/Chains With Fallbacks" << std::endl; }
	if (locallyRelevantSubgraphs) { parametersLog << "- LRS" << std::endl; }
	parametersLog << std::endl;

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
		
	parametersLog << "Donor Assignment: " << KPDFunctions::donorAssignmentToString(donorAssignment) << std::endl;	
	parametersLog << "Timeline: " << KPDFunctions::timelineToString(timeline) << std::endl;	
	parametersLog << "Assumed Match Failure: " << KPDFunctions::matchFailureToString(assumedMatchFailure) << std::endl;
	parametersLog << "Actual Match Failure: " << KPDFunctions::matchFailureToString(actualMatchFailure) << std::endl;

	parametersLog << std::endl;
		
	//Numerical Parameters
	parametersLog << "--------------------" << std::endl;
	parametersLog << "Numerical Parameters" << std::endl;
	parametersLog << "--------------------" << std::endl << std::endl;

	parametersLog << "Simulation Iterations: " << numberOfIterations << " (From " << startingIterationID << "-" << startingIterationID + numberOfIterations - 1 << ")" << std::endl;

	parametersLog << "Maximum Cycle Size: " << maxCycleSize << std::endl;
	parametersLog << "Maximum Chain Length: " << maxChainLength << std::endl;
	if (locallyRelevantSubgraphs) {
		parametersLog << "Maximum LRS Size: " << maxLRSSize << std::endl;
	}

	parametersLog << std::endl;

	if (timeline == TIMELINE_CONTINUOUS){
		parametersLog << "Pairs Transition States on Continuous Timeline" << std::endl;
		parametersLog << "Time Frame: " << timeSpan << " Units" << std::endl;

		parametersLog << "Arrival Rate: " << arrivals << std::endl;
		parametersLog << "Chance of NDD Being Added: " << ndds << std::endl;

		parametersLog << "Initial Match Run: " << initMatchRun << std::endl;
		parametersLog << "Match Run Frequency: Every " << freqMatchRun << " Units" << std::endl;
		parametersLog << "Processing Time: " << processingTime << std::endl;

		parametersLog << "NDD Failure Rates:" << std::endl;
		parametersLog << "   Active --> Inactive Rate: " << assumedNDDActiveToInactive << " (Assumed), " << actualNDDActiveToInactive << " (Actual)" << std::endl;
		parametersLog << "   Active --> Withdrawn Rate: " << assumedNDDActiveToWithdrawn << " (Assumed), " << actualNDDActiveToWithdrawn << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Active Rate: " << assumedNDDInactiveToActive << " (Assumed), " << actualNDDInactiveToActive << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Withdrawn Rate: " << assumedNDDInactiveToWithdrawn << " (Assumed), " << actualNDDInactiveToWithdrawn << " (Actual)" << std::endl;

		parametersLog << "Donor Failure Rates:" << std::endl;
		parametersLog << "   Active --> Inactive Rate: " << assumedDonorActiveToInactive << " (Assumed), " << actualDonorActiveToInactive << " (Actual)" << std::endl;
		parametersLog << "   Active --> Withdrawn Rate: " << assumedDonorActiveToWithdrawn << " (Assumed), " << actualDonorActiveToWithdrawn << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Active Rate: " << assumedDonorInactiveToActive << " (Assumed), " << actualDonorInactiveToActive << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Withdrawn Rate: " << assumedDonorInactiveToWithdrawn << " (Assumed), " << actualDonorInactiveToWithdrawn << " (Actual)" << std::endl;

		parametersLog << "Candidate Failure Rates:" << std::endl;
		parametersLog << "   Active --> Inactive Rate: " << assumedCandidateActiveToInactive << " (Assumed), " << actualCandidateActiveToInactive << " (Actual)" << std::endl;
		parametersLog << "   Active --> Withdrawn Rate: " << assumedCandidateActiveToWithdrawn << " (Assumed), " << actualCandidateActiveToWithdrawn << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Active Rate: " << assumedCandidateInactiveToActive << " (Assumed), " << actualCandidateInactiveToActive << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Withdrawn Rate: " << assumedCandidateInactiveToWithdrawn << " (Assumed), " << actualCandidateInactiveToWithdrawn << " (Actual)" << std::endl;
			
		parametersLog << std::endl;
	}
	else if (timeline == TIMELINE_FIXED){
		parametersLog << "Pairs Transition based on Fixed Match Run Intervals" << std::endl;
		parametersLog << "Time Frame: " << timeSpan << " Units" << std::endl;

		parametersLog << "Pair Arrival Per Match Run: " << arrivals << std::endl;
		parametersLog << "NDD Arrival Per Match Run: " << ndds << std::endl;

		parametersLog << "Initial Match Run: " << initMatchRun << std::endl;
		parametersLog << "Match Run Frequency: Every " << freqMatchRun << " Units" << std::endl;
		parametersLog << "Processing Time: " << processingTime << std::endl;

		parametersLog << "NDD Failure Probabilities:" << std::endl;
		parametersLog << "   Active --> Inactive Probability: " << assumedNDDActiveToInactive << " (Assumed), " << actualNDDActiveToInactive << " (Actual)" << std::endl;
		parametersLog << "   Active --> Withdrawn Probability: " << assumedNDDActiveToWithdrawn << " (Assumed), " << actualNDDActiveToWithdrawn << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Active Probability: " << assumedNDDInactiveToActive << " (Assumed), " << actualNDDInactiveToActive << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Withdrawn Probability: " << assumedNDDInactiveToWithdrawn << " (Assumed), " << actualNDDInactiveToWithdrawn << " (Actual)" << std::endl;

		parametersLog << "Donor Failure Probabilities:" << std::endl;
		parametersLog << "   Active --> Inactive Probability: " << assumedDonorActiveToInactive << " (Assumed), " << actualDonorActiveToInactive << " (Actual)" << std::endl;
		parametersLog << "   Active --> Withdrawn Probability: " << assumedDonorActiveToWithdrawn << " (Assumed), " << actualDonorActiveToWithdrawn << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Active Probability: " << assumedDonorInactiveToActive << " (Assumed), " << actualDonorInactiveToActive << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Withdrawn Probability: " << assumedDonorInactiveToWithdrawn << " (Assumed), " << actualDonorInactiveToWithdrawn << " (Actual)" << std::endl;

		parametersLog << "Candidate Failure Probabilities:" << std::endl;
		parametersLog << "   Active --> Inactive Probability: " << assumedCandidateActiveToInactive << " (Assumed), " << actualCandidateActiveToInactive << " (Actual)" << std::endl;
		parametersLog << "   Active --> Withdrawn Probability: " << assumedCandidateActiveToWithdrawn << " (Assumed), " << actualCandidateActiveToWithdrawn << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Active Probability: " << assumedCandidateInactiveToActive << " (Assumed), " << actualCandidateInactiveToActive << " (Actual)" << std::endl;
		parametersLog << "   Inactive --> Withdrawn Probability: " << assumedCandidateInactiveToWithdrawn << " (Assumed), " << actualCandidateInactiveToWithdrawn << " (Actual)" << std::endl;

		parametersLog << std::endl;
	}

	if (assumedMatchFailure == MATCH_FAILURE_RANDOM) {
		if (assumedMatchFailureLowerBound == assumedMatchFailureUpperBound) {
			parametersLog << "Assume Match Failure Probability of " << assumedMatchFailureLowerBound + assumedMatchFailureAdjustment;
		}
		else {
			parametersLog << "Assume Match Failure Probability ~ U(" << assumedMatchFailureLowerBound << "," << assumedMatchFailureUpperBound << ")";
			if (assumedMatchFailureAdjustment > 0) {
				parametersLog << " + " << assumedMatchFailureAdjustment;
			}
		}
	}
	else {
		parametersLog << "Assume Match Failure Probability Based on PRA";
		if (assumedMatchFailureAdjustment > 0) {
			parametersLog << " + " << assumedMatchFailureAdjustment;
		}
	}
	parametersLog << std::endl;

	if (actualMatchFailure == MATCH_FAILURE_RANDOM) {
		if (actualMatchFailureLowerBound == actualMatchFailureUpperBound) {
			parametersLog << "Actual Match Failure Probability of " << actualMatchFailureLowerBound + actualMatchFailureAdjustment;
		}
		else {
			parametersLog << "Actual Match Failure Probability ~ U(" << actualMatchFailureLowerBound << "," << actualMatchFailureUpperBound << ")";
			if (actualMatchFailureAdjustment > 0) {
				parametersLog << " + " << actualMatchFailureAdjustment;
			}
		}
	}
	else {
		parametersLog << "Actual Match Failure Probability based on PRA";
		if (actualMatchFailureAdjustment > 0) {
			parametersLog << " + " << actualMatchFailureAdjustment;
		}
	}
	parametersLog << std::endl << std::endl;

	//Additional Options
	parametersLog << "------------------" << std::endl;
	parametersLog << "Additional Options" << std::endl;
	parametersLog << "------------------" << std::endl << std::endl;
	
	if (estimateEU == true){
		parametersLog << "Number of Expected Utility Iterations: " << numberOfEUIterations << std::endl;
	}

	if (donorAssignment == DONOR_ASSIGNMENT_RANDOM){
		parametersLog << "Probability of Pair Having Two Donors: " << twoDonorsProbability << std::endl;
		parametersLog << "Probability of Pair Having Three Donors: " << threeDonorsProbability << std::endl;
	}

	if (addAdvantage == true){
		parametersLog << "Add Advantage to PRA Recipients Above " << advantageCutoff << std::endl;
		parametersLog << "Additional Utility: " << advantageValue << std::endl;
	}
	else {
		parametersLog << "Do Not Add Advantage to High PRA Recipients" << std::endl;
	}

	if (reserveOForO == false){
		parametersLog << "Do Not ";
	}
	parametersLog << "Reserve O Donors for O Candidates" << std::endl;	
	if (includeCompatible == false){
		parametersLog << "Do Not ";
	}
	parametersLog << "Include Compatible Pairs In Match Runs" << std::endl;
	if (excludeABDonors == false){
		parametersLog << "Do Not ";
	}
	parametersLog << "Exclude AB Donors From Entering Simulation" << std::endl;
	if (allowABBridge == false){
		parametersLog << "Do Not ";
	}
	parametersLog << "Include Chains Which End With AB Bridge Donor" << std::endl;
	if (desensitize == false) {
		parametersLog << "Do Not ";
	}
	parametersLog << "Allow Candidate Densitization for Matches That Require Desensitization" << std::endl;

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

	parametersLog << "Characteristics RNG Seed: " << rngSeedCharacteristics << std::endl;
	parametersLog << "NDD Generation RNG Seed: " << rngSeedNDD << std::endl;
	parametersLog << "Pair Selection RNG Seed: " << rngSeedSelection << std::endl;
	parametersLog << "Additional Donor RNG Seed: " << rngSeedAdditionalDonor << std::endl;
	parametersLog << "Arrival Time RNG Seed: " << rngSeedArrival << std::endl;
	parametersLog << "Match Utility RNG Seed: " << rngSeedUtility << std::endl;
	parametersLog << "Lab Crossmatch RNG Seed: " << rngSeedCrossmatch << std::endl;
	parametersLog << "Pair Failure RNG Seed: " << rngSeedPairFailure << std::endl;
	parametersLog << "Match Failure RNG Seed: " << rngSeedMatchFailure << std::endl;
	parametersLog << "Donor Generation RNG Seed: " << rngSeedDonorGenerator << std::endl;
	parametersLog << "Transitions RNG Seed: " << rngSeedTransition << std::endl;
	parametersLog << "Expected Utility Estimation RNG Seed: " << rngSeedExpectedUtilityEstimation << std::endl;

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

bool KPDParameters::runCyclesAndChains() {
	return cyclesAndChains;
}

bool KPDParameters::runCyclesAndChainsWithFallbacks() {
	return cyclesAndChainsWithFallbacks;
}

bool KPDParameters::runLocallyRelevantSubgraphs() {
	return locallyRelevantSubgraphs;
}

KPDUtilityScheme KPDParameters::getUtilityScheme(){
	return utilityScheme;
}

KPDDonorAssignment KPDParameters::getDonorAssignment() {
	return donorAssignment;
}

KPDTimeline KPDParameters::getTimeline(){
	return timeline;
}

KPDMatchFailure KPDParameters::getAssumedMatchFailure(){
	return assumedMatchFailure;
}

KPDMatchFailure KPDParameters::getActualMatchFailure(){
	return actualMatchFailure;
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