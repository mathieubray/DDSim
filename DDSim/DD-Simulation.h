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
	double currentTime;
	
	// Node Information
	int nNodes;
	std::vector<KPDNode *> nodeVector;
	std::vector<KPDNodeType> nodeTypeVector; // Needs to be separate! Can be updated over the course of the simulation!

	std::vector<KPDStatus> candidateUnderlyingStatus;
	std::vector<std::vector<KPDStatus> > donorUnderlyingStatus;

	std::vector<KPDTransplant> nodeTransplantationStatus;

	std::deque<int> matchRunTimes;

	std::vector<std::deque<KPDStatus> > candidateStateTransitionMatrix;
	std::vector<std::deque<double> > candidateStateTransitionTimeMatrix;
	std::vector<std::vector<std::deque<KPDStatus> > > donorStateTransitionMatrix;
	std::vector<std::vector<std::deque<double> > > donorStateTransitionTimeMatrix;

	// Crossmatch Information
	std::vector<std::vector<std::vector<KPDMatch *> > > matchMatrix;
	std::vector<std::vector <bool> > incidenceMatrix;
	std::vector<std::vector<bool> > incidenceMatrixReduced;

	std::deque<KPDArrangement *> transplantArrangements;

	// Helper Functions
	std::vector<int> getNextStateTransition();
	KPDStatus getNodeStatus(int nodeID);
	void updateStatus(int nodeID, int donorID, double transitionTime, KPDStatus newState);
	void correctBridgeDonor(int id);
	void removeEdge(int donorNodeID, int recipNodeID, int donorID);	
	void resetSimulationState();

	// Simulation Stages
	double stateTransitionStage(int nodeID, int donorID);
	int matchRunStage(KPDOptimizationScheme scheme);
	void transplantationStage(double time);
	
	std::stringstream outputExchangeInformation;
	std::stringstream outputTransplantList;
	std::stringstream outputPopulationList;

	std::stringstream kpdSimulationLog;

public:

	KPDSimulation(KPDParameters * parameters);
	~KPDSimulation();

	// Simulation Functions
	void prepareIteration(int iteration);
	void runIteration(KPDOptimizationScheme scheme);

	// Getter Functions
	std::vector<KPDNode *> getNodes();
	std::vector<KPDNodeType> getNodeTypes();

	std::vector<KPDStatus> getCandidateUnderlyingStatuses();
	std::vector<std::vector<KPDStatus> > getDonorUnderlyingStatuses();

	std::vector<KPDTransplant> getNodeTransplantationStatuses();
	
	std::vector<std::vector<std::vector<KPDMatch *> > > getMatchMatrix();
	std::vector<std::vector<bool> > getIncidence();
	std::vector<std::vector<bool> > getIncidenceReduced();

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

std::vector<int> KPDSimulation::getNextStateTransition() {
	double upcomingTransitionTime = 0;
	std::vector<int> upcomingTransition(2, -1); //First value represents Node ID, Second value represents Donor ID (-1 if we're dealing with a candidate)

	bool eventsToProcess = false;

	//Iterate thorugh candidates
	for (unsigned i = 1; i < candidateStateTransitionTimeMatrix.size(); i++) {
		//If pair has transitions remaining
		if (candidateStateTransitionTimeMatrix[i].size() > 0) {
			//If no events have been processed yet, or if we have not reached the time of the next state transition
			if (!eventsToProcess || (eventsToProcess && candidateStateTransitionTimeMatrix[i].front() < upcomingTransitionTime)) {
				//Update node/donor index and upcoming transition time 
				upcomingTransition[0] = i;
				upcomingTransition[1] = -1;
				upcomingTransitionTime = candidateStateTransitionTimeMatrix[i].front();
			}
			eventsToProcess = true;
		}
	}

	//Iterate through donors
	for (unsigned i = 1; i < donorStateTransitionTimeMatrix.size(); i++) {
		for (unsigned k = 0; k < donorStateTransitionTimeMatrix[i].size(); k++) {
			if (donorStateTransitionTimeMatrix[i][k].size() > 0) {
				//If no events have been processed yet, or if we have not reached the time of the next state transition
				if (!eventsToProcess || (eventsToProcess && donorStateTransitionTimeMatrix[i][k].front() < upcomingTransitionTime)) {
					//Update node/donor index and upcoming transition time 
					upcomingTransition[0] = i;
					upcomingTransition[1] = k;
					upcomingTransitionTime = donorStateTransitionTimeMatrix[i][k].front();
				}
				eventsToProcess = true;
			}
		}
	}

	return upcomingTransition; // If no upcoming transitions, first element of vector will be -1
}

