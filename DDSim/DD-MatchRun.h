/* ---------------------------------------------
DD-MatchRun.h
Functions Relating to a Match Run of a KPD Program
---------------------------------------------- */

#ifndef MATCHRUN_H
#define MATCHRUN_H

#include "gurobi_c++.h"

#include "DD-Enums-Functions.h"
#include "DD-Parameters.h"
#include "DD-Node.h"
#include "DD-Match.h"
#include "DD-RNG.h"

#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>

class KPDMatchRun {

private:

	//Node Information
	std::vector<KPDNode *> matchRunNodeVector;
	std::vector<KPDNodeType> matchRunNodeTypeVector;

	std::vector<KPDStatus> matchRunCandidateUnderlyingStatus;
	std::vector<std::vector<KPDStatus> > matchRunDonorUnderlyingStatus;

	std::vector<KPDTransplant> matchRunNodeTransplantationStatus;

	std::vector<std::vector<std::vector<KPDMatch *> > > matchRunMatchMatrix;
	std::vector<std::vector <bool> > matchRunIncidenceMatrix;
	std::vector<std::vector<bool > > matchRunIncidenceMatrixReduced;

	std::vector<std::vector <bool> > matchRunSubIncidenceMatrix;
	std::vector<std::vector <bool> > matchRunSubIncidenceMatrixReduced; // No Implicit Transplants

	// Match Run Information
	int matchRunNumberOfPairs;
	int matchRunNumberOfNDDs;
	int matchRunNumberOfNodes;

	std::vector<int> currentMatchRunNodes;	

	// Relevant Parameters
	int maxChainLength;
	int maxCycleSize;
	int maximum;
	int maxLRSSize;

	KPDTimeline timeline;
	int processingTime;
	bool estimateEU;

	KPDUtilityScheme utilityScheme;

	bool addAdvantageToHighPRACandidates;
	double praAdvantageCutoff;
	double praAdvantageValue;
	bool allowABBridgeDonors;

	int numberOfExpectedUtilityIterations;

	//Helper Functions For Collecting Arrangements
	KPDStatus getNodeStatus(int nodeID);
	int selectDonor(int donorNodeID, int candidateNodeID);
	int getChild(int start, int v, std::vector<int> &visitedVector, std::vector<std::vector<bool> > &incidence);

	void getLRSPairsOnly(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<std::vector<bool> > & incidence);
	int getChildLRSPairsOnly(std::vector<std::vector<bool> > &incidence, std::vector<int> &tree, std::vector<int> &level, std::vector<int> &nodes, std::vector<int> &pairs, std::vector<bool> &visited, int next, int childLevel);

	void getLRSWithNDDs(std::vector<std::vector<int> > & currentMatchRunArrangements, int numberOfNDDs, std::vector<std::vector<bool> > &incidence);
	int getChildLRSWithNDDs(std::vector<std::vector<bool> > &incidence, bool &childIsAdjacentToLowerLevels, std::vector<std::vector<int> > &trees, std::vector<std::vector<int> > &levels, std::vector<int> &inducedLevels,
		std::vector<bool> &visited, int &next, int &nextNDD, int &childLevel, int &childLevelNDD, bool &childCanBeNDD, int &ndds, int &matchRunSize);

	bool validateLRSBounds(std::vector<int> &arrangement, int order);

	//Helper Functions For Assigning Expected Utilities
	double calculateExpectedUtility(std::vector<int> &arrangement);
	double estimateExpectedUtility(std::vector<int> &arrangement);
	double estimateExpectedUtilityMatrix(std::vector<int> &arrangement, bool print);
	double calculatePartialUtility(int nV, std::vector<std::vector<bool> > &incidence, std::vector<std::vector<std::vector<double> > > &utility, std::vector<KPDNodeType> &nodeTypes, std::vector<std::vector<KPDBloodType> > &bloodTypes);

	void checkValidSolution(int nSubCycles, int nVertices, std::vector<int> subCycleFlags, int next, std::vector<std::vector<int> > &subCyclesSubChains, std::vector<std::vector<int> > &possibleSolutions);

	// Random Number Generators
	RNG rngExpectedUtilityEstimation;

	// Logs
	std::stringstream matchRunLog;

public:

	KPDMatchRun(KPDParameters * params, int currentIteration, int currentMatchRun, double currentTime,
		std::vector<KPDNode *> nodeVector, std::vector<KPDNodeType> nodeTypeVector,
		std::vector<KPDStatus> candidateUnderlyingStatus, std::vector<std::vector<KPDStatus> > donorUnderlyingStatus, std::vector<KPDTransplant> nodeTransplantationStatus,
		std::vector<std::vector<std::vector<KPDMatch *> > > matchMatrix, std::vector<std::vector <bool> > incidenceMatrix, std::vector<std::vector<bool> > incidenceMatrixReduced);
	~KPDMatchRun();

	//Collect Arrangements
	void collectCyclesAndChainsForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements);
	void collectLRSForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements);

	//Assign Expected Utilities
	void assignUtilitiesForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements);
	void assignExpectedUtilitiesForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements);

	//Select Arrangements by Optimization
	void getOptimalSolutionForCurrentMatchRun(KPDOptimizationScheme scheme, std::vector<int> & optimalSolution, std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements);

	std::string printLog();

};


KPDMatchRun::KPDMatchRun(KPDParameters * params, int currentIteration, int currentMatchRun, double currentTime,
	std::vector<KPDNode *> nodeVector, std::vector<KPDNodeType> nodeTypeVector,
	std::vector<KPDStatus> candidateUnderlyingStatus, std::vector<std::vector<KPDStatus> > donorUnderlyingStatus, std::vector<KPDTransplant> nodeTransplantationStatus,
	std::vector<std::vector<std::vector<KPDMatch *> > > matchMatrix, std::vector<std::vector <bool> > incidenceMatrix, std::vector<std::vector<bool > > incidenceMatrixReduced) {
	
	matchRunLog << "Match Run " << currentMatchRun << " at Time " << currentTime << std::endl;

	//Set Matrices
	matchRunNodeVector = nodeVector;
	matchRunNodeTypeVector = nodeTypeVector;

	matchRunCandidateUnderlyingStatus = candidateUnderlyingStatus;
	matchRunDonorUnderlyingStatus = donorUnderlyingStatus;

	matchRunNodeTransplantationStatus = nodeTransplantationStatus;

	matchRunMatchMatrix = matchMatrix;
	matchRunIncidenceMatrix = incidenceMatrix;
	matchRunIncidenceMatrixReduced = incidenceMatrixReduced;

	//Retrieve Relevant Parameters
	maxChainLength = params->getMaxChainLength();
	maxCycleSize = params->getMaxCycleSize();
	maxLRSSize = params->getMaxLRSSize();
	maximum = std::max(maxChainLength + 1, maxCycleSize);

	timeline = params->getTimeline();
	processingTime = params->getProcessingTime();
	estimateEU = params->estimateExpectedUtility();

	utilityScheme = params->getUtilityScheme();

	addAdvantageToHighPRACandidates = params->addAdvantageToHighPRACandidates();
	praAdvantageCutoff = params->getPRAAdvantageCutoff();
	praAdvantageValue = params->getPRAAdvantageValue();
	allowABBridgeDonors = params->allowABBridgeDonors();

	numberOfExpectedUtilityIterations = params->getNumberOfExpectedUtilityIterations();

	//Set Match Run Values
	rngExpectedUtilityEstimation.setSeed(params->getRNGSeedExpectedUtilityEstimation() * currentIteration + currentMatchRun);

	matchRunNumberOfPairs = 0;
	matchRunNumberOfNDDs = 0;

	// Node is added to current match run only if it is NOT_TRANSPLANTED and STATUS_ACTIVE	
	matchRunLog << "Active Pool: ";
	currentMatchRunNodes.push_back(0);
	std::vector<int> tempNodes;

	// Order nodes with NDDs in front
	for (unsigned i = 1; i < nodeVector.size(); i++) {
		//If pair is active and not already transplanted
		if (nodeTransplantationStatus[i] == NOT_TRANSPLANTED && getNodeStatus(i) == STATUS_ACTIVE) {
			if (nodeTypeVector[i] == PAIR) {
				tempNodes.push_back(i);
				matchRunNumberOfPairs++;
			}
			else {
				currentMatchRunNodes.push_back(i);
				matchRunLog << i << " ";
				matchRunNumberOfNDDs++;
			}
		}
	}
	for (int i = 0; i < tempNodes.size(); i++) {
		currentMatchRunNodes.push_back(tempNodes[i]);
		matchRunLog << tempNodes[i] << " ";
	}

	matchRunLog << std::endl;

	// Save Size of Current Match Run
	matchRunNumberOfNodes = matchRunNumberOfPairs + matchRunNumberOfNDDs;

	// Build Match Run Specific Incidence Matrices
	matchRunSubIncidenceMatrix.assign(1 + matchRunNumberOfNodes, std::vector<bool>(1 + matchRunNumberOfNodes, false));
	matchRunSubIncidenceMatrixReduced.assign(1 + matchRunNumberOfNodes, std::vector<bool>(1 + matchRunNumberOfNodes, false));
	
	for (int i = 1; i <= matchRunNumberOfNodes; i++) {
		for (int j = 1; j <= matchRunNumberOfNodes; j++) {
			matchRunSubIncidenceMatrix[i][j] = incidenceMatrix[currentMatchRunNodes[i]][currentMatchRunNodes[j]];
			if (nodeTypeVector[currentMatchRunNodes[j]] == PAIR) {
				matchRunSubIncidenceMatrixReduced[i][j] = incidenceMatrix[currentMatchRunNodes[i]][currentMatchRunNodes[j]];
			}
		}
	}
}

