
#ifndef ARRANGEMENT_H
#define ARRANGEMENT_H

#include "DD-Enums-Functions.h"
#include "DD-Node.h"
#include "DD-Match.h"
#include "DD-Parameters.h"

#include <vector>
#include <string>
#include <deque>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "gurobi_c++.h"

class KPDArrangement {

private:
	
	//Containers

	std::vector<KPDNode *> arrangementNodes;
	std::vector<KPDNodeType> arrangementNodeTypes; // Won't change within arrangement, but good shortcut to have

	std::vector<bool> arrangementCandidateAvailability;
	std::vector<std::vector<bool> > arrangementDonorAvailability;
	
	std::vector<std::vector<std::vector<KPDMatch *> > > arrangementMatchMatrix;

	std::vector<std::vector<std::vector<bool> > > arrangementTransplantSelected;

	std::vector<std::vector<int> > arrangementFallbackOptions;
	std::vector<double> arrangementUtilityOfFallbackOptions;
	
	//Arrangement Information

	KPDOptimizationScheme optimizationScheme;
	bool arrangementHasNDD;

	int currentIteration;
	int matchRun;
	double matchRunTime;
	double timeToTransplantation;
	double assignedUtility;

	//Arrangement Parameters

	KPDUtilityScheme utilityScheme;
	bool allowABBridgeDonors;
	int maxCycleSize;
	int maxChainLength;

	//Helper Functions

	int getChild(int lower, int current, std::vector<int> & visited, std::vector<std::vector<bool> > & incidence);
	int indexOf(int nodeID);
	
public:
	KPDArrangement(KPDParameters * params, int iteration, int mr, double mrTime, double tTime, double util,
		std::vector<KPDNode *> arrangement,
		std::vector<KPDNodeType> nodeTypes,
		std::vector<std::vector<std::vector<KPDMatch *> > > & matches,
		std::vector<std::vector<KPDUnderlyingStatus> > & donorStatus,
		bool ndd, KPDOptimizationScheme scheme);

	~KPDArrangement();
	
	bool hasNDD();
	bool isLRS();

	std::vector<int> getNodeIDs();
	int getNumberOfNodes();

	int getCandidateAvailability(int nodeIndex);
	int getDonorAvailability(int nodeIndex, int donorIndex);

	double getTransplantationTime();
	
	bool getIncidence(int donorNode, int candidateNode, int donorID);

	void setFailure(int nodeID, int donorID, double time);
	void setSelectedTransplant(int donorNodeID, int candidateNodeID, int donorID);

	int selectDonor(int donorNode, int candidateNode, bool convertIDtoIndex);	

	void collectFallBackOptions();
	int numberOfFallBackOptions();
	void calculateUtilityOfFallBackOptions();
	void getOptimalSolutionForFallBackOptions(std::vector<std::vector<int> > & options);	
	
	std::string toLabel();
	std::string toTransplantString();

};

