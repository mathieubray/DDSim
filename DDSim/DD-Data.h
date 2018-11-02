/* ---------------------------------------------
DD-Data.h
Collects and Stores Data from Files
---------------------------------------------- */

#ifndef DATA_H
#define DATA_H

#include "DD-Enums-Functions.h"
#include "DD-Candidate.h"
#include "DD-Donor.h"
#include "DD-Parameters.h"
#include "DD-RNG.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <math.h>

class KPDData {

private:

	KPDParameters * kpdParameters;

	//Raw data
	std::vector<std::vector<std::vector<std::string> > > hlaFrequencyData;
	std::vector<std::vector<std::vector<std::string> > > hlaDictionaryData;
	std::vector<std::vector<std::vector<std::string> > > characteristicsData;

	std::vector<std::vector<std::vector<std::string> > > kpdData;
	std::vector<std::vector<std::vector<std::string> > > deceasedDonorData;
	std::vector<std::vector<std::vector<std::string> > > waitlistCandidatesData;

	//Clean data	
	std::vector<std::vector<std::string> > donorHLA;
	std::vector<double> donorHLAFrequency;

	std::map<std::string, std::vector<std::string> > hlaDictionary;
	std::map<std::string, std::vector<double> > survivalParameters;
	std::map<std::string, std::vector<double> > characteristicsFrequency;

	std::vector<KPDDonor *> nddPool;
	std::vector<KPDCandidate *> candidatePool;
	std::vector<KPDCandidate *> pairedCandidatesPool;
	std::map<int, std::vector<KPDDonor *> > pairedDonorsPool;

	std::deque<KPDDonor *> deceasedDonors;
	std::map<int, KPDCandidate *> waitlistCandidates;
	std::vector<int> waitlistCandidateIDs;


	//Convert raw to clean data

	void readDataFromFile(std::vector<std::vector<std::vector<std::string> > >  & parsedData, std::string fileName); // Reads and parses data from files

	void formDonorHLAFrequency();
	void formHLADictionary();	
	void formSurvivalParameters();	
	void formCharacteristicsFrequency();

	void formKPDPopulation();
	void formDeceasedDonorPopulation();
	void formWaitlistPopulation();

	std::stringstream kpdDataLog;

public:

	KPDData(KPDParameters * params);
	~KPDData();

	// Retrieve data on deceased donors and waitlisted candidates
	std::deque<KPDDonor *> getDeceasedDonors();
	std::deque<int> getDeceasedDonorArrivalTimes();

	std::vector<KPDCandidate *> getWaitlistedCandidates();
	std::vector<int> getWaitlistedCandidateIDs();
	std::vector<std::deque<KPDStatus> > getWaitlistCandidatesStateTransitionMatrix();
	std::vector<std::deque<int> > getWaitlistCandidatesStateTransitionTimeMatrix();


	// Stochastic draws from data
	std::pair<KPDCandidate *, int> drawCandidate(double u);
	KPDDonor * drawNDD(double u);
	KPDDonor * generateDonor(std::vector<double> u);

	// Crossmatch and survival calculations
	KPDCrossmatch performCrossmatch(KPDCandidate * candidate, KPDDonor * donor);
	bool allowableMatch(KPDCrossmatch crossmatch);
	double calculateSurvival(KPDCandidate * candidate, KPDDonor * donor, int fiveyear);
	
	void printLog();
};

KPDData::KPDData(KPDParameters * params) {
	
	kpdParameters = params;

	kpdDataLog << "--------------------------" << std::endl;
	kpdDataLog << "Reading Data from Files..." << std::endl;
	kpdDataLog << "--------------------------" << std::endl;

	kpdDataLog << std::endl;
	
	// Read data 
	readDataFromFile(hlaFrequencyData, "data/" + params->getFileHLAFrequency());
	readDataFromFile(hlaDictionaryData, "data/" + params->getFileHLADictionary());
	readDataFromFile(characteristicsData, "data/" + params->getFileSurvivalParameters());

	readDataFromFile(kpdData, "data/" + params->getFileKPDData());
	readDataFromFile(deceasedDonorData, "data/" + params->getFileDeceasedDonors());
	readDataFromFile(waitlistCandidatesData, "data/" + params->getFileWaitingListCandidates());

	kpdDataLog << std::endl;
	
	// Assemble objects from raw data
	formDonorHLAFrequency();
	formHLADictionary();
	formSurvivalParameters();
	formCharacteristicsFrequency();

	formKPDPopulation();
	formDeceasedDonorPopulation();
	formWaitlistPopulation();
}