KPDMatchRun::~KPDMatchRun() {	

}


void KPDMatchRun::collectCyclesAndChainsForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements) {	

	// Depth-First Search for Cycles and Chains
	int start = 1;
	std::vector<int> visitedVector(1 + matchRunNumberOfNodes, 0);
	std::vector<int> stack_vec;

	while (start <= matchRunNumberOfNodes) {
		visitedVector[start] = 1;
		stack_vec.push_back(start);
		int v = getChild(start, start, visitedVector, matchRunSubIncidenceMatrix);

		while (!stack_vec.empty()) {
			if (v == -1) {
				int top = stack_vec.back();
				stack_vec.pop_back();
				if (top == start) {
					visitedVector[top] = 0;
					break;
				}
				visitedVector[top] = 0;
				v = getChild(top, stack_vec.back(), visitedVector, matchRunSubIncidenceMatrix);
			}
			else {
				visitedVector[v] = 1;
				stack_vec.push_back(v);

				//Potential Cycle or Chain Found!
				if (matchRunSubIncidenceMatrix[v][start]) {

					int multipleNDDCheck = 0;
					int index = 0;

					//Copy Potential Cycle Or Chain
					std::vector<int> potentialCycleOrChain;
					for (unsigned i = 0; i < stack_vec.size(); i++) {
						potentialCycleOrChain.push_back(currentMatchRunNodes[stack_vec[i]]);
					}

					//Count Non-Directed Donors; Save Index of Non-Directed Donor
					for (unsigned i = 0; i < potentialCycleOrChain.size(); i++) {
						if (matchRunNodeTypeVector[potentialCycleOrChain[i]] != PAIR) {
							multipleNDDCheck++;
							index = i;
						}
					}

					//If there is 1 NDD, we're dealing with a chain
					if (multipleNDDCheck == 1) {
						//Check if size of chain is appropriate
						if ((int)potentialCycleOrChain.size() <= maxChainLength + 1) {
							//Queue up NDD to front of chain
							while (index > 0) {
								int temp = *(potentialCycleOrChain.begin());
								potentialCycleOrChain.erase(potentialCycleOrChain.begin());
								potentialCycleOrChain.push_back(temp);
								index--;
							}

							//If AB bridge donors are allowed, we're all set!
							if (allowABBridgeDonors) {
								currentMatchRunArrangements.push_back(potentialCycleOrChain);
							}
							else {
								bool exclude = true;

								//If any of the active associated donors have a blood type other than AB, then we're good
								for (int k = 1; k <= matchRunNodeVector[*(potentialCycleOrChain.end() - 1)]->getNumberOfAssociatedDonors(); k++) {
									if (matchRunDonorUnderlyingStatus[*(potentialCycleOrChain.end() - 1)][k] == STATUS_ACTIVE && matchRunNodeVector[*(potentialCycleOrChain.end() - 1)]->getDonorBT(k) != BT_AB) {
										exclude = false;
									}
								}

								if (!exclude) {
									currentMatchRunArrangements.push_back(potentialCycleOrChain);
								}
							}
						}
					}
					// If there are no NDDs, we're dealing with a cycle
					else if (multipleNDDCheck == 0) {
						//Check if size of cycle is appropriate; if it is, we're good to go!
						if ((int)potentialCycleOrChain.size() <= maxCycleSize) {
							currentMatchRunArrangements.push_back(potentialCycleOrChain);
						}
					}
				}

				if ((int)stack_vec.size() >= maximum)
					v = -1;
				else
					v = getChild(start, v, visitedVector, matchRunSubIncidenceMatrix);

			}
		}
		start++;
	}

	//Output
	matchRunLog << "Cycles/Chains: " << currentMatchRunArrangements.size() << std::endl;
}

void KPDMatchRun::collectLRSForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements) {

	//Only need to run NDD sub-routine if there are NDDs in the match run (...herp de derp)
	if (matchRunNumberOfNDDs > 0) {
		getLRSWithNDDs(currentMatchRunArrangements, matchRunNumberOfNDDs, matchRunSubIncidenceMatrixReduced);
	}

	getLRSPairsOnly(currentMatchRunArrangements, matchRunSubIncidenceMatrixReduced);
}

// Assign Expected Utilities

void KPDMatchRun::assignUtilitiesForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements) {

	// Iterating through cycles and chains...
	for (std::vector<std::vector<int> >::iterator itArrangements = currentMatchRunArrangements.begin(); itArrangements != currentMatchRunArrangements.end(); itArrangements++) {
		double util = 0;

		//... and sum over all potential transplants...
		for (std::vector<int>::iterator itNodes = (*itArrangements).begin(); itNodes != (*itArrangements).end() - 1; itNodes++) {

			//... using the best potential matching donor to assign the value
			int bestDonorIndex = selectDonor(*itNodes, *(itNodes + 1));

			//5 year survival
			if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
				util += 1;
			}
			else {
				util += matchRunMatchMatrix[*itNodes][*(itNodes + 1)][bestDonorIndex]->getUtility(utilityScheme);
			}

			// Add additional advantage if specified
			if (addAdvantageToHighPRACandidates && matchRunNodeVector[*(itNodes + 1)]->getCandidatePRA() >= praAdvantageCutoff) {
				util += praAdvantageValue;
			}
		}

		// For cycles, add the final transplant back to the original candidate
		if (matchRunNodeTypeVector[*(itArrangements->begin())] == PAIR) {
			int bestDonorIndex = selectDonor(*(itArrangements->end() - 1), *(itArrangements->begin()));


			if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
				util += 1;
			}
			else {
				util += matchRunMatchMatrix[*(itArrangements->end() - 1)][*(itArrangements->begin())][bestDonorIndex]->getUtility(utilityScheme);
			}

			if (addAdvantageToHighPRACandidates && matchRunNodeVector[*(itArrangements->begin())]->getCandidatePRA() >= praAdvantageCutoff) {
				util += praAdvantageValue;
			}
		}

		assignedValueOfCurrentMatchRunArrangements.push_back(util);
	}
}

void KPDMatchRun::assignExpectedUtilitiesForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements) {

	int i = 0;

	// Iterating through arrangements...
	for (std::vector<std::vector<int> >::iterator itArrangements = currentMatchRunArrangements.begin(); itArrangements != currentMatchRunArrangements.end(); itArrangements++) {
		i++;
		double eu = 0;

		// ... and either estimate or calculate expected utility
		if (estimateEU) {
			eu = estimateExpectedUtility(*itArrangements);
			//std::cout << i << std::endl;

			//bool print = false;

			//if (i == 5) {
			//print = true;
			//}

			//eu = estimateExpectedUtilityMatrix(*itArrangements,print);
		}
		else {
			eu = calculateExpectedUtility(*itArrangements);
		}

		//std::cout << i << std::endl;

		assignedValueOfCurrentMatchRunArrangements.push_back(eu);
	}
}

// Select Arrangements by Optimization

void KPDMatchRun::getOptimalSolutionForCurrentMatchRun(KPDOptimizationScheme scheme, std::vector<int> & optimalSolution, std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements) {

	// If there are no arrangements, no need to do anything
	int nArrangements = (int)currentMatchRunArrangements.size();
	if (nArrangements == 0) return;

	double objective = 0.0;

	GRBEnv *env = 0;	//Created outside try; needs to be recollected;
	GRBVar *vars = 0;	//Created outside try; needs to be recollected;
	GRBVar *myVars = new GRBVar[nArrangements + 1]; //Created outside try; needs to be recollected;
	
	try {
		env = new GRBEnv();
		env->set(GRB_IntParam_Threads, 4); // Limit the threads

		GRBModel model = GRBModel(*env);
		model.getEnv().set(GRB_IntParam_OutputFlag, 0);// 0 means no solver output; 1 otherwise;
													  
	   // Set utilities on arrangements
		for (int i = 1; i <= nArrangements; i++) {
			std::string s = "cycle_" + KPDFunctions::intToString(i);
			myVars[i] = model.addVar(0.0, 1.0, -1 * assignedValueOfCurrentMatchRunArrangements[i - 1], GRB_BINARY, s); //The default is min, hence the -1
		}
		model.update();

		// Restriction: Each vertex can only appear at most once in solution
		for (unsigned i = 1; i < currentMatchRunNodes.size(); i++) {
			int index = currentMatchRunNodes[i];
			GRBLinExpr expr = 0;
			for (int j = 0; j <= nArrangements - 1; j++) {
				for (std::vector<int>::iterator it = currentMatchRunArrangements[j].begin(); it != currentMatchRunArrangements[j].end(); ++it) {
					if (*it == index) {
						expr += myVars[j + 1];
					}
				}
			}
			std::string s = "vertex_" + KPDFunctions::intToString(i);
			model.addConstr(expr, GRB_LESS_EQUAL, 1.0, s); // Disjoint cycles and chains
		}

		// Optimize
		model.optimize();

		// Retrive Value of Solution
		int optimstatus = model.get(GRB_IntAttr_Status);
		if (optimstatus == GRB_OPTIMAL) { // GRB_OPTIMAL when the MIPGap is reached, default is 1e-4;
			objective = model.get(GRB_DoubleAttr_ObjVal);
		}
		else if (optimstatus == GRB_INF_OR_UNBD) {
			matchRunLog << "Model is infeasible or unbounded" << std::endl;
			return;
		}
		else if (optimstatus == GRB_INFEASIBLE) {
			matchRunLog << "Model is infeasible" << std::endl;
			return;
		}
		else if (optimstatus == GRB_UNBOUNDED) {
			matchRunLog << "Model is unbounded" << std::endl;
			return;
		}
		else {
			matchRunLog << "Optimization was stopped with status = " << optimstatus << std::endl;
			return;
		}

		// Retrieve Solution
		int numvars = model.get(GRB_IntAttr_NumVars);
		vars = model.getVars();
		for (int j = 0; j < numvars; j++) {
			GRBVar v = vars[j];
			if (v.get(GRB_DoubleAttr_X) == 1) {
				optimalSolution.push_back(j);
			}
		}
	}
	catch (GRBException e) {
		matchRunLog << "Error code = " << e.getErrorCode() << std::endl;
		matchRunLog << e.getMessage() << std::endl;
	}
	catch (...) {
		matchRunLog << "Error during optimization" << std::endl;
	}

	delete[] myVars;
	delete[] vars;
	delete env;

	// Output
	std::cout << "Optimization: " << (-1)*objective << std::endl;

	matchRunLog << "Optimization: " << (-1)*objective << std::endl;
}

