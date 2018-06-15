
#ifndef DATA_H
#define DATA_H

#include "KPD-Enums-Functions.h"
#include "KPD-Candidate.h"
#include "KPD-Donor.h"
#include "KPD-Parameters.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

class KPDData {

private:

	//Raw Data
	std::vector<std::vector<std::vector<std::string> > > hlaFrequencyData;
	std::vector<std::vector<std::vector<std::string> > > hlaDictionaryData;
	std::vector<std::vector<std::vector<std::string> > > characteristicsData;
	std::vector<std::vector<std::vector<std::string> > > kpdData;

	//Clean Data	
	std::vector<std::vector<std::string> > donorHLA;
	std::vector<double> donorHLAFrequency;

	std::map<std::string, std::vector<std::string> > hlaDictionary;

	std::map<std::string, std::vector<double> > survivalParameters;

	std::map<std::string, std::vector<double> > characteristicsFrequency;

	std::vector<KPDDonor *> nddPool;
	std::vector<KPDCandidate *> candidatePool;
	std::vector<KPDCandidate *> pairedCandidatesPool;
	std::map<int, std::vector<KPDDonor *> > pairedDonorsPool;

	void readDataFromFile(std::vector<std::vector<std::vector<std::string> > >  & parsedData, std::string fileName);
		
	void formDonorHLAFrequency();
	void formHLADictionary();	
	void formSurvivalParameters();	
	void formCharacteristicsFrequency(bool excludeABDonors);
	void formKPDPopulation(bool excludeABDonors, bool includeCompatiblePairs, bool allowDesensitization); // Needs to come after loading HLA dictionary

	std::stringstream kpdDataLog;

	std::string logFile;

public:

	KPDData(KPDParameters * params);
	~KPDData();

	KPDCandidate * drawCandidate(double u);
	KPDDonor * drawNDD(double u);
	int drawPair(double u);

	KPDRelationToCandidate drawRelation(double u, double candidateAge);
	std::vector<std::string> drawHLA(double u);
	bool drawHLABW(double u, bool bw4);
	KPDBloodType drawBloodType(double u);
	KPDRace drawRace(double u);
	bool drawCigaretteUse(double u);

	KPDCandidate * retrieveCandidate(int index);
	std::vector<KPDDonor *> retrieveDonors(int index);

	KPDCrossmatchResult performCrossmatch(KPDCandidate * candidate, KPDDonor * donor);
	bool isCompatiblePair(KPDCandidate * candidate, std::vector<KPDDonor *> donors, bool reserveODonorForOCandidates, bool allowDesensitization);
	double calculateSurvival(KPDCandidate * candidate, KPDDonor * donor, int fiveyear);
	
	void printLog();
};

KPDData::KPDData(KPDParameters * params) {

	kpdDataLog << "--------------------------" << std::endl;
	kpdDataLog << "Reading Data from Files..." << std::endl;
	kpdDataLog << "--------------------------" << std::endl;

	kpdDataLog << std::endl;
		
	readDataFromFile(hlaFrequencyData, "data/" + params->getFileHLAFrequency());
	readDataFromFile(hlaDictionaryData, "data/" + params->getFileHLADictionary());
	readDataFromFile(characteristicsData, "data/" + params->getFileCharacteristics());
	readDataFromFile(kpdData, "data/" + params->getFileKPD());
	
	kpdDataLog << std::endl;
		
	formDonorHLAFrequency();
	formHLADictionary();
	formSurvivalParameters();
	formCharacteristicsFrequency(params->excludeABDonorsFromSimulation());
	formKPDPopulation(params->excludeABDonorsFromSimulation(), params->includeCompatiblePairs(), params->allowDesensitization());

	logFile = "output/KPDSim/" + params->getOutputFolder() + "/" + params->getSubFolder() + "/SimulationLogs/Log-Data.txt";
}

KPDData::~KPDData() {
	printLog();
}


// Reads information from csv files into three-dimensional arrays
void KPDData::readDataFromFile(std::vector<std::vector<std::vector<std::string> > > & parsedData, std::string fileName) {

	std::stringstream log;
	std::ifstream iFile;
	iFile.open(fileName.c_str());

	if (!iFile.is_open()) {
		std::cerr << "Cannot open file for reading: " << fileName << std::endl;
	}

	int i = 0;
	while (!iFile.eof()) {

		//Extract row
		std::stringstream rowStream;
		std::string row;
		getline(iFile, row);

		if (row.empty())
			continue;

		rowStream << row;

		if (i > 0) {

			std::vector<std::vector<std::string> > dataRow;

			while (!rowStream.eof()) {

				//Extract token
				std::stringstream tokenStream;
				std::string token;
				getline(rowStream, token, ',');

				if (token.empty())
					continue;

				tokenStream << token;

				std::vector<std::string> dataField;

				while (!tokenStream.eof()) {

					std::string subToken;
					getline(tokenStream, subToken, ';');

					dataField.push_back(subToken);
				}

				dataRow.push_back(dataField);
			}
			
			parsedData.push_back(dataRow);
		}

		i++;
	}

	iFile.close();

	kpdDataLog << "Read in " << i << " rows from " << fileName << std::endl;
}

