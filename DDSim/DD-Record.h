
#ifndef RECORD_H
#define RECORD_H

#include "DD-Enums-Functions.h"
#include "DD-Candidate.h"
#include "DD-Donor.h"
#include "DD-Data.h"
#include "DD-Parameters.h"
#include "DD-Node.h"
#include "DD-Match.h"
#include "DD-RNG.h"

#include <vector>
#include <string>
#include <deque>
#include <iostream>
#include <sstream>
#include <fstream>

class KPDRecord {

private:

	KPDData * kpdData;
	KPDParameters * kpdParameters;

	int currentIteration;

	// Pool Information
	std::vector<int> matchRunSchedule;
	
	std::vector<KPDNode *> nodeVector;
	std::vector<std::vector<std::vector<KPDMatch *> > > matchMatrix;

	std::vector<std::vector<bool> > incidenceMatrix;
	
	std::vector<std::deque<KPDUnderlyingStatus> > candidateStateTransitionMatrix;
	std::vector<std::deque<double> > candidateStateTransitionTimeMatrix;

	std::vector<std::vector<std::deque<KPDUnderlyingStatus> > > donorStateTransitionMatrix;
	std::vector<std::vector<std::deque<double> > > donorStateTransitionTimeMatrix;

	// Random Number Generators
	RNG rngNDD;
	RNG rngSelection;
	RNG rngAdditionalDonor;
	RNG rngArrival;
	RNG rngUtility;
	RNG rngCrossmatch;
	RNG rngPairFailure;
	RNG rngMatchFailure;
	RNG rngDonorGenerator;
	RNG rngTransition;

	//Pool 
	void setMatchRunSchedule();
	void setRNGSeeds();
	
	void selectNodesFixed();
	void selectNodesStochastic();	
	
	void assignNodeProperties();
	void assignMatchProperties();

	//Helper Functions 
	KPDDonor * generateNDD();
	void generateDonors(std::vector<KPDDonor *> & donors, int numberOfDonors, KPDCandidate * candidate);

	double generateChangePoint(KPDUnderlyingStatus currentState, bool isDonor, bool isNDD);
	KPDUnderlyingStatus generateStateTransition(KPDUnderlyingStatus currentState, double changePoint, KPDTimeline timeline, double activeToInactive, double activeToWithdrawn, double inactiveToActive, double inactiveToWithdrawn);
	
	bool allowableMatch(KPDCrossmatchResult crossmatch);

	std::stringstream kpdRecordLog;

	std::string logFile;


public:

	KPDRecord(KPDParameters * params);
	~KPDRecord();

	void generateKPDPool(int iteration);

	//Cloning Functions
	std::deque<int> getMatchRunTimes();

	std::vector<KPDNode *> getNodes();
	std::vector<KPDNodeType> getNodeTypes();
	std::vector<std::vector<std::vector<KPDMatch *> > > getMatchMatrix();

	std::vector<std::vector<bool> > getIncidenceMatrix();
	
	std::vector<std::deque<KPDUnderlyingStatus> > getCandidateStateTransitionMatrix();
	std::vector<std::deque<double> > getCandidateStateTransitionTimeMatrix();
	
	std::vector<std::vector<std::deque<KPDUnderlyingStatus> > > getDonorStateTransitionMatrix();
	std::vector<std::vector<std::deque<double> > > getDonorStateTransitionTimeMatrix();
	
	std::string getPopulationList();

	void printLog();
	
};

KPDRecord::KPDRecord(KPDParameters * params){

	kpdParameters = params;

	kpdData = new KPDData(params);

	setMatchRunSchedule();

	logFile = "output/DDSim/" + kpdParameters->getOutputFolder() + "/" + kpdParameters->getSubFolder() + "/SimulationLogs/Log-Record.txt";	

}

KPDRecord::~KPDRecord(){

	delete kpdData;

	printLog();
}

void KPDRecord::setMatchRunSchedule() {
			
	kpdRecordLog << "Match Runs Occur at Times: ";
	
	int matchRunTime = kpdParameters->getInitMatchRun();
	int freqMatchRun = kpdParameters->getFreqMatchRun();
	int timeSpan = kpdParameters->getTimeSpan();

	matchRunSchedule.push_back(matchRunTime);
	kpdRecordLog << matchRunTime;
	matchRunTime += freqMatchRun;

	while (matchRunTime <= timeSpan) {
		matchRunSchedule.push_back(matchRunTime);
		kpdRecordLog << " " << matchRunTime;
		matchRunTime += freqMatchRun;
	}

	if (matchRunSchedule.back() != timeSpan) {
		matchRunSchedule.push_back(timeSpan);
		kpdRecordLog << " " << timeSpan;
	}

	kpdRecordLog << std::endl;

	kpdRecordLog << "Match Run Schedule Set!" << std::endl << std::endl;
}

void KPDRecord::setRNGSeeds() {

	kpdRecordLog << "Initializing Random Number Generator Seeds" << std::endl;
	std::cout << "Initializing Random Number Generator Seeds... " << std::endl;

	rngNDD.setSeed(kpdParameters->getRNGSeedNDD() * currentIteration);
	rngSelection.setSeed(kpdParameters->getRNGSeedSelection() * currentIteration);
	rngAdditionalDonor.setSeed(kpdParameters->getRNGSeedAdditionalDonor() * currentIteration);
	rngArrival.setSeed(kpdParameters->getRNGSeedArrival() * currentIteration);
	rngUtility.setSeed(kpdParameters->getRNGSeedUtility() * currentIteration);
	rngCrossmatch.setSeed(kpdParameters->getRNGSeedCrossmatch() * currentIteration);
	rngPairFailure.setSeed(kpdParameters->getRNGSeedPairFailure() * currentIteration);
	rngMatchFailure.setSeed(kpdParameters->getRNGSeedMatchFailure() * currentIteration);
	rngDonorGenerator.setSeed(kpdParameters->getRNGSeedDonorGenerator() * currentIteration);
	rngTransition.setSeed(kpdParameters->getRNGSeedTransition() * currentIteration);
}