std::string KPDMatchRun::printLog(){

	return matchRunLog.str();
}



//// Helper Functions

// Simulation Functions

KPDStatus KPDMatchRun::getNodeStatus(int nodeID) {

	//If the candidate is withdrawn, then the entire node is withdrawn
	if (matchRunCandidateUnderlyingStatus[nodeID] == STATUS_WITHDRAWN) {
		return STATUS_WITHDRAWN;
	}
	else {

		//If the candidate is inactive, then the node is inactive
		if (matchRunCandidateUnderlyingStatus[nodeID] == STATUS_INACTIVE) {
			return STATUS_INACTIVE;
		}
		else {

			//If the candidate is active, node status is based on donors
			//If any donor is active, node is active
			//If all donors are withdrawn, node is withdrawn
			//Otherwise, node is inactive

			int withdrawnDonors = 0;

			for (int donorID = 1; donorID <= matchRunNodeVector[nodeID]->getNumberOfAssociatedDonors(); donorID++) {
				if (matchRunDonorUnderlyingStatus[nodeID][donorID] == STATUS_ACTIVE) { // Active donor exists
					return STATUS_ACTIVE;
				}
				else if (matchRunDonorUnderlyingStatus[nodeID][donorID] == STATUS_WITHDRAWN) {
					withdrawnDonors++;
				}
			}

			if (withdrawnDonors == matchRunNodeVector[nodeID]->getNumberOfAssociatedDonors()) {
				return STATUS_WITHDRAWN;
			}
			else {
				return STATUS_INACTIVE;
			}
		}
	}
}

int KPDMatchRun::selectDonor(int donorNodeID, int candidateNodeID) {
	double maxUtil = 0.0;
	int maxDonorID = -1; // -1 means no donor is selected

	//Iterate through donors
	for (int donorID = 1; donorID <= matchRunNodeVector[donorNodeID]->getNumberOfAssociatedDonors(); donorID++) {

		//Only consider donors that match between the donor node and the candidate node, and that are active!
		if (matchRunMatchMatrix[donorNodeID][candidateNodeID][donorID]->getIncidence() && matchRunDonorUnderlyingStatus[donorNodeID][donorID] == STATUS_ACTIVE) {

			//If the donor provides more utility than the current max donor, replace with new donor, and update max utility value

			if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
				if (maxUtil == 0.0) {
					maxDonorID = donorID;
					maxUtil = 1.0;
				}
			}
			else {
				if (matchRunMatchMatrix[donorNodeID][candidateNodeID][donorID]->getUtility(utilityScheme) > maxUtil) {
					maxDonorID = donorID;
					maxUtil = matchRunMatchMatrix[donorNodeID][candidateNodeID][donorID]->getUtility(utilityScheme);
				}
			}
		}

		donorID++;
	}

	return maxDonorID;
}


int KPDMatchRun::getChild(int lower, int current, std::vector<int> &visitedVector, std::vector<std::vector<bool> > &incidence) {
	int nV = (int)visitedVector.size() - 1;
	for (int j = lower + 1; j <= nV; j++) {
		if (incidence[current][j] == true && visitedVector[j] == 0)
			return j;
	}
	return -1;
}

// For Collecting Arrangements

void KPDMatchRun::getLRSPairsOnly(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<std::vector<bool> > &incidence) {

	std::vector<int> pairs;
	pairs.push_back(0);

	for (int i = 1; i <= matchRunNumberOfNodes; i++) {
		if (matchRunNodeTypeVector[currentMatchRunNodes[i]] == PAIR) {
			pairs.push_back(i);
		}
	}

	int numberOfPairs = (int)pairs.size() - 1;

	std::vector<std::vector<bool> > reverseMatrix;
	reverseMatrix.assign(1 + numberOfPairs, std::vector<bool>(1 + numberOfPairs, false));

	for (int i = 1; i <= numberOfPairs; i++) {
		for (int j = 1; j <= numberOfPairs; j++) {
			if (i != j) {
				reverseMatrix[j][i] = incidence[pairs[i]][pairs[j]];
			}
		}
	}

	//Collect LRSs	
	std::vector<int> bfsTree;
	std::vector<int> bfsLevel;
	std::vector<bool> visited(1 + numberOfPairs, false);

	int next;
	int child;
	int childLevel;

	//Iterate Through Pairs	
	for (int i = 1; i <= numberOfPairs; i++) {
		//Start the with the first pair
		bfsTree.push_back(i);
		bfsLevel.push_back(0);
		visited[i] = true;

		//"next" is the next node
		next = i + 1;
		childLevel = 1;

		//While there are still pairs in the bfsTree
		while (!bfsTree.empty()) {

			// Get child
			child = getChildLRSPairsOnly(incidence, bfsTree, bfsLevel, currentMatchRunNodes, pairs, visited, next, childLevel);

			// If (i) the size of the bfsTree is at the limit or (ii) no children, bfsLevel is currently at the next bfsLevel of the previous node
			if ((int)bfsTree.size() == maxLRSSize || (child == -1 && childLevel == bfsLevel.back() + 1)) {
				next = bfsTree.back() + 1;
				childLevel = bfsLevel.back();

				visited[bfsTree.back()] = false;

				bfsTree.pop_back();
				bfsLevel.pop_back();
			}
			// If no children, bfsLevel is currently at the same bfsLevel of the previous node
			else if (child == -1 && childLevel == bfsLevel.back()) {
				next = i + 1;
				childLevel = bfsLevel.back() + 1;
			}
			// Otherwise
			else {

				bfsTree.push_back(child);
				bfsLevel.push_back(childLevel);

				visited[child] = true;

				int treeSize = (int)bfsTree.size();

				std::vector<bool> reverseVisited;
				reverseVisited.assign(1 + numberOfPairs, false);

				std::queue<int> reverseBFSTree;
				reverseBFSTree.push(bfsTree[0]);
				reverseVisited[bfsTree[0]] = true;

				int curr = 1;
				while (!reverseBFSTree.empty()) {
					int u = reverseBFSTree.front();
					reverseBFSTree.pop();

					for (int k = curr; k < treeSize; k++) {
						if (reverseVisited[bfsTree[k]] == false && reverseMatrix[u][bfsTree[k]] == true) {
							reverseVisited[bfsTree[k]] = true;
							reverseBFSTree.push(bfsTree[k]);
						}
					}
				}

				bool reverseTreeIsConnected = true;

				for (int k = 0; k < treeSize; k++) {
					if (!reverseVisited[bfsTree[k]]) {
						reverseTreeIsConnected = false;
						break;
					}
				}

				if (reverseTreeIsConnected) {

					std::vector<int> newLRS;
					for (std::vector<int>::iterator itTree = bfsTree.begin(); itTree != bfsTree.end(); itTree++) {
						newLRS.push_back(currentMatchRunNodes[pairs[*itTree]]);
					}

					if (validateLRSBounds(newLRS, matchRunNumberOfNodes)) {
						currentMatchRunArrangements.push_back(newLRS);
					}
				}

				next = child + 1;
			}
		}
	}
}

int KPDMatchRun::getChildLRSPairsOnly(std::vector<std::vector<bool> > &incidence, std::vector<int> &tree, std::vector<int> &level, std::vector<int> &nodes, std::vector<int> &pairs, std::vector<bool> &visited, int next, int childLevel) {

	int size = (int)pairs.size() - 1;

	for (int child = next; child <= size; child++) {
		//Can only visit children not already visited
		if (visited[child] == false) {
			//Go through previous nodes
			for (int i = 0; i<(int)level.size(); i++) {
				//At low levels, make sure potential node is not a direct successor
				if (level[i] < childLevel - 1) {
					if (incidence[pairs[tree[i]]][pairs[child]]) {
						break;
					}
				}
				//At previous level, if potential node is a successor, it is the next child
				if (level[i] == childLevel - 1) {
					if (incidence[pairs[tree[i]]][pairs[child]]) {
						return child;
					}
				}
			}
		}
	}

	return -1;
}

