/* ---------------------------------------------
DD-Simulation.h
Organizes the steps of the simulation
---------------------------------------------- */

#ifndef SIMULATION_H
#define SIMULATION_H

#include "DD-Enums-Functions.h"
#include "DD-Node.h"
#include "DD-Match.h"
#include "DD-Arrangement.h"
#include "DD-Parameters.h"
#include "DD-Record.h"
#include "DD-MatchRun.h"

#include <vector>
#include <string>
#include <deque>
#include <iostream>
#include <sstream>
#include <fstream>

class KPDSimulation {

private:

	KPDParameters * kpdParameters;
	KPDData * kpdData;
	KPDRecord * kpdRecord;

	// Simulation Progress
	int currentIteration;
	int currentTime;
	
	std::vector<int> matchRunTimes;

	std::deque<KPDArrangement *> transplantQueue;

	// Deceased Donors Information
	std::deque<KPDDonor *> deceasedDonors;
	std::deque<int> deceasedDonorArrivalTimes;

	// Candidate Waitlist Information
	std::vector<KPDCandidate *> waitlistedCandidates;

	std::vector<KPDStatus> waitlistedCandidateStatus;
	std::vector<KPDTransplant> waitlistedCandidateTransplanted;

	std::vector<std::deque<KPDStatus> > waitlistedCandidateStateTransitions;
	std::vector<std::deque<int> > waitlistedCandidateStateTransitionTimes;
	
	// KPD Information
	std::vector<KPDNode *> kpdNodes;
	std::vector<KPDNodeType> kpdNodeTypes; // Needs to be separate! Gets updated over the course of the simulation!

	std::vector<KPDStatus> kpdNodeStatus;
	std::vector<KPDTransplant> kpdNodeTransplanted;

	std::vector<std::deque<KPDStatus> > kpdNodeStateTransitions;
	std::vector<std::deque<int> > kpdNodeStateTransitionTimes;

	// Crossmatch Information
	std::map<int, std::map<int, KPDMatch *> > deceasedDonorMatches;
	std::map<int, std::map<int, std::vector<KPDMatch*> > > waitlistedCandidateMatches;
	std::map<int, std::map<int, std::vector<KPDMatch*> > > kpdMatches;

	// Crossmatch Matrices
	std::vector<std::vector<bool> > kpdAdjacencyMatrix;
	std::vector<std::vector<bool> > kpdAdjacencyMatrixReduced; // No implicit edges back to NDDs

	std::vector<std::vector<bool> > kpdToWaitlistAdjacencyMatrix;
	
	// Helper Functions	
	int indexOfWaitlistedCandidate(int id);
	int indexOfKPDNode(int id);
	int getChild(int lower, int current, std::vector<int> &visitedVector, std::vector<std::vector<bool> > &adjacency);

	void updateStatus(int id, KPDStatus newState, bool waitlist);
	void updateFailedMatch(int donorNodeID, int candidateNodeID, int donorIndex, bool waitlist);

	void findDeceasedDonorMatches();
	void findWaitlistedCandidateMatches();

	int allocateDonor(KPDDonor * donor);

	void runStateTransitionStage();
	void runMatchRunStage();
	void runDeceasedDonorAllocationStage();
	void runTransplantationStage();
	
	// Output
	std::stringstream outputKPDResults;
	std::stringstream outputDDResults;
	std::stringstream outputWaitlistResults;

	std::stringstream outputKPDExchanges;
	std::stringstream outputDDExchanges;
	
	std::stringstream outputKPDPopulation;

	std::stringstream kpdSimulationLog;

public:

	KPDSimulation(KPDParameters * parameters);
	~KPDSimulation();

	// Simulation Function	
	void runIteration(int iteration);

	// Output Functions
	std::string getOutputKPDResults();
	std::string getOutputDDResults();

	std::string getOutputKPDExchanges();
	std::string getOutputDDExchanges();

	std::string getOutputKPDPopulation();

	void printLog();
};

KPDSimulation::KPDSimulation(KPDParameters * parameters) {
	
	kpdParameters = parameters;

	std::cout << "Collecting Data..." << std::endl;
	kpdData = new KPDData(kpdParameters);
	
	std::cout << "Preparing Record for Simulation..." << std::endl;
	kpdRecord = new KPDRecord(kpdData, kpdParameters);
		
	kpdSimulationLog << "Match Runs Occur at Times: ";

	int timeSimulation = kpdParameters->getTimeSimulation();
	int timeBetweenMatchRuns = kpdParameters->getTimeBetweenMatchRuns();

	int matchRunTime = timeBetweenMatchRuns;

	while (matchRunTime <= timeSimulation) {

		matchRunTimes.push_back(matchRunTime);
		kpdSimulationLog << " " << matchRunTime;

		matchRunTime += timeBetweenMatchRuns;
	}

	if (matchRunTimes.back() != timeSimulation) { // Final match run occurs at end of simulation, regardless of time since last match run

		matchRunTimes.push_back(timeSimulation);
		kpdSimulationLog << " " << timeSimulation;
	}

	kpdSimulationLog << std::endl;

	kpdSimulationLog << "Match Run Schedule Set!" << std::endl << std::endl;
}

KPDSimulation::~KPDSimulation() {

	printLog();

	delete kpdRecord;
	delete kpdData;
}

int KPDSimulation::indexOfWaitlistedCandidate(int id) {

	for (int i = 0; i < (int)waitlistedCandidates.size(); i++) {
		if (waitlistedCandidates[i]->getCandidateID() == id) {
			return i;
		}
	}

	return -1;

}

int KPDSimulation::indexOfKPDNode(int id) {

	for (int i = 0; i < (int)kpdNodes.size(); i++) {
		if (kpdNodes[i]->getID() == id) {
			return i;
		}
	}

	return -1;

}

int KPDSimulation::getChild(int lower, int current, std::vector<int> & visitedVector, std::vector<std::vector<bool> > & adjacency) {

	int nV = (int)visitedVector.size() - 1;
	for (int j = lower + 1; j <= nV; j++) {
		if (adjacency[current][j] == true && visitedVector[j] == 0)
			return j;
	}
	return -1;
}