KPDData::~KPDData() {
	printLog();
}

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

	for (std::vector<std::vector<std::vector<std::string> > >::iterator hlaRow = hlaFrequencyData.begin(); hlaRow != hlaFrequencyData.end(); hlaRow++) {
		std::vector<std::string> donorProfile;

		for (std::vector<std::vector<std::string> >::iterator hla = hlaRow->begin(); hla != hlaRow->begin() + 8; hla++) {

			std::string antigen = (*hla)[0];

			if (antigen.compare("NA") != 0) {
				
				donorProfile.push_back(antigen);

				kpdDataLog << antigen << " ";
			}
		}

		donorHLA.push_back(donorProfile);

		//Stores frequency info
		std::vector<std::vector<std::string> >::iterator profileFreq = (*hlaRow).begin() + 8;
		std::string freq = (*profileFreq).at(0);
		double probability = atof(freq.c_str());
		donorHLAFrequency.push_back(probability);

		kpdDataLog << "(" << probability << ")" << std::endl;
	}

	kpdDataLog << std::endl;

	kpdDataLog << "Donor HLA Frequencies Set" << std::endl << std::endl;
}

void KPDData::formHLADictionary() {

	kpdDataLog << "--------------------------------" << std::endl;
	kpdDataLog << "Loading HLA Dictionary..." << std::endl;
	kpdDataLog << "--------------------------------" << std::endl;

	kpdDataLog << std::endl;

	for (std::vector<std::vector<std::vector<std::string> > >::iterator dictionaryRow = hlaDictionaryData.begin(); dictionaryRow != hlaDictionaryData.end(); dictionaryRow++) {
		std::string antigen = (*dictionaryRow)[0][0];

		kpdDataLog << antigen << ": ";

		for (std::vector<std::string>::iterator hla = (*dictionaryRow)[1].begin(); hla != (*dictionaryRow)[1].end(); hla++) {
			hlaDictionary[antigen].push_back(*hla);

			kpdDataLog << *hla << " ";
		}

		kpdDataLog << std::endl;
	}

	kpdDataLog << std::endl;

	kpdDataLog << "HLA Dictionary Loaded" << std::endl << std::endl;
}

void KPDData::formSurvivalParameters() {

	kpdDataLog << "------------------------------" << std::endl;
	kpdDataLog << "Setting Survival Parameters..." << std::endl;
	kpdDataLog << "------------------------------" << std::endl;

	kpdDataLog << std::endl;
	
	for (std::vector<std::vector<std::vector<std::string> > >::iterator paramRow = characteristicsData.begin(); paramRow != characteristicsData.end(); paramRow++) {

		std::string characteristic = (*paramRow)[0][0];

		kpdDataLog << characteristic << ": ";

		// Store 5-year and 10-year survival parameters
		survivalParameters["5 Year " + characteristic].push_back(atof((*paramRow)[2][0].c_str()));
		kpdDataLog << (*paramRow)[2][0] << " (5-Year), ";

		survivalParameters["10 Year " + characteristic].push_back(atof((*paramRow)[3][0].c_str()));
		kpdDataLog << (*paramRow)[3][0] << " (10-Year)"  << std::endl;
	}

	kpdDataLog << std::endl;
	
	kpdDataLog << "Survival Parameters Set" << std::endl << std::endl;
}