void KPDMatchRun::getLRSWithNDDs(std::vector<std::vector<int> > & currentMatchRunArrangements, int numberOfNDDs, std::vector<std::vector<bool> > &incidence) {

	std::vector<std::vector<int> > listOfBFSTrees;
	std::vector<std::vector<int> > listOfBFSLevels;
	std::vector<int> inducedBFSLevel;

	std::vector<std::vector<bool> > adjacentToLowerLevels;

	std::vector<bool> visited(matchRunNumberOfNodes, false);

	int child;
	int next;
	int nextNDD;

	int childLevel;
	int childLevelNDD;
	int treeSize;

	bool childCanBeNDD;
	bool childIsAdjacentToLowerLevels;

	for (int i = 0; i < numberOfNDDs; i++) {

		std::vector<int> newBFSTree;
		newBFSTree.push_back(i);
		listOfBFSTrees.push_back(newBFSTree);

		treeSize = 1;

		visited[i] = true;

		std::vector<int> newBFSLevel;
		newBFSLevel.push_back(0);
		listOfBFSLevels.push_back(newBFSLevel);

		inducedBFSLevel.push_back(0);

		std::vector<bool> newAdjacencyList;
		newAdjacencyList.push_back(false);
		adjacentToLowerLevels.push_back(newAdjacencyList);

		next = numberOfNDDs; //First regular pair
		nextNDD = i + 1;
		childLevel = 1;
		childLevelNDD = 1;
 		childCanBeNDD = false;

		while (!listOfBFSTrees.empty()) {

			child = getChildLRSWithNDDs(incidence, childIsAdjacentToLowerLevels, listOfBFSTrees, listOfBFSLevels, inducedBFSLevel,
				visited, next, nextNDD, childLevel, childLevelNDD, childCanBeNDD, numberOfNDDs, matchRunNumberOfNodes);

			if (treeSize == maxLRSSize || (child == matchRunNumberOfNodes && listOfBFSLevels.back().back() + 1 == childLevel)) {
				if (listOfBFSTrees.back().back() < numberOfNDDs) {
					if (listOfBFSTrees.size() > 1) {
						//If previous super tree level is one higher than the super tree level before it
						if (inducedBFSLevel.back() == inducedBFSLevel[inducedBFSLevel.size() - 2] + 1) {
							//Assign the before super tree level to child level
							childLevel = listOfBFSLevels[listOfBFSLevels.size() - 2].back() + 1;
							//Assign the most recent super tree level to child level NDD
							childLevelNDD = inducedBFSLevel.back();

							//Assign next to the first listed pair
							next = numberOfNDDs;
						}
						else {
							childLevel = listOfBFSLevels[listOfBFSLevels.size() - 2].back();
							childLevelNDD = inducedBFSLevel.back();

							next = std::max(numberOfNDDs, listOfBFSTrees[listOfBFSTrees.size() - 2].back() + 1);
						}
					}

					inducedBFSLevel.pop_back();

					nextNDD = listOfBFSTrees.back().back() + 1;
					childCanBeNDD = true;

					visited[listOfBFSTrees.back().back()] = false;

					listOfBFSTrees.pop_back();
					listOfBFSLevels.pop_back();
					adjacentToLowerLevels.pop_back();
				}

				else {
					nextNDD = numberOfNDDs;
					next = listOfBFSTrees.back().back() + 1;
					if (listOfBFSLevels.back().back() == listOfBFSLevels.back()[listOfBFSLevels.back().size() - 2] + 1) {
						childLevelNDD = inducedBFSLevel.back() + 1;
					}
					else {
						childLevelNDD = inducedBFSLevel.back();
					}

					childLevel = listOfBFSLevels.back().back();

					if (adjacentToLowerLevels.back().back() == true) {
						childCanBeNDD = false;
					}

					visited[listOfBFSTrees.back().back()] = false;
					listOfBFSTrees.back().pop_back();
					listOfBFSLevels.back().pop_back();
					adjacentToLowerLevels.back().pop_back();
				}
				treeSize--;
			}

			else if (child == matchRunNumberOfNodes && listOfBFSLevels.back().back() == childLevel) {
				next = numberOfNDDs;
				nextNDD = i + 1;
				childLevel = childLevel + 1;
				childLevelNDD = childLevelNDD + 1;
			}

			else {
				if (child < numberOfNDDs) {

					std::vector<int> newBFSTree;
					newBFSTree.push_back(child);
					listOfBFSTrees.push_back(newBFSTree);

					std::vector<int> newBFSLevel;
					newBFSLevel.push_back(0);
					listOfBFSLevels.push_back(newBFSLevel);

					inducedBFSLevel.push_back(childLevelNDD);

					std::vector<bool> newAdjacencyList;
					newAdjacencyList.push_back(childIsAdjacentToLowerLevels);
					adjacentToLowerLevels.push_back(newAdjacencyList);

					next = numberOfNDDs;
					nextNDD = child + 1;
					childLevel = 0;

					if (childIsAdjacentToLowerLevels == false) {
						childCanBeNDD = false;
					}
				}

				else {
					listOfBFSTrees.back().push_back(child);
					listOfBFSLevels.back().push_back(childLevel);
					if (childCanBeNDD == true) {
						adjacentToLowerLevels.back().push_back(false);
					}
					else {
						adjacentToLowerLevels.back().push_back(childIsAdjacentToLowerLevels);

						if (childIsAdjacentToLowerLevels == true || (listOfBFSTrees.size() == 1 && listOfBFSTrees.front().size() == 2)) {
							childCanBeNDD = true;
						}
					}

					next = child + 1;
					nextNDD = listOfBFSTrees.back().at(0) + 1;
					childLevelNDD = inducedBFSLevel.back();
				}

				treeSize++;
				visited[child] = true;
				if (childCanBeNDD == true) {
					std::vector<int> newLRS;

					for (int i = 0; i < (int)listOfBFSTrees.size(); i++) {
						for (int j = 0; j < (int)listOfBFSTrees[i].size(); j++) {
							newLRS.push_back(currentMatchRunNodes[listOfBFSTrees[i][j] + 1]);
						}
					}

					if (validateLRSBounds(newLRS, matchRunNumberOfNodes)) {
						currentMatchRunArrangements.push_back(newLRS);
					}
				}
			}
		}
	}
}

int KPDMatchRun::getChildLRSWithNDDs(std::vector<std::vector<bool> > &incidence, bool &childIsAdjacentToLowerLevels, std::vector<std::vector<int> > &trees, std::vector<std::vector<int> > &levels, std::vector<int> &inducedLevels,
	std::vector<bool> &visited, int &next, int &nextNDD, int &childLevel, int &childLevelNDD, bool &childCanBeNDD, int &ndds, int &matchRunNumberOfNodes) {

	bool breakFlag;

	if (childCanBeNDD && !(inducedLevels.size() == 1 && childLevelNDD == 0)) {
		//Iterate through NDDs as potential child
		for (int child = nextNDD; child < ndds; child++) {
			if (visited[child] == false) {
				breakFlag = false;

				childIsAdjacentToLowerLevels = false;

				for (int i = 0; i < (int)trees.size(); i++) {
					if (inducedLevels[i] < childLevelNDD - 1) {
						for (int j = 0; j < trees[i].size(); j++) {
							int node = trees[i][j] + 1;
							int potentialChild = child + 1;

							if (incidence[node][potentialChild] || incidence[potentialChild][node]) {
								breakFlag = true;
								break;
							}
						}
					}
					else if (inducedLevels[i] == childLevelNDD - 1) {
						if (trees[i][0] < child) {
							for (int j = 0; j < trees[i].size(); j++) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (incidence[node][potentialChild]) {
									breakFlag = true;
									break;
								}

								if (incidence[potentialChild][node]) {
									childIsAdjacentToLowerLevels = true;
								}
							}
						}
						else {
							for (int j = 0; j < trees[i].size(); j++) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (incidence[potentialChild][node]) {
									breakFlag = true;
									break;
								}

								if (incidence[node][potentialChild]) {
									childIsAdjacentToLowerLevels = true;
								}
							}
						}
					}

					if (breakFlag) {
						break;
					}
				}

				if (breakFlag == false) {
					return child;
				}
			}
		}
	}

	if (!(levels.back().size() == 1 && childLevel == 0)) {
		//Start at "Next", iterate through all pairs.
		for (int child = next; child < matchRunNumberOfNodes; child++) {
			if (visited[child] == false) {
				breakFlag = false;

				childIsAdjacentToLowerLevels = false;

				for (int i = 0; i < (int)trees.size(); i++) {
					//Iterator is at last trees.
					if (i == int(trees.size()) - 1) {
						//Iterate through current trees
						for (int j = 0; j < (int)trees[i].size(); j++) {

							//Current Node is at a low level
							if (levels[i][j] < childLevel - 1) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (incidence[node][potentialChild]) {
									breakFlag = true;
									break;
								}
							}

							//Current node is at previous level
							else if (levels[i][j] == childLevel - 1) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (incidence[node][potentialChild]) {
									return child;
								}
							}
						}
					}
					//Induced BFS level is at low level
					else if (inducedLevels[i] < inducedLevels.back() - 1) {
						for (int j = 0; j < (int)trees[i].size(); j++) {
							int node = trees[i][j] + 1;
							int potentialChild = child + 1;

							if (incidence[node][potentialChild] || incidence[potentialChild][node]) {
								breakFlag = true;
								break;
							}
						}
					}
					//Induced BFS level is at previous level
					else if (inducedLevels[i] == inducedLevels.back() - 1) {
						//if root of current trees is less than root of most recent trees
						if (trees[i][0] < trees.back()[0]) {
							//Iterate through nodes
							for (int j = 0; j < trees[i].size(); j++) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (incidence[node][potentialChild]) {
									breakFlag = true;
									break;
								}

								if (incidence[potentialChild][node]) {
									childIsAdjacentToLowerLevels = true;
								}
							}
						}
						else {
							//Iterate through nodes 
							for (int j = 0; j < trees[i].size(); j++) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (incidence[potentialChild][node]) {
									breakFlag = true;
									break;
								}

								if (incidence[node][potentialChild]) {
									childIsAdjacentToLowerLevels = true;
								}
							}
						}
					}
					//Induced BFS level is at same level
					else if (inducedLevels[i] == inducedLevels.back()) {
						//If root of current trees is less than root of most recent trees
						if (trees[i][0] < trees.back()[0]) {
							//Iterate through current trees
							for (int j = 0; j < trees[i].size(); j++) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (incidence[node][potentialChild]) {
									breakFlag = true;
									break;
								}
							}
						}
					}

					if (breakFlag) {
						break;
					}
				}
			}
		}
	}

	return matchRunNumberOfNodes;
}