void KPDSimulation::updateStatus(int index, KPDStatus newState, bool waitlist) {

	// Waitlist Candidates
	if (waitlist) {

		//Only consider candidates not already transplanted and that actually change state
		if (waitlistedCandidateTransplanted[index] != TRANSPLANT_YES && waitlistedCandidateStatus[index] != newState) {

			// Output status change
			kpdSimulationLog << currentTime << ": WL " << index << " " << KPDFunctions::statusToString(waitlistedCandidateStatus[index]) << " -> " << KPDFunctions::statusToString(newState) << std::endl;
			
			// Update status
			waitlistedCandidateStatus[index] = newState;
		}
	}

	// KPD Nodes
	else {

		//Only consider nodes not already transplanted and that actually change state; ignore bridge donors
		if (kpdNodeTransplanted[index] != TRANSPLANT_YES && kpdNodeStatus[index] != newState && kpdNodeTypes[index] != BRIDGE) {

			// Output status change
			kpdSimulationLog << currentTime << ": " << kpdNodes[index]->getID() << " " << KPDFunctions::statusToString(kpdNodeStatus[index]) << " -> " << KPDFunctions::statusToString(newState) << std::endl;

			// If transplants are in progress...
			if (kpdNodeTransplanted[index] == TRANSPLANT_IN_PROGRESS) {

				// If an inactive node in progress goes returns to the active state...
				if (newState == STATUS_ACTIVE) {

					// Output additional status change information
					kpdSimulationLog << " (Upon Return to KPD Pool)" << std::endl;
				}

				//... otherwise, set the node in progress as failed in all transplants in progress
				else {
					for (std::deque<KPDArrangement *>::iterator it = transplantQueue.begin(); it != transplantQueue.end(); it++) {
						(*it)->setFailure(index);
					}
				}
			}

			// Update status
			kpdNodeStatus[index] = newState;
		}
	}	
}

void KPDSimulation::updateFailedMatch(int donorNodeID, int candidateNodeID, int donorIndex, bool waitlist) {
			
	// KPD -> Waitlist
	if (waitlist) {

		// Get indices
		int donorNodeIndex = indexOfKPDNode(donorNodeID);
		int candidateNodeIndex = indexOfWaitlistedCandidate(candidateNodeID);

		// Update match adjacency
		waitlistedCandidateMatches[donorNodeID][candidateNodeID][donorIndex]->setAdjacency(false);

		// Check if adjacency matrix needs to be updated...
		bool noAssociatedDonors = true;

		for (std::vector<KPDMatch*>::iterator it = waitlistedCandidateMatches[donorNodeID][candidateNodeID].begin(); it != waitlistedCandidateMatches[donorNodeID][candidateNodeID].end(); it++){

			if ((*it)->getAdjacency()) {
				noAssociatedDonors = false;
				break;
			}
		}
		//... if no donors associated with donor node match with candidate, update adjacency matrix to false
		if (noAssociatedDonors) {
			kpdToWaitlistAdjacencyMatrix[donorNodeIndex + 1][candidateNodeIndex + 1] = false;
		}
	}
	
	// KPD -> KPD
	else {

		// Get indices
		int donorNodeIndex = indexOfKPDNode(donorNodeID);
		int candidateNodeIndex = indexOfKPDNode(candidateNodeID);

		// Update match adjacency
		kpdMatches[donorNodeID][candidateNodeID][donorIndex]->setAdjacency(false);

		// Check if adjacency matrix needs to be updated...
		bool noAssociatedDonors = true;

		for (std::vector<KPDMatch*>::iterator it = kpdMatches[donorNodeID][candidateNodeID].begin(); it != kpdMatches[donorNodeID][candidateNodeID].end(); it++) {
			if ((*it)->getAdjacency()) {
				noAssociatedDonors = false;
				break;
			}
		}
		//...if no donors associated with donor node match with candidate, update adjacency matrices to false
		if (noAssociatedDonors) {
			kpdAdjacencyMatrix[donorNodeIndex + 1][candidateNodeIndex + 1] = false;
			kpdAdjacencyMatrixReduced[donorNodeIndex + 1][candidateNodeIndex + 1] = false;
		}
	}	
}

void KPDSimulation::findDeceasedDonorMatches() {

	// Can this be moved to either the Data or Record classes?

	std::cout << "Assigning Deceased Donor Matches..." << std::endl;

	// Iterate through deceased donors
	for (std::deque<KPDDonor *>::iterator itDonor = deceasedDonors.begin(); itDonor != deceasedDonors.end(); itDonor++) {

		KPDDonor * deceasedDonor = *itDonor;
		
		int deceasedDonorID = deceasedDonor->getDonorID();

		// Crossmatch with each KPD pair
		for (std::vector<KPDNode *>::iterator itNode = kpdNodes.begin(); itNode != kpdNodes.end(); itNode++) {

			KPDNode * node = *itNode;

			// Only concerned with candidate nodes
			if (node->getType() == PAIR) {

				KPDCandidate * nodeCandidate = node->getCandidate();

				int candidateNodeID = node->getID();

				// Perform crossmatch
				KPDCrossmatch crossmatch = kpdData->performCrossmatch(nodeCandidate, deceasedDonor, false);

				// If the crossmatch is viable...
				if (kpdData->allowableMatch(crossmatch)) {

					// Generate the match properties associated with the match and store it
					KPDMatch * match = kpdRecord->generateMatch(nodeCandidate, deceasedDonor, crossmatch, false);

					deceasedDonorMatches[deceasedDonorID][candidateNodeID] = match;

				}
			}
		}
	}
}

