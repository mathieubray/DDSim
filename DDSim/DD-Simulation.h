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
	KPDRecord * kpdRecord;

	// Simulation Progress
	int currentIteration;
	int currentMatchRun;
	int currentTime;
	
	std::deque<int> matchRunTimes;
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
	std::vector<std::vector<KPDMatch *> > deceasedDonorMatches;
	std::vector<std::vector<std::vector<KPDMatch *> > > waitlistedCandidateMatches;
	std::vector<std::vector<std::vector<KPDMatch *> > > kpdMatches;

	std::vector<std::vector<bool> > kpdAdjacencyMatrix;
	std::vector<std::vector<bool> > kpdAdjacencyMatrixReduced; // No implicit edges back to NDDs
	
	// Helper Functions	
	void updateKPDStatus(int nodeID, KPDStatus newState);
	void updateWaitlistCandidateStatus(int candidateID, KPDStatus newState);
	void updateFailedMatch(int donorNodeID, int candidateNodeID, int donorID);
	void updateBridgeDonor(int bridgeDonorID); 
	
	// Output
	std::stringstream outputExchangeInformation;
	std::stringstream outputTransplantList;
	std::stringstream outputPopulationList;

	std::stringstream kpdSimulationLog;

public:

	KPDSimulation(KPDParameters * parameters);
	~KPDSimulation();

	// Simulation Functions	
	void prepareIteration(int iteration);
	void resetIteration();
	void runIteration();

	void runStateTransitionStage();
	void runMatchRunStage();
	void runDeceasedDonorArrivalStage();
	void runTransplantationStage();

	// Output Functions
	std::string getOutputExchanges();
	std::string getOutputResults();
	std::string getOutputPopulation();

	void printLog();
};

KPDSimulation::KPDSimulation(KPDParameters * parameters) {
	
	kpdParameters = parameters;

	std::cout << "Preparing Record for Simulation" << std::endl;
	kpdRecord = new KPDRecord(parameters);

}

KPDSimulation::~KPDSimulation() {

	printLog();

	delete kpdRecord;
}


/* Helper Functions */

void KPDSimulation::updateKPDStatus(int nodeID, KPDStatus newState) {

	//Only consider nodes not already transplanted; ignore bridge donors
	if (kpdNodeTransplanted[nodeID] != TRANSPLANTED_YES && kpdNodeTypes[nodeID] != BRIDGE) {

		KPDStatus oldState = kpdNodeStatus[nodeID];

		//If status is indeed changing... (this is probably not needed anymore
		if (oldState != newState) {

			kpdSimulationLog << currentTime << ": " << nodeID << " (" << KPDFunctions::statusToString(oldState) << " -> " << KPDFunctions::statusToString(newState) << ")" << std::endl;
			
			// .. and if the new candidate state is active...
			if (newState == STATUS_ACTIVE) {
				// .. and if node is not in progress, then node is now active
				if (kpdNodeTransplanted[nodeID] == NOT_TRANSPLANTED) {
					kpdSimulationLog << "Node " << nodeID << " Active" << std::endl;
				}
				// .. and if node is currently in progress, then node will become active if returned to the pool
				else {
					kpdSimulationLog << "Node " << nodeID << " Active (On Return)" << std::endl;
				}
			}
			// .. and if the new candidate state is inactive...
			else if (newState == STATUS_INACTIVE) {
				// .. and if node is not in progress, then node is now inactive
				if (kpdNodeTransplanted[nodeID] == NOT_TRANSPLANTED) {
					kpdSimulationLog << "Node " << nodeID << " Inactive" << std::endl;
				}
				// .. and if node is in progress, then add failures to transplant arrangements involving that candidate, and node becomes inactive when returned to the pool
				else {
					for (std::deque<KPDArrangement *>::iterator it = transplantQueue.begin(); it != transplantQueue.end(); it++) {
						(*it)->setFailure(nodeID, -1, currentTime);
					}
					kpdSimulationLog << "Node " << nodeID << " Inactive" << std::endl;
				}
			}
			// .. and if the new candidate state is withdrawn...
			else {

				// ... and if node is currently in progress, then add failures to transplant arrangements involving that candidate
				if (kpdNodeTransplanted[nodeID] != NOT_TRANSPLANTED) {					
					for (std::deque<KPDArrangement *>::iterator it = transplantQueue.begin(); it != transplantQueue.end(); it++) {
						(*it)->setFailure(nodeID, -1, currentTime);
					}
				}
				kpdSimulationLog << "Node " << nodeID << " Withdrawn" << std::endl;
			}

			// Update state
			kpdNodeStatus[nodeID] = newState;
		}
	}
}