bool KPDMatchRun::validateLRSBounds(std::vector<int> &arrangement, int order) {


	// Obtain matrix of shortest path between any two nodes

	int arrangementSize = (int)arrangement.size();

	if (arrangementSize <= 1) {
		return false;
	}

	std::vector<std::vector<int> > shortestPathLengths(arrangementSize, std::vector<int>(arrangementSize, order));

	for (int i = 0; i < arrangementSize; i++) {
		shortestPathLengths[i][i] = 0;
	}

	for (int i = 1; i < arrangementSize; i++) {

		for (int j = 0; j < i; j++) {

			int sp = order;
			for (int q = 0; q <= i - 1; q++) {
				if (matchRunIncidenceMatrixReduced[arrangement[q]][arrangement[i]]) {
					sp = std::min(sp, shortestPathLengths[j][q] + 1);
				}
			}
			shortestPathLengths[j][i] = sp;

			sp = order;
			for (int q = 0; q <= i - 1; q++) {
				if (matchRunIncidenceMatrix[arrangement[i]][arrangement[q]]) {
					sp = std::min(sp, shortestPathLengths[q][j] + 1);
				}
			}
			shortestPathLengths[i][j] = sp;

		}

		for (int j = 0; j < i; j++) {
			for (int k = 0; k < j; k++) {

				int spIJ = shortestPathLengths[i][j];
				int spIK = shortestPathLengths[i][k];
				int spJI = shortestPathLengths[j][i];
				int spJK = shortestPathLengths[j][k];
				int spKI = shortestPathLengths[k][i];
				int spKJ = shortestPathLengths[k][j];

				shortestPathLengths[j][k] = std::min(spJK, spJI + spIK);
				shortestPathLengths[k][j] = std::min(spKJ, spKI + spIJ);
			}
		}

	}

	std::vector<bool> visited(arrangementSize, false);
	std::vector<int> stack;
	visited[0] = true;
	stack.push_back(0);
	int connections = 1;
	while (!stack.empty()) {
		for (int i = 0; i < arrangementSize; i++) {
			if (!visited[i]) {
				if (matchRunNodeTypeVector[arrangement[stack.front()]] != PAIR) {
					if (shortestPathLengths[stack.front()][i] <= maxChainLength + 1) {
						visited[i] = true;
						stack.push_back(i);
						connections++;
					}
				}
				else {
					if ((matchRunNodeTypeVector[arrangement[i]] == PAIR && shortestPathLengths[i][stack.front()] + shortestPathLengths[stack.front()][i] <= maxCycleSize) ||
						(matchRunNodeTypeVector[arrangement[i]] != PAIR && shortestPathLengths[i][stack.front()] <= maxChainLength + 1)) {

						visited[i] = true;
						stack.push_back(i);
						connections++;
					}
				}
			}
		}
		stack.erase(stack.begin());
	}

	return connections == arrangementSize;



}

// For Assigning Expected Utilities