void KPDSimulation::findWaitlistedCandidateMatches() {

	std::cout << "Assigning Waitlist Candidate Matches..." << std::endl;

	// Iterate through waitlist candidates
	for (std::vector<KPDCandidate *>::iterator itCandidate = waitlistedCandidates.begin(); itCandidate != waitlistedCandidates.end(); itCandidate++) {

		KPDCandidate * waitlistCandidate = *itCandidate;

		int waitlistCandidateID = waitlistCandidate->getCandidateID();

		// Crossmatch with each KPD node
		for (std::vector<KPDNode *>::iterator itNode = kpdNodes.begin(); itNode != kpdNodes.end(); itNode++) {

			KPDNode * node = *itNode;

			int donorNodeID = node->getID();

			int numberOfDonors = node->getNumberOfDonors();

			std::vector<KPDMatch *> matches;
			bool allowableMatchExists = false;

			// Iterate through donors
			for (int k = 1; k <= numberOfDonors; k++) {
				
				int donorIndex = k - 1;

				KPDDonor * donor = node->getDonor(donorIndex);

				// Perform crossmatch
				KPDCrossmatch crossmatch = kpdData->performCrossmatch(waitlistCandidate, donor, true);
				
				// If the crossmatch is viable
				if (kpdData->allowableMatch(crossmatch)) {

					// Generate the match properties and add match to list
					KPDMatch * match = kpdRecord->generateMatch(waitlistCandidate, donor, crossmatch, true);
					allowableMatchExists = true;
					
					matches.push_back(match);

				}
				else {

					// Generate blank match and add match to list
					KPDMatch *noMatch = new KPDMatch();
					noMatch->setVirtualCrossmatchResult(crossmatch);

					matches.push_back(noMatch);

				}
			}
			
			// If a match exists between the donor and waitlist candidate...
			if (allowableMatchExists) {
				// then store the list of matches
				
				//std::cout << donorNodeID << "->" << waitlistCandidateID << std::endl;

				waitlistedCandidateMatches[donorNodeID][waitlistCandidateID] = matches;
			}
		}		
	}
}

int KPDSimulation::allocateDonor(KPDDonor * donor) {

	// Function is called when a deceased donor is not allocated to their real-life candidate
	// This will have to be enhanced in order to follow the allocation protocols listed in the files

	// Iterate through candidates
	for (int i = 1; i <= waitlistedCandidates.size(); i++) {

		int candidateIndex = i - 1;
		KPDCandidate * candidate = waitlistedCandidates[candidateIndex];

		// If the candidate is active and not transplanted...
		if (waitlistedCandidateStatus[candidateIndex] == STATUS_ACTIVE &&
			waitlistedCandidateStatus[candidateIndex] == TRANSPLANT_NO) {
				
			// Perform a crossmatch			
			KPDCrossmatch crossmatch = kpdData->performCrossmatch(candidate, donor, true);

			// If the crossmatch passes, return the index of the candidate as the allocation
			if (kpdData->allowableMatch(crossmatch)) {
				return candidateIndex;
			}
		}
	}

	// If no allocation is found, return -1
	return -1;
}


void KPDSimulation::runStateTransitionStage() {

	// Iterate through waitlist candidates
	for (int i = 1; i <= (int) waitlistedCandidateStateTransitionTimes.size(); i++) {

		int candidateIndex = i - 1;
		
		// While current candidate has state transitions to process...
		while (waitlistedCandidateStateTransitionTimes[candidateIndex].size() > 0) {
			
			// If there is a state transition at the current time...
			if (waitlistedCandidateStateTransitionTimes[candidateIndex].front() == currentTime) {

				// Get the new state
				KPDStatus newState = waitlistedCandidateStateTransitions[candidateIndex].front();

				// Update the state
				updateStatus(candidateIndex, newState, true);

				// Pop out state transition information
				waitlistedCandidateStateTransitions[candidateIndex].pop_front();
				waitlistedCandidateStateTransitionTimes[candidateIndex].pop_front();
			}
			//...otherwise, quit evaluating the current candidate
			else {
				break;
			}
		}	
	}
	
	// Iterate through KPD nodes
	for (int i = 1; i <= (int) kpdNodeStateTransitionTimes.size(); i++) {

		int nodeIndex = i - 1;
	
		// While current node has state transitions to proces...
		while (kpdNodeStateTransitionTimes[nodeIndex].size() > 0) {
			
			// If there is a state transition at the current time...
			if (kpdNodeStateTransitionTimes[nodeIndex].front() == currentTime) {

				// Get the new state
				KPDStatus newState = kpdNodeStateTransitions[nodeIndex].front();

				// Update the state
				updateStatus(nodeIndex, newState, false);

				// Pop out state transition information
				kpdNodeStateTransitions[nodeIndex].pop_front();
				kpdNodeStateTransitionTimes[nodeIndex].pop_front();
			}
			//...otherwise, quit evaluating the current node
			else {
				break;
			}
		}
	}	
}