void KPDRecord::selectNodesFixed(){

	kpdRecordLog << "Selecting Nodes:" << std::endl;
	std::cout << "Selecting Nodes..." << std::endl;
	
	KPDDonorAssignmentScheme donorAssignmentScheme = kpdParameters->getDonorAssignmentScheme();

	int nPairingArrivals = (int)(kpdParameters->getArrivals() + 0.5);
	int nNDDArrivals = (int)(kpdParameters->getNDDs() + 0.5);

	int nodeID = 1;

	int nDonorsTotal = 0;
	int nPairingsTotal = 0;
	int nNDDsTotal = 0;

	//Populate Node Vector
	nodeVector.push_back(new KPDNode());

	for (unsigned i = 1; i <= matchRunSchedule.size(); i++){

		//std::cout << i << std::endl;

		int nNDDs = 0;
		int nPairings = 0;

		double inTime = matchRunSchedule[i - 1];

		//NDDs
		while (nNDDs < nNDDArrivals){

			nNDDs++;
			
			//Generate donor based on donor assignment scheme
			if (donorAssignmentScheme == RANDOM_DONORS){
				KPDDonor * newNDD = generateNDD();
				KPDNode * newNode = new KPDNode(nodeID, inTime, newNDD);
				nodeVector.push_back(newNode);

				kpdRecordLog << "   " << nodeID << "/" << nNDDs << " - NDD (Random)" << std::endl;
			}
			else {
				
				double uDonor = rngSelection.runif();

				KPDDonor * newNDD = kpdData->drawNDD(uDonor);
				KPDNode * newNode = new KPDNode(nodeID, inTime, newNDD);
				nodeVector.push_back(newNode);

				kpdRecordLog << "   " << nodeID << "/" << nNDDs << " - NDD (" << newNDD->getUniqueID() << ")" << std::endl;
			}
			
			nDonorsTotal++;
			nodeID++;
		}

		//Pairs
		while (nPairings < nPairingArrivals){

			nPairings++;

			//Set associated donors based on donor assignment scheme
			std::vector<KPDDonor *> associatedDonors;
			associatedDonors.push_back(new KPDDonor());
			
			if (donorAssignmentScheme == RANDOM_DONORS) {

				double uCandidate = rngSelection.runif();

				KPDCandidate * candidate = kpdData->drawCandidate(uCandidate);
				
				double uAdditionalDonor = rngAdditionalDonor.runif();

				int numberOfDonors = 1;
				if (uAdditionalDonor < kpdParameters->getTwoDonorsProbability()) {
					numberOfDonors = 2;
				}
				else if (uAdditionalDonor < kpdParameters->getTwoDonorsProbability() + kpdParameters->getThreeDonorsProbability()) {
					numberOfDonors = 3;
				}
				generateDonors(associatedDonors, numberOfDonors, candidate);
								
				//Create new pairing node
				KPDNode * newNode = new KPDNode(nodeID, inTime, associatedDonors, candidate);

				nodeVector.push_back(newNode);

				//Print output
				kpdRecordLog << "   " << nodeID << "/" << nPairings << " - Pair " << newNode->getCandidate()->getMatchingID() << "(" << numberOfDonors << "D - Random)" << std::endl;
				
				nDonorsTotal += numberOfDonors;
			}					
			else {

				double uPair = rngSelection.runif();
				
				int index = kpdData->drawPair(uPair);

				KPDCandidate * candidate = kpdData->retrieveCandidate(index);
				std::vector<KPDDonor *> associatedDonors = kpdData->retrieveDonors(index);

				int numberOfDonors = (int)associatedDonors.size() - 1;

				//Create new pairing node
				KPDNode * newNode = new KPDNode(nodeID, inTime, associatedDonors, candidate);

				nodeVector.push_back(newNode);

				//Print output
				kpdRecordLog << "   " << nodeID << "/" << nPairings << " - Pair: " << newNode->getCandidate()->getMatchingID() << "(" << numberOfDonors << " D - File)" << std::endl;
				
				nDonorsTotal += numberOfDonors;
			}

			nodeID++;			
		}

		kpdRecordLog << "   At Match Run " << i << ", # of Pairings Added: " << nPairings << ", # of NDDs Added: " << nNDDs << std::endl << std::endl;
		nPairingsTotal += nPairings;
		nNDDsTotal += nNDDs;
	}

	kpdRecordLog << "   Total Pairings: " << nPairingsTotal << ", Total NDDs: " << nNDDsTotal << ", Total Donors: " << nDonorsTotal <<  std::endl;
}