void KPDSimulation::updateWaitlistCandidateStatus(int nodeID, KPDStatus newState) {

	// ---TO DO--- //
	// Should be similar to the KPD case as above?

}

void KPDSimulation::updateFailedMatch(int donorNodeID, int candidateNodeID, int donorID) {

	//---TO DO---//
	// Will need to generalize this for DD->KPD and KPD->WL matches
	
	kpdMatches[donorNodeID][candidateNodeID][donorID]->setMatchProperties(false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, CROSSMATCH_FAILED_LAB, false);

	// Check if adjacency matrix needs to be updated
	bool noAssociatedDonors = true;

	for (int id = 1; id < kpdNodes[donorNodeID]->getNumberOfDonors(); id++) {
		if (kpdMatches[donorNodeID][candidateNodeID][id]->getAdjacency()) {
			noAssociatedDonors = false;
			break;
		}
	}

	if (noAssociatedDonors) {
		kpdAdjacencyMatrix[donorNodeID][candidateNodeID] = false;
		kpdAdjacencyMatrixReduced[donorNodeID][candidateNodeID] = false;
	}
}

void KPDSimulation::updateBridgeDonor(int bridgeDonorID) {

	//Iterate through all other nodes
	for (unsigned i = 1; i < kpdNodes.size(); i++) {
		if (i != bridgeDonorID) {
			//Remove edges to new bridge donor node
			if (kpdAdjacencyMatrix[i][bridgeDonorID]) {
				for (int k = 1; k <= kpdNodes[i]->getNumberOfDonors(); k++) {
					updateFailedMatch(i, bridgeDonorID, k);
				}
			}

			//Remove implict edges to all altruistic or bridge donors
			if (kpdAdjacencyMatrix[bridgeDonorID][i] && kpdNodeTypes[i] != PAIR) {
				for (int k = 1; k <= kpdNodes[bridgeDonorID]->getNumberOfDonors(); k++) { /// CHECK THIS AGAIN!
					updateFailedMatch(bridgeDonorID, i, k);
				}
			}

			//Add (implicit) edges from all pairs to new bridge donor node
			if (kpdNodeTypes[i] == PAIR) {
				kpdAdjacencyMatrix[i][bridgeDonorID] = true;

				for (int k = 1; k <= kpdNodes[i]->getNumberOfDonors(); k++) {
					kpdMatches[i][bridgeDonorID][k]->setMatchProperties(true, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, CROSSMATCH_SUCCESSFUL, true);
				}
			}
		}
	}
}


/* Simulation Functions */

void KPDSimulation::prepareIteration(int iteration) {

	// Set current iteration
	currentIteration = iteration;

	std::cout << "--------------" << std::endl;
	std::cout << "Iteration " << currentIteration << std::endl;
	std::cout << "--------------" << std::endl << std::endl;

	kpdSimulationLog << "--------------" << std::endl;
	kpdSimulationLog << "Iteration " << currentIteration << std::endl;
	kpdSimulationLog << "--------------" << std::endl << std::endl;

	// Generate new KPD pool for this iteration
	kpdRecord->generateSimulationData(iteration);

	// Output KPD pool population
	outputPopulationList.str("");
	outputPopulationList << kpdRecord->getPopulationList();

}