KPDStatus KPDSimulation::getNodeStatus(int nodeID) {

	//If the candidate is withdrawn, then the entire node is withdrawn
	if (candidateUnderlyingStatus[nodeID] == STATUS_WITHDRAWN) {
		return STATUS_WITHDRAWN;
	}
	else {

		//If the candidate is inactive, then the node is inactive
		if (candidateUnderlyingStatus[nodeID] == STATUS_INACTIVE) {
			return STATUS_INACTIVE;
		}
		else {

			//If the candidate is active, node status is based on donors
			//If any donor is active, node is active
			//If all donors are withdrawn, node is withdrawn
			//Otherwise, node is inactive

			int withdrawnDonors = 0;

			for (int donorID = 1; donorID <= nodeVector[nodeID]->getNumberOfAssociatedDonors(); donorID++) {
				if (donorUnderlyingStatus[nodeID][donorID] == STATUS_ACTIVE) { // Active donor exists
					return STATUS_ACTIVE;
				}
				else if (donorUnderlyingStatus[nodeID][donorID] == STATUS_WITHDRAWN) {
					withdrawnDonors++;
				}
			}

			if (withdrawnDonors == nodeVector[nodeID]->getNumberOfAssociatedDonors()) {
				return STATUS_WITHDRAWN;
			}
			else {
				return STATUS_INACTIVE;
			}
		}
	}
}

