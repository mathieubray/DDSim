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
	std::vector<int> waitlistedCandidateIDs;

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
	int indexOf(int id);
	int getChild(int lower, int current, std::vector<int> &visitedVector, std::vector<std::vector<bool> > &adjacency);
	void updateStatus(int id, KPDStatus newState, bool waitlistCandidate);
	void updateFailedMatch(int donorNodeID, int candidateNodeID, int donorID, bool waitlistCandidate);

	void findDeceasedDonorMatches();
	void findWaitlistedCandidateMatches();

	int allocateDeceasedDonor(KPDDonor * donor);

	void runStateTransitionStage();
	void runMatchRunStage();
	void runDeceasedDonorArrivalStage();
	void runTransplantationStage();
	
	// Output
	std::stringstream outputExchangeInformation;
	std::stringstream outputTransplantList;
	std::stringstream outputPopulationList;

	std::stringstream kpdSimulationLog;

public:

	KPDSimulation(KPDParameters * parameters);
	~KPDSimulation();

	// Simulation Function	
	void runIteration(int iteration);

	// Output Functions
	std::string getOutputExchanges();
	std::string getOutputResults();
	std::string getOutputPopulation();

	void printLog();
};

KPDSimulation::KPDSimulation(KPDParameters * parameters) {
	
	kpdParameters = parameters;

	std::cout << "Collecting Data" << std::endl;
	kpdData = new KPDData(kpdParameters);
	
	std::cout << "Preparing Record for Simulation" << std::endl;
	kpdRecord = new KPDRecord(kpdData, kpdParameters);
		
	kpdSimulationLog << "Match Runs Occur at Times: ";

	int freqMatchRun = kpdParameters->getFreqMatchRun();
	int timeSpan = kpdParameters->getTimeSpan();

	int matchRunTime = freqMatchRun;

	while (matchRunTime <= timeSpan) {

		matchRunTimes.push_back(matchRunTime);
		kpdSimulationLog << " " << matchRunTime;

		matchRunTime += freqMatchRun;
	}

	if (matchRunTimes.back() != timeSpan) { // Final match run occurs at end of simulation, regardless of time since last match run

		matchRunTimes.push_back(timeSpan);
		kpdSimulationLog << " " << timeSpan;
	}
	kpdSimulationLog << std::endl;

	kpdSimulationLog << "Match Run Schedule Set!" << std::endl << std::endl;
}

KPDSimulation::~KPDSimulation() {

	printLog();

	delete kpdRecord;
}

int KPDSimulation::indexOf(int id) {

	for (int i = 0; i < (int)waitlistedCandidateIDs.size(); i++) {
		if (waitlistedCandidateIDs[i] == id) {
			return i;
		}
	}

	return -1;

}

int KPDSimulation::getChild(int lower, int current, std::vector<int> &visitedVector, std::vector<std::vector<bool> > &adjacency) {
	int nV = (int)visitedVector.size() - 1;
	for (int j = lower + 1; j <= nV; j++) {
		if (adjacency[current][j] == true && visitedVector[j] == 0)
			return j;
	}
	return -1;
}

void KPDSimulation::updateStatus(int index, KPDStatus newState, bool waitlistCandidate) {

	if (waitlistCandidate) {

		//Only consider candidates not already transplanted
		if (waitlistedCandidateTransplanted[index] != TRANSPLANTED_YES) {

			kpdSimulationLog << currentTime << ": WL " << index << " " << KPDFunctions::statusToString(waitlistedCandidateStatus[index]) << " -> " << KPDFunctions::statusToString(newState) << std::endl;

			if (waitlistedCandidateTransplanted[index] == TRANSPLANT_IN_PROGRESS){
				
				if (newState == STATUS_ACTIVE) {
					kpdSimulationLog << " (Upon Return to Waitlist)" << std::endl;
				}
				else {
					for (std::deque<KPDArrangement *>::iterator it = transplantQueue.begin(); it != transplantQueue.end(); it++) {
						(*it)->setFailure(index);
					}
				}

				// Update state
				waitlistedCandidateStatus[index] = newState;
			}
		}
	}

	else {

		//Only consider nodes not already transplanted; ignore bridge donors
		if (kpdNodeTransplanted[index] != TRANSPLANTED_YES && kpdNodeTypes[index] != BRIDGE) {

			kpdSimulationLog << currentTime << ": " << kpdNodes[index]->getID() << " " << KPDFunctions::statusToString(kpdNodeStatus[index]) << " -> " << KPDFunctions::statusToString(newState) << std::endl;

			if (kpdNodeTransplanted[index] == TRANSPLANT_IN_PROGRESS) {

				if (newState == STATUS_ACTIVE) {
					kpdSimulationLog << " (Upon Return to KPD Pool)" << std::endl;
				}
				else {
					for (std::deque<KPDArrangement *>::iterator it = transplantQueue.begin(); it != transplantQueue.end(); it++) {
						(*it)->setFailure(index);
					}
				}

				// Update state
				kpdNodeStatus[index] = newState;
			}
		}
	}	
}