KPDArrangement::KPDArrangement(KPDParameters * params, int iteration, int mr, double mrTime, double tTime, double util,
	std::vector<KPDNode *> arrangement,
	std::vector<KPDNodeType> nodeTypes,
	std::vector<std::vector<std::vector<KPDMatch *> > > & matches,
	std::vector<std::vector<KPDUnderlyingStatus> > & donorStatus,
	bool ndd, KPDOptimizationScheme scheme) {

	//Copy Arrangement
	for (unsigned i = 1; i <= arrangement.size(); i++){
		arrangementNodes.push_back(arrangement[i-1]);
		arrangementNodeTypes.push_back(nodeTypes[i - 1]);
	}
	
	//Initialize Arrangement Properties
	int N = (int)arrangementNodes.size();

	arrangementCandidateAvailability.assign(1 + N, true); // All candidates need to be available
	arrangementDonorAvailability.assign(1 + N, std::vector<bool>(1, false));

	arrangementMatchMatrix.assign(1 + N, std::vector<std::vector<KPDMatch *> >(1 + N, std::vector<KPDMatch *>(1, new KPDMatch())));

	arrangementTransplantSelected.assign(1 + N, std::vector<std::vector<bool> >(1 + N, std::vector<bool>(1, false)));

	for (int i = 1; i <= N; i++){
		int numDonors = arrangementNodes[i - 1]->getNumberOfAssociatedDonors();

		for (int j = 1; j <= N; j++){
			arrangementMatchMatrix[i][j].assign(1 + numDonors, new KPDMatch());

			arrangementTransplantSelected[i][j].assign(1 + numDonors, false);
		}

		arrangementDonorAvailability[i].assign(1 + numDonors, false);
	}

	//Assign Arrangement Properties
	optimizationScheme = scheme;
	arrangementHasNDD = ndd;

	//For Cycles and Chains with No Fallbacks
	if (optimizationScheme == CYCLES_AND_CHAINS) {
		for (int i = 1; i < N; i++) {

			int donorNodeID = arrangementNodes[i - 1]->getID();
			int numDonors = arrangementNodes[i - 1]->getNumberOfAssociatedDonors();
			
			int candidateNodeID = arrangementNodes[(i+1) - 1]->getID();
			
			for (int k = 1; k <= numDonors; k++) {				

				//If the donor is active and matched with the candidate...
				if (donorStatus[donorNodeID][k] == ACTIVE && matches[donorNodeID][candidateNodeID][k]->getIncidence()) {

					arrangementMatchMatrix[i][i + 1][k]->setIncidence(true);
					arrangementMatchMatrix[i][i + 1][k]->setFiveYearSurvival(matches[donorNodeID][candidateNodeID][k]->getFiveYearSurvival());
					arrangementMatchMatrix[i][i + 1][k]->setTenYearSurvival(matches[donorNodeID][candidateNodeID][k]->getTenYearSurvival());
					arrangementMatchMatrix[i][i + 1][k]->setTransplantDifficultyScore(matches[donorNodeID][candidateNodeID][k]->getTransplantDifficultyScore());
					arrangementMatchMatrix[i][i + 1][k]->setRandomUtility(matches[donorNodeID][candidateNodeID][k]->getRandomUtility());
					arrangementMatchMatrix[i][i + 1][k]->setAssumedSuccessProbability(matches[donorNodeID][candidateNodeID][k]->getAssumedSuccessProbability());
					arrangementMatchMatrix[i][i + 1][k]->setActualSuccessProbability(matches[donorNodeID][candidateNodeID][k]->getActualSuccessProbability());
					arrangementMatchMatrix[i][i + 1][k]->setVirtualCrossmatchResult(matches[donorNodeID][candidateNodeID][k]->getVirtualCrossmatchResult());
					arrangementMatchMatrix[i][i + 1][k]->setLabCrossmatchResult(matches[donorNodeID][candidateNodeID][k]->getLabCrossmatchResult());
					
					arrangementDonorAvailability[i][k] = true;

					if (ndd && i != 1) { // Add implict edges for the sub-chain only!
						arrangementMatchMatrix[i][1][k]->setIncidence(true);
						arrangementMatchMatrix[i][1][k]->setAssumedSuccessProbability(1.0);
						arrangementMatchMatrix[i][1][k]->setActualSuccessProbability(1.0);
						arrangementMatchMatrix[i][1][k]->setLabCrossmatchResult(true);
					}					
				}
			}
		}
		
		//Final donation (implicit for chains)
		int donorNodeID = arrangementNodes[N - 1]->getID();
		int numDonors = arrangementNodes[N - 1]->getNumberOfAssociatedDonors();

		int candidateNodeID = arrangementNodes[0]->getID();

		for (int k = 1; k <= numDonors; k++) {

			//If the donor is active and matched with the candidate...
			if (donorStatus[donorNodeID][k] == ACTIVE && matches[donorNodeID][candidateNodeID][k]->getIncidence()) { // Should capture implicit donation information
				
				arrangementMatchMatrix[N][1][k]->setIncidence(true);
				arrangementMatchMatrix[N][1][k]->setFiveYearSurvival(matches[donorNodeID][candidateNodeID][k]->getFiveYearSurvival());
				arrangementMatchMatrix[N][1][k]->setTenYearSurvival(matches[donorNodeID][candidateNodeID][k]->getTenYearSurvival());
				arrangementMatchMatrix[N][1][k]->setTransplantDifficultyScore(matches[donorNodeID][candidateNodeID][k]->getTransplantDifficultyScore());
				arrangementMatchMatrix[N][1][k]->setRandomUtility(matches[donorNodeID][candidateNodeID][k]->getRandomUtility());
				arrangementMatchMatrix[N][1][k]->setAssumedSuccessProbability(matches[donorNodeID][candidateNodeID][k]->getAssumedSuccessProbability());
				arrangementMatchMatrix[N][1][k]->setActualSuccessProbability(matches[donorNodeID][candidateNodeID][k]->getActualSuccessProbability());
				arrangementMatchMatrix[N][1][k]->setVirtualCrossmatchResult(matches[donorNodeID][candidateNodeID][k]->getVirtualCrossmatchResult());
				arrangementMatchMatrix[N][1][k]->setLabCrossmatchResult(matches[donorNodeID][candidateNodeID][k]->getLabCrossmatchResult());

				arrangementDonorAvailability[N][k] = true;				
			}
		}
		
	}

	//For Schemes with Fallbacks
	else {
		for (int i = 1; i <= N; i++) {

			int donorNodeID = arrangementNodes[i - 1]->getID();
			int arrangementDonors = arrangementNodes[i - 1]->getNumberOfAssociatedDonors();

			for (int j = 1; j <= N; j++) {

				int candidateNodeID = arrangementNodes[j - 1]->getID();

				for (int k = 1; k <= arrangementDonors; k++) {

					if (donorStatus[donorNodeID][k] == ACTIVE) {
						
						arrangementDonorAvailability[i][k] = true;
						
						if (matches[donorNodeID][candidateNodeID][k]->getIncidence()) { // Should capture implicit donation information
							
							arrangementMatchMatrix[i][j][k]->setIncidence(true);
							arrangementMatchMatrix[i][j][k]->setFiveYearSurvival(matches[donorNodeID][candidateNodeID][k]->getFiveYearSurvival());
							arrangementMatchMatrix[i][j][k]->setTenYearSurvival(matches[donorNodeID][candidateNodeID][k]->getTenYearSurvival());
							arrangementMatchMatrix[i][j][k]->setTransplantDifficultyScore(matches[donorNodeID][candidateNodeID][k]->getTransplantDifficultyScore());
							arrangementMatchMatrix[i][j][k]->setRandomUtility(matches[donorNodeID][candidateNodeID][k]->getRandomUtility());
							arrangementMatchMatrix[i][j][k]->setAssumedSuccessProbability(matches[donorNodeID][candidateNodeID][k]->getAssumedSuccessProbability());
							arrangementMatchMatrix[i][j][k]->setActualSuccessProbability(matches[donorNodeID][candidateNodeID][k]->getActualSuccessProbability());
							arrangementMatchMatrix[i][j][k]->setVirtualCrossmatchResult(matches[donorNodeID][candidateNodeID][k]->getVirtualCrossmatchResult());
							arrangementMatchMatrix[i][j][k]->setLabCrossmatchResult(matches[donorNodeID][candidateNodeID][k]->getLabCrossmatchResult());
							
						}
					}
				}
			}
		}
	}

	//Arrangement Information
	currentIteration = iteration;
	matchRun = mr;
	matchRunTime = mrTime;
	timeToTransplantation = tTime;
	assignedUtility = util;

	//Parameters
	utilityScheme = params->getUtilityScheme();
	allowABBridgeDonors = params->allowABBridgeDonors();
	maxCycleSize = params->getMaxCycleSize();
	maxChainLength = params->getMaxChainLength();
}

