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
	std::vector<KPDNodeType> matchRunNodeTypes;

	std::vector<KPDStatus> matchRunAvailability;

	std::map<int, std::map<int, std::vector<KPDMatch *> > > matchRunMatches;

	std::vector<std::vector<bool> > matchRunAdjacencyMatrix;
	std::vector<std::vector<bool> > matchRunAdjacencyMatrixReduced;

	// Relevant Parameters
	KPDUtilityScheme utilityScheme;

	int maxChainLength;
	int maxCycleSize;
	int maximum;

	int timeBetweenSelectionAndTransplantation;

	bool estimateEU;
	int numberOfExpectedUtilityIterations;

	bool allowABBridgeDonors;

	double probPairActiveToInactive;
	double probPairInactiveToActive;
	double probPairAttrition;
	double probBridgeDonorAttrition;


	//Helper Functions For Collecting Arrangements
	int selectDonor(int donorNodeIndex, int candidateNodeIndex);
	int indexOf(int id);
	int getChild(int start, int v, std::vector<int> &visitedVector, std::vector<std::vector<bool> > &adjacency);
	
	//Helper Functions For Assigning Expected Utilities
	double calculateExpectedUtility(std::vector<int> &arrangement);
	double estimateExpectedUtility(std::vector<int> &arrangement);
	double calculatePartialUtility(int nV, std::vector<std::vector<bool> > &adjacency, std::vector<std::vector<std::vector<double> > > &utility, std::vector<KPDNodeType> &nodeTypes, std::vector<std::vector<KPDBloodType> > &bloodTypes);

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
	maximum = std::max(maxChainLength + 1, maxCycleSize);

	timeBetweenSelectionAndTransplantation = params->getTimeBetweenSelectionAndTransplantation();
	estimateEU = params->getEstimateExpectedUtility();

	utilityScheme = params->getUtilityScheme();
	allowABBridgeDonors = params->getAllowABBridgeDonors();

	numberOfExpectedUtilityIterations = params->getNumberOfExpectedUtilityIterations();

	probPairActiveToInactive = params->getProbPairActiveToInactive();
	probPairInactiveToActive = params->getProbPairInactiveToActive();
	probPairAttrition = params->getProbPairAttrition();
	probBridgeDonorAttrition = params->getProbBridgeDonorAttrition();

	//Set Match Run Values
	rngExpectedUtility.setSeed(params->getRNGSeedExpectedUtility() * currentIteration + matchRunTime);

	matchRunNumberOfPairs = 0;
	matchRunNumberOfNDDs = 0;
	matchRunNumberOfNodes = 0;
	
	std::vector<KPDNode *> tempNodes;

	// Order nodes with NDDs in front
	// Node is added to current match run only if it is TRANSPLANT_NO and STATUS_ACTIVE
	for (int i = 1; i <= (int)nodes.size(); i++) {

		int nodeIndex = i - 1;

		//If pair is active and not already transplanted
		if (transplantStatuses[nodeIndex] == TRANSPLANT_NO && availability[nodeIndex] == STATUS_ACTIVE) {
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

						matchRunMatches[donorNodeID][candidateNodeID].push_back(match);

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
						potentialCycleOrChain.push_back(matchRunNodes[stack_vec[i] - 1]->getID());
					}

					//Count Non-Directed Donors; Save Index of Non-Directed Donor
					for (int i = 0; i < (int)potentialCycleOrChain.size(); i++) {
						if (matchRunNodeTypes[indexOf(potentialCycleOrChain[i])] != PAIR) {
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

							int bridgeDonorID = *(potentialCycleOrChain.end() - 1);
							int bridgeDonorIndex = indexOf(bridgeDonorID);

							//If any of the active associated donors have a blood type other than AB, then we're good
							for (int k = 1; k <= matchRunNodes[bridgeDonorIndex]->getNumberOfDonors(); k++) {

								int donorIndex = k - 1;
								if (matchRunNodes[bridgeDonorIndex]->getDonorBT(donorIndex) != BT_AB) {
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

// Assign Expected Utilities

void KPDMatchRun::assignUtilitiesForCurrentMatchRun(std::vector<std::vector<int> > & currentMatchRunArrangements, 
	std::vector<double> & assignedValueOfCurrentMatchRunArrangements) {

	// Iterating through cycles and chains...
	for (std::vector<std::vector<int> >::iterator itArrangements = currentMatchRunArrangements.begin(); itArrangements != currentMatchRunArrangements.end(); itArrangements++) {
		double util = 0;

		//... and sum over all potential transplants...
		for (std::vector<int>::iterator itNodes = (*itArrangements).begin(); itNodes != (*itArrangements).end() - 1; itNodes++) {

			int donorNodeID = *itNodes;
			int donorNodeIndex = indexOf(donorNodeID);
			int candidateNodeID = *(itNodes + 1);
			int candidateNodeIndex = indexOf(candidateNodeID);

			//... using the best potential matching donor to assign the value
			int bestDonorIndex = selectDonor(donorNodeIndex, candidateNodeIndex);

			//5 year survival
			if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
				util += 1;
			}
			else {
				util += matchRunMatches[donorNodeID][candidateNodeID][bestDonorIndex]->getUtility(utilityScheme);
			}
		}

		int candidateNodeID = *(itArrangements->begin());
		int candidateNodeIndex = indexOf(candidateNodeID);

		// For cycles, add the final transplant back to the original candidate
		if (matchRunNodeTypes[candidateNodeIndex] == PAIR) {

			int donorNodeID = *(itArrangements->end() - 1);
			int donorNodeIndex = indexOf(donorNodeID);

			int bestDonorIndex = selectDonor(donorNodeIndex, candidateNodeIndex);


			if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
				util += 1;
			}
			else {
				util += matchRunMatches[donorNodeID][candidateNodeID][bestDonorIndex]->getUtility(utilityScheme);
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
	//std::cout << "Optimization: " << (-1)*objective << std::endl;

	matchRunLog << "Optimization: " << (-1)*objective << std::endl;
}

std::string KPDMatchRun::printLog(){

	return matchRunLog.str();
}

int KPDMatchRun::selectDonor(int donorNodeIndex, int candidateNodeIndex) {
	
	double maxUtil = 0.0;
	int maxDonorIndex = -1; // -1 means no donor is selected

	int donorNodeID = matchRunNodes[donorNodeIndex]->getID();
	int candidateNodeID = matchRunNodes[candidateNodeIndex]->getID();

	//Iterate through donors
	for (int k = 1; k <= matchRunNodes[donorNodeIndex]->getNumberOfDonors(); k++) {

		int donorIndex = k - 1;

		//Only consider donors that match between the donor node and the candidate node, and that are active!
		if (matchRunMatches[donorNodeID][candidateNodeID][donorIndex]->getAdjacency()) {

			//If the donor provides more utility than the current max donor, replace with new donor, and update max utility value

			if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
				if (maxUtil == 0.0) {
					maxDonorIndex = donorIndex;
					maxUtil = 1.0;
				}
			}
			else {

				if (matchRunMatches[donorNodeID][candidateNodeID][donorIndex]->getUtility(utilityScheme) > maxUtil) {
					maxDonorIndex = donorIndex;
					maxUtil = matchRunMatches[donorNodeID][candidateNodeID][donorIndex]->getUtility(utilityScheme);
				}
			}
		}

		donorIndex++;
	}

	return maxDonorIndex;
}

int KPDMatchRun::indexOf(int id){

	for (int i = 0; i < (int)matchRunNodes.size(); i++) {
		if (matchRunNodes[i]->getID() == id) {
			return i;
		}
	}

	return -1;
}


int KPDMatchRun::getChild(int lower, int current, std::vector<int> & visitedVector, std::vector<std::vector<bool> > & adjacency) {

	int nV = (int)visitedVector.size() - 1;
	for (int j = lower + 1; j <= nV; j++) {
		if (adjacency[current][j] == true && visitedVector[j] == 0)
			return j;
	}
	return -1;
}

double KPDMatchRun::calculateExpectedUtility(std::vector<int> & arrangement) {
	
	double pairAssumedProbability = 0;

	int days = timeBetweenSelectionAndTransplantation;
	double p = probPairActiveToInactive + probPairAttrition;

	for (int k = 1; k <= days; k++) {
		pairAssumedProbability += (p * std::pow(1 - p, k - 1));
	}
	
	pairAssumedProbability = 1 - pairAssumedProbability;

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
				int nodeID = arrangement[arrangementIndex];
				int nodeIndex = indexOf(nodeID);

				double probNode = 1;
				
				if (matchRunNodeTypes[nodeIndex] != PAIR) {
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

					int arrangementSubsetDonorIndex = i - 1;
					int subsetDonorID = arrangement[arrangementSubsetDonorIndex];
					int subsetDonorIndex = indexOf(subsetDonorID);

					for (int j = 1; j <= N; j++) {

						int arrangementSubsetCandidateIndex = j - 1;
						int subsetCandidateID = arrangement[arrangementSubsetCandidateIndex];
						int subsetCandidateIndex = indexOf(subsetCandidateID);

						if (i != j) {							

							if (matchRunAdjacencyMatrix[i][j] && matchRunNodeTypes[subsetCandidateIndex] == PAIR) {

								for (int k = 1; k <= matchRunNodes[subsetDonorIndex]->getNumberOfDonors(); k++) {

									int donorIndex = k - 1;

									if (availabilityFlags[arrangementSubsetDonorIndex] && 
										availabilityFlags[arrangementSubsetCandidateIndex] && 
										matchRunMatches[subsetDonorID][subsetCandidateID][donorIndex]->getAdjacency()) {

										edgeSubsetDonorNodeIndices.push_back(arrangementSubsetDonorIndex);
										edgeSubsetCandidateNodeIndices.push_back(arrangementSubsetCandidateIndex);
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

					std::vector<std::vector<std::vector<double> > > reducedUtilityMatrix(N, std::vector<std::vector<double> >(N, std::vector<double>(1, 0.0)));

					std::vector<KPDNodeType> reducedNodeTypes(N, PAIR);
					std::vector<std::vector<KPDBloodType> > reducedDonorBloodTypes(N, std::vector<KPDBloodType>(1, BT_AB));

					//Build reduced matrices
					for (int i = 1; i <= N; i++) {

						int arrangementDonorIndex = i - 1;
						int donorNodeID = arrangement[arrangementDonorIndex];
						int donorNodeIndex = indexOf(donorNodeID);

						int donorNodeNumberOfDonors = matchRunNodes[donorNodeIndex]->getNumberOfDonors();

						for (int j = 1; j <= N; j++) {

							int arrangementCandidateIndex = j - 1;
							int candidateNodeID = arrangement[arrangementCandidateIndex];
							int candidateNodeIndex = indexOf(candidateNodeID);

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

						int edgeSubsetDonorNodeID = arrangement[edgeSubsetDonorNodeIndex];
						int edgeSubsetCandidateNodeID = arrangement[edgeSubsetCandidateNodeIndex];
						
						if (edgeFlags[edgeIndex] != 0) {

							probEdgeSubset = probEdgeSubset * matchRunMatches[edgeSubsetDonorNodeID][edgeSubsetCandidateNodeID][edgeSubsetDonorIndex]->getAssumedSuccessProbability();
														
							reducedAdjacencyMatrix[edgeSubsetDonorNodeIndex + 1][edgeSubsetCandidateNodeIndex + 1] = true;

							if (utilityScheme == UTILITY_TRANSPLANTS) { // Transplants
								if (matchRunNodeTypes[indexOf(edgeSubsetCandidateNodeID)] == PAIR) {
									reducedUtilityMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex][edgeSubsetDonorIndex] = 1;
								}
							}
							else {
								reducedUtilityMatrix[edgeSubsetDonorNodeIndex][edgeSubsetCandidateNodeIndex][edgeSubsetDonorIndex] = matchRunMatches[edgeSubsetDonorNodeID][edgeSubsetCandidateNodeID][edgeSubsetDonorIndex]->getUtility(utilityScheme);
							}

						}
						else {
							probEdgeSubset = probEdgeSubset * (1 - matchRunMatches[edgeSubsetDonorNodeID][edgeSubsetCandidateNodeID][edgeSubsetDonorIndex]->getAssumedSuccessProbability());
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

	double pairAssumedProbability = 0;

	int days = timeBetweenSelectionAndTransplantation;
	double p = probPairActiveToInactive + probPairAttrition;

	for (int k = 1; k <= days; k++) {
		pairAssumedProbability += (p * std::pow(1 - p, k - 1));
	}

	pairAssumedProbability = 1 - pairAssumedProbability;

	int N = (int)arrangement.size();
	double expUtility = 0;

	for (int sims = 1; sims <= numberOfExpectedUtilityIterations; sims++) {

		//Initialize random matrices
		std::vector<std::vector<bool> > randomAdjacencyMatrix(1 + N, std::vector<bool>(1 + N, false));

		std::vector<std::vector<std::vector<double> > > randomUtilityMatrix(N, std::vector<std::vector<double> >(N, std::vector<double>(1, 0.0)));

		std::vector<bool> randomAvailabilities(N, false);

		std::vector<KPDNodeType> randomNodeTypes(N, PAIR);
		std::vector<std::vector<KPDBloodType> > randomDonorBloodTypes(N, std::vector<KPDBloodType>(1, BT_AB));
		
		//Simulate pair availabilities, retrive donor information
		for (int i = 1; i <= N; i++) {

			int arrangementIndex = i - 1;
			int nodeID = arrangement[arrangementIndex];
			int nodeIndex = indexOf(nodeID);

			int numDonors = matchRunNodes[nodeIndex]->getNumberOfDonors();

			//Store node type
			randomNodeTypes[arrangementIndex] = matchRunNodeTypes[nodeIndex];
			randomDonorBloodTypes[arrangementIndex].assign(numDonors, BT_AB);

			//Randomly generate candidate availability for PAIRs
			if (matchRunNodeTypes[nodeIndex] == PAIR) {
				if (rngExpectedUtility.runif() < pairAssumedProbability) {
					randomAvailabilities[arrangementIndex] = true;
				}
			}
			else {
				randomAvailabilities[arrangementIndex] = true;
			}
		}

		//Iterate through donor-candidate combinations and randomly generate realized transplants
		for (int i = 1; i <= N; i++) {

			int arrangementDonorIndex = i - 1;
			int arrangementDonorNodeID = arrangement[i - 1];
			int arrangementDonorNodeIndex = indexOf(arrangementDonorNodeID);

			int numDonors = matchRunNodes[arrangementDonorNodeIndex]->getNumberOfDonors();

			for (int j = 1; j <= N; j++) {

				int arrangementCandidateIndex = j - 1;
				int arrangementCandidateNodeID = arrangement[j - 1];
				int arrangementCandidateNodeIndex = indexOf(arrangementCandidateNodeID);

				randomUtilityMatrix[arrangementDonorIndex][arrangementCandidateIndex].assign(numDonors, 0.0);

				if (i != j) {

					//If both nodes are available
					if (randomAvailabilities[arrangementDonorIndex] && randomAvailabilities[arrangementCandidateIndex]) {

						//If donor node is a PAIR and candidate node is a NDD or BRIDGE_DONOR, 
						//insert implicit backward edges from all available donors toward the NDD/BRIDGE node, and set the adjacency matrix to true
						if (randomNodeTypes[arrangementDonorIndex] == PAIR && randomNodeTypes[arrangementCandidateIndex] != PAIR) {
							randomAdjacencyMatrix[i][j] = true;
						}
						//If candidate node is a PAIR, generate random matches
						else if (randomNodeTypes[arrangementCandidateIndex] == PAIR) {

							//Iterate through donors
							for (int k = 1; k <= numDonors; k++) {

								int donorIndex = k - 1;

								// For existing original matches, and for donors that are available, randomly generate matches
								if (matchRunMatches[arrangementDonorNodeID][arrangementCandidateNodeID][donorIndex]->getAdjacency()) {

									if (rngExpectedUtility.runif() < matchRunMatches[arrangementDonorNodeID][arrangementCandidateNodeID][donorIndex]->getAssumedSuccessProbability()) {
										
										if (utilityScheme == UTILITY_TRANSPLANTS) {
											if (matchRunNodeTypes[arrangementCandidateNodeIndex] == PAIR) {
												randomUtilityMatrix[arrangementDonorNodeIndex][arrangementCandidateNodeIndex][donorIndex] = 1;
											}
										}
										else {
											randomUtilityMatrix[arrangementDonorNodeIndex][arrangementCandidateNodeIndex][donorIndex] = matchRunMatches[arrangementDonorNodeID][arrangementCandidateNodeID][donorIndex]->getUtility(utilityScheme);
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
		}

		//After generating matrices, calculate utility
		double partialUtil = calculatePartialUtility(N, randomAdjacencyMatrix, randomUtilityMatrix, randomNodeTypes, randomDonorBloodTypes);
		expUtility += partialUtil;

	}

	//Expected utility is the average of the calculated utility values over the number of iterations
	return expUtility / numberOfExpectedUtilityIterations;

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

						bool nonABBridgeDonor = false;

						if (!allowABBridgeDonors) {

							int bridgeDonorNodeIndex = *(potentialCycleOrChain.end() - 1);

							for (int k = 1; k < donorBloodTypes[bridgeDonorNodeIndex].size(); k++) {

								int donorIndex = k - 1;

								if (donorBloodTypes[bridgeDonorNodeIndex][donorIndex] != BT_AB) {
									nonABBridgeDonor = true;
									break;
								}
							}
						}

						if (allowABBridgeDonors || nonABBridgeDonor) {
							
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

		// Iterate through all possible combinations of cycles/chains
		for (int c = 1; c < pow((double)2, nPossibleCycles); c++) {
			
			double tempU = 0;

			std::vector<int> cycleFlags(nPossibleCycles, 0);
			std::vector<int> checkUniqueness(nV, 0);

			int numberOfCyclesInPossibleSolutions = KPDFunctions::setFlags(c, 0, cycleFlags);

			bool validSolution = true;

			// Iterate through all possible cycles
			for (int q = 0; q < nPossibleCycles; q++) {
				
				// If cycle appears in combination
				if (cycleFlags[q] == 1) {
					
					tempU += utilityOfPossibleCyclesOrChains[q];
					
					// Iterate through the nodes
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