void KPDData::formDonorHLAFrequency() {

	kpdDataLog << "--------------------------------" << std::endl;
	kpdDataLog << "Setting Donor HLA Frequencies..." << std::endl;
	kpdDataLog << "--------------------------------" << std::endl;

	kpdDataLog << std::endl;

	//Columns 1-8: Major HLA antigens; Column 9: Frequency
	int i = 1;
	for (std::vector<std::vector<std::vector<std::string> > >::iterator hlaRow = hlaFrequencyData.begin(); hlaRow != hlaFrequencyData.end(); hlaRow++) {
		std::vector<std::string> donorProfile;
		
		kpdDataLog << i << ": ";

		for (std::vector<std::vector<std::string> >::iterator hla = hlaRow->begin(); hla != hlaRow->begin() + 8; hla++) {

			std::string antigen = (*hla)[0];

			if (antigen.compare("NULL") != 0) {

				kpdDataLog << antigen << " ";

				donorProfile.push_back(antigen);
			}
		}

		donorHLA.push_back(donorProfile);

		//Stores frequency info
		std::vector<std::vector<std::string> >::iterator profileFreq = (*hlaRow).begin() + 8;
		std::string freq = (*profileFreq).at(0);
		double probability = atof(freq.c_str());
		donorHLAFrequency.push_back(probability);

		kpdDataLog << "(" << probability << ")" << std::endl;

		i++;
	}

	kpdDataLog << std::endl;

	kpdDataLog << "Donor HLA Frequencies Set" << std::endl << std::endl;
}

void KPDData::formHLADictionary() {

	kpdDataLog << "--------------------------------" << std::endl;
	kpdDataLog << "Loading HLA Dictionary..." << std::endl;
	kpdDataLog << "--------------------------------" << std::endl;

	kpdDataLog << std::endl;

	int i = 1;

	for (std::vector<std::vector<std::vector<std::string> > >::iterator dictionaryRow = hlaDictionaryData.begin(); dictionaryRow != hlaDictionaryData.end(); dictionaryRow++) {
		std::string antigen = (*dictionaryRow)[0][0];

		kpdDataLog << i << ". " << antigen << ": ";

		for (std::vector<std::string>::iterator hla = (*dictionaryRow)[1].begin(); hla != (*dictionaryRow)[1].end(); hla++) {
			hlaDictionary[antigen].push_back(*hla);

			kpdDataLog << *hla << " ";
		}

		kpdDataLog << std::endl;

		i++;
	}

	kpdDataLog << std::endl;

	kpdDataLog << "HLA Dictionary Loaded" << std::endl << std::endl;
}

void KPDData::formSurvivalParameters() {

	kpdDataLog << "------------------------------" << std::endl;
	kpdDataLog << "Setting Survival Parameters..." << std::endl;
	kpdDataLog << "------------------------------" << std::endl;

	kpdDataLog << std::endl;

	int i = 1;

	for (std::vector<std::vector<std::vector<std::string> > >::iterator paramRow = characteristicsData.begin(); paramRow != characteristicsData.end(); paramRow++) {

		std::string characteristic = (*paramRow)[0][0];

		kpdDataLog << i << ". " << characteristic << ": ";

		// Store 5-year and 10-year survival parameters
		survivalParameters["5 Year " + characteristic].push_back(atof((*paramRow)[2][0].c_str()));
		kpdDataLog << (*paramRow)[2][0] << " (5-Year), ";

		survivalParameters["10 Year " + characteristic].push_back(atof((*paramRow)[3][0].c_str()));
		kpdDataLog << (*paramRow)[3][0] << " (10-Year)"  << std::endl;

		i++;
	}

	kpdDataLog << std::endl;
	
	kpdDataLog << "Survival Parameters Set" << std::endl << std::endl;
}