KPDArrangement::~KPDArrangement(){

	arrangementNodes.clear();
	arrangementNodeTypes.clear();

	arrangementCandidateAvailability.clear();
	arrangementDonorAvailability.clear();
	
	arrangementMatchMatrix.clear();

	arrangementFallbackOptions.clear();
	arrangementUtilityOfFallbackOptions.clear();

	arrangementTransplantSelected.clear();
}

bool KPDArrangement::hasNDD() {
	return arrangementHasNDD;
}

bool KPDArrangement::isLRS(){
	return optimizationScheme == LOCALLY_RELEVANT_SUBSETS;
}

std::vector<int> KPDArrangement::getNodeIDs() {

	std::vector<int> nodeIDs;

	for (std::vector<KPDNode *>::iterator nodeIt = arrangementNodes.begin(); nodeIt != arrangementNodes.end(); nodeIt++) {
		nodeIDs.push_back((*nodeIt)->getID());
	}

	return nodeIDs;
}

int KPDArrangement::getNumberOfNodes(){
	return (int)arrangementNodes.size();
}

int KPDArrangement::getCandidateAvailability(int candidateID) {
	return arrangementCandidateAvailability[indexOf(candidateID)];
}

int KPDArrangement::getDonorAvailability(int donorNodeID, int donorID) {
	return arrangementDonorAvailability[indexOf(donorNodeID)][donorID];
}