void KPDRecord::selectNodesStochastic(){

	kpdRecordLog << "Selecting Nodes:" << std::endl;
	std::cout << "Selecting Nodes..." << std::endl;
	
	KPDDonorAssignmentScheme donorAssignmentScheme = kpdParameters->getDonorAssignmentScheme();

	double nodeArrivalLambda = kpdParameters->getArrivals();
	double probNDDArrival = kpdParameters->getNDDs();
	
	int nodeID = 1;

	int nDonorsTotal = 0;
	int nPairingsTotal = 0;
	int nNDDsTotal = 0;

	double inTime = rngArrival.rexp(kpdParameters->getArrivals());


	//Populate node vector
	nodeVector.push_back(new KPDNode());

	for (unsigned i = 1; i <= matchRunSchedule.size(); i++) {
		
		int nNDDs = 0;
		int nPairings = 0;

		while (inTime <= matchRunSchedule[i - 1]) {
						
			double uNDD = rngNDD.runif();

			//NDDs
			if (uNDD < probNDDArrival) {

				nNDDs++;

				//Generate donor based on donor assignment scheme
				if (donorAssignmentScheme == RANDOM_DONORS) {
					KPDDonor * newNDD = generateNDD();
					KPDNode * newNode = new KPDNode(nodeID, inTime, newNDD);
					nodeVector.push_back(newNode);

					kpdRecordLog << "   " << nodeID << "/" << nNDDs << " - NDD (Random)" << std::endl;

				}
				else {

					double uDonor = rngSelection.runif();
					KPDDonor * newNDD = kpdData->drawNDD(uDonor);
					KPDNode * newNode = new KPDNode(nodeID, inTime, newNDD);
					nodeVector.push_back(newNode);

					kpdRecordLog << "   " << nodeID << "/" << nNDDs << " - NDD (" << newNDD->getUniqueID() << ")" << std::endl;
				}
				
				//inTime += rngArrival.rexp(nodeArrivalLambda);
					
				nDonorsTotal++;
				nodeID++;				
			}

			else {

				nPairings++;

				//Set associated donors based on donor assignment scheme
				std::vector<KPDDonor *>  associatedDonors;
				associatedDonors.push_back(new KPDDonor());

				if (donorAssignmentScheme == RANDOM_DONORS) {

					double uCandidate = rngSelection.runif();

					KPDCandidate * candidate = kpdData->drawCandidate(uCandidate);
					
					double uAdditionalDonor = rngAdditionalDonor.runif();

					int numberOfDonors = 1;
					if (uAdditionalDonor < kpdParameters->getTwoDonorsProbability()) {
						numberOfDonors = 2;
					}
					else if (uAdditionalDonor < kpdParameters->getTwoDonorsProbability() + kpdParameters->getThreeDonorsProbability()) {
						numberOfDonors = 3;
					}

					generateDonors(associatedDonors, numberOfDonors, candidate);

					//Create new pairing node
					KPDNode * newNode = new KPDNode(nodeID, inTime, associatedDonors, candidate);

					nodeVector.push_back(newNode);

					//Print output
					kpdRecordLog << "   " << nodeID << "/" << nPairings << " - Pair: " << newNode->getCandidate()->getMatchingID() << "(" << numberOfDonors << " D - Random)" << std::endl;

					nDonorsTotal += numberOfDonors;
				}
				else {
					double uPair = rngSelection.runif();

					int index = kpdData->drawPair(uPair);

					KPDCandidate * candidate = kpdData->retrieveCandidate(index);
					std::vector<KPDDonor *> associatedDonors = kpdData->retrieveDonors(index);

					int numberOfDonors = (int)associatedDonors.size() - 1;
										
					//Create new pairing node
					KPDNode * newNode = new KPDNode(nodeID, inTime, associatedDonors, candidate);

					nodeVector.push_back(newNode);

					//Print output
					kpdRecordLog << "   " << nodeID << "/" << nPairings << " - Pair: " << newNode->getCandidate()->getMatchingID() << "(" << numberOfDonors << " D - File)" << std::endl;

					nDonorsTotal += numberOfDonors;
				}

				//inTime += rngArrival.rexp(nodeArrivalLambda);					
				
				nodeID++;				
			}

			double u = rngArrival.rexp(nodeArrivalLambda);

			inTime += u;			
		}
		
		kpdRecordLog << "   At Match Run at Time " << matchRunSchedule[i - 1] << ", # of Pairings Added: " << nPairings << ", # of NDDs Added: " << nNDDs << std::endl;
		nPairingsTotal += nPairings;
		nNDDsTotal += nNDDs;
	}

	kpdRecordLog << "   Total Pairings: " << nPairingsTotal <<  ", Total NDDs: " << nNDDsTotal << ", Total Donors: " << nDonorsTotal << std::endl;
}