void KPDSimulation::runMatchRunStage() {
	
	std::cout << "Match Run at Time " << currentTime << std::endl;

	// Set up match run
	std::vector<std::vector<int> > matchRunArrangements;
	std::vector<double> assignedValueOfMatchRunArrangements;

	KPDMatchRun * matchRun = new KPDMatchRun(kpdParameters, currentIteration, currentTime,
		kpdNodes, kpdNodeTypes, kpdNodeStatus, kpdNodeTransplanted, kpdMatches);
	
	// Find all the LRSs in the current pool
	matchRun->collectCyclesAndChainsForCurrentMatchRun(matchRunArrangements);
	std::cout << "# of Cycles and Chains = " << matchRunArrangements.size() << std::endl;
	
	// If there are arrangements in the pool...
	if (matchRunArrangements.size() > 0) {
		
		// Assign the appropriate utility values to the LRSs
		matchRun->assignExpectedUtilitiesForCurrentMatchRun(matchRunArrangements, assignedValueOfMatchRunArrangements); 
		std::cout << "Utilities Assigned" << std::endl;

		// Select optimal set of LRSs
		std::vector<int> optimalSolution;

		matchRun->getOptimalSolutionForCurrentMatchRun(optimalSolution, matchRunArrangements, assignedValueOfMatchRunArrangements);

		kpdSimulationLog << matchRun->printLog() << std::endl;

		// Add Selected Arrangements to Those in Progress
		for (int i = 1; i <= matchRunArrangements.size(); i++) {

			int arrangementIndex = i - 1;

			std::vector<int> currentArrangement = matchRunArrangements[arrangementIndex];

			// Retrieve NDD Status
			bool ndd = false;
			for (std::vector<int>::iterator nodeIt = currentArrangement.begin(); nodeIt != currentArrangement.end(); nodeIt++) {

				int nodeID = *nodeIt;
				int nodeIndex = indexOfKPDNode(nodeID);

				if (kpdNodeTypes[nodeIndex] != PAIR) {
					ndd = true;
					break;
				}
			}

			// Retrieve Utility
			double arrangementUtility = assignedValueOfMatchRunArrangements[arrangementIndex];

			// Was This Arrangement Selected?
			bool arrangementIsPartOfOptimalSolution = false;

			for (std::vector<int>::iterator optimalSolutionsIt = optimalSolution.begin(); optimalSolutionsIt != optimalSolution.end(); optimalSolutionsIt++) {
				if (*optimalSolutionsIt == arrangementIndex) {
					arrangementIsPartOfOptimalSolution = true;
					break;
				}
			}

			// If so....
			if (arrangementIsPartOfOptimalSolution) {

				std::vector<KPDNode *> arrangementNodes;
				std::vector<KPDNodeType> arrangementNodeTypes;

				//Collect All Nodes and Node Types from the Arrangment
				for (std::vector<int>::iterator nodeIt = currentArrangement.begin(); nodeIt != currentArrangement.end(); nodeIt++) {

					int nodeID = *nodeIt;
					int nodeIndex = indexOfKPDNode(nodeID);

					KPDNode * node = kpdNodes[nodeIndex];

					arrangementNodes.push_back(node->copy()); // Copy Arrangement Nodes				
					arrangementNodeTypes.push_back(kpdNodeTypes[nodeIndex]);

					kpdNodeTransplanted[nodeIndex] = TRANSPLANT_IN_PROGRESS; // Mark Nodes in Arrangement as TRANSPLANT_IN_PROGRESS
				}

				//Create New Arrangement Object and Add to Those In Progress
				int transplantationTime = currentTime + kpdParameters->getTimeBetweenSelectionAndTransplantation();

				KPDArrangement * newArrangement = new KPDArrangement(currentIteration, currentTime, transplantationTime, 
					arrangementUtility,	arrangementNodes, arrangementNodeTypes, kpdMatches);

				transplantQueue.push_back(newArrangement);

				//Output to Simulation Log
				if (newArrangement->hasNDD()) { kpdSimulationLog << "Chain "; }
				else { kpdSimulationLog << "Cycle "; }
				kpdSimulationLog << newArrangement->toLabel() << " Queued" << std::endl;
			}

			//Output to Arrangement List
			outputKPDExchanges << currentIteration << "," << currentTime << "," << kpdParameters->getTimeBetweenSelectionAndTransplantation() << "," << currentArrangement.size() << ",";
			
			outputKPDExchanges << *currentArrangement.begin();
			for (std::vector<int>::iterator it = currentArrangement.begin() + 1; it != currentArrangement.end(); it++) {
				outputKPDExchanges << ";" << *it;
			}

			outputKPDExchanges << "," << KPDFunctions::boolToYesNo(ndd) << "," << arrangementUtility << ",";
			outputKPDExchanges << KPDFunctions::boolToYesNo(arrangementIsPartOfOptimalSolution) << std::endl;

			//i++;
		}
	}
	else {
		kpdSimulationLog << matchRun->printLog() << std::endl;
	}

	std::cout << std::endl;

	delete matchRun;
}