void KPDSimulation::updateFailedMatch(int donorNodeIndex, int candidateNodeIndex, int donorIndex, bool waitlistCandidate) {
			
	// KPD -> WL
	if (waitlistCandidate) {
		waitlistedCandidateMatches[donorNodeIndex][candidateNodeIndex][donorIndex]->resetMatch();

		// Check if adjacency matrix needs to be updated
		bool noAssociatedDonors = true;

		for (std::vector<KPDMatch*>::iterator it = waitlistedCandidateMatches[donorNodeIndex][candidateNodeIndex].begin(); it != waitlistedCandidateMatches[donorNodeIndex][candidateNodeIndex].end(); it++){

			if ((*it)->getAdjacency()) {
				noAssociatedDonors = false;
				break;
			}
		}

		if (noAssociatedDonors) {
			kpdToWaitlistAdjacencyMatrix[donorNodeIndex][candidateNodeIndex] = false;
		}
	}
	
	// KPD -> KPD
	else {
		kpdMatches[donorNodeIndex][candidateNodeIndex][donorIndex]->resetMatch();

		// Check if adjacency matrix needs to be updated
		bool noAssociatedDonors = true;

		for (std::vector<KPDMatch*>::iterator it = kpdMatches[donorNodeIndex][candidateNodeIndex].begin(); it != kpdMatches[donorNodeIndex][candidateNodeIndex].end(); it++) {
			if ((*it)->getAdjacency()) {
				noAssociatedDonors = false;
				break;
			}
		}

		if (noAssociatedDonors) {
			kpdAdjacencyMatrix[donorNodeIndex][candidateNodeIndex] = false;
			kpdAdjacencyMatrixReduced[donorNodeIndex][candidateNodeIndex] = false;
		}
	}	
}

void KPDSimulation::findDeceasedDonorMatches() {

	for (std::deque<KPDDonor *>::iterator itDonor = deceasedDonors.begin(); itDonor != deceasedDonors.end(); itDonor++) {

		KPDDonor * donor = *itDonor;
		
		int donorID = donor->getUniqueID();

		for (std::vector<KPDNode *>::iterator itNode = kpdNodes.begin(); itNode != kpdNodes.end(); itNode++) {

			KPDNode * node = *itNode;

			if (node->getType() == PAIR) {

				KPDCandidate * candidate = node->getCandidate();

				int candidateID = candidate->getUniqueID();

				KPDCrossmatch crossmatch = kpdData->performCrossmatch(candidate, donor);

				if (kpdData->allowableMatch(crossmatch)) {

					KPDMatch * match = kpdRecord->generateMatch(candidate, donor, crossmatch);

					deceasedDonorMatches[donorID][candidateID] = match;

				}
			}
		}
	}
}

void KPDSimulation::findWaitlistedCandidateMatches() {

	for (std::vector<KPDCandidate *>::iterator itCandidate = waitlistedCandidates.begin(); itCandidate != waitlistedCandidates.end(); itCandidate++) {

		KPDCandidate * candidate = *itCandidate;

		int candidateID = candidate->getUniqueID();

		for (std::vector<KPDNode *>::iterator itNode = kpdNodes.begin(); itNode != kpdNodes.end(); itNode++) {

			KPDNode * node = *itNode;

			int donorID = node->getID();
			int numberOfDonors = node->getNumberOfDonors();

			for (int k = 1; k <= numberOfDonors; k++) {
				
				int donorIndex = k - 1;

				KPDDonor * donor = node->getDonor(donorIndex);

				KPDCrossmatch crossmatch = kpdData->performCrossmatch(candidate, donor);

				if (kpdData->allowableMatch(crossmatch)) {

					KPDMatch * match = kpdRecord->generateMatch(candidate, donor, crossmatch);

					waitlistedCandidateMatches[candidateID][donorID].push_back(match);

				}
				else {

					KPDMatch *noMatch = new KPDMatch();
					noMatch->setVirtualCrossmatchResult(crossmatch);

					waitlistedCandidateMatches[candidateID][donorID].push_back(noMatch);

				}
			}
		}		
	}
}