void KPDData::formCharacteristicsFrequency(bool excludeAB) {

	kpdDataLog << "------------------------------------" << std::endl;
	kpdDataLog << "Setting Characteristics Frequency..." << std::endl;
	kpdDataLog << "------------------------------------" << std::endl;

	kpdDataLog << std::endl;
		
	int i = 1;

	for (std::vector<std::vector<std::vector<std::string> > >::iterator paramRow = characteristicsData.begin(); paramRow != characteristicsData.end(); paramRow++) {

		std::string characteristic = (*paramRow)[0][0];

		// Collect probabilities for randomly assigned characterisitcs
		if (characteristic.compare("Donor Race") == 0 || characteristic.compare("Donor Cigarette Use") == 0 || characteristic.compare("Previous Transplant") == 0 ||
			characteristic.compare("Time On Dialysis") == 0 || characteristic.compare("Recipient Hepatitis C Seriology") == 0 || characteristic.compare("Recipient Insurance") == 0) {

			characteristicsFrequency[characteristic].push_back(atof((*paramRow)[4][0].c_str()));

			kpdDataLog << i << ". " << characteristic << ": " << (*paramRow)[4][0] << std::endl;
			i++;
		}
	}

	characteristicsFrequency["Blood Type"].push_back(0.44); // BT_O
	characteristicsFrequency["Blood Type"].push_back(0.42); // BT_A
	characteristicsFrequency["Blood Type"].push_back(0.10); // BT_B
	if (!excludeAB) {
		characteristicsFrequency["Blood Type"].push_back(0.04); // BT_AB
	}

	for (std::vector<double>::iterator it = characteristicsFrequency["Blood Type"].begin(); it != characteristicsFrequency["Blood Type"].end(); it++) {
		kpdDataLog << i << ". Blood Type: " << *it << std::endl;
		i++;
	}

	characteristicsFrequency["Relation To Donor"].push_back(0.4); // SIGNIFICANT_OTHER
	characteristicsFrequency["Relation To Donor"].push_back(0.1); // MOTHER
	characteristicsFrequency["Relation To Donor"].push_back(0.1); // FATHER
	characteristicsFrequency["Relation To Donor"].push_back(0.15); // RELATED
	characteristicsFrequency["Relation To Donor"].push_back(0.15); // UNRELATED
	characteristicsFrequency["Relation To Donor"].push_back(0.1); // CHILD

	for (std::vector<double>::iterator it = characteristicsFrequency["Relation To Donor"].begin(); it != characteristicsFrequency["Relation To Donor"].end(); it++) {
		kpdDataLog << i << ". Relation To Donor: " << *it << std::endl;
		i++;
	}

	characteristicsFrequency["Donor HLA BW"].push_back(0.66); // BW4
	characteristicsFrequency["Donor HLA BW"].push_back(0.85); // BW6
	
	for (std::vector<double>::iterator it = characteristicsFrequency["Donor HLA BW"].begin(); it != characteristicsFrequency["Donor HLA BW"].end(); it++) {
		kpdDataLog << i << ". Donor HLA BW: " << *it << std::endl;
		i++;
	}

	kpdDataLog << std::endl;

	kpdDataLog << "Characteristics Frequencies Set" << std::endl << std::endl;

}