void KPDData::formCharacteristicsFrequency() {

	kpdDataLog << "------------------------------------" << std::endl;
	kpdDataLog << "Setting Characteristics Frequency..." << std::endl;
	kpdDataLog << "------------------------------------" << std::endl;

	kpdDataLog << std::endl;
		
	for (std::vector<std::vector<std::vector<std::string> > >::iterator paramRow = characteristicsData.begin(); paramRow != characteristicsData.end(); paramRow++) {

		std::string characteristic = (*paramRow)[0][0];

		// Collect probabilities for randomly assigned characterisitcs
		if (characteristic.compare("Donor Race") == 0 || 
			characteristic.compare("Donor Cigarette Use") == 0 || 
			characteristic.compare("Previous Transplant") == 0 ||
			characteristic.compare("Time On Dialysis") == 0 || 
			characteristic.compare("Recipient Hepatitis C Seriology") == 0 || 
			characteristic.compare("Recipient Insurance") == 0) {

			characteristicsFrequency[characteristic].push_back(atof((*paramRow)[6][0].c_str()));

			kpdDataLog << characteristic << ": " << (*paramRow)[6][0] << std::endl;
		}
	}

	characteristicsFrequency["Blood Type"].push_back(0.44); // BT_O
	characteristicsFrequency["Blood Type"].push_back(0.42); // BT_A
	characteristicsFrequency["Blood Type"].push_back(0.10); // BT_B
	characteristicsFrequency["Blood Type"].push_back(0.04); // BT_AB

	for (std::vector<double>::iterator it = characteristicsFrequency["Blood Type"].begin(); it != characteristicsFrequency["Blood Type"].end(); it++) {
		kpdDataLog << "Blood Type: " << *it << std::endl;
	}

	characteristicsFrequency["Relation To Donor"].push_back(0.4); // SIGNIFICANT_OTHER
	characteristicsFrequency["Relation To Donor"].push_back(0.1); // MOTHER
	characteristicsFrequency["Relation To Donor"].push_back(0.1); // FATHER
	characteristicsFrequency["Relation To Donor"].push_back(0.15); // RELATED
	characteristicsFrequency["Relation To Donor"].push_back(0.15); // UNRELATED
	characteristicsFrequency["Relation To Donor"].push_back(0.1); // CHILD

	for (std::vector<double>::iterator it = characteristicsFrequency["Relation To Donor"].begin(); it != characteristicsFrequency["Relation To Donor"].end(); it++) {
		kpdDataLog << "Relation To Donor: " << *it << std::endl;
	}

	characteristicsFrequency["Donor HLA BW"].push_back(0.66); // BW4
	characteristicsFrequency["Donor HLA BW"].push_back(0.85); // BW6
	
	for (std::vector<double>::iterator it = characteristicsFrequency["Donor HLA BW"].begin(); it != characteristicsFrequency["Donor HLA BW"].end(); it++) {
		kpdDataLog << "Donor HLA BW: " << *it << std::endl;
	}

	kpdDataLog << std::endl;

	kpdDataLog << "Characteristics Frequencies Set" << std::endl << std::endl;

}

void KPDData::formKPDPopulation() {

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

			KPDRelation dRelation = RELATION_UNSPECIFIED;

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

			KPDRace dRace = RACE_OTHER;
			bool dCigaretteUse = false;

			KPDDonor * d = new KPDDonor(matchingID, donorID,
				dBT, dRelation, dAge, dMale, dRace, dHeight, dWeight, dCigaretteUse);
			d->setHLA(dHLA);

			kpdDataLog << "NDD " << donorID << " (Matching ID: " << matchingID << ") added to NDD Pool" << std::endl;
			kpdDataLog << d->donorOutput() << std::endl;

			nddPool.push_back(d);
			numberOfNDDs++;

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
			KPDInsurance cInsurance = INSURANCE_UNSPECIFIED;

			KPDCandidate * c = new KPDCandidate(matchingID, candidateID, cPRA, cBT, 
				cAge, cMale, cRace, cDiabetes, cHeight, cWeight, cPrevTrans, cTOD, cHepC, cInsurance);
			
			c->setUnacceptableHLA(cUnacceptableHLA);
			c->setDesensitizableHLA(cDesensitizableHLA);
			
			kpdDataLog << "Candidate " << candidateID << " (Matching ID: " << matchingID << ") added to Candidate Pool" << std::endl;
			kpdDataLog << c->candidateOutput() << std::endl;

			candidatePool.push_back(c);
			numberOfCandidates++;

			// Donors
			std::vector<KPDDonor *> associatedDonors;

			bool abDonor = false;
			bool compatiblePair = false;

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
				KPDRelation dRelation = RELATION_UNSPECIFIED;

				int dAge = atoi((*it)[15][0].c_str());
				bool dMale = ((*it)[16][0].compare("MALE") == 0);
				double dHeight = atof((*it)[17][0].c_str());
				double dWeight = atof((*it)[18][0].c_str());

				KPDRace dRace = RACE_OTHER;
				bool dCigaretteUse = false;

				KPDDonor * d = new KPDDonor(matchingID, donorID,
					dBT, dRelation, dAge, dMale, dRace, dHeight, dWeight, dCigaretteUse);

				d->setHLA(dHLA);

				associatedDonors.push_back(d);

				KPDCrossmatch crossmatch = performCrossmatch(c, d);
				if (allowableMatch(crossmatch)) {
					compatiblePair = true;
				}

				it++;

			} while (matchingID == atoi((*it)[0][0].c_str()) && it != kpdData.end());

			
			if (!compatiblePair) {

				kpdDataLog << "Donors Matched with Candidate " << matchingID << std::endl;
				for (std::vector<KPDDonor *>::iterator donorIt = associatedDonors.begin(); donorIt != associatedDonors.end(); donorIt++) {
					kpdDataLog << (*donorIt)->donorOutput() << std::endl;
				}

				numberOfPairedCandidates++;
				numberOfPairedDonors += (int)associatedDonors.size();

				pairedCandidatesPool.push_back(c);
				pairedDonorsPool[matchingID] = associatedDonors;
			}

			else {
				
				kpdDataLog << "Donors Excluded with Candidate " << matchingID << std::endl;
				for (std::vector<KPDDonor *>::iterator donorIt = associatedDonors.begin(); donorIt != associatedDonors.end(); donorIt++) {
					kpdDataLog << (*donorIt)->donorOutput() << std::endl;
				}
			}
		}
	}

	kpdDataLog << "Total Candidates: " << numberOfCandidates << std::endl;
	kpdDataLog << "Non-Directed Donors: " << numberOfNDDs << std::endl;
	kpdDataLog << "Paired Candidates: " << numberOfPairedCandidates << std::endl;
	kpdDataLog << "Paired Donors: " << numberOfPairedDonors << std::endl << std::endl;
}


