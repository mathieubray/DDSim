/* ---------------------------------------------
DD-Record.h
Stores random portion of specific simulation iteration
Ensures replicability and integrity of results
Elements are copied to simulation at beginning of each iteration
---------------------------------------------- */

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
	std::vector<KPDNode *> kpdNodes;	
	std::vector<std::deque<KPDStatus> > kpdNodeStateTransitionMatrix;
	std::vector<std::deque<int> > kpdNodeStateTransitionTimeMatrix;
	
	std::map<int, std::map<int, std::vector<KPDMatch *> > > kpdMatches;

	std::vector<std::vector<bool> > kpdAdjacencyMatrix;
	std::vector<std::vector<bool> > kpdAdjacencyMatrixReduced;

	// Random Number Generators
	RNG rngSelection;
	RNG rngArrival;
	RNG rngMatch;
	RNG rngDonor;
	RNG rngStatus;

	// Helper Functions 
	void assembleKPD(std::vector<int> matchRunTimes);		
	void assignStateTransitions();
	void assignMatchProperties();
	
	std::stringstream kpdRecordLog;


public:

	KPDRecord(KPDData * data, KPDParameters * params);
	~KPDRecord();
	
	KPDDonor * generateDonor();
	void generateDonors(std::vector<KPDDonor *> & donors, int numberOfDonors, KPDCandidate * candidate);
	KPDMatch * generateMatch(KPDCandidate * candidate, KPDDonor * donor, KPDCrossmatch virtualCrossmatchResult);

	void generateSimulationData(int iteration, std::vector<int> matchRunTimes);

	// Cloning Functions
	std::vector<KPDNode *> getNodes();
	std::vector<KPDNodeType> getNodeTypes();
	std::vector<std::deque<KPDStatus> > getKPDNodeStateTransitionMatrix();
	std::vector<std::deque<int> > getKPDNodeStateTransitionTimeMatrix();

	std::map<int, std::map<int, std::vector<KPDMatch *> > > getKPDMatches();

	std::vector<std::vector<bool> > getAdjacencyMatrix();
	std::vector<std::vector<bool> > getAdjacencyMatrixReduced();
	
	std::string getPopulationList();

	void printLog();
	
};

KPDRecord::KPDRecord(KPDData * data, KPDParameters * params){

	kpdData = data;
	kpdParameters = params;
}

KPDRecord::~KPDRecord(){

	delete kpdData;

	printLog();
}