void KPDRecord::assignNodeProperties() {

	kpdRecordLog << "Generating State Transitions:" << std::endl;
	std::cout << "Generating State Transitions..." << std::endl;

	KPDTimeline timeline = kpdParameters->getTimeline();

	int timeSpan = kpdParameters->getTimeSpan();
	int processingTime = kpdParameters->getProcessingTime();

	int N = (int)nodeVector.size() - 1;

	candidateStateTransitionMatrix.assign(1, std::deque<KPDUnderlyingStatus>(1, INACTIVE));
	candidateStateTransitionTimeMatrix.assign(1, std::deque<double>(1, 0.0));

	donorStateTransitionMatrix.assign(1, std::vector<std::deque<KPDUnderlyingStatus> >(1, std::deque<KPDUnderlyingStatus>(1, INACTIVE)));
	donorStateTransitionTimeMatrix.assign(1, std::vector<std::deque<double> >(1, std::deque<double>(1, INACTIVE)));

	double actualNDDActiveToInactive = kpdParameters->getActualNDDActiveToInactive();
	double actualNDDActiveToWithdrawn = kpdParameters->getActualNDDActiveToWithdrawn();
	double actualNDDInactiveToActive = kpdParameters->getActualNDDInactiveToActive();
	double actualNDDInactiveToWithdrawn = kpdParameters->getActualNDDInactiveToWithdrawn();

	double actualDonorActiveToInactive = kpdParameters->getActualDonorActiveToInactive();
	double actualDonorActiveToWithdrawn = kpdParameters->getActualDonorActiveToWithdrawn();
	double actualDonorInactiveToActive = kpdParameters->getActualDonorInactiveToActive();
	double actualDonorInactiveToWithdrawn = kpdParameters->getActualDonorInactiveToWithdrawn();

	double actualCandidateActiveToInactive = kpdParameters->getActualCandidateActiveToInactive();
	double actualCandidateActiveToWithdrawn = kpdParameters->getActualCandidateActiveToWithdrawn();
	double actualCandidateInactiveToActive = kpdParameters->getActualCandidateInactiveToActive();
	double actualCandidateInactiveToWithdrawn = kpdParameters->getActualCandidateInactiveToWithdrawn();

	//Iterate through nodes
	for (int i = 1; i <= N; i++) {

		//kpdRecordLog << "   Node " << i << std::endl;

		int numberOfDonors = nodeVector[i]->getNumberOfAssociatedDonors();

		std::deque<KPDUnderlyingStatus> candidateStateTransitions;
		std::deque<double> candidateStateTransitionTimes;
		std::vector<std::deque<KPDUnderlyingStatus> > donorStateTransitions;
		std::vector<std::deque<double> > donorStateTransitionTimes;

		double startingPoint = nodeVector[i]->getArrivalTime();
		KPDUnderlyingStatus initialState = ACTIVE;

		candidateStateTransitions.push_back(initialState);
		candidateStateTransitionTimes.push_back(startingPoint);
		donorStateTransitions.assign(1 + numberOfDonors, std::deque<KPDUnderlyingStatus>(1, initialState));
		donorStateTransitionTimes.assign(1 + numberOfDonors, std::deque<double>(1, startingPoint));

		//State transitions for pairings
		if (nodeVector[i]->getType() == PAIR) {

			kpdRecordLog << "     " << i << "-Candidate: " << startingPoint << " (" << KPDFunctions::toString(initialState) << ") ";

			double changePoint = startingPoint;			

			if (timeline == FIXED_MATCH_RUNS) {
				changePoint = changePoint - 0.5;
			}
			KPDUnderlyingStatus currentState = initialState;

			changePoint += generateChangePoint(currentState, false, false);

			//Candidate state transitions
			while (changePoint <= (timeSpan + processingTime) && currentState != WITHDRAWN) {
				KPDUnderlyingStatus newState;

				newState = generateStateTransition(currentState, changePoint, timeline,
					actualCandidateActiveToInactive, actualCandidateActiveToWithdrawn, actualCandidateInactiveToActive, actualCandidateInactiveToWithdrawn);

				if (newState != currentState) {
					candidateStateTransitions.push_back(newState);
					candidateStateTransitionTimes.push_back(changePoint);

					kpdRecordLog << changePoint << " (" << KPDFunctions::toString(newState) << ") ";
				}

				currentState = newState;

				changePoint += generateChangePoint(currentState, false, false);
			}

			kpdRecordLog << std::endl;

			//Donor state transitions
			for (int k = 1; k <= numberOfDonors; k++) {

				kpdRecordLog << "     " << i << "-Donor " << k << ": " << startingPoint << " (" << KPDFunctions::toString(initialState) << ") ";
				
				changePoint = startingPoint;
				
				if (timeline == FIXED_MATCH_RUNS) {
					changePoint = changePoint - 0.5;
				}
				currentState = initialState;

				changePoint += generateChangePoint(currentState, true, false);

				while (changePoint <= (timeSpan + processingTime) && currentState != WITHDRAWN) {
					KPDUnderlyingStatus newState;

					newState = generateStateTransition(currentState, changePoint, timeline,
						actualDonorActiveToInactive, actualDonorActiveToWithdrawn, actualDonorInactiveToActive, actualDonorInactiveToWithdrawn);

					if (newState != currentState) {
						donorStateTransitions[k].push_back(newState);
						donorStateTransitionTimes[k].push_back(changePoint);

						kpdRecordLog << changePoint << " (" << KPDFunctions::toString(newState) << ") ";
					}

					currentState = newState;

					changePoint += generateChangePoint(currentState, true, false);
				}
			}

			kpdRecordLog << std::endl;
		}

		else {

			kpdRecordLog << "     " << i << "-NDD: " << startingPoint << " (" << KPDFunctions::toString(initialState) << ") ";

			double changePoint = startingPoint;
			if (timeline == FIXED_MATCH_RUNS) {
				changePoint = changePoint - 0.5;
			}
			KPDUnderlyingStatus currentState = initialState;

			changePoint += generateChangePoint(currentState, true, true);

			while (changePoint <= (timeSpan + processingTime) && currentState != WITHDRAWN) {
				KPDUnderlyingStatus newState;

				newState = generateStateTransition(currentState, changePoint, timeline,
					actualNDDActiveToInactive, actualNDDActiveToWithdrawn, actualNDDInactiveToActive, actualNDDInactiveToWithdrawn);

				if (newState != currentState) {
					donorStateTransitions[1].push_back(newState);
					donorStateTransitionTimes[1].push_back(changePoint);

					kpdRecordLog << changePoint << " (" << newState << ") ";
				}

				currentState = newState;

				changePoint += generateChangePoint(currentState, true, true);
			}

			kpdRecordLog << std::endl;
		}

		candidateStateTransitionMatrix.push_back(candidateStateTransitions);
		candidateStateTransitionTimeMatrix.push_back(candidateStateTransitionTimes);
		donorStateTransitionMatrix.push_back(donorStateTransitions);
		donorStateTransitionTimeMatrix.push_back(donorStateTransitionTimes);
	}
}

