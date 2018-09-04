/* ---------------------------------------------
DD-Record.h
Stores data for specific simulation iteration
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
	std::vector<int> matchRunTimes;
	
	std::vector<KPDDonor *> deceasedDonors;
	std::vector<KPDCandidate *> waitlistedCandidates;
	std::vector<KPDNode *> kpdNodes;
	
	std::vector<std::deque<KPDStatus> > kpdNodeStateTransitionMatrix;
	std::vector<std::deque<int> > kpdNodeStateTransitionTimeMatrix;
	
	std::vector<std::vector<KPDMatch *> > deceasedDonorMatches;
	std::vector<std::vector<std::vector<KPDMatch *> > > waitlistedCandidateMatches;
	std::vector<std::vector<std::vector<KPDMatch *> > > kpdMatches;

	std::vector<std::vector<bool> > kpdAdjacencyMatrix;
	std::vector<std::vector<bool> > kpdAdjacencyMatrixReduced;

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

	// Helper Functions 
	void setMatchRunTimes();
	void setRNGSeeds();
	
	void assembleWaitlist();
	void assembleKPD();	
	
	void assignStateTransitions();
	void assignMatchProperties();

	//Helper Functions 
	KPDDonor * generateDonor();
	void generateDonors(std::vector<KPDDonor *> & donors, int numberOfDonors, KPDCandidate * candidate);
	
	bool allowableMatch(KPDCrossmatch crossmatch);

	std::stringstream kpdRecordLog;


public:

	KPDRecord(KPDParameters * params);
	~KPDRecord();

	void generateSimulationData(int iteration);

	// Cloning Functions
	std::deque<int> getMatchRunTimes();

	std::deque<KPDDonor *> getDeceasedDonors();
	std::deque<int> getDeceasedDonorArrivalTimes();

	std::vector<KPDCandidate *> getWaitlistedCandidates();

	std::vector<std::deque<KPDStatus> > getWaitlistCandidatesStateTransitionMatrix();
	std::vector<std::deque<int> > getWaitlistCandidatesStateTransitionTimeMatrix();
	
	std::vector<KPDNode *> getNodes();
	std::vector<KPDNodeType> getNodeTypes();

	std::vector<std::deque<KPDStatus> > getKPDNodeStateTransitionMatrix();
	std::vector<std::deque<int> > getKPDNodeStateTransitionTimeMatrix();

	std::vector<std::vector<KPDMatch *> > getDeceasedDonorMatches();
	std::vector<std::vector<std::vector<KPDMatch *> > > getWaitlistCandidateMatches();
	std::vector<std::vector<std::vector<KPDMatch *> > > getKPDMatches();

	std::vector<std::vector<bool> > getAdjacencyMatrix();
	std::vector<std::vector<bool> > getAdjacencyMatrixReduced();
	
	std::string getPopulationList();

	void printLog();
	
};

KPDRecord::KPDRecord(KPDParameters * params){

	kpdParameters = params;

	std::cout << "Collecting Data" << std::endl;
	kpdData = new KPDData(params);

	setMatchRunTimes();
}

KPDRecord::~KPDRecord(){

	delete kpdData;

	printLog();
}

void KPDRecord::setMatchRunTimes() {

	// Set aside match run times			
	kpdRecordLog << "Match Runs Occur at Times: ";
	
	int matchRunTime = 0;
	int freqMatchRun = kpdParameters->getFreqMatchRun();
	int timeSpan = kpdParameters->getTimeSpan();

	matchRunTime += freqMatchRun;

	while (matchRunTime <= timeSpan) {

		matchRunTimes.push_back(matchRunTime);
		kpdRecordLog << " " << matchRunTime;

		matchRunTime += freqMatchRun;
	}

	if (matchRunTimes.back() != timeSpan) { // Final match run occurs at end of simulation, regardless of time since last match run
		
		matchRunTimes.push_back(timeSpan);
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

void KPDRecord::assembleWaitlist() {

	// ---TO DO--- //


}

void KPDRecord::assembleKPD(){

	kpdRecordLog << "Selecting Nodes:" << std::endl;
	std::cout << "Selecting Nodes..." << std::endl;
	
	double pairArrivals = kpdParameters->getPairArrivals();
	double nddArrivals = kpdParameters->getNDDArrivals();
	
	int nDonorsTotal = 0;
	int nPairsTotal = 0;
	int nNDDsTotal = 0;

	std::vector<KPDDonor *> ndds;
	std::vector<int> nddArrivalTimes;

	std::vector<KPDCandidate *> candidates;
	std::vector<std::vector<KPDDonor *> > donors;
	std::vector<int> pairArrivalTimes;

	// Generate NDDs	
	double nddTimeTracker = rngArrival.rexp(nddArrivals); // Poisson Process for NDD arrival

	for (unsigned i = 1; i <= matchRunTimes.size(); i++) { // Will count NDDs by match run

		int nNDDs = 0;

		while (nddTimeTracker <= matchRunTimes[i - 1]) { // Generate NDDs for given match run

			nNDDs++;

			KPDDonor * newNDD = generateDonor(); // Randomly generate NDD

			ndds.push_back(newNDD);
			nddArrivalTimes.push_back((int) ceil(nddTimeTracker)); // Arrival time rounded to nearest day
			
			double u = rngNDD.rexp(nddArrivals); // Get next NDD arrival time
			nddTimeTracker += u;
		}

		kpdRecordLog << "At Match Run at Time " << matchRunTimes[i - 1] << ", # of NDDs Added: " << nNDDs << std::endl;
		nNDDsTotal += nNDDs;
	}	

	//Generate pairs
	double pairTimeTracker = rngArrival.rexp(pairArrivals); // Poisson Process for pair arrival

	for (unsigned i = 1; i <= matchRunTimes.size(); i++) { // Will count pairs by match run
		
		int nPairs = 0;

		while (pairTimeTracker <= matchRunTimes[i - 1]) { // Generate pairs for given match run
						
			nPairs++;

			std::vector<KPDDonor *>  associatedDonors;
			associatedDonors.push_back(new KPDDonor()); // Initialize donors
			
			double uCandidate = rngSelection.runif();

			KPDCandidate * candidate = kpdData->drawCandidate(uCandidate); // Draw candidate with replacement
					
			int numberOfDonors =(int) kpdData->retrieveDonors(candidate->getMatchingID()).size(); // Collect number of donors associated with drawn candidate
			nDonorsTotal += numberOfDonors;
			
			generateDonors(associatedDonors, numberOfDonors, candidate); // Generate donors for candidate

			candidates.push_back(candidate);
			donors.push_back(associatedDonors);
			pairArrivalTimes.push_back((int) ceil(pairTimeTracker)); // Arrival time rounded to nearest day
			
			double u = rngArrival.rexp(pairArrivals); // Get next pair arrival time
			pairTimeTracker += u;
		}				
		
		kpdRecordLog << "At Match Run at Time " << matchRunTimes[i - 1] << ", # of Pairs Added: " << nPairs << std::endl;
		nPairsTotal += nPairs;
	}

	kpdRecordLog << "Total Pairs: " << nPairsTotal <<  ", Total NDDs: " << nNDDsTotal << ", Total Donors: " << nDonorsTotal << std::endl;


	// Store nodes in order based on arrival time
	int nddIndex = 0;
	int pairIndex = 0;

	int nodeID = 1;
	kpdNodes.push_back(new KPDNode());

	int time = 0;
	
	while (nddIndex < ndds.size() || pairIndex < candidates.size()) {
		
		time++;

		if (nddIndex < ndds.size()) {

			int nddArrival = nddArrivalTimes[nddIndex];

			while (nddArrival == time) {

				KPDDonor * ndd = ndds[nddIndex];

				KPDNode * newNDD = new KPDNode(nodeID, nddArrival, ndd);
				kpdNodes.push_back(newNDD);
				nodeID++;

				kpdRecordLog << "At Time " << nddArrival << ", NDD " << nodeID << "Joins" << std::endl;
				kpdRecordLog << ndd->donorOutput() << std::endl;
				kpdRecordLog << std::endl;

				nddIndex++;

				if (nddIndex == ndds.size()) {
					nddArrival = -1;
				}
				else {
					nddArrival = nddArrivalTimes[nddIndex];
				}
			}
		}
		
		if (pairIndex < candidates.size()) {
			
			int pairArrival = pairArrivalTimes[pairIndex];
			
			while (pairArrival == time) {

				KPDCandidate * candidate = candidates[pairIndex];
				std::vector<KPDDonor *> pairedDonors = donors[pairIndex];

				KPDNode * newPair = new KPDNode(nodeID, pairArrival, pairedDonors, candidate);
				kpdNodes.push_back(newPair);
				nodeID++;

				kpdRecordLog << "At Time " << pairArrival << ", Pair " << nodeID << "Joins" << std::endl;
				kpdRecordLog << candidate->candidateOutput() << std::endl;
				for (int i = 0; i < pairedDonors.size(); i++) {
					kpdRecordLog << pairedDonors[i]->donorOutput() << std::endl;
				}
				kpdRecordLog << std::endl;

				pairIndex++;
				if (pairIndex == candidates.size()) {
					pairArrival = -1;
				}
				else {
					pairArrival = pairArrivalTimes[pairIndex];
				}
			}
		}
	}
}

void KPDRecord::assignStateTransitions() {

	//--- TO DO ---//

	// State transitions for waitlist candidates
	// State transitions for pairs that become bridge donors (needs to be set in advance)


	kpdRecordLog << "Generating State Transitions:" << std::endl;
	std::cout << "Generating State Transitions..." << std::endl;
	
	int timeSpan = kpdParameters->getTimeSpan();
	int postSelectionInactivePeriod = kpdParameters->getPostSelectionInactivePeriod();

	int N = (int)kpdNodes.size() - 1;

	kpdNodeStateTransitionMatrix.assign(1, std::deque<KPDStatus>(1, STATUS_INACTIVE));
	kpdNodeStateTransitionTimeMatrix.assign(1, std::deque<int>(1, 0));

	double probPairAttrition = kpdParameters->getProbPairAttrition();
	double probNDDAttrition = kpdParameters->getProbNDDAttrition();
	double probPairActiveToInactive = kpdParameters->getProbPairActiveToInactive();
	double probPairInactiveToActive = kpdParameters->getProbPairInactiveToActive();
		
	//Iterate through KPD nodes
	for (int i = 1; i <= N; i++) {

		kpdRecordLog << "Node " << i << std::endl;

		std::deque<KPDStatus> kpdNodeStateTransitions;
		std::deque<int> kpdNodeStateTransitionTimes;

		int time = kpdNodes[i]->getArrivalTime();
		KPDStatus status = STATUS_ACTIVE;

		kpdNodeStateTransitions.push_back(status);
		kpdNodeStateTransitionTimes.push_back(time);

		//State transitions for pairs
		if (kpdNodes[i]->getType() == PAIR) {

			kpdRecordLog << i << ": " << time << " (" << KPDFunctions::statusToString(status) << ") ";
					
			while (time < (timeSpan + postSelectionInactivePeriod) && status != STATUS_WITHDRAWN) {
				
				time++;
				
				double w = rngTransition.runif();
				double u = rngTransition.runif();

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

			kpdRecordLog << i << ": " << time << " (" << KPDFunctions::statusToString(status) << ") ";

			while (time <= (timeSpan + postSelectionInactivePeriod) && status != STATUS_WITHDRAWN) {
				
				time++;

				double w = rngTransition.runif();
				
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

	//--- TO DO ---//

	// Matches between deceased donors and KPD
	// Matches between KPD and waitlist candidates

	kpdRecordLog << "Generating Match Properties:" << std::endl;
	std::cout << "Generating Match Properties..." << std::endl;

	int N = (int)kpdNodes.size() - 1;

	kpdAdjacencyMatrix.assign(1 + N, std::vector<bool>(1 + N, false));
	kpdAdjacencyMatrixReduced.assign(1 + N, std::vector<bool>(1 + N, false));
	kpdMatches.assign(1 + N, std::vector<std::vector<KPDMatch *> >(1 + N, std::vector<KPDMatch *>(1, new KPDMatch())));

	//Iterate through donor nodes
	for (int i = 1; i <= N; i++) {

		//Get donor nodes
		std::vector<KPDDonor *> donors = kpdNodes[i]->getDonors();
		int numDonors = kpdNodes[i]->getNumberOfDonors();

		//Iterate through candidate nodes
		for (int j = 1; j <= N; j++) {

			//Initialize matrices
			for (int k = 0; k <= numDonors; k++) {
				kpdMatches[i][j].push_back(new KPDMatch());
			}

			if (i != j) {
				// Pair -> NDD (Implicit Backward Edge from all Donors to the NDD)
				if (kpdNodes[i]->getType() == PAIR && kpdNodes[j]->getType() == NDD) {
					kpdAdjacencyMatrix[i][j] = true;

					for (int k = 1; k <= numDonors; k++) { // These are all probably unnecessary, but are good for placeholding
						kpdMatches[i][j][k]->setAdjacency(true);
						kpdMatches[i][j][k]->setAssumedSuccessProbability(1.0);
						kpdMatches[i][j][k]->setActualSuccessProbability(1.0);
						kpdMatches[i][j][k]->setLabCrossmatchResult(true);

						kpdRecordLog << "   " << i << "[" << k << "] -> " << j << " (Implicit)" << std::endl;
					}
				}

				// Pair
				else if (kpdNodes[j]->getType() == PAIR) {
					double pra = kpdNodes[j]->getCandidatePRA();

					//Iterate through associated donors
					for (int k = 1; k <= numDonors; k++) {

						//Perform virtual crossmatch
						KPDCrossmatch virtualCrossmatchResult = kpdData->performCrossmatch(kpdNodes[j]->getCandidate(), donors[k]);

						if (allowableMatch(virtualCrossmatchResult)) {

							kpdAdjacencyMatrix[i][j] = true;
							kpdAdjacencyMatrixReduced[i][j] = true;

							//Assign utility values
							double fiveYearSurvival;
							double tenYearSurvival;
							double transplantDifficultyScore;
							double assignedUtility;

							fiveYearSurvival = kpdData->calculateSurvival(kpdNodes[j]->getCandidate(), donors[k], 1);
							tenYearSurvival = kpdData->calculateSurvival(kpdNodes[j]->getCandidate(), donors[k], 0);

							if (pra >= 97 || kpdNodes[i]->getDonorBT(k) == BT_AB) {
								transplantDifficultyScore = 1.0;
							}
							else {
								transplantDifficultyScore = 0.0001;
							}

							assignedUtility = rngUtility.runif(kpdParameters->getMatchUtilityLowerBound(), kpdParameters->getMatchUtilityUpperBound());

							//Generate probability values
							double assumedMatchSuccessProbability = 1.0;
							double actualMatchSuccessProbability = 1.0;

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
							else {
								assumedMatchSuccessProbability = 0.5;
								actualMatchSuccessProbability = 0.5;
							}
							
							bool labCrossmatchResult = rngCrossmatch.runif() < actualMatchSuccessProbability;

							kpdMatches[i][j][k]->setMatchProperties(true, fiveYearSurvival, tenYearSurvival, transplantDifficultyScore, assignedUtility, assumedMatchSuccessProbability, actualMatchSuccessProbability, virtualCrossmatchResult, labCrossmatchResult);
							
							kpdRecordLog << "   " << i << "[" << k << "] -> " << j << " " << kpdMatches[i][j][k]->matchShortOutput() << std::endl;
						}
					}
				}
			}
		}
	}
}

KPDDonor * KPDRecord::generateDonor() {

	// Simulated Donor Information	

	double uBT = rngDonorGenerator.runif();
	double uHLA1 = rngDonorGenerator.runif();
	double uHLA2 = rngDonorGenerator.runif();
	double uBW4 = rngDonorGenerator.runif();
	double uBW6 = rngDonorGenerator.runif();

	// Donor Crossmatch Information

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

	//Generate Donor
	KPDDonor * donor = new KPDDonor();
	donor->setBT(dBT);
	donor->setHLA(dHLA);
	
	return donor;

}

void KPDRecord::generateDonors(std::vector<KPDDonor *> & donors, int numberOfDonors, KPDCandidate * candidate) {

	int generatedDonors = 0;
	
	while (generatedDonors < numberOfDonors) {
		
		KPDDonor * donor = generateDonor();
		
		KPDCrossmatch crossmatch = kpdData->performCrossmatch(candidate, donor);

		if (!allowableMatch(crossmatch)) {
			donors.push_back(donor);
			generatedDonors++;
		}
	}
}


bool KPDRecord::allowableMatch(KPDCrossmatch crossmatch) {

	if (crossmatch == CROSSMATCH_SUCCESSFUL){
		return true;
	}

	if (crossmatch == CROSSMATCH_REQUIRES_DESENSITIZATION_AND_O_TO_NON_O){
		if (kpdParameters->getAllowDesensitization() && !kpdParameters->getReserveODonorsForOCandidates()){		
			return true;
		}
	}
	
	if (crossmatch == CROSSMATCH_REQUIRES_DESENSITIZATION){
		if (kpdParameters->getAllowDesensitization()) {
			return true;
		}
	}

	if (crossmatch == CROSSMATCH_O_DONOR_TO_NON_O_CANDIDATE) {
		if (!kpdParameters->getReserveODonorsForOCandidates()) {
			return true;
		}
	}

	return false;
}

void KPDRecord::generateSimulationData(int iteration){

	currentIteration = iteration;
	
	kpdRecordLog << "--------------" << std::endl;
	kpdRecordLog << "Iteration " << currentIteration << std::endl;
	kpdRecordLog << "--------------" << std::endl << std::endl;

	//Clear existing variables
	kpdRecordLog << "Resetting Simulation Data" << std::endl;
	std::cout << "Resetting Data... " << std::endl;
	
	deceasedDonors.clear();
	waitlistedCandidates.clear();
	kpdNodes.clear();

	kpdNodeStateTransitionMatrix.clear();
	kpdNodeStateTransitionMatrix.clear();

	deceasedDonorMatches.clear();
	waitlistedCandidateMatches.clear();
	kpdMatches.clear();
	
	kpdAdjacencyMatrix.clear();
	kpdAdjacencyMatrixReduced.clear();

	//Initialize random number seeds
	setRNGSeeds();

	//Select new simulation data
	assembleWaitlist();
	assembleKPD();

	//Assign node properties
	assignStateTransitions();

	//Assign match properites
	assignMatchProperties();

	kpdRecordLog << "Simulation Data for Iteration " << currentIteration << " Generated" << std::endl << std::endl;
	std::cout << "Simulation Data for Iteration " << currentIteration << " Generated" << std::endl << std::endl;
}


//Cloning Functions
std::deque<int> KPDRecord::getMatchRunTimes() {

	std::deque<int> times; // Starts at 0, Convert to deque

	for (std::vector<int>::iterator it = matchRunTimes.begin(); it != matchRunTimes.end(); ++it) {
		times.push_back(*it);
	}

	return times;
}

std::deque<KPDDonor*> KPDRecord::getDeceasedDonors(){

	std::deque<KPDDonor *> donors;

	for (std::vector<KPDDonor *>::iterator it = deceasedDonors.begin(); it != deceasedDonors.end(); it++) {
		donors.push_back((*it)->copy());
	}

	return donors;
}

std::deque<int> KPDRecord::getDeceasedDonorArrivalTimes(){

	std::deque<int> donorArrivalTimes;

	for (std::vector<KPDDonor *>::iterator it = deceasedDonors.begin(); it != deceasedDonors.end(); it++) {
		donorArrivalTimes.push_back((*it)->getRecoveryTime());
	}

	return donorArrivalTimes;
}

std::vector<KPDCandidate*> KPDRecord::getWaitlistedCandidates(){
	
	std::vector<KPDCandidate *> candidates;

	for (std::vector<KPDCandidate *>::iterator it = waitlistedCandidates.begin(); it != waitlistedCandidates.end(); it++) {
		candidates.push_back((*it)->copy());
	}

	return candidates;
}

std::vector<std::deque<KPDStatus> > KPDRecord::getWaitlistCandidatesStateTransitionMatrix(){
	
	std::vector<std::deque<KPDStatus> > waitlistCandidatesStateTransitionMatrix;
	
	for (std::vector<KPDCandidate *>::iterator it = waitlistedCandidates.begin(); it != waitlistedCandidates.end(); it++) {
		waitlistCandidatesStateTransitionMatrix.push_back((*it)->getStatuses());
	}
	
	return waitlistCandidatesStateTransitionMatrix;
}

std::vector<std::deque<int> > KPDRecord::getWaitlistCandidatesStateTransitionTimeMatrix(){
	
	std::vector<std::deque<int> > waitlistCandidatesStateTransitionTimeMatrix;

	for (std::vector<KPDCandidate *>::iterator it = waitlistedCandidates.begin(); it != waitlistedCandidates.end(); it++) {
		waitlistCandidatesStateTransitionTimeMatrix.push_back((*it)->getStatusChangeTimes());
	}

	return waitlistCandidatesStateTransitionTimeMatrix;

}

std::vector<KPDNode *> KPDRecord::getNodes() {

	std::vector<KPDNode *> nodes; // Starts at 1

	nodes.push_back(new KPDNode());

	for (std::vector<KPDNode*>::iterator it = kpdNodes.begin() + 1; it != kpdNodes.end(); it++) {
		nodes.push_back((*it)->copy()); // Deep Copy
	}

	return nodes;
}

std::vector<KPDNodeType> KPDRecord::getNodeTypes() {

	std::vector<KPDNodeType> nodeTypes; // Starts at 1

	nodeTypes.push_back(PAIR);

	for (std::vector<KPDNode*>::iterator it = kpdNodes.begin() + 1; it != kpdNodes.end(); it++) {
		nodeTypes.push_back((*it)->getType());
	}

	return nodeTypes;
}

std::vector<std::vector<KPDMatch*> > KPDRecord::getDeceasedDonorMatches(){
	
	std::vector<std::vector<KPDMatch *> > deceasedDonorMatchMatrixCopy;

	for (std::vector<std::vector<KPDMatch *> >::iterator it = deceasedDonorMatches.begin(); it != deceasedDonorMatches.end(); it++) {

		std::vector<KPDMatch *> donorMatchesCopy;

		for (std::vector<KPDMatch *>::iterator itMatch = (*it).begin(); itMatch != (*it).end(); itMatch++) {
			donorMatchesCopy.push_back((*itMatch)->copy());
		}

		deceasedDonorMatchMatrixCopy.push_back(donorMatchesCopy);
	}

	return deceasedDonorMatchMatrixCopy;
}

std::vector<std::vector<std::vector<KPDMatch*> > > KPDRecord::getWaitlistCandidateMatches(){
	
	std::vector<std::vector<std::vector<KPDMatch *> > > waitlistCandidateMatchMatrixCopy; // Starts at 1

	for (std::vector<std::vector<std::vector<KPDMatch *> > >::iterator itRow = waitlistedCandidateMatches.begin(); itRow != waitlistedCandidateMatches.end(); itRow++) {

		std::vector<std::vector<KPDMatch *> > waitlistCandidateMatchMatrixRowCopy;

		for (std::vector<std::vector<KPDMatch *> >::iterator itColumn = (*itRow).begin(); itColumn != (*itRow).end(); itColumn++) {

			std::vector<KPDMatch *> donorMatchesCopy;

			for (std::vector<KPDMatch *>::iterator itDonors = (*itColumn).begin(); itDonors != (*itColumn).end(); itDonors++) {
				donorMatchesCopy.push_back((*itDonors)->copy());
			}

			waitlistCandidateMatchMatrixRowCopy.push_back(donorMatchesCopy);
		}

		waitlistCandidateMatchMatrixCopy.push_back(waitlistCandidateMatchMatrixRowCopy);
	}

	return waitlistCandidateMatchMatrixCopy;
}

std::vector<std::vector<std::vector<KPDMatch *> > > KPDRecord::getKPDMatches() {

	std::vector<std::vector<std::vector<KPDMatch *> > > kpdMatchMatrixCopy; // Starts at 1

	for (std::vector<std::vector<std::vector<KPDMatch *> > >::iterator itRow = kpdMatches.begin(); itRow != kpdMatches.end(); itRow++) {

		std::vector<std::vector<KPDMatch *> > kpdMatchMatrixRowCopy;

		for (std::vector<std::vector<KPDMatch *> >::iterator itColumn = (*itRow).begin(); itColumn != (*itRow).end(); itColumn++) {

			std::vector<KPDMatch *> donorMatchesCopy;

			for (std::vector<KPDMatch *>::iterator itDonors = (*itColumn).begin(); itDonors != (*itColumn).end(); itDonors++) {
				donorMatchesCopy.push_back((*itDonors)->copy());
			}

			kpdMatchMatrixRowCopy.push_back(donorMatchesCopy);
		}

		kpdMatchMatrixCopy.push_back(kpdMatchMatrixRowCopy);
	}

	return kpdMatchMatrixCopy;
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

std::string KPDRecord::getPopulationList() {

	std::stringstream population;

	//Iterates through each pair involved in the simulation and collects demographic information
	for (std::vector<KPDNode *>::iterator itNode = kpdNodes.begin(); itNode != kpdNodes.end(); itNode++){

		KPDNodeType type = (*itNode)->getType();
		int numberOfDonors = (*itNode)->getNumberOfDonors();

		for (int k = 1; k <= numberOfDonors; k++){
			population << currentIteration << ",";
			population << (*itNode)->getID() << ",";
			population << k << ",";
			population << KPDFunctions::nodeTypeToString(type) << ",";
			population << (*itNode)->getArrivalTime() << ",";
			population << (*itNode)->getCandidateString() << ",";
			//if (type == PAIR) {
				//population << candidateAssumedProbability << "," << candidateActualProbability << ",";
			//}
			//else {
				population << ",,";
			//}
			population << (*itNode)->getDonorString(k) << ",";
			//if (type == PAIR) {
				//population << donorAssumedProbability << "," << donorActualProbability;
			//}
			//else {
				//population << nddAssumedProbability << "," << nddActualProbability;
			//}
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