void KPDData::formDeceasedDonorPopulation() {

	kpdDataLog << "--------------------------------" << std::endl;
	kpdDataLog << "Loading Deceased Donors..." << std::endl;
	kpdDataLog << "--------------------------------" << std::endl;

	kpdDataLog << std::endl;

	for (std::vector<std::vector<std::vector<std::string> > >::iterator srtrRow = deceasedDonorData.begin(); srtrRow != deceasedDonorData.end(); srtrRow++) {

		int id = atoi((*srtrRow)[0][0].c_str());
			
		int recoveryTimeSinceReference = 0;
		std::string recoveryTime = (*srtrRow)[1][0];
		if (recoveryTime.compare("NA") != 0) {
			recoveryTimeSinceReference = atoi(recoveryTime.c_str());
		}

		int centerID = atoi((*srtrRow)[2][0].c_str());

		KPDBloodType bt = KPDFunctions::stringToBloodType((*srtrRow)[3][0]);
		bool minorA = (*srtrRow)[4][0].compare("TRUE") == 0;

		int age = atoi((*srtrRow)[5][0].c_str());
		bool genderMale = (*srtrRow)[6][0].compare("M") == 0;
		KPDRace race = KPDFunctions::stringToRace((*srtrRow)[7][0]);
		double height = atof((*srtrRow)[8][0].c_str()) / 100.0;
		double weight = atof((*srtrRow)[9][0].c_str());
		bool cigarette = (*srtrRow)[10][0].compare("TRUE") == 0;

		std::vector<std::string> dHLA;
		if ((*srtrRow)[11][0].compare("NA") != 0) {
			dHLA.push_back("A" + (*srtrRow)[11][0]);
		}
		if ((*srtrRow)[12][0].compare("NA") != 0) {
			dHLA.push_back("A" + (*srtrRow)[12][0]);
		}
		if ((*srtrRow)[13][0].compare("NA") != 0) {
			dHLA.push_back("B" + (*srtrRow)[13][0]);
		}
		if ((*srtrRow)[14][0].compare("NA") != 0) {
			dHLA.push_back("B" + (*srtrRow)[14][0]);
		}
		if ((*srtrRow)[15][0].compare("True") == 0) {
			dHLA.push_back("BW4");
		}
		if ((*srtrRow)[16][0].compare("True") == 0) {
			dHLA.push_back("BW6");
		}
		if ((*srtrRow)[17][0].compare("NA") != 0) {
			dHLA.push_back("CW" + (*srtrRow)[17][0]);
		}
		if ((*srtrRow)[18][0].compare("NA") != 0) {
			dHLA.push_back("CW" + (*srtrRow)[18][0]);
		}
		if ((*srtrRow)[19][0].compare("NA") != 0) {
			dHLA.push_back("DR" + (*srtrRow)[19][0]);
		}
		if ((*srtrRow)[20][0].compare("NA") != 0) {
			dHLA.push_back("DR" + (*srtrRow)[20][0]);
		}
		if ((*srtrRow)[21][0].compare("True") == 0) {
			dHLA.push_back("DR51");
		}
		if ((*srtrRow)[22][0].compare("True") == 0) {
			dHLA.push_back("DR52");
		}
		if ((*srtrRow)[23][0].compare("True") == 0) {
			dHLA.push_back("DR53");
		}
		if ((*srtrRow)[24][0].compare("NA") != 0) {
			dHLA.push_back("DQ" + (*srtrRow)[24][0]);
		}
		if ((*srtrRow)[25][0].compare("NA") != 0) {
			dHLA.push_back("DQ" + (*srtrRow)[25][0]);
		}
		if ((*srtrRow)[26][0].compare("NA") != 0) {
			dHLA.push_back("DP" + (*srtrRow)[26][0]);
		}
		if ((*srtrRow)[27][0].compare("NA") != 0) {
			dHLA.push_back("DP" + (*srtrRow)[27][0]);
		}

		bool bothKidneysAvailable = atoi((*srtrRow)[28][0].c_str()) == 2;

		KPDDonor * d = new KPDDonor(id, bt, minorA,
			age, genderMale, race, height, weight, cigarette,
			recoveryTimeSinceReference, centerID);

		d->setHLA(dHLA);
		d->setBothKidneysAvailable(bothKidneysAvailable);

		deceasedDonors.push_back(d);

		kpdDataLog << "Deceased Donor " << id << std::endl;
		kpdDataLog << d->donorOutput() << std::endl;		
	}
}