double KPDMatchRun::calculateExpectedUtility(std::vector<int> & arrangement) {
	
	double nddAssumedProbability;
	double donorAssumedProbability;
	double candidateAssumedProbability;

	if (timeline == TIMELINE_FIXED) {
		nddAssumedProbability = 1;// 1 - (params->getAssumedNDDActiveToInactive() + params->getAssumedNDDActiveToWithdrawn());
		donorAssumedProbability = 1;// 1 - (params->getAssumedDonorActiveToInactive() + params->getAssumedDonorActiveToWithdrawn());
		candidateAssumedProbability = 1;// 1 - (params->getAssumedCandidateActiveToInactive() + params->getAssumedCandidateActiveToWithdrawn());
	}
	else {
		nddAssumedProbability = 1;// exp((-1)*(params->getAssumedNDDActiveToInactive() + params->getAssumedNDDActiveToWithdrawn())*params->getProcessingTime());
		donorAssumedProbability = 1;// exp((-1)*(params->getAssumedDonorActiveToInactive() + params->getAssumedDonorActiveToWithdrawn())*params->getProcessingTime());
		candidateAssumedProbability = 1;// exp((-1)*(params->getAssumedCandidateActiveToInactive() + params->getAssumedCandidateActiveToWithdrawn())*params->getProcessingTime());
	}

	int N = (int)arrangement.size();
	double utility = 0;

	//Collect and count all available donors within the arrangement
	std::vector<int> donorNodeIndices;
	std::vector<int> donorIndices;
	donorNodeIndices.push_back(0);
	donorIndices.push_back(0);

	int numberOfDonors = 0;

	int i = 1;
	for (std::vector<int>::iterator arrangementIt = arrangement.begin(); arrangementIt != arrangement.end(); arrangementIt++) {

		for (int k = 1; k <= matchRunNodeVector[*arrangementIt]->getNumberOfAssociatedDonors(); k++) {
			if (matchRunDonorUnderlyingStatus[*arrangementIt][k] == STATUS_ACTIVE) {
				donorNodeIndices.push_back(i);
				donorIndices.push_back(k);
				numberOfDonors++;
			}
		}
		i++;
	}

	//Iterates through each possible subset of nodes
	for (int d = 3; d < pow((double)2, numberOfDonors); d++) {

		std::vector<int> nodeFlags(numberOfDonors + 1, 0);

		int donorSubsetSize = KPDFunctions::setFlags(d, nodeFlags);

		//Must be at least 2 nodes
		if (donorSubsetSize > 1) {

			std::vector<std::vector<bool> > subsetDonorAvailability(1 + N, std::vector<bool>(1, false));
			std::vector<bool> subsetCandidateAvailability(1 + N, false);

			for (int i = 1; i <= N; i++) {
				int nodeNumberOfDonors = matchRunNodeVector[arrangement[i - 1]]->getNumberOfAssociatedDonors();
				subsetDonorAvailability[i].assign(1 + nodeNumberOfDonors, false);
			}
			//Iterate through nodes and mark availability
			for (int flag = 1; flag <= numberOfDonors; flag++) {

				int nodeIndex = donorNodeIndices[flag];
				int nodeID = arrangement[nodeIndex - 1];

				int donorID = donorIndices[flag];

				if (nodeFlags[flag] != 0) {
					subsetDonorAvailability[nodeIndex][donorID] = true;
					subsetCandidateAvailability[nodeIndex] = true;
				}
			}

			double probSubset = 1;

			//Calculate probability of subset
			for (int i = 1; i <= N; i++) {
				int nodeID = arrangement[i - 1];

				double probNode = 1;

				//Update with probability of candidate subset
				if (matchRunNodeTypeVector[nodeID] != PAIR) {
					for (int k = 1; k <= matchRunNodeVector[nodeID]->getNumberOfAssociatedDonors(); k++) {
						if (matchRunDonorUnderlyingStatus[nodeID][k] == STATUS_ACTIVE) {
							if (subsetDonorAvailability[i][k]) {
								if (matchRunNodeTypeVector[nodeID] == NDD) {
									probNode = probNode * nddAssumedProbability;
								}
								else {
									probNode = probNode * donorAssumedProbability;
								}
							}
							else {
								if (matchRunNodeTypeVector[nodeID] == NDD) {
									probNode = probNode * (1 - nddAssumedProbability);
								}
								else {
									probNode = probNode * (1 - donorAssumedProbability);
								}
							}
						}
					}
				}

				if (matchRunNodeTypeVector[nodeID] == PAIR) {

					if (subsetCandidateAvailability[i]) {

						for (int k = 1; k <= matchRunNodeVector[nodeID]->getNumberOfAssociatedDonors(); k++) {
							if (matchRunDonorUnderlyingStatus[nodeID][k] == STATUS_ACTIVE) {
								if (subsetDonorAvailability[i][k]) {
									probNode = probNode * donorAssumedProbability;
								}
								else {
									probNode = probNode * (1 - donorAssumedProbability);
								}
							}
						}

						probNode = probNode * candidateAssumedProbability;
					}
					else {

						for (int k = 1; k <= matchRunNodeVector[nodeID]->getNumberOfAssociatedDonors(); k++) {
							if (matchRunDonorUnderlyingStatus[nodeID][k] == STATUS_ACTIVE) {
								probNode = probNode * (1 - donorAssumedProbability);
							}
						}

						probNode = 1 - (1 - probNode) * candidateAssumedProbability;
					}
				}

				probSubset = probSubset * probNode;
			}

			if (probSubset > 0) {

				std::vector<int> edgeSubsetCandidateIndices;
				std::vector<int> edgeSubsetDonorNodeIndices;
				std::vector<int> edgeSubsetDonorIDs;

				edgeSubsetCandidateIndices.push_back(0);
				edgeSubsetDonorNodeIndices.push_back(0);
				edgeSubsetDonorIDs.push_back(0);

				int numberOfEdgesInDonorSubset = 0;

				//Collect possible edges
				for (int i = 1; i <= N; i++) {
					int donorNodeID = arrangement[i - 1];
					for (int j = 1; j <= N; j++) {
						if (i != j) {

							int candidateNodeID = arrangement[j - 1];

							if (matchRunIncidenceMatrix[donorNodeID][candidateNodeID] && matchRunNodeTypeVector[candidateNodeID] == PAIR) {

								for (int k = 1; k <= matchRunNodeVector[donorNodeID]->getNumberOfAssociatedDonors(); k++) {
									if (subsetDonorAvailability[i][k] && subsetCandidateAvailability[j] && matchRunMatchMatrix[donorNodeID][candidateNodeID][k]->getIncidence()) {
										edgeSubsetDonorNodeIndices.push_back(i);
										edgeSubsetCandidateIndices.push_back(j);
										edgeSubsetDonorIDs.push_back(k);

										numberOfEdgesInDonorSubset++;
									}
								}
							}
						}
					}
				}

				// Iterate through every possible combination of edges
				for (int e = 1; e < pow((double)2, numberOfEdgesInDonorSubset); e++) {

					std::vector<int> edgeFlags(1 + numberOfEdgesInDonorSubset, 0);

					int edgeSubsetSize = KPDFunctions::setFlags(e, edgeFlags);

					double probEdgeSubset = probSubset;

					std::vector<std::vector<bool> > reducedIncidenceMatrix(1 + N, std::vector<bool>(1 + N, false));
					std::vector<std::vector<std::vector<bool> > > reducedDonorIncidenceMatrix(1 + N, std::vector<std::vector<bool> >(1 + N, std::vector<bool>(1, false)));
					std::vector<std::vector<std::vector<double> > > reducedUtilityMatrix(1 + N, std::vector<std::vector<double> >(1 + N, std::vector<double>(1, 0.0)));

					std::vector<KPDNodeType> reducedNodeTypeVector(1 + N, PAIR);
					std::vector<std::vector<KPDBloodType> > reducedDonorBloodTypes(1 + N, std::vector<KPDBloodType>(1, BT_AB));

					//Build reduced matrices
					for (int i = 1; i <= N; i++) {

						int donorNodeID = arrangement[i - 1];

						int arrangementNodeDonors = matchRunNodeVector[donorNodeID]->getNumberOfAssociatedDonors();

						for (int j = 1; j <= N; j++) {
							reducedDonorIncidenceMatrix[i][j].assign(1 + arrangementNodeDonors, false);
							reducedUtilityMatrix[i][j].assign(1 + arrangementNodeDonors, 0.0);

							//Implicit backward edges
							int candidateNodeID = arrangement[j - 1];
							if (matchRunNodeTypeVector[donorNodeID] == PAIR && matchRunNodeTypeVector[candidateNodeID] != PAIR) {
								reducedIncidenceMatrix[i][j] = true;
							}
						}

						reducedNodeTypeVector[i] = matchRunNodeTypeVector[donorNodeID];

						reducedDonorBloodTypes[i].assign(1 + arrangementNodeDonors, BT_AB);
						for (int k = 1; k <= arrangementNodeDonors; k++) {
							reducedDonorBloodTypes[i][k] = matchRunNodeVector[donorNodeID]->getDonorBT(k);
						}
					}

					for (int l = 1; l <= numberOfEdgesInDonorSubset; l++) {

						int edgeSubsetDonorNodeIndex = edgeSubsetDonorNodeIndices[l];
						int edgeSubsetCandidateNodeIndex = edgeSubsetCandidateIndices[l];
						int edgeSubsetDonorID = edgeSubsetDonorIDs[l];

						int edgeSubsetDonorNodeID = arrangement[edgeSubsetDonorNodeIndex - 1];
						int edgeSubsetCandidateNodeID = arrangement[edgeSubsetCandidateNodeIndex - 1];

						if (edgeFlags[l] != 0) {

							probEdgeSubset = probEdgeSubset * matchRunMatchMatrix[edgeSubsetDonorNodeID][edgeSubsetCandidateNodeID][edgeSubsetDonorID]->getAssumedSuccessProbability();

							reducedDonorIncidenceMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex][edgeSubsetDonorID] = true;
							reducedIncidenceMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex] = true;

							if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
								if (matchRunNodeTypeVector[edgeSubsetCandidateNodeID] == PAIR) {
									reducedUtilityMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex][edgeSubsetDonorID] = 1;
								}
							}
							else {
								reducedUtilityMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex][edgeSubsetDonorID] = matchRunMatchMatrix[edgeSubsetDonorNodeID][edgeSubsetCandidateNodeID][edgeSubsetDonorID]->getUtility(utilityScheme);
							}

							if (addAdvantageToHighPRACandidates && matchRunNodeTypeVector[edgeSubsetCandidateNodeID] == PAIR && matchRunNodeVector[edgeSubsetCandidateNodeID]->getCandidatePRA() >= praAdvantageCutoff) {
								reducedUtilityMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex][edgeSubsetDonorID] += praAdvantageValue;
							}
						}
						else {
							probEdgeSubset = probEdgeSubset * (1 - matchRunMatchMatrix[edgeSubsetDonorNodeID][edgeSubsetCandidateNodeID][edgeSubsetDonorID]->getAssumedSuccessProbability());
						}
					}

					if (probEdgeSubset > 0) {

						double addUtil = calculatePartialUtility(N, reducedIncidenceMatrix, reducedUtilityMatrix, reducedNodeTypeVector, reducedDonorBloodTypes);

						if (addUtil > 0) {
							utility += probEdgeSubset*addUtil;
						}
					}
				}
			}
		}
	}

	return utility;
}

double KPDMatchRun::estimateExpectedUtility(std::vector<int> &arrangement) {


	double nddAssumedProbability;
	double donorAssumedProbability;
	double candidateAssumedProbability;

	if (timeline == TIMELINE_FIXED) {
		nddAssumedProbability = 1;// 1 - (params->getAssumedNDDActiveToInactive() + params->getAssumedNDDActiveToWithdrawn());
		donorAssumedProbability = 1;// 1 - (params->getAssumedDonorActiveToInactive() + params->getAssumedDonorActiveToWithdrawn());
		candidateAssumedProbability = 1;// 1 - (params->getAssumedCandidateActiveToInactive() + params->getAssumedCandidateActiveToWithdrawn());
	}
	else {
		nddAssumedProbability = 1;//exp((-1)*(params->getAssumedNDDActiveToInactive() + params->getAssumedNDDActiveToWithdrawn())*params->getProcessingTime());
		donorAssumedProbability = 1;//exp((-1)*(params->getAssumedDonorActiveToInactive() + params->getAssumedDonorActiveToWithdrawn())*params->getProcessingTime());
		candidateAssumedProbability = 1;// exp((-1)*(params->getAssumedCandidateActiveToInactive() + params->getAssumedCandidateActiveToWithdrawn())*params->getProcessingTime());
	}

	int N = (int)arrangement.size();

	double expUtility = 0;

	for (int sims = 1; sims <= numberOfExpectedUtilityIterations; sims++) {

		//Initialize random matrices
		std::vector<std::vector<bool> > randomIncidenceMatrix(1 + N, std::vector<bool>(1 + N, false));
		std::vector<std::vector<std::vector<bool> > > randomDonorIncidenceMatrix(1 + N, std::vector<std::vector<bool> >(1 + N, std::vector<bool>(1, false)));
		std::vector<std::vector<std::vector<double> > > randomUtilityMatrix(1 + N, std::vector<std::vector<double> >(1 + N, std::vector<double>(1, 0.0)));

		std::vector<bool> randomCandidateAvailabilityVector(1 + N, false);
		std::vector<std::vector<bool> > randomDonorAvailabilityVector(1 + N, std::vector<bool>(1, false));

		std::vector<KPDNodeType> subNodeTypeVector(1 + N, PAIR);
		std::vector<std::vector<KPDBloodType> > subDonorBloodTypeVector(1 + N, std::vector<KPDBloodType>(1, BT_AB));

		int i = 1;

		//Simulate candidate and donor availabilities, retrive donor information
		for (std::vector<int>::iterator itNodes = arrangement.begin(); itNodes != arrangement.end(); ++itNodes, i++) {

			//Store node type
			subNodeTypeVector[i] = matchRunNodeTypeVector[*itNodes];

			//Randomly generate donor availability (and store donor blood types)
			int numDonors = matchRunNodeVector[*itNodes]->getNumberOfAssociatedDonors();

			randomDonorAvailabilityVector[i].assign(1 + numDonors, false);
			subDonorBloodTypeVector[i].assign(1 + numDonors, BT_AB);

			for (int k = 1; k <= numDonors; k++) {

				if (matchRunDonorUnderlyingStatus[*itNodes][k] == STATUS_ACTIVE) {

					double u = rngExpectedUtilityEstimation.runif();

					if ((matchRunNodeTypeVector[*itNodes] == NDD && u < nddAssumedProbability) || (matchRunNodeTypeVector[*itNodes] != NDD && u < donorAssumedProbability)) {
						randomDonorAvailabilityVector[i][k] = true;

						if (matchRunNodeTypeVector[*itNodes] != PAIR) { // Add marker for NDD availability (acts as a shortcut for the next section)
							randomCandidateAvailabilityVector[i] = true;
						}

					}
					subDonorBloodTypeVector[i][k] = matchRunNodeVector[*itNodes]->getDonorBT(k);
				}
			}

			//Randomly generate candidate availability for PAIRs
			if (matchRunNodeTypeVector[*itNodes] == PAIR) {
				if (rngExpectedUtilityEstimation.runif() < candidateAssumedProbability) {
					randomCandidateAvailabilityVector[i] = true;
				}
			}
		}

		//Iterate through donor-candidate combinations and randomly generate realized transplants
		int j = 1;
		i = 1;

		for (std::vector<int>::iterator itDonors = arrangement.begin(); itDonors != arrangement.end(); ++itDonors, i++) {

			int numDonors = matchRunNodeVector[*itDonors]->getNumberOfAssociatedDonors();

			for (std::vector<int>::iterator itCandidates = arrangement.begin(); itCandidates != arrangement.end(); ++itCandidates, j++) {

				randomDonorIncidenceMatrix[i][j].assign(1 + numDonors, 0);
				randomUtilityMatrix[i][j].assign(1 + numDonors, 0.0);

				if (i != j) {

					//If both nodes are available
					if (randomCandidateAvailabilityVector[i] && randomCandidateAvailabilityVector[j]) {

						//If donor node is a PAIR and candidate node is a NDD or BRIDGE_DONOR, 
						//insert implicit backward edges from all available donors toward the NDD/BRIDGE node, and set the incidence matrix to true
						if (subNodeTypeVector[i] == PAIR && subNodeTypeVector[j] != PAIR) {
							for (int k = 1; k <= numDonors; k++) {
								if (randomDonorAvailabilityVector[i][k]) {
									randomDonorIncidenceMatrix[i][j][k] = true;
									randomIncidenceMatrix[i][j] = true;
								}
							}
						}
						//If candidate node is a PAIR, generate random matches
						else if (subNodeTypeVector[j] == PAIR) {

							//Iterate through donors
							for (int k = 1; k <= numDonors; k++) {
								// For existing original matches, and for donors that are available, randomly generate matches
								if (matchRunMatchMatrix[*itDonors][*itCandidates][k]->getIncidence() && randomDonorAvailabilityVector[i][k]) {

									if (rngExpectedUtilityEstimation.runif() < matchRunMatchMatrix[*itDonors][*itCandidates][k]->getAssumedSuccessProbability()) {

										randomDonorIncidenceMatrix[i][j][k] = 1;

										if (utilityScheme == UTILITY_TRANSPLANTS) {
											if (matchRunNodeTypeVector[*itCandidates] == PAIR) {
												randomUtilityMatrix[i][j][k] = 1;
											}
										}
										else {
											randomUtilityMatrix[i][j][k] = matchRunMatchMatrix[*itDonors][*itCandidates][k]->getUtility(utilityScheme);
										}

										if (addAdvantageToHighPRACandidates && matchRunNodeVector[*itCandidates]->getCandidatePRA() >= praAdvantageCutoff && matchRunNodeTypeVector[*itCandidates] == PAIR) {
											randomUtilityMatrix[i][j][k] += praAdvantageValue;
										}

										// Set incidence matrix to true
										randomIncidenceMatrix[i][j] = true;
									}
								}
							}
						}
					}
				}
			}

			j = 1;
		}

		//After generating matrices, calculate utility
		double partialUtil = calculatePartialUtility(N, randomIncidenceMatrix, randomUtilityMatrix, subNodeTypeVector, subDonorBloodTypeVector);
		expUtility += partialUtil;

	}

	//Expected utility is the average of the calculated utility values over the number of iterations
	return expUtility / numberOfExpectedUtilityIterations;

}