void KPDSimulation::runDeceasedDonorAllocationStage() {

	while (deceasedDonorArrivalTimes.front() == currentTime) {

		// Deceased donor arrives

		KPDDonor * deceasedDonor = deceasedDonors.front();
		int deceasedDonorID = deceasedDonor->getDonorID();

		kpdSimulationLog << "Allocation of Deceased Donor " << deceasedDonorID << " at Time " << currentTime << std::endl;

		deceasedDonors.pop_front();
		deceasedDonorArrivalTimes.pop_front();

		int kidneysToWaitlist = 1;

		// Is the deceased donor organ eligible to match to KPD? In other words, are both kidneys available, and is the KDPI between 20% and 35%?
		
		if (deceasedDonor->getBothKidneysAvailable()) {

			if (deceasedDonor->getKDPI() > 20 && deceasedDonor->getKDPI() <= 35) {

				std::map<int, KPDMatch *> matchesInKPD = deceasedDonorMatches[deceasedDonorID];

				std::vector<int> matchedNodesInKPD;

				// Does the deceased donor match with an eligible candidate in the KPD?
				// Iterate through all possible matches
				for (std::map<int, KPDMatch*>::iterator it = matchesInKPD.begin(); it != matchesInKPD.end(); it++) {

					int kpdNodeID = it->first;
					int kpdNodeIndex = indexOfKPDNode(kpdNodeID);

					KPDMatch * matchToKPD = it->second;

					//If KPD candidate is not transplanted and is in active status and if match is still considered possible
					if (kpdNodeTransplanted[kpdNodeIndex] == TRANSPLANT_NO &&
						kpdNodeStatus[kpdNodeIndex] == STATUS_ACTIVE &&
						matchToKPD->getAdjacency()) {

						double kpdNodePRA = kpdNodes[kpdNodeIndex]->getCandidatePRA();

						// If additional requirements for node to be involved in DD chain are satisfied
						if (kpdParameters->getPRAEligibilityMin() <= kpdNodePRA &&
							kpdParameters->getPRAEligibilityMax() >= kpdNodePRA) {

							matchedNodesInKPD.push_back(kpdNodeIndex);
						}
					}
				}

				int numberOfMatchedNodesInKPD = (int)matchedNodesInKPD.size();

				if (numberOfMatchedNodesInKPD > 0) {

					// Are there eligible candidates for this deceased donor on the waitlist?
					// Is there a priority candidate in the waitlist that must be transplanted first?

					std::vector<int> eligibleWaitlistCandidates;
					bool priorityCandidateFound = false;

					int deceasedDonorOPO = deceasedDonor->getOPO();
					KPDBloodType deceasedDonorBT = deceasedDonor->getBT();

					std::vector<std::string> deceasedDonorHLA = deceasedDonor->getHLA();

					// Iterate through waitlist to find eligible candidates
					for (std::vector<KPDCandidate *>::iterator it = waitlistedCandidates.begin(); it != waitlistedCandidates.end(); it++) {

						KPDCandidate * waitlistCandidate = *it;
						int waitlistCandidateID = waitlistCandidate->getCandidateID();
						int waitlistCandidateIndex = indexOfWaitlistedCandidate(waitlistCandidateID);

						// Eligible and priority candidates need to be active, untransplanted...
						if (waitlistedCandidateTransplanted[waitlistCandidateIndex] == TRANSPLANT_NO &&
							waitlistedCandidateStatus[waitlistCandidateIndex] == STATUS_ACTIVE) {

							//Eligible candidates must be in the same DSA, have EPTS priority, and be of the same blood type as the donor
							int waitlistCandidateOPO = waitlistCandidate->getOPO();
							KPDBloodType waitlistCandidateBT = waitlistCandidate->getBT();
							bool waitlistCandidateEPTSPriority = waitlistCandidate->getEPTSPriority();

							if (deceasedDonorOPO == waitlistCandidateOPO && waitlistCandidateEPTSPriority && deceasedDonorBT == waitlistCandidateBT) {
								eligibleWaitlistCandidates.push_back(waitlistCandidateIndex);
							}

							KPDCrossmatch crossmatch = kpdData->performCrossmatch(waitlistCandidate, deceasedDonor, true);

							//Priority candidates must match with the donor
							if (kpdData->allowableMatch(crossmatch)) {

								int waitlistCandidatePRA = waitlistCandidate->getPRA();
								//int waitlistCandidatePrevTrans = waitlistCandidate->getPrevTrans();
								int waitlistCandidateAge = waitlistCandidate->getAge();

								std::vector<std::string> waitlistCandidateHLA = waitlistCandidate->getHLA();

								bool mismatch = false;
								for (std::vector<std::string>::iterator dHLAIt = deceasedDonorHLA.begin(); dHLAIt != deceasedDonorHLA.end(); dHLAIt++) {
									for (std::vector<std::string>::iterator cHLAIt = waitlistCandidateHLA.begin(); cHLAIt != waitlistCandidateHLA.end(); cHLAIt++) {
										if (*dHLAIt == *cHLAIt) {
											mismatch = true;
											break;
										}
									}

									if (mismatch) {
										break;
									}
								}

								if (waitlistCandidatePRA == 100 ||
									waitlistCandidatePRA == 99 ||
									waitlistCandidatePRA == 98 && waitlistCandidateOPO == deceasedDonorOPO ||
									!mismatch ||
									//waitlistCandidatePrevTrans ||
									(waitlistCandidateAge <= 17 && waitlistCandidateOPO == deceasedDonorOPO)) {

									priorityCandidateFound = true;
									break;
								}
							}
						}
					}

					if (priorityCandidateFound) {

						kpdSimulationLog << "Priority Candidate Found; Allocation Direct to Waitlist" << std::endl;

						kidneysToWaitlist = 2;
					}

					else if (eligibleWaitlistCandidates.size() == 0) {

						kpdSimulationLog << "No Eligible Candidates on Waitlist for Chain Transplantation; Allocation Direct to Waitlist" << std::endl;

						kidneysToWaitlist = 2;
					}

					else {

						// Select best possible KPD Donor to Waitlist Candidate Match
						int selectedKPDNodeID = -1;
						int selectedKPDNodeIndex = -1;
						int selectedDonorIndex = -1;
						int selectedWaitlistCandidateID = -1;
						int selectedWaitlistCandidateIndex = -1;
						double optnScore = -1;

						for (std::vector<int>::iterator wlIt = eligibleWaitlistCandidates.begin(); wlIt != eligibleWaitlistCandidates.end(); wlIt++) {

							double candidateBasePoints = 0.0;

							int candidateIndex = *wlIt;
							KPDCandidate * candidate = waitlistedCandidates[candidateIndex];
							int candidateID = candidate->getCandidateID();

							int candidatePRA = candidate->getPRA();

							candidateBasePoints += (1 / 365)*(currentTime - candidate->getListingTime());
							if (candidate->getPrevTrans()) {
								candidateBasePoints += 1;
							}
							if (candidatePRA >= 20 && candidatePRA <= 29) {
								candidateBasePoints += 0.08;
							}
							else if (candidatePRA >= 30 && candidatePRA <= 39) {
								candidateBasePoints += 0.21;
							}
							else if (candidatePRA >= 40 && candidatePRA <= 49) {
								candidateBasePoints += 0.34;
							}
							else if (candidatePRA >= 50 && candidatePRA <= 59) {
								candidateBasePoints += 0.48;
							}
							else if (candidatePRA >= 60 && candidatePRA <= 69) {
								candidateBasePoints += 0.81;
							}
							else if (candidatePRA >= 70 && candidatePRA <= 74) {
								candidateBasePoints += 1.09;
							}
							else if (candidatePRA >= 75 && candidatePRA <= 79) {
								candidateBasePoints += 1.58;
							}
							else if (candidatePRA >= 80 && candidatePRA <= 84) {
								candidateBasePoints += 2.46;
							}
							else if (candidatePRA >= 85 && candidatePRA <= 89) {
								candidateBasePoints += 4.05;
							}
							else if (candidatePRA >= 90 && candidatePRA <= 94) {
								candidateBasePoints += 6.71;
							}
							else if (candidatePRA == 95) {
								candidateBasePoints += 10.82;
							}
							else if (candidatePRA == 96) {
								candidateBasePoints += 12.17;
							}
							else if (candidatePRA == 97) {
								candidateBasePoints += 17.30;
							}
							else if (candidatePRA == 98) {
								candidateBasePoints += 24.40;
							}
							else if (candidatePRA == 99) {
								candidateBasePoints += 50.09;
							}
							else if (candidatePRA == 100) {
								candidateBasePoints += 202.10;
							}

							for (std::vector<int>::iterator kpdIt = matchedNodesInKPD.begin(); kpdIt != matchedNodesInKPD.end(); kpdIt++) {

								int kpdNodeIndex = *kpdIt;
								KPDNode * kpdNode = kpdNodes[kpdNodeIndex];
								int kpdNodeID = kpdNode->getID();

								if (waitlistedCandidateMatches[kpdNodeID][candidateID].size() > 0) {

									int numberOfDonors = kpdNode->getNumberOfDonors();

									for (int k = 1; k <= numberOfDonors; k++) {

										int donorIndex = k - 1;

										if (waitlistedCandidateMatches[kpdNodeID][candidateID][donorIndex]->getAdjacency()) {

											std::vector<std::string> donorHLA = kpdNode->getDonor(donorIndex)->getHLA();
											std::vector<std::string> candidateHLA = candidate->getHLA();

											int mismatches = 0;
											for (std::vector<std::string>::iterator dHLAIt = donorHLA.begin(); dHLAIt != donorHLA.end(); dHLAIt++) {
												for (std::vector<std::string>::iterator cHLAIt = candidateHLA.begin(); cHLAIt != candidateHLA.end(); cHLAIt++) {
													if (*dHLAIt == *cHLAIt) {
														mismatches++;
													}
												}
											}
											
											double totalPoints = candidateBasePoints;

											if (candidate->getAge() <= 10 && mismatches == 0) {
												totalPoints += 4;
											}
											else if (candidate->getAge() >= 11 && candidate->getAge() <= 17 && mismatches == 0) {
												totalPoints += 3;
											}
											else if (candidate->getAge() <= 10 && kpdNode->getDonor(donorIndex)->getKDPI() < 35) {
												totalPoints += 1;
											}
											else if (mismatches == 1) {
												totalPoints += 1;
											}
											else if (mismatches == 0) {
												totalPoints += 2;
											}

											if (totalPoints > optnScore) {
												selectedKPDNodeID = kpdNodeID;
												selectedKPDNodeIndex = kpdNodeIndex;
												selectedDonorIndex = donorIndex;
												selectedWaitlistCandidateID = candidateID;
												selectedWaitlistCandidateIndex = candidateIndex;
											}
										}
									}
								}
							}
						}

						// If a chain is possible, as evidenced by a positive OPTN score
						if (optnScore > 0) {
							
							KPDMatch * deceasedDonorToKPDMatch = deceasedDonorMatches[deceasedDonorID][selectedKPDNodeID];
							
							if (!deceasedDonorToKPDMatch->getSuccessfulMatch()) {

								kpdSimulationLog << "Deceased Donor to KPD Transplant Unviable; Allocation Direct to Waitlist" << std::endl;

								kidneysToWaitlist = 2;
							}

							else {

								KPDMatch * kpdToWaitlistMatch = waitlistedCandidateMatches[selectedKPDNodeID][selectedWaitlistCandidateID][selectedDonorIndex];

								kpdNodeTransplanted[indexOfKPDNode(selectedKPDNodeID)] = TRANSPLANT_YES;

								if (!kpdToWaitlistMatch->getSuccessfulMatch()) {

									kpdSimulationLog << "KPD Transplant to Waitlist Refused; Re-Allocation of KPD Donor to Waitlist" << std::endl;

									int allocatedCandidateIndex = allocateDonor(kpdNodes[selectedKPDNodeIndex]->getDonor(selectedDonorIndex));

									waitlistedCandidateTransplanted[allocatedCandidateIndex] = TRANSPLANT_YES;

								}

								else {

									kpdSimulationLog <<  "Successful Chain!" << std::endl;

									waitlistedCandidateTransplanted[selectedWaitlistCandidateIndex] = TRANSPLANT_YES;

								}
							}

						}

						else {

							kpdSimulationLog << "No Possible Chains Through KPD; Allocation Direct to Waitlist" << std::endl;

							kidneysToWaitlist = 2;
						}
					}
				}

				else {

					kpdSimulationLog << "No Compatible Pairs in KPD Pool; Allocation Direct to Waitlist" << std::endl;

					kidneysToWaitlist = 2;
				}
			}

			else {

				kpdSimulationLog << "Deceased Donor not Eligible for Transplant through KPD; Allocation Direct to Waitlist" << std::endl;

				kidneysToWaitlist = 2;
			}
		}

		else {
			kpdSimulationLog << "Only 1 Donor Organ Available; Allocation Direct to Waitlist" << std::endl;
		}

		// Remaining kidneys gets allocated to deceased donor list

		std::vector<int> realMatchedCandidateIDs = kpdData->getActualTransplantedCandidates(deceasedDonorID); // Need function to get matched candidates

		for (int i = 1; i <= kidneysToWaitlist; i++) {

			bool foundWaitlistCandidate = false;

			for (std::vector<int>::iterator it = realMatchedCandidateIDs.begin(); it != realMatchedCandidateIDs.end(); it++) {

				int realMatchedCandidateIndex = indexOfWaitlistedCandidate(*it);

				if (waitlistedCandidateTransplanted[realMatchedCandidateIndex] == TRANSPLANT_NO) {
					waitlistedCandidateTransplanted[realMatchedCandidateIndex] = TRANSPLANT_YES;
					foundWaitlistCandidate = true;
				}
			}

			if (!foundWaitlistCandidate) {
				int simulatedMatchedCandidateIndex = allocateDonor(deceasedDonor);
				waitlistedCandidateTransplanted[simulatedMatchedCandidateIndex] = TRANSPLANT_YES;
			}
		}
	}

}