void KPDRecord::assembleKPD(std::vector<int> matchRunTimes){

	kpdRecordLog << "Selecting Nodes:" << std::endl;
	std::cout << "Selecting Nodes..." << std::endl;
	
	double pairArrivals = kpdParameters->getPairArrivals();
	double nddArrivals = kpdParameters->getNDDArrivals();
	
	int nDonorsTotal = 0;
	int nPairsTotal = 0;
	int nNDDsTotal = 0;

	// Generate NDDs	
	double nddTimeTracker = rngArrival.rexp(nddArrivals); // Poisson Process for NDD arrival
	double pairTimeTracker = rngArrival.rexp(pairArrivals); // Poisson Process for pair arrival

	int time = 1;
	int nodeID = 1;

	for (std::vector<int>::iterator it = matchRunTimes.begin(); it != matchRunTimes.end(); it++){

		int matchRunTime = *it;

		int nNDDs = 0;
		int nPairs = 0;

		while (time <= matchRunTime) {

			while (nddTimeTracker <= time) { // Generate NDDs for given match run

				KPDDonor * newNDD = generateDonor(); // Randomly generate NDD

				KPDNode * newNDDNode = new KPDNode(nodeID, time, newNDD);
				kpdNodes.push_back(newNDDNode);
				nodeID++;

				kpdRecordLog << "At Time " << time << ", NDD " << nodeID << "Joins" << std::endl;
				kpdRecordLog << newNDD->donorOutput() << std::endl;
				kpdRecordLog << std::endl;

				nddTimeTracker += rngArrival.rexp(nddArrivals); // Get next NDD arrival time
				nNDDs++;
			}			

			while (pairTimeTracker <= matchRunTime) { // Generate pairs for given match run				

				std::pair<KPDCandidate *, int> candidateInfo = kpdData->drawCandidate(rngSelection.runif()); // Draw candidate with replacement
				
				KPDCandidate * candidate = candidateInfo.first;
				int numberOfDonors = candidateInfo.second;

				std::vector<KPDDonor *>  donors;

				nDonorsTotal += numberOfDonors;

				generateDonors(donors, numberOfDonors, candidate); // Generate donors for candidate

				KPDNode * newPairNode = new KPDNode(nodeID, time, donors, candidate);
				kpdNodes.push_back(newPairNode);
				nodeID++;

				kpdRecordLog << "At Time " << time << ", Pair " << nodeID << "Joins" << std::endl;
				kpdRecordLog << candidate->candidateOutput() << std::endl;
				for (int k = 1; k <= (int)donors.size(); k++) {

					int donorIndex = k - 1;

					kpdRecordLog << donors[donorIndex]->donorOutput() << std::endl;
				}
				kpdRecordLog << std::endl;

				pairTimeTracker += rngArrival.rexp(pairArrivals); // Get next pair arrival time
				nPairs++;
			}

			time++;
		}

		kpdRecordLog << "At Match Run at Time " << matchRunTime << ", # of NDDs Added: " << nNDDs << ", # of Pairs Added: " << nPairs << std::endl;
		nNDDsTotal += nNDDs;
		nPairsTotal += nPairs;
	}	

	kpdRecordLog << "Total Pairs: " << nPairsTotal <<  ", Total NDDs: " << nNDDsTotal << ", Total Donors: " << nDonorsTotal << std::endl;

}

void KPDRecord::assignStateTransitions() {
	
	kpdRecordLog << "Generating State Transitions:" << std::endl;
	std::cout << "Generating State Transitions..." << std::endl;
	
	int timeSpan = kpdParameters->getTimeSpan();
	int postSelectionInactivePeriod = kpdParameters->getPostSelectionInactivePeriod();

	double probPairAttrition = kpdParameters->getProbPairAttrition();
	double probNDDAttrition = kpdParameters->getProbNDDAttrition();
	double probPairActiveToInactive = kpdParameters->getProbPairActiveToInactive();
	double probPairInactiveToActive = kpdParameters->getProbPairInactiveToActive();
		
	//Iterate through KPD nodes
	for (std::vector<KPDNode *>::iterator it = kpdNodes.begin(); it != kpdNodes.end(); it++) {

		KPDNode * node = *it;
		int nodeID = node->getID();

		kpdRecordLog << "Node " << nodeID << std::endl;

		std::deque<KPDStatus> kpdNodeStateTransitions;
		std::deque<int> kpdNodeStateTransitionTimes;

		int time = node->getArrivalTime();
		KPDStatus status = STATUS_ACTIVE;

		kpdNodeStateTransitions.push_back(status);
		kpdNodeStateTransitionTimes.push_back(time);

		//State transitions for pairs
		if (node->getType() == PAIR) {

			kpdRecordLog << nodeID << ": " << time << " (" << KPDFunctions::statusToString(status) << ") ";
					
			while (time < (timeSpan + postSelectionInactivePeriod) && status != STATUS_WITHDRAWN) {
				
				time++;
				
				double w = rngStatus.runif(); // Withdrawal
				double u = rngStatus.runif(); // Active to Inactive or Inactive to Active

				if (w < probPairAttrition) {
					status = STATUS_WITHDRAWN;

					kpdNodeStateTransitions.push_back(status);
					kpdNodeStateTransitionTimes.push_back(time);

					kpdRecordLog << time << " (" << KPDFunctions::statusToString(status) << ") ";
				}

				else if (status == STATUS_ACTIVE && u < probPairActiveToInactive) {
					status = STATUS_INACTIVE;

					kpdNodeStateTransitions.push_back(status);
					kpdNodeStateTransitionTimes.push_back(time);

					kpdRecordLog << time << " (" << KPDFunctions::statusToString(status) << ") ";
				}

				else if (status == STATUS_INACTIVE && u < probPairInactiveToActive) {
					status = STATUS_ACTIVE;

					kpdNodeStateTransitions.push_back(status);
					kpdNodeStateTransitionTimes.push_back(time);

					kpdRecordLog << time << " (" << KPDFunctions::statusToString(status) << ") ";
				}
			}

			kpdRecordLog << std::endl;
		}

		else {

			kpdRecordLog << nodeID << ": " << time << " (" << KPDFunctions::statusToString(status) << ") ";

			while (time <= (timeSpan + postSelectionInactivePeriod) && status != STATUS_WITHDRAWN) {
				
				time++;

				double w = rngStatus.runif();
				
				if (w < probNDDAttrition){
					status = STATUS_WITHDRAWN;

					kpdNodeStateTransitions.push_back(status);
					kpdNodeStateTransitionTimes.push_back(time);

					kpdRecordLog << time << " (" << KPDFunctions::statusToString(status) << ") ";
				}
			}

			kpdRecordLog << std::endl;
		}

		kpdNodeStateTransitionMatrix.push_back(kpdNodeStateTransitions);
		kpdNodeStateTransitionTimeMatrix.push_back(kpdNodeStateTransitionTimes);
	}
}

