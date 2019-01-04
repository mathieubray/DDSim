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
std::string fileKPDResults;
std::string fileDDResults;

std::string fileKPDExchanges;
std::string fileDDExchanges;

std::string fileKPDPopulation;

void buildDirectoryStructure(){

	// Variable names
	std::string kpdResultsVariableNames = "Simulation,MatchRun,MatchRunTime,TransplantationTime,DonorType,DonorNodeID,DonorID,DonorArrivalTime,DonorAvailable,CandidateNodeID,CandidateArrivalTime,CandidateAssociatedDonors,CandidateAvailable,Surv5Year,Surv10Year,Score,RandomUtility,TransplantAssumedProb,TransplantActualProb,VirtualCrossmatch,LabCrossmatch,Transplanted";
	std::string deceasedDonorResultsVariableNames = "";

	std::string selectedKPDExchangesVariableNames = "";
	std::string selectedDeceasedDonorChainsVariableNames = "";
	
	std::string kpdPopulationVariableNames = "Simulation,NodeID,DonorID,NodeType,ArrivalTime,rBT,rPRA,rAge,rMale,rRace,rDiabetes,rHeight,rWeight,rBMI,rPrevTrans,rTOD,rHepC,rInsurance,rAssumedProb,rActualProb,dBT,relationToCandidate,dAge,dMale,dRace,dHeight,dWeight,dBMI,dCigaretteUse,dAssumedProb,dActualProb";
	

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

	// Add results file to sub-directory
	fileKPDResults = folderPath + "/KPDResults.csv";

	outputStream.open(fileKPDResults.c_str());
	outputStream << kpdResultsVariableNames << std::endl;
	outputStream.close();

	fileDDResults = folderPath + "/DDResults.csv";

	outputStream.open(fileDDResults.c_str());
	outputStream << deceasedDonorResultsVariableNames << std::endl;
	outputStream.close();

	// Add exchange information file to sub-directory
	if (!kpdParameters->suppressExchangeOutput()) {
		fileKPDExchanges = folderPath + "/KPDExchanges.csv";

		outputStream.open(fileKPDExchanges.c_str());
		outputStream << selectedKPDExchangesVariableNames << std::endl;
		outputStream.close();

		fileDDExchanges = folderPath + "/DDExchanges.csv";

		outputStream.open(fileDDExchanges.c_str());
		outputStream << selectedDeceasedDonorChainsVariableNames << std::endl;
		outputStream.close();
	}

	// Add KPD population information file to sub-directory
	if (!kpdParameters->suppressPopulationOutput()) {
		fileKPDPopulation = folderPath + "/KPDPopulation.csv";

		outputStream.open(fileKPDPopulation.c_str());
		outputStream << kpdPopulationVariableNames << std::endl;
		outputStream.close();
	}
}

int main(int argc, const char* argv[]){

	// Initialize default parameters object
	kpdParameters = new KPDParameters();

	// First argument points to parameter file; process this file to collect parameters
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

	while(currentIteration < startingIteration + numberOfIterations) { // Run for specified number iterations, starting from a given iteration
		
		//Initialize current iteration
		kpdSimulation->runIteration(currentIteration);

		//Print results
		outputStream.open(fileKPDResults.c_str(), std::ofstream::app);
		outputStream << kpdSimulation->getOutputKPDResults();
		outputStream.close();

		outputStream.open(fileDDResults.c_str(), std::ofstream::app);
		outputStream << kpdSimulation->getOutputDDResults();
		outputStream.close();

		//Print exchange information
		if (!kpdParameters->suppressExchangeOutput()) {
			outputStream.open(fileKPDExchanges.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputKPDExchanges();
			outputStream.close();

			outputStream.open(fileDDExchanges.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputDDExchanges();
			outputStream.close();
		}

		//Print population information
		if (!kpdParameters->suppressPopulationOutput()) {
			outputStream.open(fileKPDPopulation.c_str(), std::ofstream::app);
			outputStream << kpdSimulation->getOutputKPDPopulation();
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