void KPDSimulation::runTransplantationStage() {
	
	//If there are transplant arrangements waiting...
	if (transplantQueue.size() > 0) {
		// .. and if a transplant arrangement has reached its transplantation time, then... 
		if (transplantQueue.front()->getTransplantationTime() <= currentTime) {

			//Get Transplant Arrangement Information and Output to File
			KPDArrangement * transplantArrangement = transplantQueue.front();
			std::vector<int> nodesInProgress = transplantArrangement->getNodeIDs();

			bool chain = transplantArrangement->hasNDD();

			kpdSimulationLog << transplantArrangement->getTransplantationTime() << ": TX - " << transplantArrangement->toLabel() << std::endl << std::endl;

			//Update Matrices to Reflect Revealed Crossmatches
			for (std::vector<int>::iterator itCandidateNode = nodesInProgress.begin(); itCandidateNode != nodesInProgress.end(); ++itCandidateNode) {

				int candidateNodeID = *itCandidateNode;
				int candidateNodeIndex = indexOfKPDNode(candidateNodeID);

				if (kpdNodeTypes[candidateNodeIndex] == PAIR) {
					for (std::vector<int>::iterator itDonorNode = nodesInProgress.begin(); itDonorNode != nodesInProgress.end(); ++itDonorNode) {

						int donorNodeID = *itDonorNode;
						int donorNodeIndex = indexOfKPDNode(donorNodeID);

						//...iterate through all donors...
						for (int k = 1; k <= kpdNodes[donorNodeIndex]->getNumberOfDonors(); k++) {

							int donorIndex = k - 1;

							//Only consider relevant exchanges within the transplant arrangement
							if (transplantArrangement->getAdjacency(donorNodeID, candidateNodeID, donorIndex)) {

								//Output availability of the nodes
								kpdSimulationLog << "Donor Node " << donorNodeID;
								if (!transplantArrangement->getAvailability(donorNodeID)) {
									kpdSimulationLog << " not";
								}
								kpdSimulationLog << " Available" << std::endl << std::endl;


								kpdSimulationLog << "Candidate Node " << candidateNodeID;
								if (!transplantArrangement->getAvailability(candidateNodeID)) {
									kpdSimulationLog << " not";
								}
								kpdSimulationLog << " Available" << std::endl << std::endl;

								//Reveal results of the lab crossmatch...
								if (kpdMatches[donorNodeID][candidateNodeID][donorIndex]->getSuccessfulMatch()) {
									kpdSimulationLog << "Match " << donorNodeID << "[" << k << "] -> " << candidateNodeID << " Successful (-> 1)" << std::endl;
									if (kpdMatches[donorNodeID][candidateNodeID][donorIndex]->getAssumedSuccessProbability() != 1.0) {
										kpdMatches[donorNodeID][candidateNodeID][donorIndex]->setAssumedSuccessProbability(1.0); // Probability updated to reflect successful crossmatch
									}
								}
								else {
									kpdSimulationLog << "Match " << donorNodeID << "[" << k << "] -> " << candidateNodeID << " Failed (Removed)" << std::endl;
									updateFailedMatch(donorNodeID, candidateNodeID, donorIndex, false); // Remove edge from KPD graph to reflect unsuccessful crossmatch
								}
							}
						}
					}					
				}
			}

			// Find Best Transplant Option(s) Remaining in Arrangement
			std::vector<std::vector<int> > options;
			transplantArrangement->getOptimalSolutionForFallBackOptions(options, kpdParameters->getMaxCycleSize(), kpdParameters->getMaxChainLength(), kpdParameters->getAllowABBridgeDonors(), kpdParameters->getUtilityScheme());

			if (options.size() == 0) {
				kpdSimulationLog << "No Options" << std::endl;
			}
			else {

				//Iterate through options:
				for (std::vector<std::vector<int> >::iterator itOptions = options.begin(); itOptions != options.end(); itOptions++) {

					std::vector<int> currentOption = *itOptions;

					bool chain = kpdNodeTypes[indexOfKPDNode(*currentOption.begin())] != PAIR;
					int length = (int)currentOption.size();

					//Realized Chain
					if (chain) {
						length--;
						
						//Iterate through nodes and mark transplants
						for (std::vector<int >::iterator itNodes = currentOption.begin(); itNodes != currentOption.end() - 1; ++itNodes) {

							int donorNodeID = *itNodes;
							int candidateNodeID = *(itNodes + 1);

							int selectedDonor = transplantArrangement->selectDonor(donorNodeID, candidateNodeID, kpdParameters->getUtilityScheme(), true); // Select Donor
							
							kpdNodeTransplanted[indexOfKPDNode(donorNodeID)] = TRANSPLANT_YES; // Update Transplantation Status of Donor Node

							kpdSimulationLog << "TX: " << donorNodeID << "[" << selectedDonor << "] -> " << candidateNodeID << " Recorded" << std::endl; // Output
						}

						//Set up bridge node and output
						int bridgeNodeID = *(currentOption.end() - 1);
						int bridgeNodeIndex = indexOfKPDNode(bridgeNodeID);

						kpdNodeTypes[bridgeNodeIndex] = BRIDGE;
						kpdNodeTransplanted[bridgeNodeIndex] = TRANSPLANT_NO;
						
						//Correct bridge donor
						for (int i = 1; i <= (int)kpdNodes.size(); i++) {
							
							int nodeIndex = i - 1;

							if (nodeIndex != bridgeNodeIndex) {
								//Remove matches to new bridge donor node
								if (kpdAdjacencyMatrix[i][bridgeNodeIndex + 1]) {
									for (int k = 1; k <= kpdNodes[i]->getNumberOfDonors(); k++) {

										int donorIndex = k - 1;
										updateFailedMatch(kpdNodes[nodeIndex]->getID(), bridgeNodeID, donorIndex, false);
									}
								}

								//Remove implict matches to all altruistic or bridge donors
								if (kpdAdjacencyMatrix[bridgeNodeIndex + 1][i] && kpdNodeTypes[nodeIndex] != PAIR) {
									for (int k = 1; k <= kpdNodes[bridgeNodeIndex]->getNumberOfDonors(); k++) { /// CHECK THIS AGAIN!

										int donorIndex = k - 1;
										updateFailedMatch(bridgeNodeID, kpdNodes[nodeIndex]->getID(), donorIndex, false);
									}
								}

								//Add (implicit) matches from all pairs to new bridge donor node
								if (kpdNodeTypes[nodeIndex] == PAIR) {
									kpdAdjacencyMatrix[i][bridgeNodeIndex + 1] = true;
								}
							}
						}
						
						kpdSimulationLog << "Donors " << bridgeNodeID << " now BD, returned to pool" << std::endl;
					}

					//Realized Cycle
					else {

						//Get information from realized cycle
						for (std::vector<int >::iterator itNodes = currentOption.begin(); itNodes != currentOption.end() - 1; ++itNodes) {

							int donorNodeID = *itNodes;
							int candidateNodeID = *(itNodes + 1);

							int selectedDonor = transplantArrangement->selectDonor(donorNodeID, candidateNodeID, kpdParameters->getUtilityScheme(), true); // Select Donor

							kpdNodeTransplanted[indexOfKPDNode(candidateNodeID)] = TRANSPLANT_YES; // Update Transplantation Status of Candidate Node

							kpdSimulationLog << "Transplant " << donorNodeID << "[" << selectedDonor << "] -> " << candidateNodeID << " Recorded" << std::endl;  //Output							
						}

						int donorNodeID = *(currentOption.end() - 1);
						int candidateNodeID = *(currentOption.begin());

						int selectedDonor = transplantArrangement->selectDonor(donorNodeID, candidateNodeID, kpdParameters->getUtilityScheme(), true); // Final Transplant

						kpdNodeTransplanted[indexOfKPDNode(candidateNodeID)] = TRANSPLANT_YES; // Update Transplantation Status of Candidate Node

						kpdSimulationLog << "Transplant " << donorNodeID << "[" << selectedDonor << "] -> " << candidateNodeID << " Recorded" << std::endl; //Output

					}
				}
			}

			//Return All Untransplanted Nodes to Pool
			for (std::vector<int>::iterator it = nodesInProgress.begin(); it != nodesInProgress.end(); it++) {

				int nodeIndex = indexOfKPDNode(*it);

				if (kpdNodeTransplanted[nodeIndex] == TRANSPLANT_IN_PROGRESS) {
					kpdNodeTransplanted[nodeIndex] = TRANSPLANT_NO;
					kpdSimulationLog << KPDFunctions::nodeTypeToString(kpdNodeTypes[nodeIndex]) << " " << *it << " returned ";					
				}
			}

			//Print Transplant Information to File
			outputKPDResults << transplantArrangement->toTransplantString();

			transplantQueue.pop_front();

			runTransplantationStage(); // Check for another transplant
		}
	}
}

