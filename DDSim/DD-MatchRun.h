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
#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>

class KPDMatchRun {

private:

	// Match Run Information
	int currentIteration;
	int matchRunTime;

	int matchRunNumberOfPairs;
	int matchRunNumberOfNDDs;
	int matchRunNumberOfNodes;

	//Node Information
	std::vector<KPDNode *> matchRunNodes;
	std::vector<int> matchRunNodeIDs;
	std::vector<KPDNodeType> matchRunNodeTypes;

	std::vector<KPDStatus> matchRunAvailability;

	std::map<int, std::map<int, std::vector<KPDMatch *> > > matchRunMatches;

	std::vector<std::vector<bool> > matchRunAdjacencyMatrix;
	std::vector<std::vector<bool> > matchRunAdjacencyMatrixReduced;

	// Relevant Parameters
	KPDUtilityScheme utilityScheme;

	int maxChainLength;
	int maxCycleSize;
	int maxLRSSize;
	int maximum;

	int postSelectionInactivePeriod;

	bool estimateEU;
	int numberOfExpectedUtilityIterations;

	bool allowABBridgeDonors;

	double probPairActiveToInactive;
	double probPairInactiveToActive;
	double probPairAttrition;
	double probNDDAttrition;


	//Helper Functions For Collecting Arrangements
	int selectDonor(int donorNodeID, int candidateNodeID);
	int getChild(int start, int v, std::vector<int> &visitedVector, std::vector<std::vector<bool> > &adjacency);

	void getLRSPairsOnly(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<std::vector<bool> > & adjacency);
	int getChildLRSPairsOnly(std::vector<std::vector<bool> > &adjacency, std::vector<int> &tree, std::vector<int> &level, std::vector<int> &pairs, std::vector<bool> &visited, int next, int childLevel);

	void getLRSWithNDDs(std::vector<std::vector<int> > & currentMatchRunArrangements, int numberOfNDDs, std::vector<std::vector<bool> > &adjacency);
	int getChildLRSWithNDDs(std::vector<std::vector<bool> > &adjacency, bool &childIsAdjacentToLowerLevels, std::vector<std::vector<int> > &trees, std::vector<std::vector<int> > &levels, std::vector<int> &inducedLevels,
		std::vector<bool> &visited, int &next, int &nextNDD, int &childLevel, int &childLevelNDD, bool &childCanBeNDD, int &ndds, int &matchRunSize);

	bool validateLRSBounds(std::vector<int> &arrangement, int order);

	//Helper Functions For Assigning Expected Utilities
	double calculateExpectedUtility(std::vector<int> &arrangement);
	double estimateExpectedUtility(std::vector<int> &arrangement);
	double calculatePartialUtility(int nV, std::vector<std::vector<bool> > &adjacency, std::vector<std::vector<std::vector<double> > > &utility, std::vector<KPDNodeType> &nodeTypes, std::vector<std::vector<KPDBloodType> > &bloodTypes);

	void checkValidSolution(int nSubCycles, int nVertices, std::vector<int> subCycleFlags, int next, std::vector<std::vector<int> > &subCyclesSubChains, std::vector<std::vector<int> > &possibleSolutions);

	// Random Number Generators
	RNG rngExpectedUtility;

	// Logs
	std::stringstream matchRunLog;

public:

	KPDMatchRun(KPDParameters * params, int iteration, int mrTime,
		std::vector<KPDNode *> nodes, 
		std::vector<KPDNodeType> nodeTypes,
		std::vector<KPDStatus> availability, 
		std::vector<KPDTransplant> transplantStatuses,
		std::map<int, std::map<int, std::vector<KPDMatch *> > > matches);
	~KPDMatchRun();

	//Collect Arrangements
	void collectCyclesAndChainsForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements);
	void collectLRSForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements);

	//Assign Expected Utilities
	void assignUtilitiesForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements);
	void assignExpectedUtilitiesForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements);

	//Select Arrangements by Optimization
	void getOptimalSolutionForCurrentMatchRun(std::vector<int> & optimalSolution, std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements);

	std::string printLog();

};