//Forms population of pairs from separate donor and candidate files
void KPDData::formKPDPopulation(bool excludeABDonors, bool includeCompatiblePairs, bool allowDesensitization) {

	kpdDataLog << "-------------------------" << std::endl;
	kpdDataLog << "Forming KPD Population..." << std::endl;
	kpdDataLog << "-------------------------" << std::endl;

	kpdDataLog << std::endl;

	int numberOfCandidates = 0;
	int numberOfNDDs = 0;
	int numberOfPairedCandidates = 0;
	int numberOfPairedDonors = 0;

	std::vector<std::vector<std::vector<std::string> > >::iterator it = kpdData.begin();

	while (it != kpdData.end()) {

		int matchingID = atoi((*it)[0][0].c_str());
		std::string ndd = (*it)[1][0];

		//Non-Directed Donor
		if (ndd.compare("TRUE") == 0) {

			KPDRelationToCandidate dRelation = NON_DIRECTED;

			int donorID = atoi((*it)[3][0].c_str());

			//Donor Crossmatch Information
			KPDBloodType dBT = KPDFunctions::stringToBloodType((*it)[7][0]);

			std::vector<std::string> dHLA;
			for (std::vector<std::string>::iterator hla = (*it)[8].begin(); hla != (*it)[8].end(); hla++) {
				dHLA.push_back(*hla);
			}

			//Donor Characteristics
			int dAge = atoi((*it)[15][0].c_str());
			bool dMale = ((*it)[16][0].compare("MALE") == 0);
			double dHeight = atof((*it)[17][0].c_str());
			double dWeight = atof((*it)[18][0].c_str());

			KPDRace dRace = OTHER_RACE;
			bool dCigaretteUse = false;

			KPDDonor * d = new KPDDonor(matchingID, donorID, dBT, dHLA, dRelation, dAge, dMale, dRace, dHeight, dWeight, dCigaretteUse);

			if (!(dBT == BT_AB && excludeABDonors)) {

				kpdDataLog << "NDD " << donorID << " added to NDD Pool" << std::endl;
				kpdDataLog << d->donorOutput(3) << std::endl;

				nddPool.push_back(d);
				numberOfNDDs++;
			}
			else {
				kpdDataLog << "NDD" << donorID << " Excluded" << std::endl;
				kpdDataLog << d->donorOutput(3) << std::endl;
			}

			it++;
		}

		//Incompatible Donor-Candidate Pairing
		else {
			int candidateID = atoi((*it)[2][0].c_str());

			//Recipient Crossmatch Information
			int cPRA = atoi((*it)[4][0].c_str());
			KPDBloodType cBT = KPDFunctions::stringToBloodType((*it)[5][0]);

			std::vector<std::string> cUnacceptableHLA;
			std::vector<std::string> cDesensitizableHLA;
			for (std::vector<std::string>::iterator hla = (*it)[6].begin(); hla != (*it)[6].end(); hla++) {
				cUnacceptableHLA.push_back(*hla);
			}

			//Recipient Characteristics
			int cAge = atoi((*it)[9][0].c_str());
			bool cMale = ((*it)[10][0].compare("MALE") == 0);
			KPDRace cRace = KPDFunctions::stringToRace((*it)[11][0]);
			bool cDiabetes = ((*it)[12][0].compare("TRUE") == 0);
			double cHeight = atof((*it)[13][0].c_str());
			double cWeight = atof((*it)[14][0].c_str());

			bool cPrevTrans = false;
			double cTOD = 1.5;
			bool cHepC = false;
			KPDInsurance cInsurance = OTHER_INSURANCE;

			KPDCandidate * c = new KPDCandidate(matchingID, candidateID, cPRA, cBT, cUnacceptableHLA, cDesensitizableHLA,
				cAge, cMale, cRace, cDiabetes, cHeight, cWeight, cPrevTrans, cTOD, cHepC, cInsurance);

			kpdDataLog << "Candidate " << candidateID << " (Matching ID: " << matchingID << ") added to Candidate Pool" << std::endl;
			kpdDataLog << c->candidateOutput(3) << std::endl;

			candidatePool.push_back(c);
			numberOfCandidates++;

			// Donors
			std::vector<KPDDonor *> associatedDonors;
			associatedDonors.push_back(new KPDDonor());

			bool abDonor = false;

			do {
				int donorID = atoi((*it)[3][0].c_str());

				//Donor Crossmatch Information
				KPDBloodType dBT = KPDFunctions::stringToBloodType((*it)[7][0]);

				if (dBT == BT_AB) {
					abDonor = true;
				}

				std::vector<std::string> dHLA;
				for (std::vector<std::string>::iterator hla = (*it)[8].begin(); hla != (*it)[8].end(); hla++) {
					dHLA.push_back(*hla);
				}

				//Donor Characteristics
				KPDRelationToCandidate dRelation = UNRELATED;

				int dAge = atoi((*it)[15][0].c_str());
				bool dMale = ((*it)[16][0].compare("MALE") == 0);
				double dHeight = atof((*it)[17][0].c_str());
				double dWeight = atof((*it)[18][0].c_str());

				KPDRace dRace = OTHER_RACE;
				bool dCigaretteUse = false;

				KPDDonor * d = new KPDDonor(matchingID, donorID, dBT, dHLA, dRelation, dAge, dMale, dRace, dHeight, dWeight, dCigaretteUse);

				associatedDonors.push_back(d);

				it++;

			} while (matchingID == atoi((*it)[0][0].c_str()) && it != kpdData.end());

			if (!((abDonor && excludeABDonors) || (!includeCompatiblePairs && isCompatiblePair(c, associatedDonors, false, allowDesensitization)))) {

				kpdDataLog << "Donors Matched with Candidate " << matchingID << std::endl;
				for (std::vector<KPDDonor *>::iterator donorIt = associatedDonors.begin() + 1; donorIt != associatedDonors.end(); donorIt++) {
					kpdDataLog << (*donorIt)->donorOutput(3) << std::endl;
				}

				numberOfPairedCandidates++;
				numberOfPairedDonors += (int)associatedDonors.size() - 1;

				pairedCandidatesPool.push_back(c);
				pairedDonorsPool[matchingID] = associatedDonors;
			}

			else {
				kpdDataLog << "Donors Excluded with Candidate " << matchingID << std::endl;
				for (std::vector<KPDDonor *>::iterator donorIt = associatedDonors.begin() + 1; donorIt != associatedDonors.end(); donorIt++) {
					kpdDataLog << (*donorIt)->donorOutput(3) << std::endl;
				}
			}
		}
	}

	kpdDataLog << "Total Candidates: " << numberOfCandidates << std::endl;
	kpdDataLog << "Non-Directed Donors: " << numberOfNDDs << std::endl;
	kpdDataLog << "Paired Candidates: " << numberOfPairedCandidates << std::endl;
	kpdDataLog << "Paired Donors: " << numberOfPairedDonors << std::endl << std::endl;
}

KPDCandidate * KPDData::drawCandidate(double u) {

	int candidateIndex = (int)(u * pairedCandidatesPool.size());
	KPDCandidate * candidate = pairedCandidatesPool[candidateIndex]->copy();

	return candidate;
}

KPDDonor * KPDData::drawNDD(double u) {

	int nddIndex = (int)(u * nddPool.size());
	KPDDonor * donor = nddPool[nddIndex]->copy();

	return donor;
}

int KPDData::drawPair(double u){

	int pairIndex = (int)(u * pairedCandidatesPool.size());
	
	return pairIndex;
}

KPDRelationToCandidate KPDData::drawRelation(double u, double candidateAge){
	
	int adjustment = 0;
	if (candidateAge < 40) {
		adjustment = 1;
	}

	double total = 0.0;

	for (std::vector<double>::iterator it = characteristicsFrequency["Relation To Donor"].begin(); it != characteristicsFrequency["Relation To Donor"].end() - adjustment; it++) {
		total += *it;
	}

	double adjustedU = u * total;

	int relationIndex = KPDFunctions::retrieveDiscreteSampleIndex(adjustedU, characteristicsFrequency["Relation To Donor"]);

	KPDRelationToCandidate relation = SIGNIFICANT_OTHER;
	if (relationIndex == 1) {
		relation = MOTHER;
	}
	else if (relationIndex == 2) {
		relation = FATHER;
	}
	else if (relationIndex == 3) {
		relation = RELATED;
	}
	else if (relationIndex == 4) {
		relation = UNRELATED;
	}
	else if (relationIndex == 5) {
		relation = CHILD;
	}

	return relation;
}