void KPDSimulation::resetIteration() {

	// Reset counters
	currentMatchRun = 0;
	currentTime = 0;

	matchRunTimes.clear();
	transplantQueue.clear();

	deceasedDonors.clear();
	deceasedDonorArrivalTimes.clear();

	waitlistedCandidates.clear();

	waitlistedCandidateStatus.clear();
	waitlistedCandidateTransplanted.clear();

	waitlistedCandidateStateTransitions.clear();
	waitlistedCandidateStateTransitionTimes.clear();

	kpdNodes.clear();
	kpdNodeTypes.clear();

	kpdNodeStatus.clear();
	kpdNodeTransplanted.clear();

	kpdNodeStateTransitions.clear();
	kpdNodeStateTransitionTimes.clear();

	deceasedDonorMatches.clear();
	waitlistedCandidateMatches.clear();
	kpdMatches.clear();

	kpdAdjacencyMatrix.clear();
	kpdAdjacencyMatrixReduced.clear();


	// Copy master information from record
	matchRunTimes = kpdRecord->getMatchRunTimes();

	deceasedDonors = kpdRecord->getDeceasedDonors();
	deceasedDonorArrivalTimes = kpdRecord->getDeceasedDonorArrivalTimes();

	waitlistedCandidates = kpdRecord->getWaitlistedCandidates();

	waitlistedCandidateStatus.assign(waitlistedCandidates.size(), STATUS_INACTIVE);
	waitlistedCandidateTransplanted.assign(waitlistedCandidates.size(), NOT_TRANSPLANTED);

	waitlistedCandidateStateTransitions = kpdRecord->getWaitlistCandidatesStateTransitionMatrix();
	waitlistedCandidateStateTransitionTimes = kpdRecord->getWaitlistCandidatesStateTransitionTimeMatrix();

	kpdNodes = kpdRecord->getNodes();
	kpdNodeTypes = kpdRecord->getNodeTypes();

	kpdNodeStatus.assign(kpdNodes.size(), STATUS_INACTIVE);
	kpdNodeTransplanted.assign(kpdNodes.size(), NOT_TRANSPLANTED);

	kpdNodeStateTransitions = kpdRecord->getKPDNodeStateTransitionMatrix();
	kpdNodeStateTransitionTimes = kpdRecord->getKPDNodeStateTransitionTimeMatrix();

	deceasedDonorMatches = kpdRecord->getDeceasedDonorMatches();
	waitlistedCandidateMatches = kpdRecord->getWaitlistCandidateMatches();
	kpdMatches = kpdRecord->getKPDMatches();

	kpdAdjacencyMatrix = kpdRecord->getAdjacencyMatrix();
	kpdAdjacencyMatrixReduced = kpdRecord->getAdjacencyMatrixReduced();

	// Clear output streams
	outputExchangeInformation.str("");
	outputTransplantList.str("");

	kpdSimulationLog.str("");

}