void KPDSimulation::updateStatus(int nodeID, int donorID, double transitionTime, KPDStatus newState) {

	//Advance Simulation Time
	currentTime = transitionTime;

	//Only consider nodes not already transplanted; ignore bridge donors
	if (nodeTransplantationStatus[nodeID] != TRANSPLANTED_YES && nodeTypeVector[nodeID] != BRIDGE) {
		KPDStatus oldState;

		//If it is a donor that is changing status
		if (donorID > 0) {
			oldState = donorUnderlyingStatus[nodeID][donorID];

			//If status is indeed changing
			if (oldState != newState) {

				//Output
				kpdSimulationLog << transitionTime << ": Donor " << nodeID << "[" << donorID << "]";
				kpdSimulationLog << " (" << KPDFunctions::statusToString(oldState) << " -> " << KPDFunctions::statusToString(newState) << ")" << std::endl;

				//Before changing status, check if another donor is active
				bool otherActiveDonorExists = false;
				int k = 1;
				while (k <= nodeVector[nodeID]->getNumberOfAssociatedDonors() && !otherActiveDonorExists) {
					if (donorUnderlyingStatus[nodeID][k] == STATUS_ACTIVE && k != donorID) {
						otherActiveDonorExists = true;
					}
					k++;
				}

				//If candidate is active, but no other active donor currently exists...
				if (candidateUnderlyingStatus[nodeID] == STATUS_ACTIVE && !otherActiveDonorExists) {
					// ... and if the new donor state is active... 
					if (newState == STATUS_ACTIVE) {
						// ... and if node is not in progress, then node is now active
						if (nodeTransplantationStatus[nodeID] == NOT_TRANSPLANTED) {
							nNodes++;
							kpdSimulationLog << "Node " << nodeID << " A {" << nNodes << "}" << std::endl;
						}
						// ... and if node is currently in progress, then node will become active if returned to the pool
						else {
							kpdSimulationLog << "Node " << nodeID << " A (On Return) {" << nNodes << "}" << std::endl;
						}
					}
					// ... and if the new donor state is inactive...
					else if (newState == STATUS_INACTIVE) {
						// .. and if node is not in progress, then node is now inactive
						if (nodeTransplantationStatus[nodeID] == NOT_TRANSPLANTED) {
							nNodes--;
							kpdSimulationLog << "Node " << nodeID << " I {" << nNodes << "}" << std::endl;
						}
						// .. and if node is in progress, then add failures to transplant arrangements involving that donor, and node becomes inactive when returned to the pool
						else {
							for (std::deque<KPDArrangement *>::iterator it = transplantArrangements.begin(); it != transplantArrangements.end(); it++) {
								(*it)->setFailure(nodeID, donorID, transitionTime);
							}
							kpdSimulationLog << "Node " << nodeID << " I (On Return) {" << nNodes << "}" << std::endl;
						}
					}
					// ... and if the new donor state is withdrawn...
					else {
						//.. and if node is not in progress, it is withdrawn
						if (nodeTransplantationStatus[nodeID] == NOT_TRANSPLANTED) {
							nNodes--;
						}
						//.. and if node is in progress, then add failures to transplant arrangements involving that donor
						else {
							for (std::deque<KPDArrangement *>::iterator it = transplantArrangements.begin(); it != transplantArrangements.end(); it++) {
								(*it)->setFailure(nodeID, donorID, transitionTime);
							}
						}
						kpdSimulationLog << "Node " << nodeID << " W {" << nNodes << "}" << std::endl;
					}
				}

				//Update donor status
				donorUnderlyingStatus[nodeID][donorID] = newState;
				//kpdSimulationLog << "# Nodes: " << nNodes << std::endl;
			}
		}

		// If it is a candidate changing status
		else {
			oldState = candidateUnderlyingStatus[nodeID];

			//If status is indeed changing
			if (oldState != newState) {

				// Output
				kpdSimulationLog << transitionTime << ": Candidate " << nodeID;
				kpdSimulationLog << " (" << KPDFunctions::statusToString(oldState) << " -> " << KPDFunctions::statusToString(newState) << ")" << std::endl;

				// Before changing status, check if there is an active donor
				bool activeDonorExists = false;
				int k = 1;
				while (k <= nodeVector[nodeID]->getNumberOfAssociatedDonors() && !activeDonorExists) {
					if (donorUnderlyingStatus[nodeID][k] == STATUS_ACTIVE) {
						activeDonorExists = true;
					}
					k++;
				}

				// If there is an active donor...
				if (activeDonorExists) {
					// .. and if the new candidate state is active...
					if (newState == STATUS_ACTIVE) {
						// .. and if node is not in progress, then node is now active
						if (nodeTransplantationStatus[nodeID] == NOT_TRANSPLANTED) {
							nNodes++;
							kpdSimulationLog << "Node " << nodeID << " A {" << nNodes << "}" << std::endl;
						}
						// .. and if node is currently in progress, then node will become active if returned to the pool
						else {
							kpdSimulationLog << "Node " << nodeID << " A (On Return) {" << nNodes << "}" << std::endl;
						}
					}
					// .. and if the new candidate state is inactive...
					else if (newState == STATUS_INACTIVE) {
						// .. and if node is not in progress, then node is now inactive
						if (nodeTransplantationStatus[nodeID] == NOT_TRANSPLANTED) {
							nNodes--;
							kpdSimulationLog << "Node " << nodeID << " I {" << nNodes << "}" << std::endl;
						}
						// .. and if node is in progress, then add failures to transplant arrangements involving that candidate, and node becomes inactive when returned to the pool
						else {
							for (std::deque<KPDArrangement *>::iterator it = transplantArrangements.begin(); it != transplantArrangements.end(); it++) {
								(*it)->setFailure(nodeID, donorID, transitionTime);
							}
							kpdSimulationLog << "Node " << nodeID << " I (On Return) {" << nNodes << "}" << std::endl;
						}
					}
					// .. and if the new candidate state is withdrawn...
					else {
						// .. and if node is not in progress, then node is withdrawn
						if (nodeTransplantationStatus[nodeID] == NOT_TRANSPLANTED) {
							nNodes--;
						}
						// ... and if node is currently in progress, then add failures to transplant arrangements involving that candidate
						else {
							for (std::deque<KPDArrangement *>::iterator it = transplantArrangements.begin(); it != transplantArrangements.end(); it++) {
								(*it)->setFailure(nodeID, donorID, transitionTime);
							}
						}
						kpdSimulationLog << "Node " << nodeID << " Withdrawn {" << nNodes << "}" << std::endl;
					}
				}

				// Update state
				candidateUnderlyingStatus[nodeID] = newState;
				//kpdSimulationLog << "# Nodes: " << nNodes << std::endl;
			}
		}
	}
}

