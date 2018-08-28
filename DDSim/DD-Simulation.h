/* ---------------------------------------------
DD-Simulation.h
Organizes the Steps of the Simulation
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
	double currentDate;
	
	std::deque<int> matchRunTimes;

	// Deceased Donors Information
	std::deque<KPDDonor *> deceasedDonors;
	std::deque<int> deceasedDonorDate;	

	// Candidate Waitlist Information
	std::vector<KPDCandidate *> waitlistedCandidates;

	std::vector<KPDStatus> waitlistedCandidateStatus;
	std::vector<KPDTransplant> waitlistedCandidateTransplanted;

	std::vector<std::deque<KPDStatus> > waitlistedCandidateStateTransitions;
	std::vector<std::deque<double> > waitlistedCandidateStateTransitionTimes;
	
	// KPD Information
	std::vector<KPDNode *> kpdNodes;
	std::vector<KPDNodeType> kpdNodeTypes; // Needs to be separate! Can be updated over the course of the simulation!

	std::vector<KPDStatus> kpdNodeStatus;
	std::vector<KPDTransplant> kpdNodeTransplanted;

	std::vector<std::deque<KPDStatus> > kpdNodeStateTransitions;
	std::vector<std::deque<double> > kpdNodeStateTransitionTimes;

	// Crossmatch Information

	std::vector<std::vector<KPDMatch *> > deceasedDonorMatches;

	std::vector<std::vector<std::vector<KPDMatch *> > > waitlistedCandidateMatches;

	std::vector<std::vector<std::vector<KPDMatch *> > > kpdMatches;
	std::vector<std::vector<bool> > adjacencyMatrix;
	std::vector<std::vector<bool> > adjacencyMatrixReduced;

	std::deque<KPDArrangement *> selectedExchanges;

	// Helper Functions	
	int getNextStateTransition();	
	void updateStatus(int nodeID, double transitionTime, KPDStatus newState);
	void correctBridgeDonor(int id);
	void removeMatch(int donorNodeID, int recipNodeID, int donorID);
	
	
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
	void runIteration(KPDOptimizationScheme scheme);

	double stateTransitionStage(int nodeID);
	int matchRunStage(KPDOptimizationScheme scheme);
	void deceasedDonorArrivalStage();
	void transplantationStage(double time);

	std::string getOutputExchanges();
	std::string getOutputResults();
	std::string getOutputPopulation();

	void printLog();
};

KPDSimulation::KPDSimulation(KPDParameters * parameters) {
	
	kpdParameters = parameters;

	std::cout << "Loading Record" << std::endl;
	kpdRecord = new KPDRecord(parameters);

}

KPDSimulation::~KPDSimulation() {

	printLog();

	delete kpdRecord;
}

int KPDSimulation::getNextStateTransition() {

	double transitionTime = 0;
	int transition = -1; 

	bool eventsToProcess = false;

	//Iterate through KPD nodes
	for (unsigned i = 1; i < kpdNodeStateTransitionTimes.size(); i++) {
		
		if (kpdNodeStateTransitionTimes[i].size() > 0) { //If node has transitions remaining...
			
			//... and if If no events have been processed yet, or if we have not reached the time of the next state transition
			if (!(eventsToProcess && kpdNodeStateTransitionTimes[i].front() >= transitionTime)) { 
				//Update node/donor index and upcoming transition time 
				transition = i;
				transitionTime = kpdNodeStateTransitionTimes[i].front();
			}
			eventsToProcess = true;
		}
	}
	
	return transition; // If no upcoming transitions, will be -1
}

void KPDSimulation::updateStatus(int nodeID, double transitionTime, KPDStatus newState) {

	//Advance Simulation Time
	currentDate = transitionTime;

	//Only consider nodes not already transplanted; ignore bridge donors
	if (kpdNodeTransplanted[nodeID] != TRANSPLANTED_YES && kpdNodeTypes[nodeID] != BRIDGE) {

		KPDStatus oldState;

		oldState = kpdNodeStatus[nodeID];

		//If status is indeed changing ..
		if (oldState != newState) {

			kpdSimulationLog << transitionTime << ": " << nodeID;
			kpdSimulationLog << " (" << KPDFunctions::statusToString(oldState) << " -> " << KPDFunctions::statusToString(newState) << ")" << std::endl;
			
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
					for (std::deque<KPDArrangement *>::iterator it = selectedExchanges.begin(); it != selectedExchanges.end(); it++) {
						(*it)->setFailure(nodeID, -1, transitionTime);
					}
					kpdSimulationLog << "Node " << nodeID << " Inactive" << std::endl;
				}
			}
			// .. and if the new candidate state is withdrawn...
			else {

				// ... and if node is currently in progress, then add failures to transplant arrangements involving that candidate
				if (kpdNodeTransplanted[nodeID] != NOT_TRANSPLANTED) {					
					for (std::deque<KPDArrangement *>::iterator it = selectedExchanges.begin(); it != selectedExchanges.end(); it++) {
						(*it)->setFailure(nodeID, -1, transitionTime);
					}
				}
				kpdSimulationLog << "Node " << nodeID << " Withdrawn" << std::endl;
			}

			// Update state
			kpdNodeStatus[nodeID] = newState;
		}
	}

}

void KPDSimulation::correctBridgeDonor(int bridgeDonorID) {

	//Iterate through all other nodes
	for (unsigned i = 1; i < kpdNodes.size(); i++) {
		if (i != bridgeDonorID) {
			//Remove matches to new bridge donor node
			if (adjacencyMatrix[i][bridgeDonorID]) {
				for (int k = 1; k <= kpdNodes[i]->getNumberOfAssociatedDonors(); k++) {
					removeMatch(i, bridgeDonorID, k);
				}
			}

			//Remove implict matches to all altruistic or bridge donors
			if (adjacencyMatrix[bridgeDonorID][i] && kpdNodeTypes[i] != PAIR) {
				for (int k = 1; k <= kpdNodes[bridgeDonorID]->getNumberOfAssociatedDonors(); k++) { /// CHECK THIS AGAIN!
					removeMatch(bridgeDonorID, i, k);
				}
			}

			//Add (implicit) matches from all pairs to new bridge donor node
			if (kpdNodeTypes[i] == PAIR) {
				adjacencyMatrix[i][bridgeDonorID] = true;

				for (int k = 1; k <= kpdNodes[i]->getNumberOfAssociatedDonors(); k++) {
					kpdMatches[i][bridgeDonorID][k]->setMatchProperties(true, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, CROSSMATCH_SUCCESSFUL, true);
				}
			}
		}
	}
}

void KPDSimulation::removeMatch(int donorNodeID, int recipNodeID, int donorID) {
	
	kpdMatches[donorNodeID][recipNodeID][donorID]->setMatchProperties(false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, CROSSMATCH_FAILED_LAB, false);

	// Check if adjacency matrix needs to be updated
	bool noAssociatedDonors = true;

	for (int id = 1; id < kpdNodes[donorNodeID]->getNumberOfAssociatedDonors(); id++) {
		if (kpdMatches[donorNodeID][recipNodeID][id]->getAdjacency()) {
			noAssociatedDonors = false;
			break;
		}
	}

	if (noAssociatedDonors) {
		adjacencyMatrix[donorNodeID][recipNodeID] = false;
		adjacencyMatrixReduced[donorNodeID][recipNodeID] = false;
	}
}

void KPDSimulation::resetIteration() {
	
	currentMatchRun = 0;
	currentDate = 0;

	matchRunTimes.clear();

	deceasedDonors.clear();
	deceasedDonorDate.clear();

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
	adjacencyMatrix.clear();
	adjacencyMatrixReduced.clear();
	
	selectedExchanges.clear();

	// Collect Matrices from Record

	kpdNodes = kpdRecord->getNodes();
	kpdNodeTypes = kpdRecord->getNodeTypes();

	int numberOfNodes = (int)kpdNodes.size() - 1;

	kpdNodeStatus.assign(1 + numberOfNodes, STATUS_INACTIVE);
	kpdNodeTransplanted.assign(kpdNodeTypes.size(), NOT_TRANSPLANTED);

	matchRunTimes = kpdRecord->getMatchRunTimes();

	kpdNodeStateTransitions = kpdRecord->getCandidateStateTransitionMatrix();
	kpdNodeStateTransitionTimes = kpdRecord->getCandidateStateTransitionTimeMatrix();
	
	kpdMatches = kpdRecord->getMatchMatrix();
	adjacencyMatrix = kpdRecord->getAdjacencyMatrix();
	adjacencyMatrixReduced = kpdRecord->getAdjacencyMatrixReduced();


	outputExchangeInformation.str("");
	outputTransplantList.str("");

	kpdSimulationLog.str("");
	
}

double KPDSimulation::stateTransitionStage(int nodeID) {

	KPDStatus newState;
	double transitionTime;

	// Candidate state transition
	newState = kpdNodeStateTransitions[nodeID].front();
	kpdNodeStateTransitions[nodeID].pop_front();
	transitionTime = kpdNodeStateTransitionTimes[nodeID].front();
	kpdNodeStateTransitionTimes[nodeID].pop_front();
	
	//Check if there are transplants to perform
	transplantationStage(transitionTime);

	//Update state
	updateStatus(nodeID, transitionTime, newState);

	return transitionTime;
}

int KPDSimulation::matchRunStage(KPDOptimizationScheme scheme) {

	// Get current match run time
	currentMatchRun++;

	std::cout << "Match Run: " << currentMatchRun << std::endl;

	int matchRunTime = matchRunTimes.front();
	matchRunTimes.pop_front();

	// Check if there are transplants to perform first (also sets current time)
	transplantationStage(matchRunTime);
	
	// Set up match run
	std::vector<std::vector<int> > matchRunArrangements;
	std::vector<double> assignedValueOfMatchRunArrangements;

	KPDMatchRun * matchRun = new KPDMatchRun(kpdParameters, currentIteration, currentMatchRun, currentDate,
		kpdNodes, kpdNodeTypes, kpdNodeStatus,
		kpdNodeTransplanted, kpdMatches, adjacencyMatrix, adjacencyMatrixReduced);

	// Collect relevant transplant Arrangements
	if (scheme == LOCALLY_RELEVANT_SUBSETS) {
		matchRun->collectLRSForCurrentMatchRun(matchRunArrangements);
		std::cout << "# of Transplant Arrangements = " << matchRunArrangements.size() << std::endl;
	}
	else {
		matchRun->collectCyclesAndChainsForCurrentMatchRun(matchRunArrangements);
		std::cout << "# of Cycles/Chains = " << matchRunArrangements.size() << std::endl;
	}

	// If there are arrangements in the pool...
	if (matchRunArrangements.size() > 0) {

		// Assign Utility to Each Arrangement
		if (scheme == CYCLES_AND_CHAINS) { 
			matchRun->assignUtilitiesForCurrentMatchRun(matchRunArrangements, assignedValueOfMatchRunArrangements); 
		}
		else { 
			matchRun->assignExpectedUtilitiesForCurrentMatchRun(matchRunArrangements, assignedValueOfMatchRunArrangements); 
		}
		std::cout << "Utilities Assigned" << std::endl;

		// Select Optimal Arrangements
		std::vector<int> optimalSolution;

		matchRun->getOptimalSolutionForCurrentMatchRun(scheme, optimalSolution, matchRunArrangements, assignedValueOfMatchRunArrangements);

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
				KPDArrangement * newArrangement = new KPDArrangement(kpdParameters, currentIteration, currentMatchRun, currentDate, kpdParameters->getProcessingTime(), arrangementUtility,
					arrangementNodes, arrangementNodeTypes, kpdMatches, ndd, scheme);

				selectedExchanges.push_back(newArrangement);

				//Output to Simulation Log
				if (scheme == LOCALLY_RELEVANT_SUBSETS) {
					if (newArrangement->hasNDD()) { kpdSimulationLog << "LRS (with NDDs) "; }
					else { kpdSimulationLog << "LRS (no NDDs) "; }
				}
				else {
					if (newArrangement->hasNDD()) { kpdSimulationLog << "Chain "; }
					else { kpdSimulationLog << "Cycle "; }
				}
				kpdSimulationLog << newArrangement->toLabel() << " IP" << std::endl;
			}

			//Output to Arrangement List
			outputExchangeInformation << currentIteration << "," << currentMatchRun << "," << currentDate << "," << kpdParameters->getProcessingTime() << "," << currentArrangement.size() << ",";

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

	//kpdSimulationLog << "Pool Size: " << nNodes << std::endl << std::endl;

	std::cout << std::endl;

	delete matchRun;

	return matchRunTime;
}

inline void KPDSimulation::deceasedDonorArrivalStage()
{
}


void KPDSimulation::transplantationStage(double time) {

	currentDate = time;

	//If there are transplant arrangements waiting...
	if (selectedExchanges.size() > 0) {
		// .. and if a transplant arrangement has reached its transplantation time, then... 
		if (selectedExchanges.front()->getTransplantationTime() <= time) {

			//1. Get Transplant Arrangement Information and Output to File
			KPDArrangement * transplantArrangement = selectedExchanges.front();
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
						for (int k = 1; k <= kpdNodes[*itDonorNode]->getNumberOfAssociatedDonors(); k++) {

							//Only consider relevant exchanges within the transplant arrangement
							if (transplantArrangement->getAdjacency(*itDonorNode, *itCandidateNode, k)) {

								//... and output availability.
								kpdSimulationLog << "Donor " << *itDonorNode << "[" << k << "]";
								//if (transplantArrangement->getDonorAvailability(*itDonorNode, k) == 0) { // Not available
									//kpdSimulationLog << " not Available" << std::endl;
								//}
								//else {
									kpdSimulationLog << " Available" << std::endl;
								//}

								//Then reveal results of the lab crossmatch...
								if (kpdMatches[*itDonorNode][*itCandidateNode][k]->getLabCrossmatchResult()) {
									kpdSimulationLog << "Match " << *itDonorNode << "[" << k << "] -> " << *itCandidateNode << " Successful (-> 1)" << std::endl;
									if (kpdMatches[*itDonorNode][*itCandidateNode][k]->getAssumedSuccessProbability() != 1.0) {
										kpdMatches[*itDonorNode][*itCandidateNode][k]->setAssumedSuccessProbability(1.0); // Probability updated to reflect successful crossmatch
									}
								}
								else {
									kpdSimulationLog << "Match " << *itDonorNode << "[" << k << "] -> " << *itCandidateNode << " Failed (Removed)" << std::endl;
									removeMatch(*itDonorNode, *itCandidateNode, k); // Remove edge from KPD graph to reflect unsuccessful crossmatch
								}
							}
						}
					}

					//Finally, output availability of the candidate
					kpdSimulationLog << "Candidate " << *itCandidateNode;
					if (transplantArrangement->getCandidateAvailability(*itCandidateNode) == 0) {
						kpdSimulationLog << " not";
					}
					kpdSimulationLog << " Available" << std::endl << std::endl;
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

					bool chain = (kpdNodeTypes[*currentOption.begin()] != PAIR);
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
						correctBridgeDonor(bridgeNode);
						//nNodes++;
						//kpdSimulationLog << "Donors " << bridgeNode << " now BD, returned {" << nNodes << "}" << std::endl;
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
					//if (getNodeStatus(nodesInProgress[k]) == STATUS_ACTIVE) {
						//nNodes++;
					//}
					//kpdSimulationLog << "(" << KPDFunctions::statusToString(getNodeStatus(nodesInProgress[k])) << " {" << nNodes << "})" << std::endl;
				}
			}

			//5. Print Transplant Information to File
			outputTransplantList << transplantArrangement->toTransplantString();

			//kpdSimulationLog << "Pool Size: " << nNodes << std::endl << std::endl;

			selectedExchanges.pop_front();

			transplantationStage(currentDate);
		}
	}
}

void KPDSimulation::prepareIteration(int iteration){

	outputPopulationList.str("");

	// Set current iteration
	currentIteration = iteration;

	std::cout << "--------------" << std::endl;
	std::cout << "Iteration " << currentIteration << std::endl;
	std::cout << "--------------" << std::endl << std::endl;
	
	// Generate new KPD pool for this iteration
	kpdRecord->generateKPDPool(iteration);
	
	// Output KPD pool population
	outputPopulationList << kpdRecord->getPopulationList();

}

void KPDSimulation::runIteration(KPDOptimizationScheme scheme) {
	
	kpdSimulationLog << "------------------------" << std::endl;
	kpdSimulationLog << "Iteration " << currentIteration << ": " << KPDFunctions::optimizationSchemeToString(scheme)  << std::endl;
	kpdSimulationLog << "------------------------" << std::endl << std::endl;

	std::cout << "*** " << KPDFunctions::optimizationSchemeToString(scheme) << " ***" << std::endl << std::endl;
	
	double cutOff = kpdParameters->getTimeSpan() + kpdParameters->getProcessingTime();
	bool continueSimulation = true;

	while (continueSimulation) {

		// Obtain next state transition
		int nodeID = getNextStateTransition();

		if (nodeID > 0) { // State transitions remain
			if (matchRunTimes.size() > 0) { // Match runs remain
				if (matchRunTimes.front() < kpdNodeStateTransitionTimes[nodeID].front()) { // Match run takes place before next state transition
										
					currentDate = matchRunStage(scheme); // Perform match run
				}

				else { // State transition takes place before match run
					currentDate = stateTransitionStage(nodeID);
				}
			}

			else { // No more match runs
				currentDate = stateTransitionStage(nodeID);
			}
		}

		else { // No more state transitions
			if (matchRunTimes.size() > 0) { // Match runs remain
				matchRunStage(scheme); // Perform match run
			}
			else { // No more match runs

				// Transplant remaining arrangements
				transplantationStage(cutOff);

				// End simulation
				continueSimulation = false;
			}
		}
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
