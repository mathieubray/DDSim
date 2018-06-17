#ifdef _WIN32
#include <direct.h>
#else
#include "sys/stat.h"
#endif

#include "DD-Enums-Functions.h"
#include "DD-Parameters.h"
#include "DD-Simulation.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>

// Parameters
KPDParameters * kpdParameters;

// Output Stream
std::ofstream outputStream;

// Output Files
std::string fileCyclesAndChainsArrangementList;
std::string fileCyclesAndChainsTransplantList;
std::string fileCyclesAndChainsSimulationLog;

std::string fileFallbacksArrangementList;
std::string fileFallbacksTransplantList;
std::string fileFallbacksSimulationLog;

std::string fileLRSArrangementList;
std::string fileLRSTransplantList;
std::string fileLRSSimulationLog;

std::string filePopulation;

//Transfer output to file
void updateFile(std::string output, KPDOptimizationScheme scheme, KPDOutputFileType file){
		
	if (scheme == CYCLES_AND_CHAINS){
		if (file == ARRANGEMENT_LIST){
			outputStream.open(fileCyclesAndChainsArrangementList.c_str(), std::ofstream::app);
		}
		else if (file == TRANSPLANT_LIST){
			outputStream.open(fileCyclesAndChainsTransplantList.c_str(), std::ofstream::app);
		}
		else if (file == SIMULATION_LOG){
			outputStream.open(fileCyclesAndChainsSimulationLog.c_str(), std::ofstream::app);
		}
	}
	else if (scheme == CYCLES_AND_CHAINS_WITH_FALLBACKS){
		if (file == ARRANGEMENT_LIST){
			outputStream.open(fileFallbacksArrangementList.c_str(), std::ofstream::app);
		}
		else if (file == TRANSPLANT_LIST){
			outputStream.open(fileFallbacksTransplantList.c_str(), std::ofstream::app);
		}
		else if (file == SIMULATION_LOG){
			outputStream.open(fileFallbacksSimulationLog.c_str(), std::ofstream::app);
		}
	}
	else if (scheme == LOCALLY_RELEVANT_SUBSETS){
		if (file == ARRANGEMENT_LIST){
			outputStream.open(fileLRSArrangementList.c_str(), std::ofstream::app);
		}
		else if (file == TRANSPLANT_LIST){
			outputStream.open(fileLRSTransplantList.c_str(), std::ofstream::app);
		}
		else if (file == SIMULATION_LOG){
			outputStream.open(fileLRSSimulationLog.c_str(), std::ofstream::app);
		}
	}

	outputStream << output;
	outputStream.close();
}

//Print simulation output
void printOutput(KPDOptimizationScheme scheme, KPDSimulation * kpdSimulation){			
	
	if (kpdParameters->outputArrangementList()) {
		updateFile(kpdSimulation->getOutputArrangementList(), scheme, ARRANGEMENT_LIST);
	}
	if (kpdParameters->outputTransplantList()) {
		updateFile(kpdSimulation->getOutputTransplantList(), scheme, TRANSPLANT_LIST);
	}
	if (kpdParameters->outputSimulationLog()){
		updateFile(kpdSimulation->printLog(), scheme, SIMULATION_LOG);
	}
}