void KPDSimulation::correctBridgeDonor(int bridgeDonorID) {

	//Iterate through all other nodes
	for (unsigned i = 1; i < nodeVector.size(); i++) {
		if (i != bridgeDonorID) {
			//Remove edges to new bridge donor node
			if (incidenceMatrix[i][bridgeDonorID]) {
				for (int k = 1; k <= nodeVector[i]->getNumberOfAssociatedDonors(); k++) {
					removeEdge(i, bridgeDonorID, k);
				}
			}

			//Remove implict edges to all altruistic or bridge donors
			if (incidenceMatrix[bridgeDonorID][i] && nodeTypeVector[i] != PAIR) {
				for (int k = 1; k <= nodeVector[bridgeDonorID]->getNumberOfAssociatedDonors(); k++) { /// CHECK THIS AGAIN!
					removeEdge(bridgeDonorID, i, k);
				}
			}

			//Add (implicit) edges from all pairs to new bridge donor node
			if (nodeTypeVector[i] == PAIR) {
				incidenceMatrix[i][bridgeDonorID] = true;

				for (int k = 1; k <= nodeVector[i]->getNumberOfAssociatedDonors(); k++) {
					matchMatrix[i][bridgeDonorID][k]->setMatchProperties(true, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, CROSSMATCH_SUCCESSFUL, true);
				}
			}
		}
	}
}

void KPDSimulation::removeEdge(int donorNodeID, int recipNodeID, int donorID) {
	
	matchMatrix[donorNodeID][recipNodeID][donorID]->setMatchProperties(false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, CROSSMATCH_FAILED_LAB, false);

	// Check if incidence matrix needs to be updated
	bool noAssociatedDonors = true;

	for (int id = 1; id < nodeVector[donorNodeID]->getNumberOfAssociatedDonors(); id++) {
		if (matchMatrix[donorNodeID][recipNodeID][id]->getIncidence()) {
			noAssociatedDonors = false;
			break;
		}
	}

	if (noAssociatedDonors) {
		incidenceMatrix[donorNodeID][recipNodeID] = false;
		incidenceMatrixReduced[donorNodeID][recipNodeID] = false;
	}
}

void KPDSimulation::resetSimulationState() {

	// Clear Simulation Information

	currentMatchRun = 0;
	currentTime = 0;

	nNodes = 0;		
	nodeVector.clear();
	nodeTypeVector.clear();

	candidateUnderlyingStatus.clear();
	donorUnderlyingStatus.clear();

	nodeTransplantationStatus.clear();

	matchRunTimes.clear();

	candidateStateTransitionMatrix.clear();
	candidateStateTransitionTimeMatrix.clear();
	donorStateTransitionMatrix.clear();
	donorStateTransitionTimeMatrix.clear();
	
	matchMatrix.clear();
	incidenceMatrix.clear();
	incidenceMatrixReduced.clear();
	
	transplantArrangements.clear();

	// Collect Matrices from Record

	nodeVector = kpdRecord->getNodes();
	nodeTypeVector = kpdRecord->getNodeTypes();

	int numberOfNodes = (int)nodeVector.size() - 1;

	candidateUnderlyingStatus.assign(1 + numberOfNodes, STATUS_INACTIVE);
	donorUnderlyingStatus.assign(1, std::vector<KPDStatus>(1, STATUS_INACTIVE));
	for (int i = 1; i <= numberOfNodes; i++) {
		int numberOfDonors = nodeVector[i]->getNumberOfAssociatedDonors();
		donorUnderlyingStatus.push_back(std::vector<KPDStatus>(1 + numberOfDonors, STATUS_INACTIVE));
	}

	nodeTransplantationStatus.assign(nodeTypeVector.size(), NOT_TRANSPLANTED);

	matchRunTimes = kpdRecord->getMatchRunTimes();

	candidateStateTransitionMatrix = kpdRecord->getCandidateStateTransitionMatrix();
	candidateStateTransitionTimeMatrix = kpdRecord->getCandidateStateTransitionTimeMatrix();
	donorStateTransitionMatrix = kpdRecord->getDonorStateTransitionMatrix();
	donorStateTransitionTimeMatrix = kpdRecord->getDonorStateTransitionTimeMatrix();

	matchMatrix = kpdRecord->getMatchMatrix();
	incidenceMatrix = kpdRecord->getIncidenceMatrix();
	incidenceMatrixReduced = kpdRecord->getIncidenceMatrixReduced();
	
}