std::vector<std::string> KPDData::drawHLA(double u){

	int hlaIndex = KPDFunctions::retrieveDiscreteSampleIndex(u, donorHLAFrequency);

	std::vector<std::string> hla;

	for (std::vector<std::string>::iterator it = donorHLA[hlaIndex].begin(); it != donorHLA[hlaIndex].end(); it++) {
		hla.push_back(*it);
	}

	return hla;
}

bool KPDData::drawHLABW(double u, bool bw4){

	if (bw4) {
		return u <= characteristicsFrequency["Donor HLA BW"][0];
	}
	else {
		return u <= characteristicsFrequency["Donor HLA BW"][1];
	}
}

KPDBloodType KPDData::drawBloodType(double u){

	double total = 0.0;

	for (std::vector<double>::iterator it = characteristicsFrequency["Blood Type"].begin(); it != characteristicsFrequency["Blood Type"].end(); it++) {
		total += *it;
	}

	double adjustedU = u * total;

	int btIndex = KPDFunctions::retrieveDiscreteSampleIndex(adjustedU, characteristicsFrequency["Blood Type"]);

	KPDBloodType bt = BT_O;
	if (btIndex == 1) {
		bt = BT_A;
	}
	else if (btIndex == 2) {
		bt = BT_B;
	}
	else if (btIndex == 3) {
		bt = BT_AB;
	}

	return bt;
}

KPDRace KPDData::drawRace(double u){

	int raceIndex = KPDFunctions::retrieveDiscreteSampleIndex(u, characteristicsFrequency["Donor Race"]);
	
	KPDRace race = WHITE;

	if (raceIndex == 1) { race = BLACK; }
	else if (raceIndex == 2) { race = HISPANIC; }

	return race;
}

bool KPDData::drawCigaretteUse(double u){

	return u > characteristicsFrequency["Donor Cigarette Use"][0];
}

KPDCandidate * KPDData::retrieveCandidate(int index){
	
	return pairedCandidatesPool[index]->copy();
}

std::vector<KPDDonor*> KPDData::retrieveDonors(int index){

	std::vector<KPDDonor *> donors;
	
	donors.push_back(new KPDDonor());

	for (std::vector<KPDDonor *>::iterator it = pairedDonorsPool[index].begin() + 1; it != pairedDonorsPool[index].end(); it++) {
		donors.push_back((*it)->copy());
	}

	return donors;
}

//Checks for Match Between Candidate and Donor
KPDCrossmatchResult KPDData::performCrossmatch(KPDCandidate * candidate, KPDDonor * donor) {

	//Check BT match
	if ((donor->getBT() == BT_AB && candidate->getBT() != BT_AB) ||
		(donor->getBT() == BT_A && (candidate->getBT() == BT_O || candidate->getBT() == BT_B)) ||
		(donor->getBT() == BT_B && (candidate->getBT() == BT_O || candidate->getBT() == BT_A))) {

		return FAILED_CROSSMATCH_BT;
	}

	//Check HLA
	std::vector<std::string> candidateUnacceptableHLA = candidate->getUnacceptableHLA();
	std::vector<std::string> candidateDesensitizableHLA = candidate->getDesensitizableHLA();
	std::vector<std::string> donorHLA = donor->getHLA();

	//Unacceptable HLA
	for (std::vector<std::string>::iterator cHLA = candidateUnacceptableHLA.begin(); cHLA != candidateUnacceptableHLA.end(); cHLA++) {
		std::map<std::string, std::vector<std::string > >::iterator unacceptableAntigens = hlaDictionary.find(*cHLA);

		if (unacceptableAntigens != hlaDictionary.end()) {
			for (std::vector<std::string>::iterator antigen = unacceptableAntigens->second.begin(); antigen != unacceptableAntigens->second.end(); antigen++) {
				for (std::vector<std::string>::iterator dHLA = donorHLA.begin(); dHLA != donorHLA.end(); dHLA++) {
					if ((*dHLA).compare(*antigen) == 0) {
						return FAILED_CROSSMATCH_HLA;
					}
				}
			}
		}
	}

	bool oDonorToNonOCandidate = donor->getBT() == BT_O && candidate->getBT() != BT_O;

	// Desensitizable HLA (if candidate can be desensitized)
	for (std::vector<std::string>::iterator cHLA = candidateDesensitizableHLA.begin(); cHLA != candidateDesensitizableHLA.end(); cHLA++) {
		std::map<std::string, std::vector<std::string > >::iterator unacceptableAntigens = hlaDictionary.find(*cHLA);

		if (unacceptableAntigens != hlaDictionary.end()) {
			for (std::vector<std::string>::iterator antigen = unacceptableAntigens->second.begin(); antigen != unacceptableAntigens->second.end(); antigen++) {
				for (std::vector<std::string>::iterator dHLA = donorHLA.begin(); dHLA != donorHLA.end(); dHLA++) {
					if ((*dHLA).compare(*antigen) == 0) {
						if (oDonorToNonOCandidate) {
							return REQUIRES_DESENSITIZATION_AND_O_TO_NON_O;
						}
						else {
							return REQUIRES_DESENSITIZATION;
						}
					}
				}
			}
		}
	}

	if (oDonorToNonOCandidate) {
		return O_DONOR_TO_NON_O_CANDIDATE;
	}

	return SUCCESSFUL_CROSSMATCH;
}