void KPDRecord::assignMatchProperties() {
	
	kpdRecordLog << "Generating Match Properties:" << std::endl;
	std::cout << "Generating Match Properties..." << std::endl;

	int N = (int) kpdNodes.size();

	kpdAdjacencyMatrix.assign(N, std::vector<bool>(1 + N, false));
	kpdAdjacencyMatrixReduced.assign(N, std::vector<bool>(1 + N, false));

	//Iterate through donor nodes
	for (int i = 1; i <= N; i++) {

		int donorNodeIndex = i - 1;

		KPDNode * donorNode = kpdNodes[donorNodeIndex];
		int donorNodeID = donorNode->getID();

		//Get donor nodes
		std::vector<KPDDonor *> donors = donorNode->getDonors();
		int numDonors = donorNode->getNumberOfDonors();

		//Iterate through candidate nodes
		for (int j = 1; j <= N; j++) {

			int candidateNodeIndex = j - 1;

			KPDNode * candidateNode = kpdNodes[candidateNodeIndex];
			int candidateNodeID = candidateNode->getID();
			
			if (i != j) {
				// Pair -> NDD (Implicit Backward Edge from all Donors to the NDD)
				if (donorNode->getType() == PAIR && candidateNode->getType() == NDD) {
					kpdAdjacencyMatrix[i][j] = true;
				}

				// Pair
				else if (candidateNode->getType() == PAIR) {

					double pra = candidateNode->getCandidatePRA();

					//Iterate through associated donors
					for (int k = 1; k <= numDonors; k++) {

						int donorIndex = k - 1;

						//Perform virtual crossmatch
						KPDCrossmatch virtualCrossmatchResult = kpdData->performCrossmatch(candidateNode->getCandidate(), donors[donorIndex]);

						if (kpdData->allowableMatch(virtualCrossmatchResult)) {

							kpdAdjacencyMatrix[i][j] = true;
							kpdAdjacencyMatrixReduced[i][j] = true;

							KPDMatch * newMatch = generateMatch(candidateNode->getCandidate(), donorNode->getDonor(donorIndex), virtualCrossmatchResult);
							
							kpdMatches[donorNodeID][candidateNodeID].push_back(newMatch);
							
							kpdRecordLog << "   " << donorNodeID << "[" << k << "] -> " << candidateNodeID << " " << newMatch->matchShortOutput() << std::endl;
						}

						else {

							KPDMatch * noMatch = new KPDMatch();
							noMatch->setVirtualCrossmatchResult(virtualCrossmatchResult);

							kpdMatches[donorNodeID][candidateNodeID].push_back(noMatch);
						}
					}
				}
			}
		}
	}
}