double KPDSimulation::stateTransitionStage(int nodeID, int donorID) {

	KPDStatus newState;
	double transitionTime;

	if (donorID > -1) { // Donor state transition
		newState = donorStateTransitionMatrix[nodeID][donorID].front();
		donorStateTransitionMatrix[nodeID][donorID].pop_front();
		transitionTime = donorStateTransitionTimeMatrix[nodeID][donorID].front();
		donorStateTransitionTimeMatrix[nodeID][donorID].pop_front();
	}
	else { // Candidate state transition
		newState = candidateStateTransitionMatrix[nodeID].front();
		candidateStateTransitionMatrix[nodeID].pop_front();
		transitionTime = candidateStateTransitionTimeMatrix[nodeID].front();
		candidateStateTransitionTimeMatrix[nodeID].pop_front();
	}

	//Check if there are transplants to perform
	transplantationStage(transitionTime);

	//Update state
	updateStatus(nodeID, donorID, transitionTime, newState);

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

	KPDMatchRun * matchRun = new KPDMatchRun(kpdParameters, currentIteration, currentMatchRun, currentTime,
		getNodes(), getNodeTypes(), getCandidateUnderlyingStatuses(), getDonorUnderlyingStatuses(),
		getNodeTransplantationStatuses(), getMatchMatrix(), getIncidence(), getIncidenceReduced());

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
				if (nodeTypeVector[*nodeIt] != PAIR) {
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
					arrangementNodes.push_back(nodeVector[*nodeIt]->copy()); // Copy Arrangement Nodes				
					arrangementNodeTypes.push_back(nodeTypeVector[*nodeIt]);

					nodeTransplantationStatus[*nodeIt] = TRANSPLANT_IN_PROGRESS; // Mark Nodes in Arrangement as TRANSPLANT_IN_PROGRESS
					nNodes--; // Reduce Node Count
				}

				//Create New Arrangement Object and Add to Those In Progress
				KPDArrangement * newArrangement = new KPDArrangement(kpdParameters, currentIteration, currentMatchRun, currentTime, kpdParameters->getProcessingTime(), arrangementUtility,
					arrangementNodes, arrangementNodeTypes, matchMatrix, donorUnderlyingStatus, ndd, scheme);

				transplantArrangements.push_back(newArrangement);

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
			outputExchangeInformation << currentIteration << "," << currentMatchRun << "," << currentTime << "," << kpdParameters->getProcessingTime() << "," << currentArrangement.size() << ",";

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

	kpdSimulationLog << "Pool Size: " << nNodes << std::endl << std::endl;

	std::cout << std::endl;

	delete matchRun;

	return matchRunTime;
}