KPDMatchRun::KPDMatchRun(KPDParameters * params, int iteration, int mrTime,
	std::vector<KPDNode *> nodes, 
	std::vector<KPDNodeType> nodeTypes,
	std::vector<KPDStatus> availability,
	std::vector<KPDTransplant> transplantStatuses,
	std::map<int, std::map<int, std::vector<KPDMatch *> > > matches) {

	currentIteration = iteration;
	matchRunTime = mrTime;
	
	matchRunLog << "Match Run at Time " << mrTime << std::endl;
	
	//Retrieve Relevant Parameters
	maxChainLength = params->getMaxChainLength();
	maxCycleSize = params->getMaxCycleSize();
	maxLRSSize = params->getMaxLRSSize();
	maximum = std::max(maxChainLength + 1, maxCycleSize);

	postSelectionInactivePeriod = params->getPostSelectionInactivePeriod();
	estimateEU = params->getEstimateExpectedUtility();

	utilityScheme = params->getUtilityScheme();
	allowABBridgeDonors = params->getAllowABBridgeDonors();

	numberOfExpectedUtilityIterations = params->getNumberOfExpectedUtilityIterations();

	probPairActiveToInactive = params->getProbPairActiveToInactive();
	probPairInactiveToActive = params->getProbPairInactiveToActive();
	probPairAttrition = params->getProbPairAttrition();
	probNDDAttrition = params->getProbNDDAttrition();

	//Set Match Run Values
	rngExpectedUtility.setSeed(params->getRNGSeedExpectedUtility() * currentIteration + matchRunTime);

	matchRunNumberOfPairs = 0;
	matchRunNumberOfNDDs = 0;
	matchRunNumberOfNodes = 0;
	
	std::vector<KPDNode *> tempNodes;

	// Order nodes with NDDs in front
	// Node is added to current match run only if it is NOT_TRANSPLANTED and STATUS_ACTIVE
	for (int i = 1; i <= (int)nodes.size(); i++) {

		int nodeIndex = i - 1;

		//If pair is active and not already transplanted
		if (transplantStatuses[nodeIndex] == NOT_TRANSPLANTED && availability[nodeIndex] == STATUS_ACTIVE) {
			if (nodeTypes[nodeIndex] == PAIR) {
				tempNodes.push_back(nodes[nodeIndex]);
				matchRunNumberOfPairs++;
			}
			else {
				matchRunNodes.push_back(nodes[nodeIndex]);
				matchRunNodeTypes.push_back(nodeTypes[nodeIndex]);
				matchRunNumberOfNDDs++;
			}
		}
	}
	matchRunNumberOfNodes = matchRunNumberOfPairs + matchRunNumberOfNDDs;

	for (int i = 1; i <= (int) tempNodes.size(); i++) {

		int nodeIndex = i - 1;

		matchRunNodes.push_back(tempNodes[nodeIndex]);
		matchRunNodeTypes.push_back(PAIR);
	}
	
	// Print Active Pool
	matchRunLog << "Active Pool for Current Match Run: ";
	for (int i = 1; i < matchRunNumberOfNodes; i++) {

		int nodeIndex = i - 1;
		matchRunLog << matchRunNodes[nodeIndex]->getID() << " (" << matchRunNodeTypes[nodeIndex] << ") ";
	}
	matchRunLog << std::endl;
	

	// Collect Relevant Matches for Match Run and Create Match-Run Specific Adjacency Matrices
	matchRunAdjacencyMatrix.assign(1 + matchRunNumberOfNodes, std::vector<bool>(1 + matchRunNumberOfNodes, false));
	matchRunAdjacencyMatrixReduced.assign(1 + matchRunNumberOfNodes, std::vector<bool>(1 + matchRunNumberOfNodes, false));
	
	for (int i = 1; i <= matchRunNumberOfNodes; i++) {

		int donorNodeIndex = i - 1;

		int donorNodeID = matchRunNodes[donorNodeIndex]->getID();
		int numDonors = matchRunNodes[donorNodeIndex]->getNumberOfDonors();

		for (int j = 1; j <= matchRunNumberOfNodes; j++) {

			int candidateNodeIndex = j - 1;

			int candidateNodeID = matchRunNodes[candidateNodeIndex]->getID();

			if (matches.find(donorNodeID) != matches.end()) {
				if (matches[donorNodeID].find(candidateNodeID) != matches[donorNodeID].end()) {

					for (int k = 1; k <= numDonors; k++) {

						int donorIndex = k - 1;

						KPDMatch * match = matches[donorNodeID][candidateNodeID][donorIndex]->copy();

						matchRunMatches[donorNodeIndex][candidateNodeIndex].push_back(match);

						if (match->getAdjacency()) {

							matchRunAdjacencyMatrix[i][j] = true;
							if (nodeTypes[candidateNodeIndex] == PAIR) {
								matchRunAdjacencyMatrixReduced[i][j] = true;
							}
						}
					}
				}
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
		int v = getChild(start, start, visitedVector, matchRunAdjacencyMatrix);
		while (!stack_vec.empty()) {
			if (v == -1) {
				int top = stack_vec.back();
				stack_vec.pop_back();
				if (top == start) {
					visitedVector[top] = 0;
					break;
				}
				visitedVector[top] = 0;
				v = getChild(top, stack_vec.back(), visitedVector, matchRunAdjacencyMatrix);
			}
			else {
				visitedVector[v] = 1;
				stack_vec.push_back(v);

				//Potential Cycle or Chain Found!
				if (matchRunAdjacencyMatrix[v][start]) {

					int multipleNDDCheck = 0;
					int index = 0;

					//Copy Potential Cycle Or Chain
					std::vector<int> potentialCycleOrChain;

					for (int i = 0; i < (int)stack_vec.size(); i++) {
						potentialCycleOrChain.push_back(stack_vec[i] - 1);
					}

					//Count Non-Directed Donors; Save Index of Non-Directed Donor
					for (int i = 0; i < (int)potentialCycleOrChain.size(); i++) {
						if (matchRunNodeTypes[potentialCycleOrChain[i]] != PAIR) {
							multipleNDDCheck++;
							index = i;
						}
					}

					//If there is 1 NDD, we're dealing with a chain. Check if size of chain is appropriate
					if (multipleNDDCheck == 1 && (int)potentialCycleOrChain.size() <= maxChainLength + 1) {
						
						//Queue up NDD to front of chain
						while (index > 0) {
							int temp = *(potentialCycleOrChain.begin());
							potentialCycleOrChain.erase(potentialCycleOrChain.begin());
							potentialCycleOrChain.push_back(temp);
							index--;
						}
						
						if (!allowABBridgeDonors) {
							
							bool hasNonABBridgeDonor = false;

							//If any of the active associated donors have a blood type other than AB, then we're good
							for (int k = 1; k <= matchRunNodes[*(potentialCycleOrChain.end() - 1)]->getNumberOfDonors(); k++) {

								int donorIndex = k - 1;
								if (matchRunNodes[*(potentialCycleOrChain.end() - 1)]->getDonorBT(donorIndex) != BT_AB) {
									hasNonABBridgeDonor = true;
									break;
								}
							}

							if (hasNonABBridgeDonor) {
								currentMatchRunArrangements.push_back(potentialCycleOrChain);
							}
						}
						else {
							currentMatchRunArrangements.push_back(potentialCycleOrChain);
						}
						
					}
					// If there are no NDDs, we're dealing with a cycle. Check if size is appropriate
					else if (multipleNDDCheck == 0 && (int)potentialCycleOrChain.size() <= maxCycleSize) {
						currentMatchRunArrangements.push_back(potentialCycleOrChain);						
					}
				}

				if ((int)stack_vec.size() >= maximum)
					v = -1;
				else
					v = getChild(start, v, visitedVector, matchRunAdjacencyMatrix);

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
		getLRSWithNDDs(currentMatchRunArrangements, matchRunNumberOfNDDs, matchRunAdjacencyMatrixReduced);
	}

	getLRSPairsOnly(currentMatchRunArrangements, matchRunAdjacencyMatrixReduced);
}

// Assign Expected Utilities

void KPDMatchRun::assignUtilitiesForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements, 
	std::vector<double> & assignedValueOfCurrentMatchRunArrangements) {

	// Iterating through cycles and chains...
	for (std::vector<std::vector<int> >::iterator itArrangements = currentMatchRunArrangements.begin(); itArrangements != currentMatchRunArrangements.end(); itArrangements++) {
		double util = 0;

		//... and sum over all potential transplants...
		for (std::vector<int>::iterator itNodes = (*itArrangements).begin(); itNodes != (*itArrangements).end() - 1; itNodes++) {

			int donorNodeIndex = *itNodes;
			int candidateNodeIndex = *(itNodes + 1);

			//... using the best potential matching donor to assign the value
			int bestDonorIndex = selectDonor(donorNodeIndex, candidateNodeIndex);

			//5 year survival
			if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
				util += 1;
			}
			else {
				util += matchRunMatches[donorNodeIndex][candidateNodeIndex][bestDonorIndex]->getUtility(utilityScheme);
			}
		}

		// For cycles, add the final transplant back to the original candidate
		if (matchRunNodeTypes[*(itArrangements->begin())] == PAIR) {

			int donorNodeIndex = *(itArrangements->end() - 1);
			int candidateNodeIndex = *(itArrangements->begin());

			int bestDonorIndex = selectDonor(donorNodeIndex, candidateNodeIndex);


			if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
				util += 1;
			}
			else {
				util += matchRunMatches[donorNodeIndex][candidateNodeIndex][bestDonorIndex]->getUtility(utilityScheme);
			}

		}

		assignedValueOfCurrentMatchRunArrangements.push_back(util);
	}
}

void KPDMatchRun::assignExpectedUtilitiesForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<double> & assignedValueOfCurrentMatchRunArrangements) {

	// Iterating through arrangements...
	for (std::vector<std::vector<int> >::iterator itArrangements = currentMatchRunArrangements.begin(); itArrangements != currentMatchRunArrangements.end(); itArrangements++) {
		
		double eu = 0;

		// ... and either estimate or calculate expected utility
		if (estimateEU) {
			eu = estimateExpectedUtility(*itArrangements);
		}
		else {
			eu = calculateExpectedUtility(*itArrangements);
		}

		assignedValueOfCurrentMatchRunArrangements.push_back(eu);
	}
}