KPDDonor * KPDRecord::generateDonor() {

	std::vector<double> u;
	for (int i = 1; i <= 5; i++) {
		u.push_back(rngDonor.runif());
	}

	KPDDonor * newDonor = kpdData->generateDonor(u);

	return(newDonor);
}


void KPDRecord::generateDonors(std::vector<KPDDonor *> & donors, int numberOfDonors, KPDCandidate * candidate) {

	int generatedDonors = 0;
	
	while (generatedDonors < numberOfDonors) {
		
		KPDDonor * donor = generateDonor();
		
		KPDCrossmatch crossmatch = kpdData->performCrossmatch(candidate, donor);

		if (!kpdData->allowableMatch(crossmatch)) {
			donors.push_back(donor);
			generatedDonors++;
		}
	}
}

KPDMatch * KPDRecord::generateMatch(KPDCandidate * candidate, KPDDonor * donor, KPDCrossmatch virtualCrossmatchResult) {

	//Assign utility values
	double fiveYearSurvival = kpdData->calculateSurvival(candidate, donor, 1);
	double tenYearSurvival = kpdData->calculateSurvival(candidate, donor, 0);

	int pra = candidate->getPRA();

	double transplantDifficultyScore = 0.0001;
	if (pra >= 97 || donor->getBT() == BT_AB) {
		transplantDifficultyScore = 1.0;
	}

	double assignedUtility = rngMatch.runif(kpdParameters->getMatchUtilityLowerBound(), kpdParameters->getMatchUtilityUpperBound());

	//Generate probability values
	double assumedMatchSuccessProbability = 0.5;
	double actualMatchSuccessProbability = 0.5;

	if (pra < 25) {
		assumedMatchSuccessProbability = 0.95;
		actualMatchSuccessProbability = 0.95;
	}
	else if (pra >= 25 && pra < 50) {
		assumedMatchSuccessProbability = 0.8;
		actualMatchSuccessProbability = 0.8;
	}
	else if (pra >= 50 && pra < 75) {
		assumedMatchSuccessProbability = 0.65;
		actualMatchSuccessProbability = 0.65;
	}

	bool labCrossmatchResult = rngMatch.runif() < actualMatchSuccessProbability;

	KPDMatch * newMatch = new KPDMatch(true, fiveYearSurvival, tenYearSurvival, transplantDifficultyScore, assignedUtility, 
		assumedMatchSuccessProbability, actualMatchSuccessProbability, virtualCrossmatchResult, labCrossmatchResult);
	
	return newMatch;
}

void KPDRecord::generateSimulationData(int iteration, std::vector<int> matchRunTimes){

	currentIteration = iteration;
	
	kpdRecordLog << "--------------" << std::endl;
	kpdRecordLog << "Iteration " << iteration << std::endl;
	kpdRecordLog << "--------------" << std::endl << std::endl;
	
	//Initialize random number seeds

	kpdRecordLog << "Initializing Random Number Generator Seeds" << std::endl;
	std::cout << "Initializing Random Number Generator Seeds... " << std::endl;

	rngSelection.setSeed(kpdParameters->getRNGSeedSelection() * iteration);
	rngArrival.setSeed(kpdParameters->getRNGSeedArrival() * iteration);
	rngMatch.setSeed(kpdParameters->getRNGSeedMatch() * iteration);
	rngDonor.setSeed(kpdParameters->getRNGSeedDonor() * iteration);
	rngStatus.setSeed(kpdParameters->getRNGSeedStatus() * iteration);

	//Select new simulation data, assign node and match properties
	assembleKPD(matchRunTimes);
	assignStateTransitions();
	assignMatchProperties();

	kpdRecordLog << "Simulation Data for Iteration " << iteration << " Generated" << std::endl << std::endl;
	std::cout << "Simulation Data for Iteration " << iteration << " Generated" << std::endl << std::endl;
}

std::vector<KPDNode *> KPDRecord::getNodes() {

	std::vector<KPDNode *> nodes; 

	for (std::vector<KPDNode*>::iterator it = kpdNodes.begin(); it != kpdNodes.end(); it++) {
		nodes.push_back((*it)->copy()); // Deep Copy
	}

	return nodes;
}