void KPDSimulation::transplantationStage(double time) {

	currentTime = time;

	//If there are transplant arrangements waiting...
	if (transplantArrangements.size() > 0) {
		// .. and if a transplant arrangement has reached its transplantation time, then... 
		if (transplantArrangements.front()->getTransplantationTime() <= time) {

			//1. Get Transplant Arrangement Information and Output to File
			KPDArrangement * transplantArrangement = transplantArrangements.front();
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

				if (nodeTypeVector[*itCandidateNode] == PAIR) {
					for (std::vector<int>::iterator itDonorNode = nodesInProgress.begin(); itDonorNode != nodesInProgress.end(); ++itDonorNode) {

						//...iterate through all donors...
						for (int k = 1; k <= nodeVector[*itDonorNode]->getNumberOfAssociatedDonors(); k++) {

							//Only consider relevant exchanges within the transplant arrangement
							if (transplantArrangement->getIncidence(*itDonorNode, *itCandidateNode, k)) {

								//... and output availability.
								kpdSimulationLog << "Donor " << *itDonorNode << "[" << k << "]";
								if (transplantArrangement->getDonorAvailability(*itDonorNode, k) == 0) { // Not available
									kpdSimulationLog << " not Available" << std::endl;
								}
								else {
									kpdSimulationLog << " Available" << std::endl;
								}

								//Then reveal results of the lab crossmatch...
								if (matchMatrix[*itDonorNode][*itCandidateNode][k]->getLabCrossmatchResult()) {
									kpdSimulationLog << "Match " << *itDonorNode << "[" << k << "] -> " << *itCandidateNode << " Successful (-> 1)" << std::endl;
									if (matchMatrix[*itDonorNode][*itCandidateNode][k]->getAssumedSuccessProbability() != 1.0) {
										matchMatrix[*itDonorNode][*itCandidateNode][k]->setAssumedSuccessProbability(1.0); // Probability updated to reflect successful crossmatch
									}
								}
								else {
									kpdSimulationLog << "Match " << *itDonorNode << "[" << k << "] -> " << *itCandidateNode << " Failed (Removed)" << std::endl;
									removeEdge(*itDonorNode, *itCandidateNode, k); // Remove edge from KPD graph to reflect unsuccessful crossmatch
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

					bool chain = (nodeTypeVector[*currentOption.begin()] != PAIR);
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

							nodeTransplantationStatus[*itNodes] = TRANSPLANTED_YES; // Update Transplantation Status of Donor Node

							kpdSimulationLog << "TX: " << *itNodes << "[" << selectedDonor << "] -> " << *(itNodes + 1) << " Recorded" << std::endl; // Output
						}

						//Set up bridge node and output
						int bridgeNode = *(currentOption.end() - 1);
						nodeTypeVector[bridgeNode] = BRIDGE;
						nodeTransplantationStatus[bridgeNode] = NOT_TRANSPLANTED;
						correctBridgeDonor(bridgeNode);
						nNodes++;
						kpdSimulationLog << "Donors " << bridgeNode << " now BD, returned {" << nNodes << "}" << std::endl;
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

							nodeTransplantationStatus[*(itNodes + 1)] = TRANSPLANTED_YES; // Update Transplantation Status of Candidate Node

							kpdSimulationLog << "Transplant " << *itNodes << "[" << selectedDonor << "] -> " << *(itNodes + 1) << " Recorded" << std::endl;  //Output							
						}

						int selectedDonor = transplantArrangement->selectDonor(*(currentOption.end() - 1), *(currentOption.begin()), true); // Final Transplant

						transplantArrangement->setSelectedTransplant(*(currentOption.end() - 1), *(currentOption.begin()), selectedDonor); // Mark Transplant in Arrangement

						nodeTransplantationStatus[*(currentOption.begin())] = TRANSPLANTED_YES; // Update Transplantation Status of Candidate Node

						kpdSimulationLog << "Transplant " << *(currentOption.end() - 1) << "[" << selectedDonor << "] -> " << *(currentOption.begin()) << " Recorded" << std::endl; //Output

					}
				}
			}

			//4. Return All Untransplanted Nodes to Pool
			for (unsigned k = 0; k < nodesInProgress.size(); k++) {
				if (nodeTransplantationStatus[nodesInProgress[k]] == TRANSPLANT_IN_PROGRESS) {
					nodeTransplantationStatus[nodesInProgress[k]] = NOT_TRANSPLANTED;
					kpdSimulationLog << KPDFunctions::nodeTypeToString(nodeTypeVector[nodesInProgress[k]]) << " " << nodesInProgress[k] << " returned ";
					if (getNodeStatus(nodesInProgress[k]) == STATUS_ACTIVE) {
						nNodes++;
					}
					kpdSimulationLog << "(" << KPDFunctions::statusToString(getNodeStatus(nodesInProgress[k])) << " {" << nNodes << "})" << std::endl;
				}
			}

			//5. Print Transplant Information to File
			outputTransplantList << transplantArrangement->toTransplantString();

			kpdSimulationLog << "Pool Size: " << nNodes << std::endl << std::endl;

			transplantArrangements.pop_front();

			transplantationStage(currentTime);
		}
	}
}