void KPDData::formWaitlistPopulation() {

	kpdDataLog << "--------------------------------" << std::endl;
	kpdDataLog << "Loading Waitlist Candidates..." << std::endl;
	kpdDataLog << "--------------------------------" << std::endl;

	kpdDataLog << std::endl;

	for (std::vector<std::vector<std::vector<std::string> > >::iterator srtrRow = waitlistCandidatesData.begin(); srtrRow != waitlistCandidatesData.end(); srtrRow++) {

		int id = atoi((*srtrRow)[0][0].c_str());
		
		int statusTime = 0;
		std::string statusTimeBegin = (*srtrRow)[3][0];
		if (statusTimeBegin.compare("NA") != 0) {
			statusTime = atoi(statusTimeBegin.c_str());
		}

		bool active = (*srtrRow)[4][0].compare("Inactive") != 0;

		if (waitlistCandidates.count(id) == 0) {

			waitlistCandidateIDs.push_back(id);

			int listingTime = 0;
			std::string listingTimeBegin = (*srtrRow)[1][0];
			if (listingTimeBegin.compare("NA") != 0) {
				listingTime = atoi(listingTimeBegin.c_str());
			}

			int removalTime = 0;
			std::string removalTimeBegin = (*srtrRow)[2][0];
			if (removalTimeBegin.compare("NA") != 0) {
				removalTime = atoi(removalTimeBegin.c_str());
			}

			int centerID = atoi((*srtrRow)[5][0].c_str());
			int opoID = atoi((*srtrRow)[6][0].c_str());

			KPDBloodType bt = KPDFunctions::stringToBloodType((*srtrRow)[7][0]);
			bool minorA = (*srtrRow)[8][0].compare("TRUE") == 0;

			KPDAgeCategory ageCategory = KPDFunctions::stringToAgeCategory((*srtrRow)[9][0]); // Discrepancies.... go with category for now
			bool genderMale = (*srtrRow)[10][0].compare("M") == 0;
			KPDRace race = KPDFunctions::stringToRace((*srtrRow)[11][0]);
			int pra = atoi((*srtrRow)[12][0].c_str());
			double height = atof((*srtrRow)[13][0].c_str()) / 100;
			double weight = atof((*srtrRow)[14][0].c_str());
			bool hepC = (*srtrRow)[15][0].compare("Y") == 0;
			bool previousTransplant = (*srtrRow)[16][0].compare("Yes") == 0;
			int timeOnDialysis = atoi((*srtrRow)[17][0].c_str());
			bool diabetes = KPDFunctions::stringToDiagnosis((*srtrRow)[18][0]) == DIAGNOSIS_DIABETES; //Just interested in diabetes
			KPDInsurance insurance = KPDFunctions::stringToInsurance((*srtrRow)[19][0]);

			double epts = atof((*srtrRow)[20][0].c_str());
			bool eptsPriority = (*srtrRow)[21][0].compare("TRUE") == 0;

			std::vector<std::string> cHLA;
			if ((*srtrRow)[22][0].compare("0") != 0) {
				cHLA.push_back("A" + (*srtrRow)[22][0]);
			}
			if ((*srtrRow)[23][0].compare("0") != 0) {
				cHLA.push_back("A" + (*srtrRow)[23][0]);
			}
			if ((*srtrRow)[24][0].compare("0") != 0) {
				cHLA.push_back("B" + (*srtrRow)[24][0]);
			}
			if ((*srtrRow)[25][0].compare("0") != 0) {
				cHLA.push_back("B" + (*srtrRow)[25][0]);
			}
			if ((*srtrRow)[26][0].compare("0") != 0) {
				cHLA.push_back("DR" + (*srtrRow)[26][0]);
			}
			if ((*srtrRow)[27][0].compare("0") != 0) {
				cHLA.push_back("DR" + (*srtrRow)[27][0]);
			}

			int listingYear = atoi((*srtrRow)[28][0].c_str());
			bool withdrawn = (*srtrRow)[29][0].compare("FALSE") == 0;
			

			KPDCandidate * c = new KPDCandidate(id, pra, bt, minorA,
				ageCategory, genderMale, race, diabetes, height, weight, previousTransplant, timeOnDialysis, hepC, insurance,
				epts, eptsPriority,
				listingTime, listingYear, removalTime, withdrawn, centerID, opoID);

			c->setHLA(cHLA);
			c->addStatusChangeTime(statusTime);
			c->addStatusChange(active);

			waitlistCandidates[id] = c;
		}

		else {

			waitlistCandidates[id]->addStatusChangeTime(statusTime);
			waitlistCandidates[id]->addStatusChange(active);
		}
	}

	for (std::vector<int>::iterator it = waitlistCandidateIDs.begin(); it != waitlistCandidateIDs.end(); it++) {

		KPDCandidate * c = waitlistCandidates[*it];

		kpdDataLog << "Waitlist Candidate: " << *it << std::endl;
		kpdDataLog << c->candidateOutput() << std::endl;
	}

}