//Set up directories
void buildDirectoryStructure(){

	//File headers
	std::string arrangementListColumnNames = "Simulation,MatchRun,MatchRunTime,TimeToTransplantation,Nodes,NodeIDs,HasNDD,AssignedValue,Selected";
	std::string transplantListColumnNames = "Simulation,MatchRun,MatchRunTime,TransplantationTime,DonorType,DonorNodeID,DonorID,DonorArrivalTime,DonorAvailable,CandidateNodeID,CandidateArrivalTime,CandidateAssociatedDonors,CandidateAvailable,Surv5Year,Surv10Year,Score,RandomUtility,TransplantAssumedProb,TransplantActualProb,VirtualCrossmatch,LabCrossmatch,Transplanted";
	std::string populationListColumnNames = "Simulation,NodeID,DonorID,NodeType,ArrivalTime,rBT,rPRA,rAge,rMale,rRace,rDiabetes,rHeight,rWeight,rBMI,rPrevTrans,rTOD,rHepC,rInsurance,rAssumedProb,rActualProb,dBT,relationToCandidate,dAge,dMale,dRace,dHeight,dWeight,dBMI,dCigaretteUse,dAssumedProb,dActualProb";

	//Set up main output directory
	std::string outputFolder = kpdParameters->getOutputFolder();
	
	std::string folderName = "output/DDSim/" + outputFolder;
	std::string folderPath = "output\\DDSim\\" + outputFolder;
	
	#ifdef _WIN32
	_mkdir(folderPath.c_str());
	#else 
	mkdir(folderName.c_str(), 0777);
	#endif

	//Set up simulation sub-directory
	std::string subFolderName = kpdParameters->getSubFolder();
	folderName = folderName + "/" + subFolderName;
	folderPath = folderPath + "\\" + subFolderName;

	std::string folderNameSimulationLogs = folderName + "/SimulationLogs";
	std::string folderPathSimulationLogs = folderPath + "\\SimulationLogs";

	#ifdef _WIN32
	_mkdir(folderPath.c_str());
	_mkdir(folderPathSimulationLogs.c_str());
	#else 
	mkdir(folderName.c_str(), 0777);
	mkdir(folderNameSimulationLogs.c_str(), 0777);
	#endif

	//Add files to sub-directories
	bool runCyclesAndChains = kpdParameters->runCyclesAndChains();
	bool runCyclesAndChainsWithFallbacks = kpdParameters->runCyclesAndChainsWithFallbacks();
	bool runLocallyRelevantSubsets = kpdParameters->runLocallyRelevantSubsets();
	
	if (runCyclesAndChains){
		std::string folderNameCyclesAndChains = folderName + "/CyclesAndChains";
		std::string folderPathCyclesAndChains = folderPath + "\\CyclesAndChains";

		#ifdef _WIN32
		_mkdir(folderPathCyclesAndChains.c_str());
		#else 
		mkdir(folderNameCyclesAndChains.c_str(), 0777);
		#endif

		fileCyclesAndChainsArrangementList = folderNameCyclesAndChains + "/ArrangementList_CyclesAndChains.csv";
		fileCyclesAndChainsTransplantList = folderNameCyclesAndChains + "/TransplantList_CyclesAndChains.csv";
		fileCyclesAndChainsSimulationLog = folderNameCyclesAndChains + "/SimulationLog_CyclesAndChains.txt";
		
		outputStream.open(fileCyclesAndChainsArrangementList.c_str());
		outputStream << arrangementListColumnNames << std::endl;
		outputStream.close();

		outputStream.open(fileCyclesAndChainsTransplantList.c_str());
		outputStream << transplantListColumnNames << std::endl;
		outputStream.close();
		
		outputStream.open(fileCyclesAndChainsSimulationLog.c_str());
		outputStream.close();		
	}

	if (runCyclesAndChainsWithFallbacks) {
		std::string folderNameFallbacks = folderName + "/Fallbacks";
		std::string folderPathFallbacks = folderPath + "\\Fallbacks";

		#ifdef _WIN32
		_mkdir(folderPathFallbacks.c_str());
		#else 
		mkdir(folderNameFallbacks.c_str(), 0777);
		#endif

		fileFallbacksArrangementList = folderNameFallbacks + "/ArrangementList_Fallbacks.csv";
		fileFallbacksTransplantList = folderNameFallbacks + "/TransplantList_Fallbacks.csv";
		fileFallbacksSimulationLog = folderNameFallbacks + "/SimulationLog_Fallbacks.txt";
		
		outputStream.open(fileFallbacksArrangementList.c_str());
		outputStream << arrangementListColumnNames << std::endl;
		outputStream.close();

		outputStream.open(fileFallbacksTransplantList.c_str());
		outputStream << transplantListColumnNames << std::endl;
		outputStream.close();

		outputStream.open(fileFallbacksSimulationLog.c_str());
		outputStream.close();
	}

	if (runLocallyRelevantSubsets) {
		std::string folderNameLRS = folderName + "/LRS";
		std::string folderPathLRS = folderPath + "\\LRS";

		#ifdef _WIN32
		_mkdir(folderPathLRS.c_str());
		#else 
		mkdir(folderNameLRS.c_str(), 0777);
		#endif

		fileLRSArrangementList = folderNameLRS + "/ArrangementList_LRS.csv";
		fileLRSTransplantList = folderNameLRS + "/TransplantList_LRS.csv";
		fileLRSSimulationLog = folderNameLRS + "/SimulationLog_LRS.txt";

		outputStream.open(fileLRSArrangementList.c_str());
		outputStream << arrangementListColumnNames << std::endl;
		outputStream.close();

		outputStream.open(fileLRSTransplantList.c_str());
		outputStream << transplantListColumnNames << std::endl;
		outputStream.close();
		
		outputStream.open(fileLRSSimulationLog.c_str());
		outputStream.close();
	}
	
	//Add population file
	if (kpdParameters->outputPopulationStats() == true){
		filePopulation = folderName + "/PopulationList.csv";

		outputStream.open(filePopulation.c_str());
		outputStream << populationListColumnNames << std::endl;
		outputStream.close();
	}
}