bool KPDData::isCompatiblePair(KPDCandidate * candidate, std::vector<KPDDonor *> donors, bool reserveODonorsForOCandidates, bool allowDesensitization) {
	
	if (donors.size() > 1) {
		for (std::vector<KPDDonor * >::iterator itDonor = donors.begin() + 1; itDonor != donors.end(); itDonor++) {

			KPDCrossmatchResult crossmatch = performCrossmatch(candidate, *itDonor);

			if (crossmatch == SUCCESSFUL_CROSSMATCH) {
				return true;
			}
			
			if (!reserveODonorsForOCandidates && allowDesensitization) {
				if (crossmatch == O_DONOR_TO_NON_O_CANDIDATE || crossmatch == REQUIRES_DESENSITIZATION || crossmatch == REQUIRES_DESENSITIZATION_AND_O_TO_NON_O) {
					return true;
				}
			}

			if (!reserveODonorsForOCandidates) {
				if (crossmatch == O_DONOR_TO_NON_O_CANDIDATE) {
					return true;
				}
			}

			if (allowDesensitization) {
				if (crossmatch == REQUIRES_DESENSITIZATION) {
					return true;
				}
			}
		}
	}

	return false;
}

//Calculates Survival (fiveyear == 1 => 5 year survival; fiveyear == 0 => 10 year survival)
double KPDData::calculateSurvival(KPDCandidate * candidate, KPDDonor * donor, int fiveyear) {
	double survival = 0.0;

	//Age
	std::vector<double> cAge5yr = survivalParameters["5 Year Recipient Age"];
	std::vector<double> cAge10yr = survivalParameters["10 Year Recipient Age"];

	std::vector<double> dAge5yr;
	std::vector<double> dAge10yr;

	std::vector<int> donorAgeChangePoints;
	donorAgeChangePoints.push_back(30);
	donorAgeChangePoints.push_back(10);
	donorAgeChangePoints.push_back(10);
	donorAgeChangePoints.push_back(10);
	donorAgeChangePoints.push_back(10);

	int changePoint = 0;

	int donorsAge = donor->getAge();

	if (candidate->getAge() < 13) {

		dAge5yr = survivalParameters["5 Year Among Recipients Age < 12"];
		dAge10yr = survivalParameters["10 Year Among Recipients Age < 12"];

		survival += fiveyear*(cAge5yr[0]) + (1 - fiveyear)*(cAge10yr[0]);
	}
	else if (candidate->getAge() >= 13 && candidate->getAge() <= 17) {

		dAge5yr = survivalParameters["5 Year Among Recipients 13-17"];
		dAge10yr = survivalParameters["10 Year Among Recipients 13-17"];

		survival += fiveyear*(cAge5yr[1]) + (1 - fiveyear)*(cAge10yr[1]);
	}
	else if (candidate->getAge() >= 18 && candidate->getAge() <= 29) {

		dAge5yr = survivalParameters["5 Year Among Recipients 18-29"];
		dAge10yr = survivalParameters["10 Year Among Recipients 18-29"];

		survival += fiveyear*(cAge5yr[2]) + (1 - fiveyear)*(cAge10yr[2]);
	}
	else if (candidate->getAge() >= 30 && candidate->getAge() <= 39) {

		dAge5yr = survivalParameters["5 Year Among Recipients 30-39"];
		dAge10yr = survivalParameters["10 Year Among Recipients 30-39"];

		survival += fiveyear*(cAge5yr[3]) + (1 - fiveyear)*(cAge10yr[3]);
	}
	else if (candidate->getAge() >= 40 && candidate->getAge() <= 49) {

		dAge5yr = survivalParameters["5 Year Among Recipients 40-49"];
		dAge10yr = survivalParameters["10 Year Among Recipients 40-49"];
	}
	else if (candidate->getAge() >= 50 && candidate->getAge() <= 59) {

		dAge5yr = survivalParameters["5 Year Among Recipients 50-59"];
		dAge10yr = survivalParameters["10 Year Among Recipients 50-59"];

		survival += fiveyear*(cAge5yr[5]) + (1 - fiveyear)*(cAge10yr[5]);
	}
	else if (candidate->getAge() >= 60) {

		dAge5yr = survivalParameters["5 Year Among Recipients 60+"];
		dAge10yr = survivalParameters["10 Year Among Recipients 60+"];

		survival += fiveyear*(cAge5yr[6]) + (1 - fiveyear)*(cAge10yr[6]);
	}

	while (donorsAge > 0 && changePoint <= 3) {
		survival += donorsAge*(fiveyear*(dAge5yr[changePoint]) + (1 - fiveyear)*(dAge10yr[changePoint]));
		donorsAge = donorsAge - donorAgeChangePoints[changePoint];
		changePoint++;
	}

	//Sex Combination
	std::vector<double> pSex5yr = survivalParameters["5 Year Donor-Recipient Sex"];
	std::vector<double> pSex10yr = survivalParameters["10 Year Donor-Recipient Sex"];
	if (!candidate->getMale() && !donor->getMale()) {
		survival += fiveyear*(pSex5yr[0]) + (1 - fiveyear)*(pSex10yr[0]);
	}
	if (!candidate->getMale() && donor->getMale()) {
		survival += fiveyear*(pSex5yr[1]) + (1 - fiveyear)*(pSex10yr[1]);
	}
	if (candidate->getMale() && donor->getMale()) {
		survival += fiveyear*(pSex5yr[3]) + (1 - fiveyear)*(pSex10yr[3]);
	}

	//HLA Mismatches
	double pNoMismatch5yr = survivalParameters["5 Year HLA ABDR Mismatch"][0];
	double pNoMismatch10yr = survivalParameters["10 Year HLA ABDR Mismatch"][0];
	survival += fiveyear*(pNoMismatch5yr)+(1 - fiveyear)*(pNoMismatch10yr);

	//BMI
	double cBMI5yr = survivalParameters["5 Year Recipient BMI"][1];
	double cBMI10yr = survivalParameters["10 Year Recipient BMI"][1];
	double dBMI5yr = survivalParameters["5 Year Donor BMI"][1];
	double dBMI10yr = survivalParameters["10 Year Donor BMI"][1];

	if (candidate->getBMI()>30) {
		survival += fiveyear*(cBMI5yr)+(1 - fiveyear)*(cBMI10yr);
	}
	if (donor->getBMI()>30) {
		survival += fiveyear*(dBMI5yr)+(1 - fiveyear)*(dBMI10yr);
	}

	//Weight Ratio
	double weightRatio = (donor->getWeight()) / (candidate->getWeight());
	std::vector<double> pWR5yr = survivalParameters["5 Year Donor-Recipient Weight Ratio"];
	std::vector<double> pWR10yr = survivalParameters["10 Year Donor-Recipient Weight Ratio"];

	if (weightRatio < 0.75) {
		survival += fiveyear*(pWR5yr[0]) + (1 - fiveyear)*(pWR10yr[0]);
	}
	else if (weightRatio >= 0.75 && weightRatio < 0.90) {
		survival += fiveyear*(pWR5yr[1]) + (1 - fiveyear)*(pWR10yr[1]);
	}
	else if (weightRatio >= 1.15) {
		survival += fiveyear*(pWR5yr[3]) + (1 - fiveyear)*(pWR10yr[3]);
	}

	//Height Ratio
	double heightRatio = (donor->getHeight()) / (candidate->getHeight());
	std::vector<double> pHR5yr = survivalParameters["5 Year Donor-Recipient Height Ratio"];
	std::vector<double> pHR10yr = survivalParameters["10 Year Donor-Recipient Height Ratio"];

	if (heightRatio < 0.94) {
		survival += fiveyear*(pHR5yr[0]) + (1 - fiveyear)*(pHR10yr[0]);
	}
	else if (heightRatio >= 0.94 && heightRatio < 1.0) {
		survival += fiveyear*(pHR5yr[1]) + (1 - fiveyear)*(pHR10yr[1]);
	}
	else if (heightRatio >= 1.06) {
		survival += fiveyear*(pHR5yr[2]) + (1 - fiveyear)*(pHR10yr[2]);
	}

	//Race
	std::vector<double> cRace5yr = survivalParameters["5 Year Recipient Race"];
	std::vector<double> cRace10yr = survivalParameters["10 Year Recipient Race"];
	if (candidate->getRace() == WHITE) {
		survival += fiveyear*(cRace5yr[0]) + (1 - fiveyear)*(cRace10yr[0]);
	}
	else if (candidate->getRace() == BLACK) {
		survival += fiveyear*(cRace5yr[1]) + (1 - fiveyear)*(cRace10yr[1]);
	}
	else if (candidate->getRace() == HISPANIC) {
		survival += fiveyear*(cRace5yr[2]) + (1 - fiveyear)*(cRace10yr[2]);
	}
	else {
		survival += fiveyear*(cRace5yr[3]) + (1 - fiveyear)*(cRace10yr[3]);
	}

	std::vector<double> dRace5yr = survivalParameters["5 Year Donor Race"];
	std::vector<double> dRace10yr = survivalParameters["10 Year Donor Race"];
	if (donor->getRace() != BLACK && donor->getRace() != HISPANIC) {
		survival += fiveyear*(dRace5yr[0]) + (1 - fiveyear)*(dRace10yr[0]);
	}
	else if (donor->getRace() == BLACK) {
		survival += fiveyear*(dRace5yr[1]) + (1 - fiveyear)*(dRace10yr[1]);
	}
	else if (donor->getRace() == HISPANIC) {
		survival += fiveyear*(dRace5yr[2]) + (1 - fiveyear)*(dRace10yr[2]);
	}

	//Donor Cigarette Use
	std::vector<double> dCigarette5yr = survivalParameters["5 Year Donor Cigarette Use"];
	std::vector<double> dCigarette10yr = survivalParameters["10 Year Donor Cigarette Use"];
	if (donor->getCigaretteUse()) {
		survival += fiveyear*(dCigarette5yr[1]) + (1 - fiveyear)*(dCigarette10yr[1]);
	}

	//PRA
	std::vector<double> cPRA5yr = survivalParameters["5 Year PRA"];
	std::vector<double> cPRA10yr = survivalParameters["10 Year PRA"];
	if (candidate->getPRA() >= 10 && candidate->getPRA() <= 79) {
		survival += fiveyear*(cPRA5yr[1]) + (1 - fiveyear)*(cPRA10yr[1]);
	}
	else if (candidate->getPRA() >= 80 && candidate->getPRA() <= 100) {
		survival += fiveyear*(cPRA5yr[2]) + (1 - fiveyear)*(cPRA10yr[2]);
	}

	//Recipient Diabetes Diagnosis
	double cDiabetes5yr = survivalParameters["5 Year Recipient Diabetes Status"][1];
	double cDiabetes10yr = survivalParameters["10 Year Recipient Diabetes Status"][1];
	if (candidate->getDiabetes()) {
		survival += fiveyear*(cDiabetes5yr)+(1 - fiveyear)*(cDiabetes10yr);
	}

	//Previous Tranpslant
	double cPrevTrans5yr = survivalParameters["5 Year Previous Transplant"][1];
	double cPrevTrans10yr = survivalParameters["10 Year Previous Transplant"][1];
	if (candidate->getPrevTrans()) {
		survival += fiveyear*(cPrevTrans5yr)+(1 - fiveyear)*(cPrevTrans10yr);
	}

	//Time on Dialysis
	std::vector<double> cTOD5yr = survivalParameters["5 Year Time on Dialysis"];
	std::vector<double> cTOD10yr = survivalParameters["10 Year Time on Dialysis"];
	if (candidate->getTOD() == 0) {
		survival += fiveyear*(cTOD5yr[0]) + (1 - fiveyear)*(cTOD10yr[0]);
	}
	else if (candidate->getTOD() > 0 && candidate->getTOD() <= 1) {
		survival += fiveyear*(cTOD5yr[1]) + (1 - fiveyear)*(cTOD10yr[1]);
	}
	else if (candidate->getTOD() > 1 && candidate->getTOD() <= 2) {
		survival += fiveyear*(cTOD5yr[2]) + (1 - fiveyear)*(cTOD10yr[2]);
	}
	else if (candidate->getTOD() > 2 && candidate->getTOD() <= 3) {
		survival += fiveyear*(cTOD5yr[3]) + (1 - fiveyear)*(cTOD10yr[3]);
	}
	else if (candidate->getTOD() > 3) {
		survival += fiveyear*(cTOD5yr[4]) + (1 - fiveyear)*(cTOD10yr[4]);
	}

	//Hep C Seriology
	double cHepC5yr = survivalParameters["5 Year Recipient Hepatitis C Seriology"][1];
	double cHepC10yr = survivalParameters["10 Year Recipient Hepatitis C Seriology"][1];
	if (candidate->getHepC()) {
		survival += fiveyear*(cHepC5yr)+(1 - fiveyear)*(cHepC10yr);
	}

	//Insurance
	std::vector<double> cInsurance5yr = survivalParameters["5 Year Recipient Insurance"];
	std::vector<double> cInsurance10yr = survivalParameters["10 Year Recipient Insurance"];

	if (candidate->getInsurance() == PUBLIC) {
		survival += fiveyear*(cInsurance5yr[0]) + (1 - fiveyear)*(cInsurance10yr[0]);
	}
	else if (candidate->getInsurance() == PRIVATE) {
		survival += fiveyear*(cInsurance5yr[1]) + (1 - fiveyear)*(cInsurance10yr[1]);
	}
	else if (candidate->getInsurance() == OTHER_INSURANCE) {
		survival += fiveyear*(cInsurance5yr[2]) + (1 - fiveyear)*(cInsurance10yr[2]);
	}

	//Adjust for Year
	survival += fiveyear*(survivalParameters["5 Year Transplant Year"][2]) + (1 - fiveyear)*(survivalParameters["10 Year Transplant Year"][2]);

	//Calculate Survival
	double expo = exp(survival);
	if (fiveyear == 1) {
		survival = pow(survivalParameters["5 Year Baseline"][0], expo);
	}
	else {
		survival = pow(survivalParameters["10 Year Baseline"][0], expo);
	}

	return survival;
}

void KPDData::printLog() {

	std::ofstream outputFileLog(logFile.c_str());
	outputFileLog << kpdDataLog.str();
	outputFileLog.close();
}


#endif