void KPDSimulation::runIteration(int iteration) {

	// Set current iteration
	currentIteration = iteration;

	std::cout << "--------------" << std::endl;
	std::cout << "Iteration " << currentIteration << std::endl;
	std::cout << "--------------" << std::endl << std::endl;

	kpdSimulationLog << "--------------" << std::endl;
	kpdSimulationLog << "Iteration " << currentIteration << std::endl;
	kpdSimulationLog << "--------------" << std::endl << std::endl;
	
	// Generate new KPD pool for this iteration
	kpdRecord->generateSimulationData(iteration, matchRunTimes);

	// Output KPD pool population
	outputKPDPopulation.str("");
	outputKPDPopulation << kpdRecord->getPopulationList();

	// Reset information
	currentTime = 0;

	transplantQueue.clear();

	deceasedDonors = kpdData->getDeceasedDonors();
	deceasedDonorArrivalTimes = kpdData->getDeceasedDonorArrivalTimes();

	waitlistedCandidates = kpdData->getWaitlistedCandidates();

	waitlistedCandidateStatus.assign(waitlistedCandidates.size(), STATUS_INACTIVE);
	waitlistedCandidateTransplanted.assign(waitlistedCandidates.size(), TRANSPLANT_NO);

	waitlistedCandidateStateTransitions = kpdData->getWaitlistCandidatesStateTransitionMatrix();
	waitlistedCandidateStateTransitionTimes = kpdData->getWaitlistCandidatesStateTransitionTimeMatrix();

	kpdNodes = kpdRecord->getNodes();
	kpdNodeTypes = kpdRecord->getNodeTypes();

	kpdNodeStatus.assign(kpdNodes.size(), STATUS_INACTIVE);
	kpdNodeTransplanted.assign(kpdNodes.size(), TRANSPLANT_NO);

	kpdNodeStateTransitions = kpdRecord->getKPDNodeStateTransitionMatrix();
	kpdNodeStateTransitionTimes = kpdRecord->getKPDNodeStateTransitionTimeMatrix();

	deceasedDonorMatches.clear();
	findDeceasedDonorMatches(); // Collect the matches between the deceased donors and KPD

	waitlistedCandidateMatches.clear();
	findWaitlistedCandidateMatches(); // Collect the matches between the KPD and waitlist candidates

	kpdMatches = kpdRecord->getKPDMatches();

	kpdAdjacencyMatrix = kpdRecord->getAdjacencyMatrix();
	kpdAdjacencyMatrixReduced = kpdRecord->getAdjacencyMatrixReduced();

	kpdToWaitlistAdjacencyMatrix.assign(1 + (int)kpdNodes.size(), std::vector<bool>(1 + (int)waitlistedCandidates.size(), false)); // Blank matrix
	

	// Clear output streams
	outputKPDResults.str("");
	outputDDResults.str("");
	outputWaitlistResults.str("");

	outputKPDExchanges.str("");
	outputDDExchanges.str("");

	kpdSimulationLog.str("");

	int timeSimulation = kpdParameters->getTimeSimulation();
	int postSelectionInactivePeriod = kpdParameters->getTimeBetweenSelectionAndTransplantation();

	std::vector<int>::iterator it = matchRunTimes.begin();

	int matchRunTime = *it;
	it++;

	runStateTransitionStage(); // For initial KPD

	while (currentTime < timeSimulation) {

		currentTime++;

		// Update all state changes at new time
		runStateTransitionStage();

		// Perform match run
		if (currentTime == matchRunTime) {
			runMatchRunStage();

			if (it != matchRunTimes.end()) {
				matchRunTime = *it;
				it++;
			}
		}

		// Check for deceased donors
		runDeceasedDonorAllocationStage();

		// Perform transplantations
		runTransplantationStage();
	}

	// Remaining state transitions and final transplantation stage
	while (currentTime < timeSimulation + postSelectionInactivePeriod) {

		currentTime++;

		runStateTransitionStage();
		runTransplantationStage();
	}
}

std::string KPDSimulation::getOutputKPDResults() {
	return outputKPDResults.str();
}

std::string KPDSimulation::getOutputDDResults() {
	return outputDDResults.str();
}

std::string KPDSimulation::getOutputKPDExchanges() {
	return outputKPDExchanges.str();
}

std::string KPDSimulation::getOutputDDExchanges() {
	return outputDDExchanges.str();
}

std::string KPDSimulation::getOutputKPDPopulation() {
	return outputKPDPopulation.str();
}

void KPDSimulation::printLog() {

	std::string logFile = "output/" + kpdParameters->getOutputFolder() + "/" + kpdParameters->getSubFolder() + "/Log-Simulation.txt";
	std::ofstream outputFileLog(logFile.c_str());
	outputFileLog << kpdSimulationLog.str();
	outputFileLog.close();
}

#endif