double KPDMatchRun::estimateExpectedUtilityMatrix(std::vector<int>& arrangement, bool print) {	

	double nddAssumedProbability;
	double donorAssumedProbability;
	double candidateAssumedProbability;

	if (timeline == TIMELINE_FIXED) {
		nddAssumedProbability = 1;// 1 - (params->getAssumedNDDActiveToInactive() + params->getAssumedNDDActiveToWithdrawn());
		donorAssumedProbability = 1;// 1 - (params->getAssumedDonorActiveToInactive() + params->getAssumedDonorActiveToWithdrawn());
		candidateAssumedProbability = 1;// 1 - (params->getAssumedCandidateActiveToInactive() + params->getAssumedCandidateActiveToWithdrawn());
	}
	else {
		nddAssumedProbability = 1;// exp((-1)*(params->getAssumedNDDActiveToInactive() + params->getAssumedNDDActiveToWithdrawn())*params->getProcessingTime());
		donorAssumedProbability = 1;// exp((-1)*(params->getAssumedDonorActiveToInactive() + params->getAssumedDonorActiveToWithdrawn())*params->getProcessingTime());
		candidateAssumedProbability = 1;// exp((-1)*(params->getAssumedCandidateActiveToInactive() + params->getAssumedCandidateActiveToWithdrawn())*params->getProcessingTime());
	}

	//Set up Incidence and Utility Matrices

	int nV = (int)arrangement.size();
	std::vector<std::vector<bool> > incidenceMatrix(1 + nV, std::vector<bool>(1 + nV, false));
	std::vector<std::vector<std::vector<int> > > donorMatrix(1 + nV, std::vector<std::vector<int> >(1 + nV, std::vector<int>(0, 0)));
	std::vector<std::vector<std::vector<int> > > edgeMatrix(1 + nV, std::vector<std::vector<int> >(1 + nV, std::vector<int>(0, 0)));
	std::vector<std::vector<std::vector<double> > > utilityMatrix(1 + nV, std::vector<std::vector<double> >(1 + nV, std::vector<double>(0, 0.0)));
	std::vector<std::vector<int> > donorsList;
	std::vector<std::vector<int> > edgeList;
	std::vector<double> utilityList;
	int nE = 0;

	for (int i = 1; i <= nV; i++) {

		int arrangementDonors = matchRunNodeVector[arrangement[i - 1]]->getNumberOfAssociatedDonors();

		for (int j = 1; j <= nV; j++) {
			if (i != j) {
				bool incident = false;

				if (matchRunNodeTypeVector[arrangement[i - 1]] == PAIR && matchRunNodeTypeVector[arrangement[j - 1]] != PAIR) {
					incident = true;
				}
				else if (matchRunNodeTypeVector[arrangement[j - 1]] == PAIR) {
					int index = 0;
					for (int k = 1; k <= arrangementDonors; k++) {
						if (matchRunMatchMatrix[arrangement[i - 1]][arrangement[j - 1]][k]->getIncidence()) {
							incident = true;
							double utility = 0.0;

							if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
								if (matchRunNodeTypeVector[arrangement[j - 1]] == PAIR) {
									utility += 1;
								}
							}
							else {
								utility += matchRunMatchMatrix[arrangement[i - 1]][arrangement[j - 1]][k]->getUtility(utilityScheme);
							}

							edgeMatrix[i][j].push_back(nE);
							utilityMatrix[i][j].push_back(utility);

							std::vector<int> edge;
							edge.push_back(i);
							edge.push_back(j);
							edge.push_back(index);

							edgeList.push_back(edge);
							utilityList.push_back(utility);

							nE++;
							index++;
						}
					}
				}

				incidenceMatrix[i][j] = incident;
			}
		}
	}

	//Find Sub Cycles and Sub Chains

	std::vector<std::vector<int> > subCyclesAndSubChains;

	int start = 1;
	std::vector<int> visitedVector(nV + 1, 0);
	std::vector<int> stack_vec;

	while (start <= nV) {
		visitedVector[start] = 1;

		stack_vec.push_back(start);
		int v = getChild(start, start, visitedVector, incidenceMatrix);
		while (!stack_vec.empty()) {
			if (v == -1) {
				int top = stack_vec.back();
				stack_vec.pop_back();
				if (top == start) {
					visitedVector[top] = 0;
					break;
				}
				visitedVector[top] = 0;
				v = getChild(top, stack_vec.back(), visitedVector, incidenceMatrix);
			}
			else {
				visitedVector[v] = 1;
				stack_vec.push_back(v);
				if (incidenceMatrix[v][start]) {

					int multipleNDDCheck = 0;
					int index = 0;

					std::vector<int> potentialCycleOrChain;

					for (unsigned i = 0; i < stack_vec.size(); i++) {
						potentialCycleOrChain.push_back(stack_vec[i]);
					}

					for (unsigned i = 0; i < potentialCycleOrChain.size(); i++) {
						if (matchRunNodeTypeVector[arrangement[potentialCycleOrChain[i] - 1]] != PAIR) {
							multipleNDDCheck++;
							index = i;
						}
					}

					//Chain
					if (multipleNDDCheck == 1 && potentialCycleOrChain.size() <= maxChainLength) {
						//Queue up NDD to front of chain
						while (index > 0) {
							int temp = *(potentialCycleOrChain.begin());
							potentialCycleOrChain.erase(potentialCycleOrChain.begin());
							potentialCycleOrChain.push_back(temp);
							index--;
						}

						if (!allowABBridgeDonors) {

							bool hasNonABBridgeDonor = false;

							for (int k = 1; k <= matchRunNodeVector[arrangement[*(potentialCycleOrChain.end() - 1) - 1]]->getNumberOfAssociatedDonors(); k++) {
								if (matchRunNodeVector[arrangement[*(potentialCycleOrChain.end() - 1) - 1]]->getDonorBT(k) != BT_AB) {
									hasNonABBridgeDonor = true;
								}
							}

							if (hasNonABBridgeDonor) {
								subCyclesAndSubChains.push_back(potentialCycleOrChain);
							}
						}
						else {
							subCyclesAndSubChains.push_back(potentialCycleOrChain);
						}
					}

					//Cycle
					else if (multipleNDDCheck == 0 && potentialCycleOrChain.size() <= maxCycleSize) {
						subCyclesAndSubChains.push_back(potentialCycleOrChain);
					}
				}

				if ((int)stack_vec.size() >= maximum)
					v = -1;
				else
					v = getChild(start, v, visitedVector, incidenceMatrix);
			}
		}
		start++;
	}

	// Extract Node and Edge Lists
	std::vector<std::vector<int> > subCycleVertexLists;
	std::vector<std::vector<int> > subCycleEdgeLists;
	std::vector<double> utilityOfSubCycles;

	for (std::vector<std::vector<int> >::iterator cycleIt = subCyclesAndSubChains.begin(); cycleIt != subCyclesAndSubChains.end(); cycleIt++) {


	}


	// Find Potential Solutions

	int nSubCyclesAndSubChains = (int)subCyclesAndSubChains.size();

	std::vector<std::vector<int> > possibleSolutions;

	for (int i = 0; i < nSubCyclesAndSubChains; i++) {

		std::vector<int> cycleFlags(nSubCyclesAndSubChains, 0);

		checkValidSolution(nSubCyclesAndSubChains, nV, cycleFlags, i, subCyclesAndSubChains, possibleSolutions);
	}

	if (print) {
		for (int s = 0; s < (int)possibleSolutions.size(); s++) {
			for (std::vector<int>::iterator it = possibleSolutions[s].begin(); it != possibleSolutions[s].end(); it++) {
				std::cout << *it << " ";
			}
			std::cout << std::endl;
		}
	}






	return 0.0;
}