std::deque<KPDDonor*> KPDData::getDeceasedDonors() {

	return std::deque<KPDDonor* >(deceasedDonors);
}

std::deque<int> KPDData::getDeceasedDonorArrivalTimes() {

	std::deque<int> arrivalTimes;

	for (std::deque<KPDDonor *>::iterator it = deceasedDonors.begin(); it != deceasedDonors.end(); it++) {
		arrivalTimes.push_back((*it)->getRecoveryTime());
	}

	return arrivalTimes;
}

std::vector<KPDCandidate*> KPDData::getWaitlistedCandidates() {

	std::vector<KPDCandidate* > candidates;

	for (std::map<int, KPDCandidate*>::iterator it = waitlistCandidates.begin(); it != waitlistCandidates.end(); it++) {
		candidates.push_back((*it).second);
	}

	return candidates;
}

std::vector<int> KPDData::getWaitlistedCandidateIDs() {

	return std::vector<int>(waitlistCandidateIDs);
}

std::vector<std::deque<KPDStatus> > KPDData::getWaitlistCandidatesStateTransitionMatrix() {
	
	std::vector<std::deque<KPDStatus> > waitlistCandidateTransitions;

	for (std::vector<int>::iterator it = waitlistCandidateIDs.begin(); it != waitlistCandidateIDs.end(); it++) {
		waitlistCandidateTransitions.push_back(waitlistCandidates[*it]->getStatuses());
	}

	return waitlistCandidateTransitions;
}

std::vector<std::deque<int> > KPDData::getWaitlistCandidatesStateTransitionTimeMatrix() {

	std::vector<std::deque<int> > waitlistCandidateTransitionTimes;

	for (std::vector<int>::iterator it = waitlistCandidateIDs.begin(); it != waitlistCandidateIDs.end(); it++) {

		waitlistCandidateTransitionTimes.push_back(waitlistCandidates[*it]->getStatusChangeTimes());
	}

	return waitlistCandidateTransitionTimes;
}