void KPDRecord::assignMatchProperties() {

	kpdRecordLog << "Generating Match Properties:" << std::endl;
	std::cout << "Generating Match Properties..." << std::endl;

	int N = (int)nodeVector.size() - 1;

	incidenceMatrix.assign(1 + N, std::vector<bool>(1 + N, false));
	matchMatrix.assign(1 + N, std::vector<std::vector<KPDMatch *> >(1 + N, std::vector<KPDMatch *>(1, new KPDMatch())));

	//Iterate through donor nodes
	for (int i = 1; i <= N; i++) {

		//Get donor nodes
		std::vector<KPDDonor *> donors = nodeVector[i]->getDonors();
		int numDonors = nodeVector[i]->getNumberOfAssociatedDonors();

		//Iterate through candidate nodes
		for (int j = 1; j <= N; j++) {

			//Initialize matrices
			matchMatrix[i][j].assign(1 + numDonors, new KPDMatch());

			if (i != j) {
				// Pairing -> NDD (Implicit Backward Edge from all Donors to the NDD)
				if (nodeVector[i]->getType() == PAIR && nodeVector[j]->getType() == NDD) {
					incidenceMatrix[i][j] = true;

					for (int k = 1; k <= numDonors; k++) { // These are all probably unnecessary, but are good for placeholding
						matchMatrix[i][j][k]->setIncidence(true);
						matchMatrix[i][j][k]->setAssumedSuccessProbability(1.0);
						matchMatrix[i][j][k]->setActualSuccessProbability(1.0);
						matchMatrix[i][j][k]->setLabCrossmatchResult(true);

						kpdRecordLog << "   " << i << "[" << k << "] -> " << j << " (Implicit)" << std::endl;
					}
				}

				// Pairing
				else if (nodeVector[j]->getType() == PAIR) {
					double pra = nodeVector[j]->getCandidatePRA();

					//Iterate through associated donors
					for (int k = 1; k <= numDonors; k++) {

						//Perform virtual crossmatch
						KPDCrossmatchResult virtualCrossmatchResult = kpdData->performCrossmatch(nodeVector[j]->getCandidate(), donors[k]);

						if (allowableMatch(virtualCrossmatchResult)) {

							incidenceMatrix[i][j] = true;

							//Assign utility values
							double fiveYearSurvival;
							double tenYearSurvival;
							double transplantDifficultyScore;
							double assignedUtility;

							fiveYearSurvival = kpdData->calculateSurvival(nodeVector[j]->getCandidate(), donors[k], 1);
							tenYearSurvival = kpdData->calculateSurvival(nodeVector[j]->getCandidate(), donors[k], 0);

							if (pra >= 97 || nodeVector[i]->getDonorBT(k) == BT_AB) {
								transplantDifficultyScore = 1.0;
							}
							else {
								transplantDifficultyScore = 0.0001;
							}

							assignedUtility = rngUtility.runif(kpdParameters->getMatchUtilityLowerBound(), kpdParameters->getMatchUtilityUpperBound());

							//Generate probability values

							double assumedMatchSuccessProbability = 1.0;
							double actualMatchSuccessProbability = 1.0;

							KPDMatchFailureScheme assumedMatchFailureScheme = kpdParameters->getAssumedMatchFailureScheme();
							KPDMatchFailureScheme actualMatchFailureScheme = kpdParameters->getActualMatchFailureScheme();

							if (assumedMatchFailureScheme == PRA_BASED_MATCH_FAILURE) {
								if (pra < 25) {
									assumedMatchSuccessProbability = 0.95;
								}
								else if (pra >= 25 && pra < 50) {
									assumedMatchSuccessProbability = 0.8;
								}
								else if (pra >= 50 && pra < 75) {
									assumedMatchSuccessProbability = 0.65;
								}
								else {
									assumedMatchSuccessProbability = 0.5;
								}
							}
							else {
								assumedMatchSuccessProbability = 1 - rngMatchFailure.runif(kpdParameters->getAssumedMatchFailureLowerBound(), kpdParameters->getAssumedMatchFailureUpperBound());
							}

							assumedMatchSuccessProbability = KPDFunctions::truncateValue(assumedMatchSuccessProbability + kpdParameters->getAssumedMatchFailureAdjustment(), 0, 1);

							if (actualMatchFailureScheme == PRA_BASED_MATCH_FAILURE) {
								if (pra < 25) {
									actualMatchSuccessProbability = 0.95;
								}
								else if (pra >= 25 && pra < 50) {
									actualMatchSuccessProbability = 0.8;
								}
								else if (pra >= 50 && pra < 75) {
									actualMatchSuccessProbability = 0.65;
								}
								else {
									actualMatchSuccessProbability = 0.5;
								}
							}
							else {
								actualMatchSuccessProbability = 1 - rngMatchFailure.runif(kpdParameters->getActualMatchFailureLowerBound(), kpdParameters->getActualMatchFailureUpperBound());
							}

							actualMatchSuccessProbability = KPDFunctions::truncateValue(actualMatchSuccessProbability + kpdParameters->getActualMatchFailureAdjustment(), 0, 1);

							//Generate hidden crossmatch result
							bool labCrossmatchResult = rngCrossmatch.runif() < actualMatchSuccessProbability;

							matchMatrix[i][j][k]->setMatchProperties(true, fiveYearSurvival, tenYearSurvival, transplantDifficultyScore, assignedUtility, assumedMatchSuccessProbability, actualMatchSuccessProbability, virtualCrossmatchResult, labCrossmatchResult);
							
							kpdRecordLog << "   " << i << "[" << k << "] -> " << j << " " << matchMatrix[i][j][k]->matchShortOutput() << std::endl;
						}
					}
				}
			}
		}
	}
}

