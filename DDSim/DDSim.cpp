/* ---------------------------------------------
DDSim.cpp
Main function, creates output directories, runs simulations
---------------------------------------------- */

#ifdef _WIN32
#include <direct.h>
#else
#include "sys/stat.h"
#endif

#include "DD-Arrangement.h"
#include "DD-Candidate.h"
#include "DD-Data.h"
#include "DD-Donor.h"
#include "DD-Enums-Functions.h"
#include "DD-Match.h"
#include "DD-MatchRun.h"
#include "DD-Node.h"
#include "DD-Parameters.h"
#include "DD-Record.h"
#include "DD-RNG.h"
#include "DD-Simulation.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <ctime>

// Parameters
KPDParameters * kpdParameters;

// Output Stream
std::ofstream outputStream;

// Output Files
std::string fileCyclesAndChainsResults;
std::string fileCyclesAndChainsExchanges;

std::string fileFallbacksResults;
std::string fileFallbacksExchanges;

std::string fileLRSResults;
std::string fileLRSExchanges;

std::string filePopulationInformation;

void buildDirectoryStructure(){

	//Variable names
	std::string resultsVariableNames = "Simulation,MatchRun,MatchRunTime,TransplantationTime,DonorType,DonorNodeID,DonorID,DonorArrivalTime,DonorAvailable,CandidateNodeID,CandidateArrivalTime,CandidateAssociatedDonors,CandidateAvailable,Surv5Year,Surv10Year,Score,RandomUtility,TransplantAssumedProb,TransplantActualProb,VirtualCrossmatch,LabCrossmatch,Transplanted";
	std::string exchangesVariableNames = "Simulation,MatchRun,MatchRunTime,TimeToTransplantation,Nodes,NodeIDs,HasNDD,AssignedValue,Selected";
	std::string populationVariableNames = "Simulation,NodeID,DonorID,NodeType,ArrivalTime,rBT,rPRA,rAge,rMale,rRace,rDiabetes,rHeight,rWeight,rBMI,rPrevTrans,rTOD,rHepC,rInsurance,rAssumedProb,rActualProb,dBT,relationToCandidate,dAge,dMale,dRace,dHeight,dWeight,dBMI,dCigaretteUse,dAssumedProb,dActualProb";

	//Set up main output directory
	std::string outputFolder = kpdParameters->getOutputFolder();	
	std::string folderPath = "output/" + outputFolder;
	std::string folderPathWindows = "output\\" + outputFolder;
	
	#ifdef _WIN32
	_mkdir(folderPathWindows.c_str());
	#else 
	mkdir(folderPath.c_str(), 0777);
	#endif

	//Set up simulation sub-directory
	std::string subFolderName = kpdParameters->getSubFolder();
	folderPath = folderPath + "/" + subFolderName;
	folderPathWindows = folderPathWindows + "\\" + subFolderName;
	
	#ifdef _WIN32
	_mkdir(folderPathWindows.c_str());
	#else 
	mkdir(folderPath.c_str(), 0777);
	#endif

	//Add files to sub-directories...

	// Cycles and Chains
	if (kpdParameters->runCyclesAndChains()){

		// 'Cycles and Chains' results file
		fileCyclesAndChainsResults = folderPath + "/Results_CyclesAndChains.csv";

		outputStream.open(fileCyclesAndChainsResults.c_str());
		outputStream << resultsVariableNames << std::endl;
		outputStream.close();

		// 'Cycles and Chains' exchange information file
		if (!kpdParameters->suppressExchangeOutput()) {
			fileCyclesAndChainsExchanges = folderPath + "/Exchanges_CyclesAndChains.csv";

			outputStream.open(fileCyclesAndChainsExchanges.c_str());
			outputStream << exchangesVariableNames << std::endl;
			outputStream.close();
		}
	}

	// Cycles and Chains with Fallbacks
	if (kpdParameters->runCyclesAndChainsWithFallbacks()) {

		// 'Fallbacks' results file
		fileFallbacksResults = folderPath + "/Results_Fallbacks.csv";

		outputStream.open(fileFallbacksResults.c_str());
		outputStream << resultsVariableNames << std::endl;
		outputStream.close();

		// 'Fallbacks' exchange information file
		if (!kpdParameters->suppressExchangeOutput()) {
			fileFallbacksExchanges = folderPath + "/Exchanges_Fallbacks.csv";

			outputStream.open(fileFallbacksExchanges.c_str());
			outputStream << exchangesVariableNames << std::endl;
			outputStream.close();
		}
	}

	// Locally Relevant Subgraphs
	if (kpdParameters->runLocallyRelevantSubgraphs()) {

		// 'LRS' results file
		fileLRSResults = folderPath + "/Results_LRS.csv";

		outputStream.open(fileLRSResults.c_str());
		outputStream << resultsVariableNames << std::endl;
		outputStream.close();

		// 'LRS' exchange information file
		if (!kpdParameters->suppressExchangeOutput()) {
			fileLRSExchanges = folderPath + "/Exchanges_LRS.csv";

			outputStream.open(fileLRSExchanges.c_str());
			outputStream << exchangesVariableNames << std::endl;
			outputStream.close();
		}
	}
	
	// Population information file
	if (!kpdParameters->suppressPopulationOutput()){
		filePopulationInformation = folderPath + "/PopulationList.csv";

		outputStream.open(filePopulationInformation.c_str());
		outputStream << populationVariableNames << std::endl;
		outputStream.close();
	}
}