std::pair<KPDCandidate *, int> KPDData::drawCandidate(double u) {

	int candidateIndex = (int)(u * pairedCandidatesPool.size());
	KPDCandidate * candidate = pairedCandidatesPool[candidateIndex]->copy();

	int numberOfDonors = (int) pairedDonorsPool[candidate->getMatchingID()].size();

	return std::pair<KPDCandidate *, int>(candidate, numberOfDonors);
}

KPDDonor * KPDData::drawNDD(double u) {

	int nddIndex = (int)(u * nddPool.size());
	KPDDonor * donor = nddPool[nddIndex]->copy();

	return donor;
}

KPDDonor * KPDData::generateDonor(std::vector<double> u) {

	// Blood Type
	int btIndex = KPDFunctions::retrieveDiscreteSampleIndex(u[0], characteristicsFrequency["Blood Type"]);

	KPDBloodType dBT = BT_O;
	if (btIndex == 1) {
		dBT = BT_A;
	}
	else if (btIndex == 2) {
		dBT = BT_B;
	}
	else if (btIndex == 3) {
		dBT = BT_AB;
	}

	// HLA
	std::vector<std::string> dHLA;

	int hlaIndex1 = KPDFunctions::retrieveDiscreteSampleIndex(u[1], donorHLAFrequency);

	for (std::vector<std::string>::iterator it = donorHLA[hlaIndex1].begin(); it != donorHLA[hlaIndex1].end(); it++) {
		dHLA.push_back(*it);
	}

	int hlaIndex2 = KPDFunctions::retrieveDiscreteSampleIndex(u[2], donorHLAFrequency);

	for (std::vector<std::string>::iterator it = donorHLA[hlaIndex2].begin(); it != donorHLA[hlaIndex2].end(); it++) {
		dHLA.push_back(*it);
	}

	if (u[3] <= characteristicsFrequency["Donor HLA BW"][0]) {
		dHLA.push_back("BW4");
	}
	if (u[4] <= characteristicsFrequency["Donor HLA BW"][1]) {
		dHLA.push_back("BW6");
	}

	KPDDonor * donor = new KPDDonor();
	donor->setBT(dBT);
	donor->setHLA(dHLA);

	return donor;

}

//Checks for Match Between Candidate and Donor
KPDCrossmatch KPDData::performCrossmatch(KPDCandidate * candidate, KPDDonor * donor) {

	//Check BT match
	if ((donor->getBT() == BT_AB && candidate->getBT() != BT_AB) ||
		(donor->getBT() == BT_A && (candidate->getBT() == BT_O || candidate->getBT() == BT_B)) ||
		(donor->getBT() == BT_B && (candidate->getBT() == BT_O || candidate->getBT() == BT_A))) {

		return CROSSMATCH_FAILED_BT;
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
						return CROSSMATCH_FAILED_HLA;
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
							return CROSSMATCH_REQUIRES_DESENSITIZATION_AND_O_TO_NON_O;
						}
						else {
							return CROSSMATCH_REQUIRES_DESENSITIZATION;
						}
					}
				}
			}
		}
	}

	if (oDonorToNonOCandidate) {
		return CROSSMATCH_O_DONOR_TO_NON_O_CANDIDATE;
	}

	return CROSSMATCH_SUCCESSFUL;
}