KPDDonor * KPDRecord::generateNDD() {

	////Simulated Donor Information	

	double uRelation = rngDonorGenerator.runif();
	double uBT = rngDonorGenerator.runif();
	double uHLA1 = rngDonorGenerator.runif();
	double uHLA2 = rngDonorGenerator.runif();
	double uBW4 = rngDonorGenerator.runif();
	double uBW6 = rngDonorGenerator.runif();

	double uAge = rngDonorGenerator.runif(20, 70);
	double uGender = rngDonorGenerator.runif();
	double uRace = rngDonorGenerator.runif();
	double uHeight = rngDonorGenerator.runif(1.2, 2);
	double uWeight = rngDonorGenerator.runif(40, 100);
	double uCigaretteUse = rngDonorGenerator.runif();

	KPDRelationToCandidate dRelation = UNRELATED;

	//Donor Crossmatch Information

	KPDBloodType dBT = kpdData->drawBloodType(uBT);

	std::vector<std::string> dHLA = kpdData->drawHLA(uHLA1);
	std::vector<std::string> additionalHLA = kpdData->drawHLA(uHLA2);

	for (std::vector<std::string>::iterator hla = additionalHLA.begin(); hla != additionalHLA.end(); hla++) {
		dHLA.push_back(*hla);
	}

	bool bw4 = kpdData->drawHLABW(uBW4, true);
	bool bw6 = kpdData->drawHLABW(uBW6, false);

	if (bw4) {
		dHLA.push_back("BW4");
	}
	if (bw6) {
		dHLA.push_back("BW6");
	}

	//Donor Characteristics
	int dAge = (int)(uAge + 0.5);
	bool dMale = uGender <= 0.5;
	KPDRace dRace = kpdData->drawRace(uRace);

	double dHeight = uHeight;
	double dWeight = uWeight;
	bool dCigaretteUse = kpdData->drawCigaretteUse(uCigaretteUse);

	//Generate Donor
	KPDDonor * donor = new KPDDonor(dBT, dHLA, dRelation, dAge, dMale, dRace, dHeight, dWeight, dCigaretteUse);

	return donor;

}

void KPDRecord::generateDonors(std::vector<KPDDonor *> & donors, int numberOfDonors, KPDCandidate * candidate) {

	int generatedDonors = 0;
	
	while (generatedDonors < numberOfDonors) {
		
		////Simulated Donor Information	
		double uRelation = rngDonorGenerator.runif();
		double uBT = rngDonorGenerator.runif();
		double uHLA1 = rngDonorGenerator.runif();
		double uHLA2 = rngDonorGenerator.runif();
		double uBW4 = rngDonorGenerator.runif();
		double uBW6 = rngDonorGenerator.runif();

		double uAge = rngDonorGenerator.runif(20, 70);
		double uGender = rngDonorGenerator.runif();
		double uRace = rngDonorGenerator.runif();
		double uHeight = rngDonorGenerator.runif(1.2, 2);
		double uWeight = rngDonorGenerator.runif(40, 100);
		double uCigaretteUse = rngDonorGenerator.runif();

		//Donor Relation Information
		KPDRelationToCandidate dRelation = kpdData->drawRelation(uRelation, candidate->getAge());

		//Donor Crossmatch Information
		KPDBloodType dBT = kpdData->drawBloodType(uBT);

		std::vector<std::string> dHLA = kpdData->drawHLA(uHLA1);
		std::vector<std::string> additionalHLA = kpdData->drawHLA(uHLA2);

		for (std::vector<std::string>::iterator hla = additionalHLA.begin(); hla != additionalHLA.end(); hla++) {
			dHLA.push_back(*hla);
		}

		bool bw4 = kpdData->drawHLABW(uBW4, true);
		bool bw6 = kpdData->drawHLABW(uBW6, false);

		if (bw4) {
			dHLA.push_back("BW4");
		}
		if (bw6) {
			dHLA.push_back("BW6");
		}

		//Donor Characteristics
		int dAge = (int)(uAge + 0.5);
		bool dMale = uGender <= 0.5;
		KPDRace dRace = kpdData->drawRace(uRace);

		double dHeight = uHeight;
		double dWeight = uWeight;
		bool dCigaretteUse = kpdData->drawCigaretteUse(uCigaretteUse);

		//Generate Donor
		KPDDonor * donor = new KPDDonor(dBT, dHLA, dRelation, dAge, dMale, dRace, dHeight, dWeight, dCigaretteUse);

		KPDCrossmatchResult crossmatch = kpdData->performCrossmatch(candidate, donor);

		if (!allowableMatch(crossmatch)) {
			donors.push_back(donor);
			generatedDonors++;
		}
	}
}


//Generates time until next transition time
double KPDRecord::generateChangePoint(KPDUnderlyingStatus currentState, bool isDonor, bool isNDD) {

	KPDTimeline timeline = kpdParameters->getTimeline();
	int freqMatchRun = kpdParameters->getFreqMatchRun();

	double changePoint = 0;

	if (timeline == FIXED_MATCH_RUNS) {
		changePoint = freqMatchRun;
	}
	else if (timeline == CONTINUOUS_TIMELINE) {
		if (currentState == ACTIVE) {
			if (isDonor) {
				if (isNDD) {
					changePoint = rngTransition.rexp(kpdParameters->getActualNDDActiveToInactive() + kpdParameters->getActualNDDActiveToWithdrawn());
				}
				else {
					changePoint = rngTransition.rexp(kpdParameters->getActualDonorActiveToInactive() + kpdParameters->getActualDonorActiveToWithdrawn());
				}
			}
			else {
				changePoint = rngTransition.rexp(kpdParameters->getActualCandidateActiveToInactive() + kpdParameters->getActualCandidateActiveToWithdrawn());
			}
		}
		else if (currentState == INACTIVE) {
			if (isDonor) {
				if (isNDD) {
					changePoint = rngTransition.rexp(kpdParameters->getActualNDDInactiveToActive() + kpdParameters->getActualNDDInactiveToWithdrawn());
				}
				else {
					changePoint = rngTransition.rexp(kpdParameters->getActualDonorInactiveToActive() + kpdParameters->getActualDonorInactiveToWithdrawn());
				}
			}
			else {
				changePoint = rngTransition.rexp(kpdParameters->getActualCandidateInactiveToActive() + kpdParameters->getActualCandidateInactiveToWithdrawn());
			}
		}
	}

	return changePoint;
}