int main(int argc, const char* argv[]){

	// Initialize default parameters object
	kpdParameters = new KPDParameters();

	// First argument points to parameter file. Process this file to collect parameters.
	if (argc > 1){
		std::string parameterFile = argv[1];
		parameterFile = "parameters/" + parameterFile;
		kpdParameters->processParameters(parameterFile);
	}

	// Set up output directories
	buildDirectoryStructure();
	
	//Run simulation
	std::cout << "Output Folder: output/" << kpdParameters->getOutputFolder() << "/" << kpdParameters->getSubFolder() << std::endl;
	std::cout << std::endl;
	std::cout << "Beginning Simulation" << std::endl;
	
	KPDSimulation * kpdSimulation = new KPDSimulation(kpdParameters);

	int numberOfIterations = kpdParameters->getNumberOfIterations();
	int startingIteration = kpdParameters->getStartingIterationID();
	int currentIteration = startingIteration;
	bool runCyclesAndChains = kpdParameters->runCyclesAndChains();
	bool runCyclesAndChainsWithFallbacks = kpdParameters->runCyclesAndChainsWithFallbacks();
	bool runLocallyRelevantSubgraphs = kpdParameters->runLocallyRelevantSubgraphs();

	while(currentIteration < numberOfIterations + startingIteration){ // Run for 'numberOfIterations' iterations, starting from 'startingIteration'
		
		//Initialize current iteration
		kpdSimulation->prepareIteration(currentIteration);

		//Cycles and Chains
		if (runCyclesAndChains){
			kpdSimulation->runIteration(CYCLES_AND_CHAINS);
			
			//Print 'Cycles and Chains' results
			outputStream.open(fileCyclesAndChainsResults.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputResults();
			outputStream.close();

			//Print 'Cycles and Chains' exchange information
			if (!kpdParameters->suppressExchangeOutput()) {
				outputStream.open(fileCyclesAndChainsExchanges.c_str(), std::ofstream::app);
				outputStream << kpdSimulation->getOutputExchanges();
				outputStream.close();
			}

		}

		//Cycles and Chains with Fallbacks
		if (runCyclesAndChainsWithFallbacks){			
			kpdSimulation->runIteration(CYCLES_AND_CHAINS_WITH_FALLBACKS);
			
			//Print 'Fallbacks' results
			outputStream.open(fileFallbacksResults.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputResults();
			outputStream.close();

			//Print 'Fallbacks' exchange information
			if (!kpdParameters->suppressExchangeOutput()) {
				outputStream.open(fileFallbacksExchanges.c_str(), std::ofstream::app);
				outputStream << kpdSimulation->getOutputExchanges();
				outputStream.close();
			}
		}

		//Locally Relevant Subgraphs
		if (runLocallyRelevantSubgraphs){
			kpdSimulation->runIteration(LOCALLY_RELEVANT_SUBSETS);
			
			//Print 'LRS' results
			outputStream.open(fileLRSResults.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputResults();
			outputStream.close();

			//Print 'LRS' exchange information
			if (!kpdParameters->suppressExchangeOutput()) {
				outputStream.open(fileLRSExchanges.c_str(), std::ofstream::app);
				outputStream << kpdSimulation->getOutputExchanges();
				outputStream.close();
			}
		}

		//Print population information
		if (!kpdParameters->suppressPopulationOutput()) {
			outputStream.open(filePopulationInformation.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputPopulation();
			outputStream.close();
		}

		currentIteration++;
	}	

	std::cout << "Done!" << std::endl;

	//Delete simulation and parameters
	delete kpdSimulation;
	delete kpdParameters;

	#ifdef _WIN32
	system("PAUSE");
	#endif	
	
	return 0;
}