void KPDSimulation::runIteration() {

	int timeSpan = kpdParameters->getTimeSpan();
	int postSelectionInactivePeriod = kpdParameters->getPostSelectionInactivePeriod();
	
	int matchRunTime = matchRunTimes.front();
	matchRunTimes.pop_front();

	while (currentTime < timeSpan) {

		currentTime++;

		// Update all state changes at new time
		runStateTransitionStage();

		// Perform match run
		if (currentTime == matchRunTime) {
			runMatchRunStage();

			matchRunTime = matchRunTimes.front();
			matchRunTimes.pop_front();
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

	currentTime = matchRunTime;
	

}

void KPDSimulation::runStateTransitionStage() {

	//---TO DO---//
	// Make sure state transitions for waitlist candidates work properly

	// KPD nodes
	for (unsigned i = 1; i < kpdNodeStateTransitionTimes.size(); i++) {

		if (kpdNodeStateTransitionTimes[i].size() > 0) { //If node has transitions remaining, and we have reached the time of the next state transition
			
			if (kpdNodeStateTransitionTimes[i].front() == currentTime) {

				KPDStatus newState = kpdNodeStateTransitions[i].front();
				kpdNodeStateTransitions[i].pop_front();

				updateKPDStatus(i, newState);
			}
		}
	}

	// Waitlist candidates
	for (unsigned i = 1; i < waitlistedCandidateStateTransitionTimes.size(); i++) {

		if (waitlistedCandidateStateTransitionTimes[i].size() > 0) { //If candidate has transitions remaining, and we have reached the time of the next state transition

			if (waitlistedCandidateStateTransitionTimes[i].front() == currentTime) {

				KPDStatus newState = waitlistedCandidateStateTransitions[i].front();
				waitlistedCandidateStateTransitions[i].pop_front();

				updateWaitlistCandidateStatus(i, newState);
			}
		}
	}
}

void KPDSimulation::runMatchRunStage() {

	// Get current match run time
	currentMatchRun++;

	std::cout << "Match Run " << currentMatchRun << " at Time " << currentTime << std::endl;

	// Set up match run
	std::vector<std::vector<int> > matchRunArrangements;
	std::vector<double> assignedValueOfMatchRunArrangements;

	KPDMatchRun * matchRun = new KPDMatchRun(kpdParameters, currentIteration, currentMatchRun, currentTime,
		kpdNodes, kpdNodeTypes, kpdNodeStatus,
		kpdNodeTransplanted, kpdMatches, kpdAdjacencyMatrix, kpdAdjacencyMatrixReduced);
	
	matchRun->collectLRSForCurrentMatchRun(matchRunArrangements);
	std::cout << "# of Transplant Arrangements = " << matchRunArrangements.size() << std::endl;
	
	// If there are arrangements in the pool...
	if (matchRunArrangements.size() > 0) {
		
		matchRun->assignExpectedUtilitiesForCurrentMatchRun(matchRunArrangements, assignedValueOfMatchRunArrangements); 
		std::cout << "Utilities Assigned" << std::endl;

		// Select Optimal Arrangements
		std::vector<int> optimalSolution;

		matchRun->getOptimalSolutionForCurrentMatchRun(LOCALLY_RELEVANT_SUBSETS, optimalSolution, matchRunArrangements, assignedValueOfMatchRunArrangements);

		kpdSimulationLog << std::endl;
		kpdSimulationLog << matchRun->printLog();
		kpdSimulationLog << std::endl;

		// Add Selected Arrangements to Those in Progress
		int i = 0;
		for (std::vector<std::vector<int> >::iterator arrangementIt = matchRunArrangements.begin(); arrangementIt != matchRunArrangements.end(); arrangementIt++) {

			std::vector<int> currentArrangement = *arrangementIt;

			// Retrieve NDD Status
			bool ndd = false;
			for (std::vector<int>::iterator nodeIt = currentArrangement.begin(); nodeIt != currentArrangement.end(); nodeIt++) {
				if (kpdNodeTypes[*nodeIt] != PAIR) {
					ndd = true;
				}
			}

			// Retrieve Utility
			double arrangementUtility = assignedValueOfMatchRunArrangements[i];

			// Was This Arrangement Selected?
			bool arrangementIsPartOfOptimalSolution = false;

			for (std::vector<int>::iterator optimalSolutionsIt = optimalSolution.begin(); optimalSolutionsIt != optimalSolution.end(); optimalSolutionsIt++) {
				if (*optimalSolutionsIt == i) {
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
				KPDArrangement * newArrangement = new KPDArrangement(kpdParameters, currentIteration, currentMatchRun, currentTime, kpdParameters->getPostSelectionInactivePeriod(), arrangementUtility,
					arrangementNodes, arrangementNodeTypes, kpdMatches, ndd, LOCALLY_RELEVANT_SUBSETS);

				transplantQueue.push_back(newArrangement);

				//Output to Simulation Log
				if (newArrangement->hasNDD()) { kpdSimulationLog << "LRS (with NDDs) "; }
				else { kpdSimulationLog << "LRS (no NDDs) "; }
				kpdSimulationLog << newArrangement->toLabel() << " IP" << std::endl;
			}

			//Output to Arrangement List
			outputExchangeInformation << currentIteration << "," << currentMatchRun << "," << currentTime << "," << kpdParameters->getPostSelectionInactivePeriod() << "," << currentArrangement.size() << "," << *currentArrangement.begin();

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

	//---TO DO---//
	// This whole thing

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
			if (transplantArrangement->isLRS()) {
				kpdSimulationLog << "LRS ";
			}
			else {
				if (transplantArrangement->hasNDD()) {
					kpdSimulationLog << "Chain ";
				}
				else {
					kpdSimulationLog << "Cycle ";
				}
			}
			kpdSimulationLog << "TX : " << transplantArrangement->toLabel() << std::endl << std::endl;

			//2. Update Matrices to Reflect Revealed Crossmatches
			for (std::vector<int>::iterator itCandidateNode = nodesInProgress.begin(); itCandidateNode != nodesInProgress.end(); ++itCandidateNode) {

				if (kpdNodeTypes[*itCandidateNode] == PAIR) {
					for (std::vector<int>::iterator itDonorNode = nodesInProgress.begin(); itDonorNode != nodesInProgress.end(); ++itDonorNode) {

						//...iterate through all donors...
						for (int k = 1; k <= kpdNodes[*itDonorNode]->getNumberOfDonors(); k++) {

							//Only consider relevant exchanges within the transplant arrangement
							if (transplantArrangement->getAdjacency(*itDonorNode, *itCandidateNode, k)) {

								//Output availability of the nodes
								kpdSimulationLog << "Donor Node " << *itDonorNode;
								if (transplantArrangement->getCandidateAvailability(*itDonorNode) == 0) {
									kpdSimulationLog << " not";
								}
								kpdSimulationLog << " Available" << std::endl << std::endl;


								kpdSimulationLog << "Candidate Node " << *itCandidateNode;
								if (transplantArrangement->getCandidateAvailability(*itCandidateNode) == 0) {
									kpdSimulationLog << " not";
								}
								kpdSimulationLog << " Available" << std::endl << std::endl;

								//Reveal results of the lab crossmatch...
								if (kpdMatches[*itDonorNode][*itCandidateNode][k]->getLabCrossmatchResult()) {
									kpdSimulationLog << "Match " << *itDonorNode << "[" << k << "] -> " << *itCandidateNode << " Successful (-> 1)" << std::endl;
									if (kpdMatches[*itDonorNode][*itCandidateNode][k]->getAssumedSuccessProbability() != 1.0) {
										kpdMatches[*itDonorNode][*itCandidateNode][k]->setAssumedSuccessProbability(1.0); // Probability updated to reflect successful crossmatch
									}
								}
								else {
									kpdSimulationLog << "Match " << *itDonorNode << "[" << k << "] -> " << *itCandidateNode << " Failed (Removed)" << std::endl;
									updateFailedMatch(*itDonorNode, *itCandidateNode, k); // Remove edge from KPD graph to reflect unsuccessful crossmatch
								}
							}
						}
					}					
				}
			}

			// 3. Find Best Transplant Option(s) Remaining in Arrangement
			std::vector<std::vector<int> > options;
			transplantArrangement->getOptimalSolutionForFallBackOptions(options);

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

						//Begin output
						if (!transplantArrangement->isLRS() && length == transplantArrangement->getNumberOfNodes() - 1) { //Complete Chain
							kpdSimulationLog << "Complete Chain";
						}
						else {
							kpdSimulationLog << "Fallback Chain"; //Fallback Chain
						}
						kpdSimulationLog << " of Length " << length << std::endl;

						//Iterate through nodes and mark transplants
						for (std::vector<int >::iterator itNodes = currentOption.begin(); itNodes != currentOption.end() - 1; ++itNodes) {

							int selectedDonor = transplantArrangement->selectDonor(*itNodes, *(itNodes + 1), true); // Select Donor

							transplantArrangement->setSelectedTransplant(*itNodes, *(itNodes + 1), selectedDonor); // Mark Transplant in Arrangement

							kpdNodeTransplanted[*itNodes] = TRANSPLANTED_YES; // Update Transplantation Status of Donor Node

							kpdSimulationLog << "TX: " << *itNodes << "[" << selectedDonor << "] -> " << *(itNodes + 1) << " Recorded" << std::endl; // Output
						}

						//Set up bridge node and output
						int bridgeNode = *(currentOption.end() - 1);
						kpdNodeTypes[bridgeNode] = BRIDGE;
						kpdNodeTransplanted[bridgeNode] = NOT_TRANSPLANTED;
						updateBridgeDonor(bridgeNode);
						
						//Correct bridge donor
						for (unsigned i = 1; i < kpdNodes.size(); i++) {
							if (i != bridgeNode) {
								//Remove matches to new bridge donor node
								if (kpdAdjacencyMatrix[i][bridgeNode]) {
									for (int k = 1; k <= kpdNodes[i]->getNumberOfDonors(); k++) {
										updateFailedMatch(i, bridgeNode, k);
									}
								}

								//Remove implict matches to all altruistic or bridge donors
								if (kpdAdjacencyMatrix[bridgeNode][i] && kpdNodeTypes[i] != PAIR) {
									for (int k = 1; k <= kpdNodes[bridgeNode]->getNumberOfDonors(); k++) { /// CHECK THIS AGAIN!
										updateFailedMatch(bridgeNode, i, k);
									}
								}

								//Add (implicit) matches from all pairs to new bridge donor node
								if (kpdNodeTypes[i] == PAIR) {
									kpdAdjacencyMatrix[i][bridgeNode] = true;

									for (int k = 1; k <= kpdNodes[i]->getNumberOfDonors(); k++) {
										kpdMatches[i][bridgeNode][k]->setMatchProperties(true, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, CROSSMATCH_SUCCESSFUL, true);
									}
								}
							}
						}
						
						kpdSimulationLog << "Donors " << bridgeNode << " now BD, returned to pool" << std::endl;
					}

					//Realized Cycle
					else {
						if (!transplantArrangement->isLRS() && length == transplantArrangement->getNumberOfNodes()) { // Complete Cycle
							kpdSimulationLog << "Complete Cycle of Size " << length << std::endl;
						}
						else {
							kpdSimulationLog << "Fallback Cycle of Size " << length << std::endl; // Fallback Cycle							
						}

						//Get information from realized cycle
						for (std::vector<int >::iterator itNodes = currentOption.begin(); itNodes != currentOption.end() - 1; ++itNodes) {

							int selectedDonor = transplantArrangement->selectDonor(*itNodes, *(itNodes + 1), true); // Select Donor

							transplantArrangement->setSelectedTransplant(*itNodes, *(itNodes + 1), selectedDonor); // Mark Transplant in Arrangement

							kpdNodeTransplanted[*(itNodes + 1)] = TRANSPLANTED_YES; // Update Transplantation Status of Candidate Node

							kpdSimulationLog << "Transplant " << *itNodes << "[" << selectedDonor << "] -> " << *(itNodes + 1) << " Recorded" << std::endl;  //Output							
						}

						int selectedDonor = transplantArrangement->selectDonor(*(currentOption.end() - 1), *(currentOption.begin()), true); // Final Transplant

						transplantArrangement->setSelectedTransplant(*(currentOption.end() - 1), *(currentOption.begin()), selectedDonor); // Mark Transplant in Arrangement

						kpdNodeTransplanted[*(currentOption.begin())] = TRANSPLANTED_YES; // Update Transplantation Status of Candidate Node

						kpdSimulationLog << "Transplant " << *(currentOption.end() - 1) << "[" << selectedDonor << "] -> " << *(currentOption.begin()) << " Recorded" << std::endl; //Output

					}
				}
			}

			//4. Return All Untransplanted Nodes to Pool
			for (unsigned k = 0; k < nodesInProgress.size(); k++) {
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


/* Output Functions */

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