double KPDArrangement::getTransplantationTime() {
	return matchRunTime + timeToTransplantation;
}

bool KPDArrangement::getIncidence(int donorNode, int candidateNode, int donorID) {
	return arrangementMatchMatrix[indexOf(donorNode)][indexOf(candidateNode)][donorID]->getIncidence();
}

void KPDArrangement::setFailure(int nodeID, int donorID, double time){
	int nodeIndex = indexOf(nodeID);

	if (nodeIndex != -1){
		bool donor = (donorID > 0); // Check if donor or recipient

		if (donor){
			arrangementDonorAvailability[nodeIndex][donorID] = false;
		}
		else {
			arrangementCandidateAvailability[nodeIndex] = false;
		}
	}
}

void KPDArrangement::setSelectedTransplant(int donorNode, int candidateNode, int donorID)
{
	arrangementTransplantSelected[indexOf(donorNode)][indexOf(candidateNode)][donorID] = true;
}

int KPDArrangement::selectDonor(int donorNode, int candidateNode, bool convertIDtoIndex){

	int donorNodeID;
	int candidateNodeID;

	if (convertIDtoIndex){
		donorNodeID = indexOf(donorNode);
		candidateNodeID = indexOf(candidateNode);
	}
	else {
		donorNodeID = donorNode;
		candidateNodeID = candidateNode;
	}

	double maxVal = 0.0;
	int maxIndex = -1; // No Donor Found

	int numDonors = arrangementNodes[donorNodeID-1]->getNumberOfAssociatedDonors();

	for (int k = 1; k <= numDonors; k++) {

		if (arrangementMatchMatrix[donorNodeID][candidateNodeID][k]->getIncidence() && arrangementDonorAvailability[donorNodeID][k]) {
			
			if (utilityScheme == TRANSPLANTS && arrangementDonorAvailability[donorNodeID][k]) {
				if (maxVal == 0.0) {
					maxVal = 1.0;
					maxIndex = k;
				}
			}
			else if (arrangementMatchMatrix[donorNodeID][candidateNodeID][k]->getUtility(utilityScheme) > maxVal) {
				maxVal = arrangementMatchMatrix[donorNodeID][candidateNodeID][k]->getUtility(utilityScheme);
				maxIndex = k;
			}			
		}
	}

	return maxIndex;
}