std::vector<KPDNodeType> KPDRecord::getNodeTypes() {

	std::vector<KPDNodeType> nodeTypes;

	for (std::vector<KPDNode*>::iterator it = kpdNodes.begin(); it != kpdNodes.end(); it++) {
		nodeTypes.push_back((*it)->getType());
	}

	return nodeTypes;
}

std::vector<std::vector<bool> > KPDRecord::getAdjacencyMatrix() {

	std::vector<std::vector<bool> > adjacencyMatrixClone(kpdAdjacencyMatrix);

	return adjacencyMatrixClone;
}

std::vector<std::vector<bool> > KPDRecord::getAdjacencyMatrixReduced() {

	std::vector<std::vector<bool> > adjacencyMatrixReducedClone(kpdAdjacencyMatrixReduced);

	return adjacencyMatrixReducedClone;
}

std::vector<std::deque<KPDStatus> > KPDRecord::getKPDNodeStateTransitionMatrix() {

	std::vector<std::deque<KPDStatus> > kpdNodeStateTransitionMatrixClone(kpdNodeStateTransitionMatrix);

	return kpdNodeStateTransitionMatrixClone;
}

std::vector<std::deque<int> > KPDRecord::getKPDNodeStateTransitionTimeMatrix() {

	std::vector<std::deque<int> > kpdNodeStateTransitionTimeMatrixClone(kpdNodeStateTransitionTimeMatrix);

	return kpdNodeStateTransitionTimeMatrixClone;
}

std::map<int, std::map<int, std::vector<KPDMatch*> > > KPDRecord::getKPDMatches() {

	std::map<int, std::map<int, std::vector<KPDMatch*> > > kpdMatchesCopy;

	for (std::map<int, std::map<int, std::vector<KPDMatch*> > >::iterator itDonor = kpdMatches.begin(); itDonor != kpdMatches.end(); itDonor++) {

		int donorNodeID = itDonor->first;

		std::map<int, std::vector<KPDMatch*> > donorMatches = itDonor->second;
		
		for (std::map<int, std::vector<KPDMatch *> >::iterator itCandidate = donorMatches.begin(); itCandidate != donorMatches.end(); itCandidate++) {

			int candidateNodeID = itCandidate->first;

			std::vector<KPDMatch*> matches = itCandidate->second;

			std::vector<KPDMatch *> matchCopy;

			for (std::vector<KPDMatch *>::iterator itMatch = matches.begin(); itMatch != matches.end(); itMatch++) {
				matchCopy.push_back((*itMatch)->copy());
			}

			kpdMatchesCopy[donorNodeID][candidateNodeID] = matchCopy;
		}
	}

	return kpdMatchesCopy;
}

std::string KPDRecord::getPopulationList() {

	std::stringstream population;

	//Iterates through each pair involved in the simulation and collects demographic information
	for (std::vector<KPDNode *>::iterator itNode = kpdNodes.begin(); itNode != kpdNodes.end(); itNode++){

		int numberOfDonors = (*itNode)->getNumberOfDonors();

		for (int k = 1; k <= numberOfDonors; k++){

			int donorIndex = k - 1;

			population << currentIteration << ",";
			population << (*itNode)->getID() << ",";
			population << k << ",";
			population << KPDFunctions::nodeTypeToString((*itNode)->getType()) << ",";
			population << (*itNode)->getArrivalTime() << ",";
			population << (*itNode)->getCandidateString() << ",";
			population << (*itNode)->getDonorString(donorIndex) << ",";
			population << "," << std::endl;
			population << std::endl;
		}
	}

	return population.str();
}

void KPDRecord::printLog(){
	
	std::string logFile = "output/" + kpdParameters->getOutputFolder() + "/" + kpdParameters->getSubFolder() + "/Log-Record.txt";
	std::ofstream outputFileLog(logFile.c_str());
	outputFileLog << kpdRecordLog.str();
	outputFileLog.close();
}


#endif