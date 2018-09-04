/* ---------------------------------------------
DDSim.cpp
Creates output directories, runs simulations, main file
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
std::string fileResults;
std::string fileExchanges;
std::string filePopulation;

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

	// Results file
	fileResults = folderPath + "/Results.csv";

	outputStream.open(fileResults.c_str());
	outputStream << resultsVariableNames << std::endl;
	outputStream.close();

	// Exchange information file
	if (!kpdParameters->suppressExchangeOutput()) {
		fileExchanges = folderPath + "/Exchanges.csv";

		outputStream.open(fileExchanges.c_str());
		outputStream << exchangesVariableNames << std::endl;
		outputStream.close();
	}

	// Population information file
	if (!kpdParameters->suppressPopulationOutput()) {
		filePopulation = folderPath + "/Population.csv";

		outputStream.open(filePopulation.c_str());
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
	
	// Screen output
	std::cout << "Output Folder: output/" << kpdParameters->getOutputFolder() << "/" << kpdParameters->getSubFolder() << std::endl;
	std::cout << std::endl;
	std::cout << "Beginning Simulation..." << std::endl;
	
	// Run simulation
	KPDSimulation * kpdSimulation = new KPDSimulation(kpdParameters);

	int numberOfIterations = kpdParameters->getNumberOfIterations();
	int startingIteration = kpdParameters->getStartingIterationID();
	int currentIteration = startingIteration;

	while(currentIteration < numberOfIterations + startingIteration){ // Run for 'numberOfIterations' iterations, starting from 'startingIteration'
		
		//Initialize current iteration
		kpdSimulation->prepareIteration(currentIteration);

		kpdSimulation->resetIteration();
		kpdSimulation->runIteration();

		//Print results
		outputStream.open(fileResults.c_str(), std::ofstream::app);
		outputStream << kpdSimulation->getOutputResults();
		outputStream.close();

		//Print exchange information
		if (!kpdParameters->suppressExchangeOutput()) {
			outputStream.open(fileExchanges.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputExchanges();
			outputStream.close();
		}

		//Print population information
		if (!kpdParameters->suppressPopulationOutput()) {
			outputStream.open(filePopulation.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputPopulation();
			outputStream.close();
		}

		currentIteration++;
	}	

	std::cout << "...Ending Simulation" << std::endl;

	//Delete simulation and parameters
	delete kpdSimulation;
	delete kpdParameters;

	#ifdef _WIN32
	system("PAUSE");
	#endif	
	
	return 0;
}