bool KPDData::allowableMatch(KPDCrossmatch crossmatch) {

	if (crossmatch == CROSSMATCH_SUCCESSFUL) {
		return true;
	}

	if (crossmatch == CROSSMATCH_REQUIRES_DESENSITIZATION_AND_O_TO_NON_O) {
		if (kpdParameters->getAllowDesensitization() && !kpdParameters->getReserveODonorsForOCandidates()) {
			return true;
		}
	}

	if (crossmatch == CROSSMATCH_REQUIRES_DESENSITIZATION) {
		if (kpdParameters->getAllowDesensitization()) {
			return true;
		}
	}

	if (crossmatch == CROSSMATCH_O_DONOR_TO_NON_O_CANDIDATE) {
		if (!kpdParameters->getReserveODonorsForOCandidates()) {
			return true;
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

	if (candidate->getWaitlist()) {

		if (candidate->getAgeCategory() == AGE_LESS_THAN_18) {

			dAge5yr = survivalParameters["5 Year Among Recipients 13-17"];
			dAge10yr = survivalParameters["10 Year Among Recipients 13-17"];

			survival += fiveyear*(cAge5yr[1]) + (1 - fiveyear)*(cAge10yr[1]);
		}
		else if (candidate->getAgeCategory() == AGE_18_29) {

			dAge5yr = survivalParameters["5 Year Among Recipients 18-29"];
			dAge10yr = survivalParameters["10 Year Among Recipients 18-29"];

			survival += fiveyear*(cAge5yr[2]) + (1 - fiveyear)*(cAge10yr[2]);
		}
		else if (candidate->getAgeCategory() == AGE_30_39) {

			dAge5yr = survivalParameters["5 Year Among Recipients 30-39"];
			dAge10yr = survivalParameters["10 Year Among Recipients 30-39"];

			survival += fiveyear*(cAge5yr[3]) + (1 - fiveyear)*(cAge10yr[3]);
		}
		else if (candidate->getAgeCategory() == AGE_40_49) {

			dAge5yr = survivalParameters["5 Year Among Recipients 40-49"];
			dAge10yr = survivalParameters["10 Year Among Recipients 40-49"];
		}
		else if (candidate->getAgeCategory() == AGE_50_59) {

			dAge5yr = survivalParameters["5 Year Among Recipients 50-59"];
			dAge10yr = survivalParameters["10 Year Among Recipients 50-59"];

			survival += fiveyear*(cAge5yr[5]) + (1 - fiveyear)*(cAge10yr[5]);
		}
		else if (candidate->getAgeCategory() == AGE_MORE_THAN_60) {

			dAge5yr = survivalParameters["5 Year Among Recipients 60+"];
			dAge10yr = survivalParameters["10 Year Among Recipients 60+"];

			survival += fiveyear*(cAge5yr[6]) + (1 - fiveyear)*(cAge10yr[6]);
		}


	}

	else {
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
	if (candidate->getRace() == RACE_WHITE) {
		survival += fiveyear*(cRace5yr[0]) + (1 - fiveyear)*(cRace10yr[0]);
	}
	else if (candidate->getRace() == RACE_BLACK) {
		survival += fiveyear*(cRace5yr[1]) + (1 - fiveyear)*(cRace10yr[1]);
	}
	else if (candidate->getRace() == RACE_HISPANIC) {
		survival += fiveyear*(cRace5yr[2]) + (1 - fiveyear)*(cRace10yr[2]);
	}
	else {
		survival += fiveyear*(cRace5yr[3]) + (1 - fiveyear)*(cRace10yr[3]);
	}

	std::vector<double> dRace5yr = survivalParameters["5 Year Donor Race"];
	std::vector<double> dRace10yr = survivalParameters["10 Year Donor Race"];
	if (donor->getRace() != RACE_BLACK && donor->getRace() != RACE_HISPANIC) {
		survival += fiveyear*(dRace5yr[0]) + (1 - fiveyear)*(dRace10yr[0]);
	}
	else if (donor->getRace() == RACE_BLACK) {
		survival += fiveyear*(dRace5yr[1]) + (1 - fiveyear)*(dRace10yr[1]);
	}
	else if (donor->getRace() == RACE_HISPANIC) {
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

	if (candidate->getInsurance() == INSURANCE_PUBLIC || candidate->getInsurance() == INSURANCE_MEDICARE || candidate->getInsurance() == INSURANCE_MEDICARE_PLUS || candidate->getInsurance() == INSURANCE_MEDICAID) {
		survival += fiveyear*(cInsurance5yr[0]) + (1 - fiveyear)*(cInsurance10yr[0]);
	}
	else if (candidate->getInsurance() == INSURANCE_PRIVATE || candidate->getInsurance() == INSURANCE_PRIVATE_PLUS) {
		survival += fiveyear*(cInsurance5yr[1]) + (1 - fiveyear)*(cInsurance10yr[1]);
	}
	else if (candidate->getInsurance() == INSURANCE_OTHER) {
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

	std::string logFile = "output/" + kpdParameters->getOutputFolder() + "/" + kpdParameters->getSubFolder() + "/Log-Data.txt";
	std::ofstream outputFileLog(logFile.c_str());
	outputFileLog << kpdDataLog.str();
	outputFileLog.close();
}


#endif