int KPDSimulation::allocateDeceasedDonor(KPDDonor * donor) {

	for (int i = 1; i <= waitlistedCandidateIDs.size(); i++) {

		int candidateIndex = i - 1;
		KPDCandidate * candidate = waitlistedCandidates[waitlistedCandidateIDs[candidateIndex]];

		if (waitlistedCandidateStatus[candidateIndex] == STATUS_ACTIVE &&
			waitlistedCandidateStatus[candidateIndex] == NOT_TRANSPLANTED) {
			
			KPDCrossmatch crossmatch = kpdData->performCrossmatch(candidate, donor);

			if (kpdData->allowableMatch(crossmatch)) {
				
				// Record Transplant
				return candidateIndex;
			}

		}

	}

	return -1;
}


void KPDSimulation::runStateTransitionStage() {

	// WL	
	for (int i = 1; i <= (int) waitlistedCandidateStateTransitionTimes.size(); i++) {

		int candidateIndex = i - 1;

		if (waitlistedCandidateStateTransitionTimes[candidateIndex].size() > 0) { //If candidate has transitions remaining, and we have reached the time of the next state transition

			if (waitlistedCandidateStateTransitionTimes[candidateIndex].front() == currentTime) {

				KPDStatus newState = waitlistedCandidateStateTransitions[candidateIndex].front();

				updateStatus(candidateIndex, newState, true);

				waitlistedCandidateStateTransitions[candidateIndex].pop_front();
				waitlistedCandidateStateTransitionTimes[candidateIndex].pop_front();
			}
		}

		candidateIndex++;
	}
	
	// KPD 
	for (int i = 1; i <= (int) kpdNodeStateTransitionTimes.size(); i++) {

		int nodeIndex = i - 1;
	
		if (kpdNodeStateTransitionTimes[nodeIndex].size() > 0) { //If node has transitions remaining, and we have reached the time of the next state transition
			
			if (kpdNodeStateTransitionTimes[nodeIndex].front() == currentTime) {

				KPDStatus newState = kpdNodeStateTransitions[nodeIndex].front();

				updateStatus(nodeIndex, newState, false);

				kpdNodeStateTransitions[nodeIndex].pop_front();
				kpdNodeStateTransitionTimes[nodeIndex].pop_front();
			}
		}

		nodeIndex++;
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
	matchRun->collectLRSForCurrentMatchRun(matchRunArrangements);
	std::cout << "# of Transplant Arrangements = " << matchRunArrangements.size() << std::endl;
	
	// If there are indeed arrangements in the pool...
	if (matchRunArrangements.size() > 0) {
		
		// Assign the appropriate utility values to the LRSs
		matchRun->assignExpectedUtilitiesForCurrentMatchRun(matchRunArrangements, assignedValueOfMatchRunArrangements); 
		std::cout << "Utilities Assigned" << std::endl;

		// Select optimal set of LRSs
		std::vector<int> optimalSolution;

		matchRun->getOptimalSolutionForCurrentMatchRun(optimalSolution, matchRunArrangements, assignedValueOfMatchRunArrangements);

		kpdSimulationLog << std::endl;
		kpdSimulationLog << matchRun->printLog();
		kpdSimulationLog << std::endl;

		// Add Selected Arrangements to Those in Progress
		for (int i = 1; i <= matchRunArrangements.size(); i++) {

			int arrangementIndex = i - 1;

			std::vector<int> currentArrangement = matchRunArrangements[arrangementIndex];

			// Retrieve NDD Status
			bool ndd = false;
			for (std::vector<int>::iterator nodeIt = currentArrangement.begin(); nodeIt != currentArrangement.end(); nodeIt++) {
				if (kpdNodeTypes[*nodeIt] != PAIR) {
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
					arrangementNodes.push_back(kpdNodes[*nodeIt]->copy()); // Copy Arrangement Nodes				
					arrangementNodeTypes.push_back(kpdNodeTypes[*nodeIt]);

					kpdNodeTransplanted[*nodeIt] = TRANSPLANT_IN_PROGRESS; // Mark Nodes in Arrangement as TRANSPLANT_IN_PROGRESS
				}

				//Create New Arrangement Object and Add to Those In Progress
				int transplantationTime = currentTime + kpdParameters->getPostSelectionInactivePeriod();

				KPDArrangement * newArrangement = new KPDArrangement(currentIteration, currentTime, transplantationTime, 
					arrangementUtility,	arrangementNodes, arrangementNodeTypes, kpdMatches);

				transplantQueue.push_back(newArrangement);

				//Output to Simulation Log
				if (newArrangement->hasNDD()) { kpdSimulationLog << "LRS (with NDDs) "; }
				else { kpdSimulationLog << "LRS (no NDDs) "; }
				kpdSimulationLog << newArrangement->toLabel() << " Queued" << std::endl;
			}

			//Output to Arrangement List
			outputExchangeInformation << currentIteration << "," << currentTime << "," << kpdParameters->getPostSelectionInactivePeriod() << "," << currentArrangement.size() << ",";
			
			outputExchangeInformation << *currentArrangement.begin();
			for (std::vector<int>::iterator it = currentArrangement.begin() + 1; it != currentArrangement.end(); it++) {
				outputExchangeInformation << ";" << *it;
			}

			outputExchangeInformation << "," << KPDFunctions::boolToYesNo(ndd) << "," << arrangementUtility << ",";
			outputExchangeInformation << KPDFunctions::boolToYesNo(arrangementIsPartOfOptimalSolution) << std::endl;

			i++;
		}
	}
	else {
		kpdSimulationLog << std::endl;
		kpdSimulationLog << matchRun->printLog();
		kpdSimulationLog << std::endl;
	}

	std::cout << std::endl;

	delete matchRun;
}

void KPDSimulation::runDeceasedDonorArrivalStage() {

	// Repeat for all deceased donors that arrive at current time

	while (deceasedDonorArrivalTimes.front() == currentTime) {

		// Deceased donor arrives

		KPDDonor * donor = deceasedDonors.front();
		int donorID = donor->getUniqueID();

		deceasedDonors.pop_front();
		deceasedDonorArrivalTimes.pop_front();

		// Can it start a KPD chain?
		// If so, find the best on and process it
		std::map<int, KPDMatch *> matches = deceasedDonorMatches[donorID];

		std::vector<int> firstNodes;
		firstNodes.push_back(0);
		int possibleNodes = 0;

		std::vector<std::vector<int> > chainsUnderConsideration;

		for (std::map<int, KPDMatch*>::iterator it = matches.begin(); it != matches.end(); it++) {

			int nodeIndex = it->first; // Check if index or ID

			if (kpdNodeTransplanted[nodeIndex] == NOT_TRANSPLANTED && kpdNodeStatus[nodeIndex] == STATUS_ACTIVE) {

				// Check additional requirements for node to be involved in DD transplant
				firstNodes.push_back((*it).first);
				possibleNodes++;
			}
		}

		bool assignDirectlyToWaitlist = true;

		if (possibleNodes > 0) {

			// Find best option in KPD, that ends on the waitlist
			
			//int start = 1;
			std::vector<int> visitedVec(possibleNodes + 1, 0);
			std::vector<int> stack_vec;

			for (std::vector<int>::iterator it = firstNodes.begin(); it != firstNodes.end(); it++) {

				int start = *it;

				visitedVec[start] = 1;

				stack_vec.push_back(start);
				int v = getChild(start, stack_vec.back(), visitedVec, kpdAdjacencyMatrixReduced);
				while (!stack_vec.empty()) {
					if (v == -1) {
						int top = stack_vec.back();
						stack_vec.pop_back();
						if (top == start) {
							visitedVec[top] = 0;
							break;
						}
						visitedVec[top] = 0;
						v = getChild(top, stack_vec.back(), visitedVec, kpdAdjacencyMatrixReduced);
					}
					else {
						visitedVec[v] = 1;
						stack_vec.push_back(v);

						std::map<int, std::vector<KPDMatch *> > matches = waitlistedCandidateMatches[v];

						if (matches.size() > 0) { // Reasonable candidate in waiting list to end the chain
							
							for (std::map<int, std::vector<KPDMatch *> >::iterator it2 = matches.begin(); it2 != matches.end(); it2++) {
								// Need to more extensively look at this...

								int waitlistCandidateID = it2->first;

								if (waitlistedCandidateStatus[waitlistCandidateID] == STATUS_ACTIVE &&
									waitlistedCandidateTransplanted[waitlistCandidateID] == NOT_TRANSPLANTED) {

									std::vector<int> potentialCycleOrChain;

									for (int i = 0; i < (int)stack_vec.size(); i++) {
										potentialCycleOrChain.push_back(stack_vec[i] - 1);
									}

									chainsUnderConsideration.push_back(potentialCycleOrChain);
									assignDirectlyToWaitlist = false;
								}
							}

						}

						if ((int)stack_vec.size() >= kpdParameters->getMaxChainLength())
							v = -1;
						else
							v = getChild(start, v, visitedVec, kpdAdjacencyMatrixReduced);

					}
				}
			}
		}

		// Go through chains under consideration and select one


		// If not, assign directly to waitlist
		if(assignDirectlyToWaitlist) {

			std::vector<int> realMatchedCandidateIDs; // Need function to get matched candidates

			for (std::vector<int>::iterator it = realMatchedCandidateIDs.begin(); it != realMatchedCandidateIDs.end(); it++) {

				int realMatchedCandidateIndex = indexOf(*it);

				if (waitlistedCandidateTransplanted[realMatchedCandidateIndex] == NOT_TRANSPLANTED) {

					waitlistedCandidateTransplanted[realMatchedCandidateIndex] = TRANSPLANTED_YES;
					// Print Statements

				}
				else {

					int simulatedMatchedCandidateIndex = allocateDeceasedDonor(donor);
					waitlistedCandidateTransplanted[simulatedMatchedCandidateIndex] = TRANSPLANTED_YES;
					// Print Statements

				}
			}
		}
	}
}


void KPDSimulation::runTransplantationStage() {
	
	//If there are transplant arrangements waiting...
	if (transplantQueue.size() > 0) {
		// .. and if a transplant arrangement has reached its transplantation time, then... 
		if (transplantQueue.front()->getTransplantationTime() <= currentTime) {

			//1. Get Transplant Arrangement Information and Output to File
			KPDArrangement * transplantArrangement = transplantQueue.front();
			std::vector<int> nodesInProgress = transplantArrangement->getNodeIDs();

			bool chain = transplantArrangement->hasNDD();

			kpdSimulationLog << std::endl << transplantArrangement->getTransplantationTime() << ": ";
			kpdSimulationLog << "TX : " << transplantArrangement->toLabel() << std::endl << std::endl;

			//2. Update Matrices to Reflect Revealed Crossmatches
			for (std::vector<int>::iterator itCandidateNode = nodesInProgress.begin(); itCandidateNode != nodesInProgress.end(); ++itCandidateNode) {

				if (kpdNodeTypes[*itCandidateNode] == PAIR) {
					for (std::vector<int>::iterator itDonorNode = nodesInProgress.begin(); itDonorNode != nodesInProgress.end(); ++itDonorNode) {

						//...iterate through all donors...
						for (int k = 1; k <= kpdNodes[*itDonorNode]->getNumberOfDonors(); k++) {

							int donorIndex = k - 1;

							//Only consider relevant exchanges within the transplant arrangement
							if (transplantArrangement->getAdjacency(*itDonorNode, *itCandidateNode, donorIndex)) {

								//Output availability of the nodes
								kpdSimulationLog << "Donor Node " << *itDonorNode;
								if (!transplantArrangement->getAvailability(*itDonorNode)) {
									kpdSimulationLog << " not";
								}
								kpdSimulationLog << " Available" << std::endl << std::endl;


								kpdSimulationLog << "Candidate Node " << *itCandidateNode;
								if (!transplantArrangement->getAvailability(*itCandidateNode)) {
									kpdSimulationLog << " not";
								}
								kpdSimulationLog << " Available" << std::endl << std::endl;

								//Reveal results of the lab crossmatch...
								if (kpdMatches[*itDonorNode][*itCandidateNode][donorIndex]->getLabCrossmatchResult()) {
									kpdSimulationLog << "Match " << *itDonorNode << "[" << k << "] -> " << *itCandidateNode << " Successful (-> 1)" << std::endl;
									if (kpdMatches[*itDonorNode][*itCandidateNode][donorIndex]->getAssumedSuccessProbability() != 1.0) {
										kpdMatches[*itDonorNode][*itCandidateNode][donorIndex]->setAssumedSuccessProbability(1.0); // Probability updated to reflect successful crossmatch
									}
								}
								else {
									kpdSimulationLog << "Match " << *itDonorNode << "[" << k << "] -> " << *itCandidateNode << " Failed (Removed)" << std::endl;
									updateFailedMatch(*itDonorNode, *itCandidateNode, donorIndex, false); // Remove edge from KPD graph to reflect unsuccessful crossmatch
								}
							}
						}
					}					
				}
			}

			// 3. Find Best Transplant Option(s) Remaining in Arrangement
			std::vector<std::vector<int> > options;
			transplantArrangement->getOptimalSolutionForFallBackOptions(options, kpdParameters->getMaxCycleSize(), kpdParameters->getMaxChainLength(), kpdParameters->getAllowABBridgeDonors(), kpdParameters->getUtilityScheme());

			if (options.size() == 0) {
				kpdSimulationLog << "No Options" << std::endl;
			}
			else {

				//Iterate through options:
				for (std::vector<std::vector<int> >::iterator itOptions = options.begin(); itOptions != options.end(); itOptions++) {

					std::vector<int> currentOption = *itOptions;

					bool chain = kpdNodeTypes[*currentOption.begin()] != PAIR;
					int length = (int)currentOption.size();

					//Realized Chain
					if (chain) {
						length--;
						
						//Iterate through nodes and mark transplants
						for (std::vector<int >::iterator itNodes = currentOption.begin(); itNodes != currentOption.end() - 1; ++itNodes) {

							int selectedDonor = transplantArrangement->selectDonor(*itNodes, *(itNodes + 1), kpdParameters->getUtilityScheme(), true); // Select Donor
							
							kpdNodeTransplanted[*itNodes] = TRANSPLANTED_YES; // Update Transplantation Status of Donor Node

							kpdSimulationLog << "TX: " << *itNodes << "[" << selectedDonor << "] -> " << *(itNodes + 1) << " Recorded" << std::endl; // Output
						}

						//Set up bridge node and output
						int bridgeNode = *(currentOption.end() - 1);
						kpdNodeTypes[bridgeNode] = BRIDGE;
						kpdNodeTransplanted[bridgeNode] = NOT_TRANSPLANTED;
						
						//Correct bridge donor
						for (int i = 1; i <= (int)kpdNodes.size(); i++) {
							
							int nodeIndex = i - 1;

							if (nodeIndex != bridgeNode) {
								//Remove matches to new bridge donor node
								if (kpdAdjacencyMatrix[nodeIndex][bridgeNode]) {
									for (int k = 1; k <= kpdNodes[i]->getNumberOfDonors(); k++) {

										int donorIndex = k - 1;
										updateFailedMatch(nodeIndex, bridgeNode, donorIndex, false);
									}
								}

								//Remove implict matches to all altruistic or bridge donors
								if (kpdAdjacencyMatrix[bridgeNode][i] && kpdNodeTypes[i] != PAIR) {
									for (int k = 1; k <= kpdNodes[bridgeNode]->getNumberOfDonors(); k++) { /// CHECK THIS AGAIN!

										int donorIndex = k - 1;
										updateFailedMatch(bridgeNode, nodeIndex, donorIndex, false);
									}
								}

								//Add (implicit) matches from all pairs to new bridge donor node
								if (kpdNodeTypes[i] == PAIR) {
									kpdAdjacencyMatrix[nodeIndex][bridgeNode] = true;
								}
							}
						}
						
						kpdSimulationLog << "Donors " << bridgeNode << " now BD, returned to pool" << std::endl;
					}

					//Realized Cycle
					else {

						//Get information from realized cycle
						for (std::vector<int >::iterator itNodes = currentOption.begin(); itNodes != currentOption.end() - 1; ++itNodes) {

							int selectedDonor = transplantArrangement->selectDonor(*itNodes, *(itNodes + 1), kpdParameters->getUtilityScheme(), true); // Select Donor

							kpdNodeTransplanted[*(itNodes + 1)] = TRANSPLANTED_YES; // Update Transplantation Status of Candidate Node

							kpdSimulationLog << "Transplant " << *itNodes << "[" << selectedDonor << "] -> " << *(itNodes + 1) << " Recorded" << std::endl;  //Output							
						}

						int selectedDonor = transplantArrangement->selectDonor(*(currentOption.end() - 1), *(currentOption.begin()), kpdParameters->getUtilityScheme(), true); // Final Transplant

						kpdNodeTransplanted[*(currentOption.begin())] = TRANSPLANTED_YES; // Update Transplantation Status of Candidate Node

						kpdSimulationLog << "Transplant " << *(currentOption.end() - 1) << "[" << selectedDonor << "] -> " << *(currentOption.begin()) << " Recorded" << std::endl; //Output

					}
				}
			}

			//4. Return All Untransplanted Nodes to Pool
			for (int k = 0; k < (int) nodesInProgress.size(); k++) {
				if (kpdNodeTransplanted[nodesInProgress[k]] == TRANSPLANT_IN_PROGRESS) {
					kpdNodeTransplanted[nodesInProgress[k]] = NOT_TRANSPLANTED;
					kpdSimulationLog << KPDFunctions::nodeTypeToString(kpdNodeTypes[nodesInProgress[k]]) << " " << nodesInProgress[k] << " returned ";					
				}
			}

			//5. Print Transplant Information to File
			outputTransplantList << transplantArrangement->toTransplantString();

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
	outputPopulationList.str("");
	outputPopulationList << kpdRecord->getPopulationList();

	// Reset counters
	currentTime = 0;

	// Retrieve master information
	deceasedDonors = kpdData->getDeceasedDonors();
	deceasedDonorArrivalTimes = kpdData->getDeceasedDonorArrivalTimes();

	waitlistedCandidates = kpdData->getWaitlistedCandidates();
	waitlistedCandidateIDs = kpdData->getWaitlistedCandidateIDs();

	waitlistedCandidateStatus.assign(waitlistedCandidates.size(), STATUS_INACTIVE);
	waitlistedCandidateTransplanted.assign(waitlistedCandidates.size(), NOT_TRANSPLANTED);

	waitlistedCandidateStateTransitions = kpdData->getWaitlistCandidatesStateTransitionMatrix();
	waitlistedCandidateStateTransitionTimes = kpdData->getWaitlistCandidatesStateTransitionTimeMatrix();

	kpdNodes = kpdRecord->getNodes();
	kpdNodeTypes = kpdRecord->getNodeTypes();

	kpdNodeStatus.assign(kpdNodes.size(), STATUS_INACTIVE);
	kpdNodeTransplanted.assign(kpdNodes.size(), NOT_TRANSPLANTED);

	kpdNodeStateTransitions = kpdRecord->getKPDNodeStateTransitionMatrix();
	kpdNodeStateTransitionTimes = kpdRecord->getKPDNodeStateTransitionTimeMatrix();

	kpdMatches = kpdRecord->getKPDMatches();

	kpdAdjacencyMatrix = kpdRecord->getAdjacencyMatrix();
	kpdAdjacencyMatrixReduced = kpdRecord->getAdjacencyMatrixReduced();

	findDeceasedDonorMatches();
	findWaitlistedCandidateMatches();

	// Clear output streams
	outputExchangeInformation.str("");
	outputTransplantList.str("");

	kpdSimulationLog.str("");

	int timeSpan = kpdParameters->getTimeSpan();
	int postSelectionInactivePeriod = kpdParameters->getPostSelectionInactivePeriod();

	std::vector<int>::iterator it = matchRunTimes.begin();

	int matchRunTime = *it;
	it++;

	while (currentTime < timeSpan) {

		currentTime++;

		// Update all state changes at new time
		runStateTransitionStage();

		// Perform match run
		if (currentTime == matchRunTime) {
			runMatchRunStage();

			matchRunTime = *it;
			it++;
		}

		// Check for deceased donors
		runDeceasedDonorArrivalStage();

		// Perform transplantations
		runTransplantationStage();
	}

	// Remaining state transitions and final transplantation stage
	while (currentTime < timeSpan + postSelectionInactivePeriod) {

		currentTime++;

		runStateTransitionStage();
		runTransplantationStage();
	}
}

std::string KPDSimulation::getOutputExchanges() {
	return outputExchangeInformation.str();
}

std::string KPDSimulation::getOutputResults() {
	return outputTransplantList.str();
}

std::string KPDSimulation::getOutputPopulation() {
	return outputPopulationList.str();
}

void KPDSimulation::printLog() {

	std::string logFile = "output/" + kpdParameters->getOutputFolder() + "/" + kpdParameters->getSubFolder() + "/Log-Simulation.txt";
	std::ofstream outputFileLog(logFile.c_str());
	outputFileLog << kpdSimulationLog.str();
	outputFileLog.close();
}

#endif