void KPDArrangement::collectFallBackOptions(){

	int maximum = std::max(maxCycleSize, maxChainLength+1);

	//Set up incidence matrix

	int N = (int)arrangementNodes.size();
	std::vector<std::vector<bool> > incidenceMatrix(1 + N, std::vector<bool>(1 + N, false));

	for (int i = 1; i <= N; i++){

		int arrangementDonors = arrangementNodes[i - 1]->getNumberOfAssociatedDonors();

		for (int j = 1; j <= N; j++){
			if (i != j) {
				bool incident = false;

				if (arrangementNodeTypes[i-1] == PAIR && arrangementNodeTypes[j - 1] != PAIR) {
					incident = true;
				}
				else if (arrangementNodeTypes[j - 1] == PAIR) {
					for (int k = 1; k <= arrangementDonors; k++) {
						if (arrangementMatchMatrix[i][j][k]->getIncidence() && 
							arrangementMatchMatrix[i][j][k]->getLabCrossmatchResult() && 
							arrangementDonorAvailability[i][k] && 
							arrangementCandidateAvailability[j]) {

							incident = true;
						}
					}
				}

				incidenceMatrix[i][j] = incident;
			}
		}
	}

	//Find remaining options

	int start = 1;
	int nV = (int)arrangementNodes.size();
	std::vector<int> visitedVector(nV + 1, 0);
	std::vector<int> stack_vec;

	while (start <= nV){
		visitedVector[start] = 1;

		stack_vec.push_back(start);
		int v = getChild(start, start, visitedVector, incidenceMatrix);
		while (!stack_vec.empty()){
			if (v == -1){
				int top = stack_vec.back();
				stack_vec.pop_back();
				if (top == start){
					visitedVector[top] = 0;
					break;
				}
				visitedVector[top] = 0;
				v = getChild(top, stack_vec.back(), visitedVector, incidenceMatrix);
			}
			else{
				visitedVector[v] = 1;
				stack_vec.push_back(v);
				if (incidenceMatrix[v][start]){

					int multipleNDDCheck = 0;
					int index = 0;

					std::vector<int> potentialCycleOrChain;

					for (unsigned i = 0; i < stack_vec.size(); i++){
						potentialCycleOrChain.push_back(stack_vec[i]);
					}

					for (unsigned i = 0; i < potentialCycleOrChain.size(); i++){
						if (arrangementNodeTypes[potentialCycleOrChain[i]-1] != PAIR){
							multipleNDDCheck++;
							index = i;
						}
					}

					//Chain
					if (multipleNDDCheck == 1 && potentialCycleOrChain.size() <= maxChainLength + 1){
						//Queue up NDD to front of chain
						while (index > 0){
							int temp = *(potentialCycleOrChain.begin());
							potentialCycleOrChain.erase(potentialCycleOrChain.begin());
							potentialCycleOrChain.push_back(temp);
							index--;
						}

						if (!allowABBridgeDonors){

							bool hasNonABBridgeDonor = false;

							for (int k = 1; k <= arrangementNodes[*(potentialCycleOrChain.end() - 1) - 1]->getNumberOfAssociatedDonors(); k++){
								if (arrangementNodes[*(potentialCycleOrChain.end() - 1) - 1]->getDonorBT(k) != BT_AB){
									hasNonABBridgeDonor = true;
								}
							}
							
							if (hasNonABBridgeDonor){
								arrangementFallbackOptions.push_back(potentialCycleOrChain);
							}
						}
						else {
							arrangementFallbackOptions.push_back(potentialCycleOrChain);
						}
					}

					//Cycle
					else if (multipleNDDCheck == 0 && potentialCycleOrChain.size() <= maxCycleSize){
						arrangementFallbackOptions.push_back(potentialCycleOrChain);
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
}

int KPDArrangement::numberOfFallBackOptions(){
	return (int)arrangementFallbackOptions.size();
}

void KPDArrangement::calculateUtilityOfFallBackOptions(){
	if (numberOfFallBackOptions() >= 1){
		for (std::vector<std::vector<int> >::iterator itOption = arrangementFallbackOptions.begin(); itOption != arrangementFallbackOptions.end(); itOption++){
			double u = 0;
			for (std::vector<int>::iterator itNode = (*itOption).begin(); itNode != (*itOption).end() - 1; itNode++){

				int donorID = selectDonor(*itNode, *(itNode + 1), false);
					
				if (utilityScheme == TRANSPLANTS) { // Transplants
					if (arrangementNodeTypes[*(itNode + 1) - 1] == PAIR) {
						u += 1;
					}
				}
				else {
					u += arrangementMatchMatrix[*itNode][*(itNode + 1)][donorID]->getUtility(utilityScheme);
				}				
			}

			int donorID = selectDonor(*(itOption->end() - 1), *(itOption->begin()), false);

			if (utilityScheme == TRANSPLANTS) { // Transplants
				if (arrangementNodeTypes[*(itOption->begin()) - 1] == PAIR) {
					u += 1;
				}
			}
			else {
				u += arrangementMatchMatrix[*(itOption->end() - 1)][*(itOption->begin())][donorID]->getUtility(utilityScheme);
			}			

			arrangementUtilityOfFallbackOptions.push_back(u);
		}
	}
}

void KPDArrangement::getOptimalSolutionForFallBackOptions(std::vector<std::vector<int> > & options){

	collectFallBackOptions();
	calculateUtilityOfFallBackOptions();

	int numOptions = (int)arrangementFallbackOptions.size();
	std::vector<int> selectedFallbacks;

	if (numOptions == 0) return;

	GRBEnv *env = 0; //Created outside try; needs to be recollected;
	GRBVar *vars = 0; //Created outside try; needs to be recollected;
	GRBVar *myVars = new GRBVar[numberOfFallBackOptions() + 1]; //Created outside try; needs to be recollected;
	try{
		env = new GRBEnv();
		env->set(GRB_IntParam_Threads, 4); // Limit the threads

		GRBModel model = GRBModel(*env);
		model.getEnv().set(GRB_IntParam_OutputFlag, 0);// 0 means no solver output; 1 otherwise;

		for (int i = 1; i <= numOptions; i++){
			std::string s = "cycle_" + KPDFunctions::intToString(i);
			myVars[i] = model.addVar(0.0, 1.0, -1 * arrangementUtilityOfFallbackOptions[i - 1], GRB_BINARY, s); //The default is min, hence the -1
		}
		model.update(); //Lazy update;

		for (int i = 1; i <= (int)arrangementNodes.size(); i++){
			GRBLinExpr expr = 0;
			for (int j = 0; j <= numOptions - 1; j++){
				for (std::vector<int>::iterator it = arrangementFallbackOptions[j].begin(); it != arrangementFallbackOptions[j].end(); ++it){
					if (*it == i){
						expr += myVars[j + 1];
					}
				}
			}
			std::string s = "vertex_" + KPDFunctions::intToString(i);
			model.addConstr(expr, GRB_LESS_EQUAL, 1.0, s);
		}

		model.optimize();

		int optimstatus = model.get(GRB_IntAttr_Status);
		//std::cout << "Arrangement - Optimization complete" << std::endl;
		double objval = 0;
		if (optimstatus == GRB_OPTIMAL) { // GRB_OPTIMAL when the MIPGap is reached, default is 1e-4;
			objval = model.get(GRB_DoubleAttr_ObjVal);
			//std::cout << "Optimal objective: " << (-1)*objval << std::endl << std::endl;
		}
		else if (optimstatus == GRB_INF_OR_UNBD) {
			std::cout << "Model is infeasible or unbounded" << std::endl; return;
		}
		else if (optimstatus == GRB_INFEASIBLE) {
			std::cout << "Model is infeasible" << std::endl; return;
		}
		else if (optimstatus == GRB_UNBOUNDED) {
			std::cout << "Model is unbounded" << std::endl; return;
		}
		else {
			std::cout << "Optimization was stopped with status = " << optimstatus << std::endl; return;
		}

		int numvars = model.get(GRB_IntAttr_NumVars);
		vars = model.getVars();

		for (int j = 0; j < numvars; j++) {
			GRBVar v = vars[j];
			if (v.get(GRB_DoubleAttr_X) == 1){ selectedFallbacks.push_back(j); }
		}

	}
	catch (GRBException e) {
		std::cout << "Error code = " << e.getErrorCode() << std::endl;
		std::cout << e.getMessage() << std::endl;
	}
	catch (...) {
		std::cout << "Error during optimization" << std::endl;
	}

	delete[] myVars;
	delete[] vars;
	delete env;

	for (std::vector<int>::iterator fallbackOptionIt = selectedFallbacks.begin(); fallbackOptionIt != selectedFallbacks.end(); fallbackOptionIt++){
		std::vector<int> cycleOrChain;
		for (std::vector<int>::iterator nodeIt = arrangementFallbackOptions[*fallbackOptionIt].begin(); nodeIt != arrangementFallbackOptions[*fallbackOptionIt].end(); nodeIt++){
			cycleOrChain.push_back(arrangementNodes[(*nodeIt) - 1]->getID());
		}
		options.push_back(cycleOrChain);
	}
}

int KPDArrangement::indexOf(int nodeID) {
	for (unsigned i = 1; i <= arrangementNodes.size(); i++) {
		if (arrangementNodes[i - 1]->getID() == nodeID) {
			return i;
		}
	}

	return -1;
}

int KPDArrangement::getChild(int lower, int current, std::vector<int> &visitedVector, std::vector<std::vector<bool> > &incidence){
	int nV = (int)visitedVector.size() - 1;
	for (int j = lower + 1; j <= nV; j++){
		if (incidence[current][j] == true && visitedVector[j] == 0)
			return j;
	}
	return -1;
}

std::string KPDArrangement::toLabel() {
	std::stringstream ss;
	for (unsigned i = 1; i < arrangementNodes.size(); i++) {
		ss << arrangementNodes[i-1]->getID() << "-";
	}
	ss << arrangementNodes.back()->getID();

	return ss.str();
}

std::string KPDArrangement::toTransplantString(){
	std::stringstream ss;

	int N = (int)arrangementNodes.size();
	
	for (int i = 1; i <= N; i++) {

		KPDNode * donorNode = arrangementNodes[i - 1];
		KPDNodeType donorNodeType = arrangementNodeTypes[i - 1];

		int numDonors = donorNode->getNumberOfAssociatedDonors();

		for (int j = 1; j <= N; j++) {

			if (arrangementNodeTypes[j - 1] == PAIR) {
				KPDNode * candidateNode = arrangementNodes[j - 1];

				for (int k = 1; k <= numDonors; k++) {

					KPDMatch * match = arrangementMatchMatrix[i][j][k];

					if (match->getIncidence()) {

						ss << currentIteration << "," << matchRun << "," << matchRunTime << "," << timeToTransplantation << ",";
						
						ss << KPDFunctions::toString(donorNodeType) << "," << donorNode->getID() << "," << k << "," << donorNode->getArrivalTime() << "," << KPDFunctions::boolToString(arrangementDonorAvailability[i][k]) << ",";
						//ss << donorNode->getDonorString(k) << ","; 

						ss << candidateNode->getID() << "," << candidateNode->getArrivalTime() << "," << candidateNode->getNumberOfAssociatedDonors() << "," << KPDFunctions::boolToString(arrangementCandidateAvailability[j]) << ",";
						//ss << candidateNode->getCandidateString() << ",";
						
						ss << match->matchString() << ",";
						
						ss << KPDFunctions::boolToString(arrangementTransplantSelected[i][j][k]) << std::endl;

					}
				}
			}
		}
	}	

	return ss.str();
}


#endif