//Generates the next state transition
KPDUnderlyingStatus KPDRecord::generateStateTransition(KPDUnderlyingStatus currentState, double changePoint, KPDTimeline timeline, double activeToInactive, double activeToWithdrawn, double inactiveToActive, double inactiveToWithdrawn) {

	KPDUnderlyingStatus newState = currentState;

	double u = rngTransition.runif();

	if (timeline == FIXED_MATCH_RUNS) {
		if (currentState == ACTIVE) {
			if (u < activeToInactive) {
				newState = INACTIVE;
			}
			else if (u < activeToInactive + activeToWithdrawn) {
				newState = WITHDRAWN;
			}
			else {
				newState = ACTIVE;
			}
		}
		else if (currentState == INACTIVE) {
			if (u < inactiveToActive) {
				newState = ACTIVE;
			}
			else if (u < inactiveToActive + inactiveToWithdrawn) {
				newState = WITHDRAWN;
			}
			else {
				newState = INACTIVE;
			}
		}
		else {
			newState = WITHDRAWN;
		}
	}

	else {
		if (currentState == ACTIVE) {
			if (u < activeToInactive / (activeToInactive + activeToWithdrawn)) {
				newState = INACTIVE;
			}
			else {
				newState = WITHDRAWN;
			}
		}
		else if (currentState == INACTIVE) {
			if (u < inactiveToActive / (inactiveToActive + inactiveToWithdrawn)) {
				newState = ACTIVE;
			}
			else {
				newState = WITHDRAWN;
			}
		}
		else {
			newState = WITHDRAWN;
		}
	}

	return newState;
}

bool KPDRecord::allowableMatch(KPDCrossmatchResult crossmatch) {

	if (crossmatch == SUCCESSFUL_CROSSMATCH){
		return true;
	}

	if (crossmatch == REQUIRES_DESENSITIZATION_AND_O_TO_NON_O){
		if (kpdParameters->allowDesensitization() && !kpdParameters->reserveODonorsForOCandidates()) {
			return true;
		}
	}
	
	if (crossmatch == REQUIRES_DESENSITIZATION){
		if (kpdParameters->allowDesensitization()) {
			return true;
		}
	}

	if (crossmatch == O_DONOR_TO_NON_O_CANDIDATE) {
		if (!kpdParameters->reserveODonorsForOCandidates()) {
			return true;
		}
	}

	return false;
}

void KPDRecord::generateKPDPool(int iteration){

	currentIteration = iteration;
	
	kpdRecordLog << "--------------" << std::endl;
	kpdRecordLog << "Iteration " << currentIteration << std::endl;
	kpdRecordLog << "--------------" << std::endl << std::endl;

	//Clear existing variables
	kpdRecordLog << "Resetting Pool" << std::endl;
	std::cout << "Resetting Pool... " << std::endl;

	nodeVector.clear();
	matchMatrix.clear();
	incidenceMatrix.clear();

	candidateStateTransitionMatrix.clear();
	candidateStateTransitionTimeMatrix.clear();

	donorStateTransitionMatrix.clear();
	donorStateTransitionTimeMatrix.clear();

	//Initialize random number seeds
	setRNGSeeds();

	//Select nodes	
	KPDTimeline timeline = kpdParameters->getTimeline();
	if (timeline == FIXED_MATCH_RUNS){
		selectNodesFixed();
	}
	else if (timeline == CONTINUOUS_TIMELINE){
		selectNodesStochastic();
	}

	//Assign node properties
	assignNodeProperties();

	//Assign match properites
	assignMatchProperties();

	kpdRecordLog << "Pool for Iteration " << currentIteration << " Generated" << std::endl << std::endl;
	std::cout << "Pool for Iteration " << currentIteration << " Generated" << std::endl << std::endl;
}


//Cloning Functions
std::deque<int> KPDRecord::getMatchRunTimes() {

	std::deque<int> matchRunTimes; // Starts at 0, Convert to deque

	for (std::vector <int>::iterator it = matchRunSchedule.begin(); it != matchRunSchedule.end(); ++it) {
		matchRunTimes.push_back(*it);
	}

	return matchRunTimes;
}

std::vector<KPDNode *> KPDRecord::getNodes() {

	std::vector<KPDNode *> nodes; // Starts at 1

	nodes.push_back(new KPDNode());

	for (std::vector<KPDNode*>::iterator it = nodeVector.begin() + 1; it != nodeVector.end(); it++) {
		nodes.push_back((*it)->copy()); // Deep Copy
	}

	return nodes;
}

std::vector<KPDNodeType> KPDRecord::getNodeTypes() {

	std::vector<KPDNodeType> nodeTypes; // Starts at 1

	nodeTypes.push_back(PAIR);

	for (std::vector<KPDNode*>::iterator it = nodeVector.begin() + 1; it != nodeVector.end(); it++) {
		nodeTypes.push_back((*it)->getType());
	}

	return nodeTypes;
}

std::vector<std::vector<std::vector<KPDMatch *> > > KPDRecord::getMatchMatrix() {

	std::vector<std::vector<std::vector<KPDMatch *> > > matchMatrixCopy; // Starts at 1

	for (std::vector<std::vector<std::vector<KPDMatch *> > >::iterator itRow = matchMatrix.begin(); itRow != matchMatrix.end(); itRow++) {

		std::vector<std::vector<KPDMatch *> > matchMatrixRowCopy;

		for (std::vector<std::vector<KPDMatch *> >::iterator itColumn = (*itRow).begin(); itColumn != (*itRow).end(); itColumn++) {

			std::vector<KPDMatch *> donorMatchesCopy;

			for (std::vector<KPDMatch *>::iterator itDonors = (*itColumn).begin(); itDonors != (*itColumn).end(); itDonors++) {
				donorMatchesCopy.push_back((*itDonors)->copy());
			}

			matchMatrixRowCopy.push_back(donorMatchesCopy);
		}

		matchMatrixCopy.push_back(matchMatrixRowCopy);
	}

	return matchMatrixCopy;
}