void KPDSimulation::prepareIteration(int iteration){

	outputPopulationList.str("");

	// Set Current Iteration
	currentIteration = iteration;

	std::cout << "--------------" << std::endl;
	std::cout << "Iteration " << currentIteration << std::endl;
	std::cout << "--------------" << std::endl << std::endl;
	
	// Generate New KPD Pool for this Iteration
	kpdRecord->generateKPDPool(iteration);
	
	// Output KPD Pool Population
	outputPopulationList << kpdRecord->getPopulationList();

}

void KPDSimulation::runIteration(KPDOptimizationScheme scheme) {
	
	// Reset simulation matrices
	resetSimulationState();	

	// Clear Streams
	outputExchangeInformation.str("");
	outputTransplantList.str("");

	kpdSimulationLog.str("");
	
	// Run simulation
	kpdSimulationLog << "------------------------" << std::endl;
	kpdSimulationLog << "Iteration " << currentIteration << ": " << KPDFunctions::optimizationSchemeToString(scheme)  << std::endl;
	kpdSimulationLog << "------------------------" << std::endl << std::endl;

	std::cout << "*** " << KPDFunctions::optimizationSchemeToString(scheme) << " ***" << std::endl << std::endl;
	
	
	double cutOff = kpdParameters->getTimeSpan() + kpdParameters->getProcessingTime();
	bool continueSimulation = true;

	while (continueSimulation) {

		// Obtain next state transition
		std::vector<int> transition = getNextStateTransition();
		int nodeID = transition[0];
		int donorID = transition[1];

		if (nodeID > 0) { // State transitions remain
			if (matchRunTimes.size() > 0) { // Match runs remain
				if ((donorID > -1 && matchRunTimes.front() < donorStateTransitionTimeMatrix[nodeID][donorID].front()) ||
					(donorID == -1 && matchRunTimes.front() < candidateStateTransitionTimeMatrix[nodeID].front())) { // Match run takes place before next state transition
										
					currentTime = matchRunStage(scheme); // Perform match run
				}

				else { // State transition takes place before match run
					currentTime = stateTransitionStage(nodeID, donorID);
				}
			}

			else { // No more match runs
				currentTime = stateTransitionStage(nodeID, donorID);
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

std::vector<KPDNode*> KPDSimulation::getNodes(){

	std::vector<KPDNode *> nodes; // Starts at 1

	nodes.push_back(new KPDNode());

	for (std::vector<KPDNode*>::iterator it = nodeVector.begin() + 1; it != nodeVector.end(); it++) {
		nodes.push_back((*it)->copy()); // Deep Copy
	}

	return nodes;
}

std::vector<KPDNodeType> KPDSimulation::getNodeTypes(){

	std::vector<KPDNodeType> nodeTypes; // Starts at 1

	nodeTypes.push_back(PAIR);

	for (std::vector<KPDNode*>::iterator it = nodeVector.begin() + 1; it != nodeVector.end(); it++) {
		nodeTypes.push_back((*it)->getType());
	}

	return nodeTypes;
}

std::vector<KPDStatus> KPDSimulation::getCandidateUnderlyingStatuses() {

	std::vector<KPDStatus> candidateUnderlyingStatusClone(candidateUnderlyingStatus);

	return candidateUnderlyingStatusClone;
}

std::vector<std::vector<KPDStatus> > KPDSimulation::getDonorUnderlyingStatuses(){

	std::vector<std::vector<KPDStatus> > donorUnderlyingStatusClone(donorUnderlyingStatus);

	return donorUnderlyingStatusClone;
}

std::vector<KPDTransplant> KPDSimulation::getNodeTransplantationStatuses(){

	std::vector<KPDTransplant> nodeTransplantationStatusClone(nodeTransplantationStatus);

	return nodeTransplantationStatusClone;
}

std::vector<std::vector<std::vector<KPDMatch*> > > KPDSimulation::getMatchMatrix(){

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

std::vector<std::vector<bool> > KPDSimulation::getIncidence(){

	std::vector<std::vector<bool> > incidenceMatrixClone(incidenceMatrix);

	return incidenceMatrixClone;
}

std::vector<std::vector<bool>> KPDSimulation::getIncidenceReduced()
{
	std::vector<std::vector<bool> > incidenceMatrixReducedClone(incidenceMatrixReduced);

	return incidenceMatrixReducedClone;
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