void KPDMatchRun::checkValidSolution(int nSubCycles, int nVertices, std::vector<int> subCycleFlags, int next, std::vector<std::vector<int> > & subCyclesSubChains, std::vector<std::vector<int> > & possibleSolutions) {

	std::vector<int> tempSolution;

	subCycleFlags[next] = 1;

	std::vector<int> checkUniqueness(nVertices + 1, 0);
	bool validSolution = true;
	for (int q = 0; q <= next; q++) {
		if (subCycleFlags[q] == 1) {

			tempSolution.push_back(q);

			for (std::vector<int>::iterator it = subCyclesSubChains.at(q).begin(); it != subCyclesSubChains.at(q).end(); ++it) {
				checkUniqueness[*it]++;
				if (checkUniqueness[*it] > 1) {
					validSolution = false;
					break;
				}
			}
		}
		if (!validSolution) {
			break;
		}
	}

	if (validSolution) {

		possibleSolutions.push_back(tempSolution);

		if (next + 1 < nSubCycles) {
			for (int i = next + 1; i < nSubCycles; i++) {
				checkValidSolution(nSubCycles, nVertices, subCycleFlags, i, subCyclesSubChains, possibleSolutions);
			}
		}
	}

}


double KPDMatchRun::calculatePartialUtility(int nV, std::vector<std::vector<bool> > & incidence, std::vector<std::vector<std::vector<double> > > & utility,
	std::vector<KPDNodeType> & nodeTypes, std::vector<std::vector<KPDBloodType> > & donorBloodTypes) {

	std::vector<std::vector<int> > possibleCyclesOrChains;
	std::vector<double> utilityOfPossibleCyclesOrChains;

	int maximum = std::max(maxChainLength + 1, maxCycleSize);

	double utilityValue = 0;

	int start = 1;
	std::vector<int> visitedVec(nV + 1, 0);
	std::vector<int> stack_vec;

	while (start <= nV) {
		visitedVec[start] = 1;

		stack_vec.push_back(start);
		int v = getChild(start, stack_vec.back(), visitedVec, incidence);
		while (!stack_vec.empty()) {
			if (v == -1) {
				int top = stack_vec.back();
				stack_vec.pop_back();
				if (top == start) {
					visitedVec[top] = 0;
					break;
				}
				visitedVec[top] = 0;
				v = getChild(top, stack_vec.back(), visitedVec, incidence);
			}
			else {
				visitedVec[v] = 1;
				stack_vec.push_back(v);

				if (incidence[v][start] == true) {
					int multipleNDDCheck = 0;
					int index = 0;

					std::vector<int> potentialCycleOrChain;

					for (unsigned i = 0; i < stack_vec.size(); i++) {
						potentialCycleOrChain.push_back(stack_vec[i]);
						if (nodeTypes[stack_vec[i]] != PAIR) {
							multipleNDDCheck++;
							index = i;
						}
					}

					double tempUtil = 0;

					if (multipleNDDCheck == 1 && (int)potentialCycleOrChain.size() <= maxChainLength + 1) {

						while (index > 0) {
							int tempVec = *(potentialCycleOrChain.begin());
							potentialCycleOrChain.erase(potentialCycleOrChain.begin());
							potentialCycleOrChain.push_back(tempVec);
							index--;
						}

						if (!allowABBridgeDonors) {
							bool nonABBridgeDonor = false;
							for (int k = 1; k < donorBloodTypes[*(potentialCycleOrChain.end() - 1)].size(); k++) {
								if (donorBloodTypes[*(potentialCycleOrChain.end() - 1)][k] != BT_AB) {
									nonABBridgeDonor = true;
								}
							}

							if (nonABBridgeDonor) {
								for (std::vector<int>::iterator it = potentialCycleOrChain.begin(); it != potentialCycleOrChain.end() - 1; it++) {
									double selectedDonorUtility = 0;

									for (int k = 1; k < utility[*it][*(it + 1)].size(); k++) {
										if (utility[*it][*(it + 1)][k] > selectedDonorUtility) {
											selectedDonorUtility = utility[*it][*(it + 1)][k];
										}
									}
									tempUtil += selectedDonorUtility;
								}

								possibleCyclesOrChains.push_back(std::vector<int>(potentialCycleOrChain));
								utilityOfPossibleCyclesOrChains.push_back(tempUtil);
							}
						}
						else {
							for (std::vector<int>::iterator it = potentialCycleOrChain.begin(); it != potentialCycleOrChain.end() - 1; it++) {
								double selectedDonorUtility = 0;

								for (int k = 1; k < (int)utility[*it][*(it + 1)].size(); k++) {
									if (utility[*it][*(it + 1)][k] > selectedDonorUtility) {
										selectedDonorUtility = utility[*it][*(it + 1)][k];
									}
								}
								tempUtil += selectedDonorUtility;
							}

							possibleCyclesOrChains.push_back(std::vector<int>(potentialCycleOrChain));
							utilityOfPossibleCyclesOrChains.push_back(tempUtil);
						}
					}

					else if ((int)potentialCycleOrChain.size() <= maxCycleSize) {

						for (std::vector<int>::iterator it = potentialCycleOrChain.begin(); it != potentialCycleOrChain.end() - 1; it++) {
							double selectedDonorUtility = 0;

							for (int k = 1; k < utility[*it][*(it + 1)].size(); k++) {
								if (utility[*it][*(it + 1)][k] > selectedDonorUtility) {
									selectedDonorUtility = utility[*it][*(it + 1)][k];
								}
							}
							tempUtil += selectedDonorUtility;
						}

						double selectedDonorUtility = 0;

						for (int k = 1; k < utility[*(potentialCycleOrChain.end() - 1)][*(potentialCycleOrChain.begin())].size(); k++) {
							if (utility[*(potentialCycleOrChain.end() - 1)][*(potentialCycleOrChain.begin())][k] > selectedDonorUtility) {
								selectedDonorUtility = utility[*(potentialCycleOrChain.end() - 1)][*(potentialCycleOrChain.begin())][k];
							}
						}
						tempUtil += selectedDonorUtility;

						possibleCyclesOrChains.push_back(std::vector<int>(potentialCycleOrChain));
						utilityOfPossibleCyclesOrChains.push_back(tempUtil);

					}
				}

				if ((int)stack_vec.size() >= maximum)
					v = -1;
				else
					v = getChild(start, v, visitedVec, incidence);

			}
		}
		start++;
	}

	if (possibleCyclesOrChains.size() == 1) {
		utilityValue = utilityOfPossibleCyclesOrChains.at(0);
	}

	else if (possibleCyclesOrChains.size() > 1) {

		int nPossibleCycles = (int)possibleCyclesOrChains.size();

		for (int c = 1; c < pow((double)2, nPossibleCycles); c++) {
			double tempU = 0;

			std::vector<int> cycleFlags(1 + nPossibleCycles, 0);

			int numberOfCyclesInPossibleSolutions = KPDFunctions::setFlags(c, cycleFlags);

			std::vector<int> checkUniqueness(nV + 1, 0);
			bool validSolution = true;
			for (int q = 0; q < nPossibleCycles; q++) {
				if (cycleFlags[q + 1] == 1) {
					tempU += utilityOfPossibleCyclesOrChains[q];
					for (std::vector<int>::iterator it = possibleCyclesOrChains.at(q).begin(); it != possibleCyclesOrChains.at(q).end(); ++it) {
						checkUniqueness[*it]++;
						if (checkUniqueness[*it] > 1) {
							validSolution = false;
							break;
						}
					}

					if (!validSolution) {
						break;
					}
				}
			}

			if (validSolution && tempU > utilityValue) {
				utilityValue = tempU;
			}
		}
	}

	return utilityValue;
}


#endif