std::vector<std::vector<bool> > KPDRecord::getIncidenceMatrix() {

	std::vector<std::vector<bool> > incidenceMatrixClone(incidenceMatrix);

	return incidenceMatrixClone;
}

std::vector<std::deque<KPDUnderlyingStatus> > KPDRecord::getCandidateStateTransitionMatrix() {

	std::vector<std::deque<KPDUnderlyingStatus> > candidateStateTransitionMatrixClone(candidateStateTransitionMatrix);

	return candidateStateTransitionMatrixClone;
}

std::vector<std::deque<double> > KPDRecord::getCandidateStateTransitionTimeMatrix() {

	std::vector<std::deque<double> > candidateStateTransitionTimeMatrixClone(candidateStateTransitionTimeMatrix);

	return candidateStateTransitionTimeMatrixClone;
}

std::vector<std::vector<std::deque<KPDUnderlyingStatus> > > KPDRecord::getDonorStateTransitionMatrix() {

	std::vector<std::vector<std::deque<KPDUnderlyingStatus> > > donorStateTransitionMatrixClone(donorStateTransitionMatrix);

	return donorStateTransitionMatrixClone;
}

std::vector<std::vector<std::deque<double> > > KPDRecord::getDonorStateTransitionTimeMatrix() {

	std::vector<std::vector<std::deque<double> > > donorStateTransitionTimeMatrixClone(donorStateTransitionTimeMatrix);

	return donorStateTransitionTimeMatrixClone;
}

std::string KPDRecord::getPopulationList(){

	std::stringstream population;

	double nddAssumedProbability;
	double nddActualProbability;
	double donorAssumedProbability;
	double donorActualProbability;
	double candidateAssumedProbability;
	double candidateActualProbability;

	if (kpdParameters->getTimeline() == FIXED_MATCH_RUNS) {

		nddAssumedProbability = 1 - kpdParameters->getAssumedNDDActiveToInactive() + kpdParameters->getAssumedNDDActiveToWithdrawn();
		nddActualProbability = 1 - kpdParameters->getActualNDDActiveToInactive() + kpdParameters->getActualNDDActiveToWithdrawn();

		donorAssumedProbability = 1 - kpdParameters->getAssumedDonorActiveToInactive() + kpdParameters->getAssumedDonorActiveToWithdrawn();
		donorActualProbability = 1 - kpdParameters->getActualDonorActiveToInactive() + kpdParameters->getActualDonorActiveToWithdrawn();

		candidateAssumedProbability = 1 - kpdParameters->getAssumedCandidateActiveToInactive() + kpdParameters->getAssumedCandidateActiveToWithdrawn();
		candidateActualProbability = 1 - kpdParameters->getActualCandidateActiveToInactive() + kpdParameters->getActualCandidateActiveToWithdrawn();
	}
	else {

		nddAssumedProbability = exp((-1)*(kpdParameters->getAssumedNDDActiveToInactive() + kpdParameters->getAssumedNDDActiveToWithdrawn())*kpdParameters->getProcessingTime());
		nddActualProbability = exp((-1)*(kpdParameters->getActualNDDActiveToInactive() + kpdParameters->getActualNDDActiveToWithdrawn())*kpdParameters->getProcessingTime());

		donorAssumedProbability = exp((-1)*(kpdParameters->getAssumedDonorActiveToInactive() + kpdParameters->getAssumedDonorActiveToWithdrawn())*kpdParameters->getProcessingTime());
		donorActualProbability = exp((-1)*(kpdParameters->getActualDonorActiveToInactive() + kpdParameters->getActualDonorActiveToWithdrawn())*kpdParameters->getProcessingTime());

		candidateAssumedProbability = exp((-1)*(kpdParameters->getAssumedCandidateActiveToInactive() + kpdParameters->getAssumedCandidateActiveToWithdrawn())*kpdParameters->getProcessingTime());
		candidateActualProbability = exp((-1)*(kpdParameters->getActualCandidateActiveToInactive() + kpdParameters->getActualCandidateActiveToWithdrawn())*kpdParameters->getProcessingTime());
	}

	//Iterates through each pair involved in the simulation and collects demographic information
	for (std::vector<KPDNode *>::iterator itNode = nodeVector.begin(); itNode != nodeVector.end(); itNode++){

		KPDNodeType type = (*itNode)->getType();
		int numberOfDonors = (*itNode)->getNumberOfAssociatedDonors();

		for (int k = 1; k <= numberOfDonors; k++){
			population << currentIteration << ",";
			population << (*itNode)->getID() << ",";
			population << k << ",";
			population << KPDFunctions::toString(type) << ",";
			population << (*itNode)->getArrivalTime() << ",";
			population << (*itNode)->getCandidateString() << ",";
			if (type == PAIR) {
				population << candidateAssumedProbability << "," << candidateActualProbability << ",";
			}
			else {
				population << ",,";
			}
			population << (*itNode)->getDonorString(k) << ",";
			if (type == PAIR) {
				population << donorAssumedProbability << "," << donorActualProbability;
			}
			else {
				population << nddAssumedProbability << "," << nddActualProbability;
			}
			population << std::endl;
		}
	}

	return population.str();
}

void KPDRecord::printLog(){

	std::ofstream outputFileLog(logFile.c_str());
	outputFileLog << kpdRecordLog.str();
	outputFileLog.close();
}


#endif