int main(int argc, const char* argv[]){

	// Initialize default parameters object
	kpdParameters = new KPDParameters();

	// First argument points to parameter file. Process this file to collect parameters.
	if (argc > 1){
		std::string parameterFile = argv[1];
		parameterFile = "parameters/DDSim/" + parameterFile;
		kpdParameters->processParameters(parameterFile);
	}

	// Set up output directories
	buildDirectoryStructure();
	
	// Screen output
	bool runCyclesAndChains = kpdParameters->runCyclesAndChains();
	bool runCyclesAndChainsWithFallbacks = kpdParameters->runCyclesAndChainsWithFallbacks();
	bool runLocallyRelevantSubsets = kpdParameters->runLocallyRelevantSubsets();

	std::cout << "Output Folder: output/" << kpdParameters->getOutputFolder() << "/" << kpdParameters->getSubFolder() << std::endl << std::endl;
		
	// Create and run simulation
	KPDSimulation * kpdSimulation = new KPDSimulation(kpdParameters);

	int numberOfIterations = kpdParameters->getNumberOfIterations();
	int startingIteration = kpdParameters->getStartingIterationID();
	int currentIteration = startingIteration;

	while(currentIteration < numberOfIterations + startingIteration){
		
		kpdSimulation->prepareIteration(currentIteration);

		if (runCyclesAndChains){
			kpdSimulation->runIteration(CYCLES_AND_CHAINS);
			printOutput(CYCLES_AND_CHAINS, kpdSimulation);
		}
		if (runCyclesAndChainsWithFallbacks){			
			kpdSimulation->runIteration(CYCLES_AND_CHAINS_WITH_FALLBACKS);
			printOutput(CYCLES_AND_CHAINS_WITH_FALLBACKS, kpdSimulation);
		}
		if (runLocallyRelevantSubsets){
			kpdSimulation->runIteration(LOCALLY_RELEVANT_SUBSETS);
			printOutput(LOCALLY_RELEVANT_SUBSETS, kpdSimulation);
		}

		if (kpdParameters->outputPopulationStats()) {
			outputStream.open(filePopulation.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputPopulationList();
			outputStream.close();
		}

		currentIteration++;
	}	

	delete kpdSimulation;
	delete kpdParameters;

	#ifdef _WIN32
	system("PAUSE");
	#endif	
	
	return 0;
}