// Select Arrangements by Optimization

void KPDMatchRun::getOptimalSolutionForCurrentMatchRun(std::vector<int> & optimalSolution,
	std::vector<std::vector<int> > & currentMatchRunArrangements, 
	std::vector<double> & assignedValueOfCurrentMatchRunArrangements) {

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
		for (int i = 1; i <= matchRunNumberOfNodes; i++) {

			GRBLinExpr expr = 0;
			for (int j = 0; j <= nArrangements - 1; j++) {
				for (std::vector<int>::iterator it = currentMatchRunArrangements[j].begin(); it != currentMatchRunArrangements[j].end(); ++it) {
					if (*it == i) {
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

int KPDMatchRun::selectDonor(int donorNodeIndex, int candidateNodeIndex) {
	
	double maxUtil = 0.0;
	int maxDonorIndex = -1; // -1 means no donor is selected

	//Iterate through donors
	for (int k = 1; k <= matchRunNodes[donorNodeIndex]->getNumberOfDonors(); k++) {

		int donorIndex = k - 1;

		//Only consider donors that match between the donor node and the candidate node, and that are active!
		if (matchRunMatches[donorNodeIndex][candidateNodeIndex][donorIndex]->getAdjacency()) {

			//If the donor provides more utility than the current max donor, replace with new donor, and update max utility value

			if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
				if (maxUtil == 0.0) {
					maxDonorIndex = donorIndex;
					maxUtil = 1.0;
				}
			}
			else {
				if (matchRunMatches[donorNodeIndex][candidateNodeIndex][donorIndex]->getUtility(utilityScheme) > maxUtil) {
					maxDonorIndex = donorIndex;
					maxUtil = matchRunMatches[donorNodeIndex][candidateNodeIndex][donorIndex]->getUtility(utilityScheme);
				}
			}
		}

		donorIndex++;
	}

	return maxDonorIndex;
}


int KPDMatchRun::getChild(int lower, int current, std::vector<int> &visitedVector, std::vector<std::vector<bool> > &adjacency) {
	int nV = (int)visitedVector.size() - 1;
	for (int j = lower + 1; j <= nV; j++) {
		if (adjacency[current][j] == true && visitedVector[j] == 0)
			return j;
	}
	return -1;
}

// For Collecting Arrangements

void KPDMatchRun::getLRSPairsOnly(std::vector<std::vector<int> > & currentMatchRunArrangements, std::vector<std::vector<bool> > & adjacency) {

	std::vector<int> pairs;
	pairs.push_back(-1);

	for (int i = 1; i <= matchRunNumberOfNodes; i++) {

		int nodeIndex = i - 1;

		if (matchRunNodeTypes[nodeIndex] == PAIR) {
			pairs.push_back(nodeIndex);
		}
	}

	int numberOfPairs = (int)pairs.size() - 1;

	std::vector<std::vector<bool> > reverseMatrix;
	reverseMatrix.assign(1 + numberOfPairs, std::vector<bool>(1 + numberOfPairs, false));

	for (int i = 1; i <= numberOfPairs; i++) {

		for (int j = 1; j <= numberOfPairs; j++) {

			if (i != j) {
				reverseMatrix[j][i] = adjacency[pairs[i]][pairs[j]];
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
			child = getChildLRSPairsOnly(adjacency, bfsTree, bfsLevel, pairs, visited, next, childLevel);

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
						newLRS.push_back(pairs[*itTree]);
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

int KPDMatchRun::getChildLRSPairsOnly(std::vector<std::vector<bool> > &adjacency, std::vector<int> &tree, std::vector<int> &level, std::vector<int> &pairs, std::vector<bool> &visited, int next, int childLevel) {

	int size = (int)pairs.size() - 1;

	for (int child = next; child <= size; child++) {
		//Can only visit children not already visited
		if (visited[child] == false) {
			//Go through previous nodes
			for (int i = 0; i < (int)level.size(); i++) {
				//At low levels, make sure potential node is not a direct successor
				if (level[i] < childLevel - 1) {
					if (adjacency[pairs[tree[i]] + 1][pairs[child] + 1]) {
						break;
					}
				}
				//At previous level, if potential node is a successor, it is the next child
				if (level[i] == childLevel - 1) {
					if (adjacency[pairs[tree[i]] + 1][pairs[child] + 1]) {
						return child;
					}
				}
			}
		}
	}

	return -1;
}

void KPDMatchRun::getLRSWithNDDs(std::vector<std::vector<int> > & currentMatchRunArrangements, int numberOfNDDs, std::vector<std::vector<bool> > &adjacency) {

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

			child = getChildLRSWithNDDs(adjacency, childIsAdjacentToLowerLevels, listOfBFSTrees, listOfBFSLevels, inducedBFSLevel,
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
							newLRS.push_back(listOfBFSTrees[i][j]); // Changed
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

int KPDMatchRun::getChildLRSWithNDDs(std::vector<std::vector<bool> > &adjacency, bool &childIsAdjacentToLowerLevels, std::vector<std::vector<int> > &trees, std::vector<std::vector<int> > &levels, std::vector<int> &inducedLevels,
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

							if (adjacency[node][potentialChild] || adjacency[potentialChild][node]) {
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

								if (adjacency[node][potentialChild]) {
									breakFlag = true;
									break;
								}

								if (adjacency[potentialChild][node]) {
									childIsAdjacentToLowerLevels = true;
								}
							}
						}
						else {
							for (int j = 0; j < trees[i].size(); j++) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (adjacency[potentialChild][node]) {
									breakFlag = true;
									break;
								}

								if (adjacency[node][potentialChild]) {
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

								if (adjacency[node][potentialChild]) {
									breakFlag = true;
									break;
								}
							}

							//Current node is at previous level
							else if (levels[i][j] == childLevel - 1) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (adjacency[node][potentialChild]) {
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

							if (adjacency[node][potentialChild] || adjacency[potentialChild][node]) {
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

								if (adjacency[node][potentialChild]) {
									breakFlag = true;
									break;
								}

								if (adjacency[potentialChild][node]) {
									childIsAdjacentToLowerLevels = true;
								}
							}
						}
						else {
							//Iterate through nodes 
							for (int j = 0; j < trees[i].size(); j++) {
								int node = trees[i][j] + 1;
								int potentialChild = child + 1;

								if (adjacency[potentialChild][node]) {
									breakFlag = true;
									break;
								}

								if (adjacency[node][potentialChild]) {
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

								if (adjacency[node][potentialChild]) {
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
				if (matchRunAdjacencyMatrixReduced[arrangement[q]][arrangement[i]]) {
					sp = std::min(sp, shortestPathLengths[j][q] + 1);
				}
			}
			shortestPathLengths[j][i] = sp;

			sp = order;
			for (int q = 0; q <= i - 1; q++) {
				if (matchRunAdjacencyMatrixReduced[arrangement[i]][arrangement[q]]) { // Reduced?
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
				if (matchRunNodeTypes[arrangement[stack.front()]] != PAIR) {
					if (shortestPathLengths[stack.front()][i] <= maxChainLength + 1) {
						visited[i] = true;
						stack.push_back(i);
						connections++;
					}
				}
				else {
					if ((matchRunNodeTypes[arrangement[i]] == PAIR && shortestPathLengths[i][stack.front()] + shortestPathLengths[stack.front()][i] <= maxCycleSize) ||
						(matchRunNodeTypes[arrangement[i]] != PAIR && shortestPathLengths[i][stack.front()] <= maxChainLength + 1)) {

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
	
	double nddAssumedProbability = 1 - (probNDDAttrition);
	double pairAssumedProbability = 1 - (probPairActiveToInactive + probPairAttrition);
	
	int N = (int)arrangement.size();
	double utility = 0;

	//Iterates through each possible subset of nodes
	for (int d = 3; d < pow((double)2, N); d++) {

		std::vector<int> availabilityFlags(N, 0);

		int subsetSize = KPDFunctions::setFlags(d, 0, availabilityFlags);

		//Must be at least 2 nodes
		if (subsetSize > 1) {
			
			double probSubset = 1;

			//Calculate probability of subset
			for (int i = 1; i <= N; i++) {
				int arrangementIndex = i - 1;
				int nodeIndex = arrangement[arrangementIndex];

				double probNode = 1;
				
				if (matchRunNodeTypes[nodeIndex] != PAIR) {
					if (availabilityFlags[arrangementIndex]) {
						probNode = probNode * nddAssumedProbability;
					}
					else {
						probNode = probNode * (1 - nddAssumedProbability);
					}
				}
				else {
					if (availabilityFlags[arrangementIndex]) {
						probNode = probNode * pairAssumedProbability;
					}
					else {
						probNode = probNode * (1 - pairAssumedProbability);
					}
				}

				probSubset = probSubset * probNode;
			}

			if (probSubset > 0) {

				std::vector<int> edgeSubsetCandidateNodeIndices;
				std::vector<int> edgeSubsetDonorNodeIndices;
				std::vector<int> edgeSubsetDonorIndices;

				int numberOfEdgesInDonorSubset = 0;

				//Collect possible edges
				for (int i = 1; i <= N; i++) {

					int arrangementDonorIndex = i - 1;
					int donorNodeIndex = arrangement[arrangementDonorIndex];

					for (int j = 1; j <= N; j++) {

						int arrangementCandidateIndex = j - 1;
						int candidateNodeIndex = arrangement[arrangementCandidateIndex];

						if (i != j) {							

							if (matchRunAdjacencyMatrix[donorNodeIndex][candidateNodeIndex] && matchRunNodeTypes[candidateNodeIndex] == PAIR) {

								for (int k = 1; k <= matchRunNodes[donorNodeIndex]->getNumberOfDonors(); k++) {

									int donorIndex = k - 1;

									if (availabilityFlags[arrangementDonorIndex] && 
										availabilityFlags[arrangementCandidateIndex] && 
										matchRunMatches[donorNodeIndex][candidateNodeIndex][donorIndex]->getAdjacency()) {

										edgeSubsetDonorNodeIndices.push_back(arrangementDonorIndex);
										edgeSubsetCandidateNodeIndices.push_back(arrangementCandidateIndex);
										edgeSubsetDonorIndices.push_back(donorIndex);

										numberOfEdgesInDonorSubset++;
									}
								}
							}
						}
					}
				}

				// Iterate through every possible combination of edges
				for (int e = 1; e < pow((double)2, numberOfEdgesInDonorSubset); e++) {

					std::vector<int> edgeFlags(numberOfEdgesInDonorSubset, 0);

					int edgeSubsetSize = KPDFunctions::setFlags(e, 0, edgeFlags);

					double probEdgeSubset = probSubset;

					std::vector<std::vector<bool> > reducedAdjacencyMatrix(1 + N, std::vector<bool>(1 + N, false));

					std::vector<std::vector<std::vector<bool> > > reducedDonorAdjacencyMatrix(N, std::vector<std::vector<bool> >(N, std::vector<bool>(1, false)));
					std::vector<std::vector<std::vector<double> > > reducedUtilityMatrix(N, std::vector<std::vector<double> >(N, std::vector<double>(1, 0.0)));

					std::vector<KPDNodeType> reducedNodeTypes(N, PAIR);
					std::vector<std::vector<KPDBloodType> > reducedDonorBloodTypes(N, std::vector<KPDBloodType>(1, BT_AB));

					//Build reduced matrices
					for (int i = 1; i <= N; i++) {

						int arrangementDonorIndex = i - 1;
						int donorNodeIndex = arrangement[arrangementDonorIndex];

						int donorNodeNumberOfDonors = matchRunNodes[donorNodeIndex]->getNumberOfDonors();

						for (int j = 1; j <= N; j++) {

							int arrangementCandidateIndex = j - 1;
							int candidateNodeIndex = arrangement[arrangementCandidateIndex];

							reducedDonorAdjacencyMatrix[arrangementDonorIndex][arrangementCandidateIndex].assign(donorNodeNumberOfDonors, false);
							reducedUtilityMatrix[arrangementDonorIndex][arrangementCandidateIndex].assign(donorNodeNumberOfDonors, 0.0);

							//Implicit backward edges
							if (matchRunNodeTypes[donorNodeIndex] == PAIR && matchRunNodeTypes[candidateNodeIndex] != PAIR) {
								reducedAdjacencyMatrix[i][j] = true;
							}
						}

						reducedNodeTypes[arrangementDonorIndex] = matchRunNodeTypes[donorNodeIndex];

						reducedDonorBloodTypes[arrangementDonorIndex].assign(donorNodeNumberOfDonors, BT_AB);
						for (int k = 1; k <= donorNodeNumberOfDonors; k++) {

							int donorIndex = k - 1;

							reducedDonorBloodTypes[arrangementDonorIndex][donorIndex] = matchRunNodes[donorNodeIndex]->getDonorBT(donorIndex);
						}
					}

					for (int l = 1; l <= numberOfEdgesInDonorSubset; l++) {

						int edgeIndex = l - 1;

						int edgeSubsetDonorNodeIndex = edgeSubsetDonorNodeIndices[edgeIndex];
						int edgeSubsetCandidateNodeIndex = edgeSubsetCandidateNodeIndices[edgeIndex];
						int edgeSubsetDonorIndex = edgeSubsetDonorIndices[edgeIndex];

						int edgeSubsetDonorNode = arrangement[edgeSubsetDonorNodeIndex];
						int edgeSubsetCandidateNode = arrangement[edgeSubsetCandidateNodeIndex];

						if (edgeFlags[edgeIndex] != 0) {

							probEdgeSubset = probEdgeSubset * matchRunMatches[edgeSubsetDonorNode][edgeSubsetCandidateNode][edgeSubsetDonorIndex]->getAssumedSuccessProbability();

							reducedDonorAdjacencyMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex][edgeSubsetDonorIndex] = true;
							
							reducedAdjacencyMatrix[edgeSubsetDonorNodeIndex + 1][edgeSubsetCandidateNodeIndex + 1] = true;

							if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
								if (matchRunNodeTypes[edgeSubsetCandidateNode] == PAIR) {
									reducedUtilityMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex][edgeSubsetDonorIndex] = 1;
								}
							}
							else {
								reducedUtilityMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex][edgeSubsetDonorIndex] = matchRunMatches[edgeSubsetDonorNode][edgeSubsetCandidateNode][edgeSubsetDonorIndex]->getUtility(utilityScheme);
							}

						}
						else {
							probEdgeSubset = probEdgeSubset * (1 - matchRunMatches[edgeSubsetDonorNode][edgeSubsetCandidateNode][edgeSubsetDonorIndex]->getAssumedSuccessProbability());
						}
					}

					if (probEdgeSubset > 0) {

						double addUtil = calculatePartialUtility(N, reducedAdjacencyMatrix, reducedUtilityMatrix, reducedNodeTypes, reducedDonorBloodTypes);

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

double KPDMatchRun::estimateExpectedUtility(std::vector<int> & arrangement) {

	double nddAssumedProbability = 1 - (probNDDAttrition);
	double pairAssumedProbability = 1 - (probPairActiveToInactive + probPairAttrition);

	int N = (int)arrangement.size();

	double expUtility = 0;

	for (int sims = 1; sims <= numberOfExpectedUtilityIterations; sims++) {

		//Initialize random matrices
		std::vector<std::vector<bool> > randomAdjacencyMatrix(1 + N, std::vector<bool>(1 + N, false));
		std::vector<std::vector<std::vector<bool> > > randomDonorAdjacencyMatrix(1 + N, std::vector<std::vector<bool> >(1 + N, std::vector<bool>(1, false)));
		std::vector<std::vector<std::vector<double> > > randomUtilityMatrix(1 + N, std::vector<std::vector<double> >(1 + N, std::vector<double>(1, 0.0)));

		std::vector<bool> randomCandidateAvailabilityVector(1 + N, false);
		std::vector<std::vector<bool> > randomDonorAvailabilityVector(1 + N, std::vector<bool>(1, false));

		std::vector<KPDNodeType> subNodeTypeVector(1 + N, PAIR);
		std::vector<std::vector<KPDBloodType> > subDonorBloodTypeVector(1 + N, std::vector<KPDBloodType>(1, BT_AB));

		int i = 1;

		//Simulate candidate and donor availabilities, retrive donor information
		for (std::vector<int>::iterator itNodes = arrangement.begin(); itNodes != arrangement.end(); ++itNodes, i++) {

			//Store node type
			subNodeTypeVector[i] = matchRunNodeTypes[*itNodes];

			//Randomly generate donor availability (and store donor blood types)
			int numDonors = matchRunNodes[*itNodes]->getNumberOfDonors();

			randomDonorAvailabilityVector[i].assign(1 + numDonors, false);
			subDonorBloodTypeVector[i].assign(1 + numDonors, BT_AB);

			for (int k = 1; k <= numDonors; k++) {

				//if (matchRunDonorUnderlyingStatus[*itNodes][k] == STATUS_ACTIVE) {

					double u = rngExpectedUtility.runif();

					if ((matchRunNodeTypes[*itNodes] != PAIR && u < nddAssumedProbability) || 
						(matchRunNodeTypes[*itNodes] == PAIR && u < pairAssumedProbability)) {

						randomDonorAvailabilityVector[i][k] = true;

						if (matchRunNodeTypes[*itNodes] != PAIR) { // Add marker for NDD availability (acts as a shortcut for the next section)
							randomCandidateAvailabilityVector[i] = true;
						}

					}
					subDonorBloodTypeVector[i][k] = matchRunNodes[*itNodes]->getDonorBT(k);
				//}
			}

			//Randomly generate candidate availability for PAIRs
			if (matchRunNodeTypes[*itNodes] == PAIR) {
				if (rngExpectedUtility.runif() < pairAssumedProbability) {
					randomCandidateAvailabilityVector[i] = true;
				}
			}
		}

		//Iterate through donor-candidate combinations and randomly generate realized transplants
		int j = 1;
		i = 1;

		for (std::vector<int>::iterator itDonors = arrangement.begin(); itDonors != arrangement.end(); ++itDonors, i++) {

			int numDonors = matchRunNodes[*itDonors]->getNumberOfDonors();

			for (std::vector<int>::iterator itCandidates = arrangement.begin(); itCandidates != arrangement.end(); ++itCandidates, j++) {

				randomDonorAdjacencyMatrix[i][j].assign(1 + numDonors, 0);
				randomUtilityMatrix[i][j].assign(1 + numDonors, 0.0);

				if (i != j) {

					//If both nodes are available
					if (randomCandidateAvailabilityVector[i] && randomCandidateAvailabilityVector[j]) {

						//If donor node is a PAIR and candidate node is a NDD or BRIDGE_DONOR, 
						//insert implicit backward edges from all available donors toward the NDD/BRIDGE node, and set the adjacency matrix to true
						if (subNodeTypeVector[i] == PAIR && subNodeTypeVector[j] != PAIR) {
							for (int k = 1; k <= numDonors; k++) {
								if (randomDonorAvailabilityVector[i][k]) {
									randomDonorAdjacencyMatrix[i][j][k] = true;
									randomAdjacencyMatrix[i][j] = true;
								}
							}
						}
						//If candidate node is a PAIR, generate random matches
						else if (subNodeTypeVector[j] == PAIR) {

							//Iterate through donors
							for (int k = 1; k <= numDonors; k++) {
								// For existing original matches, and for donors that are available, randomly generate matches
								if (matchRunMatches[*itDonors][*itCandidates][k]->getAdjacency() && randomDonorAvailabilityVector[i][k]) {

									if (rngExpectedUtility.runif() < matchRunMatches[*itDonors][*itCandidates][k]->getAssumedSuccessProbability()) {

										randomDonorAdjacencyMatrix[i][j][k] = 1;

										if (utilityScheme == UTILITY_TRANSPLANTS) {
											if (matchRunNodeTypes[*itCandidates] == PAIR) {
												randomUtilityMatrix[i][j][k] = 1;
											}
										}
										else {
											randomUtilityMatrix[i][j][k] = matchRunMatches[*itDonors][*itCandidates][k]->getUtility(utilityScheme);
										}

										// Set adjacency matrix to true
										randomAdjacencyMatrix[i][j] = true;
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
		double partialUtil = calculatePartialUtility(N, randomAdjacencyMatrix, randomUtilityMatrix, subNodeTypeVector, subDonorBloodTypeVector);
		expUtility += partialUtil;

	}

	//Expected utility is the average of the calculated utility values over the number of iterations
	return expUtility / numberOfExpectedUtilityIterations;

}


void KPDMatchRun::checkValidSolution(int nSubCycles, int nVertices,
	std::vector<int> subCycleFlags, 
	int next, std::vector<std::vector<int> > & subCyclesSubChains, 
	std::vector<std::vector<int> > & possibleSolutions) {

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


double KPDMatchRun::calculatePartialUtility(int nV, std::vector<std::vector<bool> > & adjacency, 
	std::vector<std::vector<std::vector<double> > > & utility,
	std::vector<KPDNodeType> & nodeTypes, 
	std::vector<std::vector<KPDBloodType> > & donorBloodTypes) {

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
		int v = getChild(start, stack_vec.back(), visitedVec, adjacency);
		while (!stack_vec.empty()) {
			if (v == -1) {
				int top = stack_vec.back();
				stack_vec.pop_back();
				if (top == start) {
					visitedVec[top] = 0;
					break;
				}
				visitedVec[top] = 0;
				v = getChild(top, stack_vec.back(), visitedVec, adjacency);
			}
			else {
				visitedVec[v] = 1;
				stack_vec.push_back(v);

				if (adjacency[v][start] == true) {
					int multipleNDDCheck = 0;
					int index = 0;

					std::vector<int> potentialCycleOrChain;

					for (int i = 0; i < (int)stack_vec.size(); i++) {
						potentialCycleOrChain.push_back(stack_vec[i] - 1);
					}

					for (int i = 0; i < (int) potentialCycleOrChain.size(); i++) {
						if (nodeTypes[potentialCycleOrChain[i]] != PAIR) {
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

								int donorIndex = k - 1;
								if (donorBloodTypes[*(potentialCycleOrChain.end() - 1)][donorIndex] != BT_AB) {
									nonABBridgeDonor = true;
									break;
								}
							}

							if (nonABBridgeDonor) {
								for (std::vector<int>::iterator it = potentialCycleOrChain.begin(); it != potentialCycleOrChain.end() - 1; it++) {
									double selectedDonorUtility = 0;

									int donorNodeIndex = *it;
									int candidateNodeIndex = *(it + 1);

									for (int k = 1; k < utility[donorNodeIndex][candidateNodeIndex].size(); k++) {

										int donorIndex = k - 1;

										if (utility[donorNodeIndex][candidateNodeIndex][donorIndex] > selectedDonorUtility) {
											selectedDonorUtility = utility[donorNodeIndex][candidateNodeIndex][donorIndex];
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

								int donorNodeIndex = *it;
								int candidateNodeIndex = *(it + 1);

								for (int k = 1; k < utility[donorNodeIndex][candidateNodeIndex].size(); k++) {

									int donorIndex = k - 1;

									if (utility[donorNodeIndex][candidateNodeIndex][donorIndex] > selectedDonorUtility) {
										selectedDonorUtility = utility[donorNodeIndex][candidateNodeIndex][donorIndex];
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

							int donorNodeIndex = *it;
							int candidateNodeIndex = *(it + 1);

							for (int k = 1; k < utility[donorNodeIndex][candidateNodeIndex].size(); k++) {

								int donorIndex = k - 1;

								if (utility[donorNodeIndex][candidateNodeIndex][donorIndex] > selectedDonorUtility) {
									selectedDonorUtility = utility[donorNodeIndex][candidateNodeIndex][donorIndex];
								}
							}
							tempUtil += selectedDonorUtility;
						}

						double selectedDonorUtility = 0;

						int donorNodeIndex = *(potentialCycleOrChain.end() - 1);
						int candidateNodeIndex = *(potentialCycleOrChain.begin());

						for (int k = 1; k < utility[donorNodeIndex][candidateNodeIndex].size(); k++) {

							int donorIndex = k - 1;

							if (utility[donorNodeIndex][candidateNodeIndex][donorIndex] > selectedDonorUtility) {
								selectedDonorUtility = utility[donorNodeIndex][candidateNodeIndex][donorIndex];
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
					v = getChild(start, v, visitedVec, adjacency);

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

			int numberOfCyclesInPossibleSolutions = KPDFunctions::setFlags(c, 1, cycleFlags);

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
