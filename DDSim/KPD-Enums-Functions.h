#ifndef ENUMSFUNCTIONS_H
#define ENUMSFUNCTIONS_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

/* Enums */

// Simulation Specifications
enum KPDOptimizationScheme { CYCLES_AND_CHAINS, CYCLES_AND_CHAINS_WITH_FALLBACKS, LOCALLY_RELEVANT_SUBSETS };
enum KPDUtilityScheme { TRANSPLANTS, FIVE_YEAR_SURVIVAL, TEN_YEAR_SURVIVAL, TRANSPLANT_DIFFICULTY, RANDOM_UTILITY };
enum KPDDonorAssignmentScheme { PAIRED_DONORS, RANDOM_DONORS };

enum KPDTimeline { FIXED_MATCH_RUNS, CONTINUOUS_TIMELINE };

enum KPDMatchFailureScheme { PRA_BASED_MATCH_FAILURE, RANDOM_MATCH_FAILURE };

// Characteristics
enum KPDRelationToCandidate { CHILD, SIGNIFICANT_OTHER, MOTHER, FATHER, RELATED, UNRELATED, NON_DIRECTED };

enum KPDBloodType { BT_O, BT_A, BT_B, BT_AB };
enum KPDRace { WHITE, BLACK, HISPANIC, OTHER_RACE };
enum KPDInsurance { PUBLIC, PRIVATE, OTHER_INSURANCE };

// Node Properties
enum KPDUnderlyingStatus{ ACTIVE, INACTIVE, WITHDRAWN };
enum KPDTransplantationStatus { NOT_TRANSPLANTED, IN_PROGRESS, TRANSPLANTED };
enum KPDNodeType { PAIR, NDD, BRIDGE };

// Match Properties
enum KPDCrossmatchResult { SUCCESSFUL_CROSSMATCH, O_DONOR_TO_NON_O_CANDIDATE, REQUIRES_DESENSITIZATION, REQUIRES_DESENSITIZATION_AND_O_TO_NON_O, FAILED_CROSSMATCH_HLA, FAILED_CROSSMATCH_BT, FAILED_CROSSMATCH_LAB };

// Output
enum KPDOutputFileType { ARRANGEMENT_LIST, TRANSPLANT_LIST, POPULATION_LIST, SIMULATION_LOG };


/* Functions */

namespace KPDFunctions {
	
	// Enum->String Functions

	inline std::string toString(KPDOptimizationScheme type) {
		if (type == CYCLES_AND_CHAINS) {
			return "Cycles and Chains";
		}
		else if (type == CYCLES_AND_CHAINS_WITH_FALLBACKS) {
			return "Cycles and Chains with Fallbacks";
		}
		else if (type == LOCALLY_RELEVANT_SUBSETS) {
			return "Locally Relevant Subsets";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDUtilityScheme scheme) {
		if (scheme == TRANSPLANTS) {
			return "# Of Transplants";
		}
		else if (scheme == FIVE_YEAR_SURVIVAL) {
			return "5-Year Survival";
		}
		else if (scheme == TEN_YEAR_SURVIVAL) {
			return "10-Year Survival";
		}
		else if (scheme == TRANSPLANT_DIFFICULTY) {
			return "Difficult to Transplant Candidates";
		}
		else if (scheme == RANDOM_UTILITY) {
			return "Random Utility Assignment";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDDonorAssignmentScheme scheme) {
		if (scheme == PAIRED_DONORS) {
			return "Paired Donor";
		}
		else if (scheme == RANDOM_DONORS) {
			return "Randomly Generated Donor";
		}
		else {
			return "";
		}
	}


	inline std::string toString(KPDTimeline timeline) {
		if (timeline == FIXED_MATCH_RUNS) {
			return "Fixed Match Runs";
		}
		else if (timeline == CONTINUOUS_TIMELINE) {
			return "Continuous Timeline";
		}
		else {
			return "";
		}
	}

	
	inline std::string toString(KPDMatchFailureScheme scheme) {
		if (scheme == PRA_BASED_MATCH_FAILURE) {
			return "Match Failure Probability Based On PRA";
		}
		else if (scheme == RANDOM_MATCH_FAILURE) {
			return "Random Match Failure Probability";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDRelationToCandidate relation) {
		if (relation == CHILD) {
			return "Child";
		}
		else if (relation == SIGNIFICANT_OTHER) {
			return "Significant Other";
		}
		else if (relation == MOTHER) {
			return "Mother";
		}
		else if (relation == FATHER) {
			return "Father";
		}
		else if (relation == RELATED) {
			return "Related";
		}
		else if (relation == UNRELATED) {
			return "Unrelated";
		}
		else if (relation == NON_DIRECTED) {
			return "Non-Directed";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDBloodType bloodType) {
		if (bloodType == BT_O) {
			return "O";
		}
		else if (bloodType == BT_A) {
			return "A";
		}
		else if (bloodType == BT_B) {
			return "B";
		}
		else if (bloodType == BT_AB) {
			return "AB";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDRace race) {
		if (race == WHITE) {
			return "White";
		}
		else if (race == BLACK) {
			return "Black";
		}
		else if (race == HISPANIC) {
			return "Hispanic";
		}
		else if (race == OTHER_RACE) {
			return "Other";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDInsurance insurance) {
		if (insurance == PUBLIC) {
			return "Public Primary Payer";
		}
		else if (insurance == PRIVATE) {
			return "Private Primary Payer";
		}
		else if (insurance == OTHER_INSURANCE) {
			return "Other Insurance";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDUnderlyingStatus state) {
		if (state == ACTIVE) {
			return "A";
		}
		else if (state == INACTIVE) {
			return "I";
		}
		else if (state == WITHDRAWN) {
			return "W";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDTransplantationStatus state) {
		if (state == NOT_TRANSPLANTED) {
			return "Not Transplanted";
		}
		else if (state == IN_PROGRESS) {
			return "In Progress";
		}
		else if (state == TRANSPLANTED) {
			return "Transplanted";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDNodeType type) {
		if (type == PAIR) {
			return "Pair";
		}
		else if (type == NDD) {
			return "NDD";
		}
		else if (type == BRIDGE) {
			return "Bridge Donor";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDCrossmatchResult result) {
		if (result == SUCCESSFUL_CROSSMATCH) { 
			return "Successful Crossmatch"; 
		}
		else if (result == O_DONOR_TO_NON_O_CANDIDATE) { 
			return "O Donor to Non-O Candidate"; 
		}
		else if (result == REQUIRES_DESENSITIZATION) { 
			return "Requires Desensitization"; 
		}
		else if (result == REQUIRES_DESENSITIZATION_AND_O_TO_NON_O) {
			return "Requires Desensitization and O Donor to Non-O Candidate";
		}
		else if (result == FAILED_CROSSMATCH_HLA) { 
			return "Failed Crossmatch (Based on HLA)"; 
		}
		else if (result == FAILED_CROSSMATCH_BT) { 
			return "Failed Crossmatch (Based on BT)";
		}
		else if (result == FAILED_CROSSMATCH_LAB) {
			return "Failed Crossmatch (Lab Crossmatch)";
		}
		else { 
			return ""; 
		}
	}

	inline std::string toShortString(KPDCrossmatchResult result) {
		if (result == SUCCESSFUL_CROSSMATCH) {
			return "S";
		}
		else if (result == O_DONOR_TO_NON_O_CANDIDATE) {
			return "O-O'";
		}
		else if (result == REQUIRES_DESENSITIZATION) {
			return "D";
		}
		else if (result == REQUIRES_DESENSITIZATION_AND_O_TO_NON_O) {
			return "D O-O'";
		}
		else if (result == FAILED_CROSSMATCH_HLA) {
			return "FHLA";
		}
		else if (result == FAILED_CROSSMATCH_BT) {
			return "FBT";
		}
		else if (result == FAILED_CROSSMATCH_LAB) {
			return "FXM";
		}
		else {
			return "";
		}
	}

	inline std::string toString(KPDOutputFileType type) {
		if (type == ARRANGEMENT_LIST) {
			return "Arrangement List";
		}
		else if (type == TRANSPLANT_LIST) {
			return "Transplant List";
		}
		else if (type == SIMULATION_LOG) {
			return "Simulation Log";
		}
		else {
			return "";
		}
	}


	//String->Enum Functions
	inline KPDOptimizationScheme stringToOptimizationScheme(std::string arrangementType) {
		KPDOptimizationScheme type = CYCLES_AND_CHAINS;
		if (arrangementType.compare("CYCLES_AND_CHAINS_WITH_FALLBACKS") == 0) { type = CYCLES_AND_CHAINS_WITH_FALLBACKS; }
		else if (arrangementType.compare("LOCALLY_RELEVANT_SUBSETS") == 0) { type = LOCALLY_RELEVANT_SUBSETS;  }
		
		return type;
	}

	inline KPDUtilityScheme stringToUtilityScheme(std::string utilityScheme) {
		KPDUtilityScheme scheme = TRANSPLANTS;
		if (utilityScheme.compare("FIVE_YEAR_SURVIVAL") == 0) { scheme = FIVE_YEAR_SURVIVAL; }
		else if (utilityScheme.compare("TEN_YEAR_SURVIVAL") == 0) { scheme = TEN_YEAR_SURVIVAL; }
		else if (utilityScheme.compare("TRANSPLANT_DIFFICULTY") == 0) { scheme = TRANSPLANT_DIFFICULTY; }
		else if (utilityScheme.compare("RANDOM_UTILITY") == 0) { scheme = RANDOM_UTILITY;  }

		return scheme;
	}

	inline KPDDonorAssignmentScheme stringToDonorAssignmentScheme(std::string donorAssignmentScheme) {
		KPDDonorAssignmentScheme scheme = PAIRED_DONORS;
		if (donorAssignmentScheme.compare("RANDOM_DONORS") == 0) { scheme = RANDOM_DONORS; }

		return scheme;
	}

	inline KPDTimeline stringToTimeline(std::string timeline) {
		KPDTimeline scheme = FIXED_MATCH_RUNS;
		if (timeline.compare("CONTINUOUS_TIMELINE") == 0) { scheme = CONTINUOUS_TIMELINE; }

		return scheme;
	}	

	inline KPDMatchFailureScheme stringToMatchFailureScheme(std::string failureScheme) {
		KPDMatchFailureScheme scheme = PRA_BASED_MATCH_FAILURE;
		if (failureScheme.compare("RANDOM_MATCH_FAILURE") == 0) { scheme = RANDOM_MATCH_FAILURE; }

		return scheme;
	}

	inline KPDRelationToCandidate stringToRelation(std::string relation) {
		KPDRelationToCandidate relationToCandidate = CHILD;
		if (relation.compare("SIGNIFICANT_OTHER") == 0) { relationToCandidate = SIGNIFICANT_OTHER; }
		else if (relation.compare("MOTHER") == 0) { relationToCandidate = MOTHER; }
		else if (relation.compare("FATHER") == 0) { relationToCandidate = FATHER; }
		else if (relation.compare("RELATED") == 0) { relationToCandidate = RELATED; }
		else if (relation.compare("UNRELATED") == 0) { relationToCandidate = UNRELATED; }
		else if (relation.compare("NON_DIRECTED") == 0) { relationToCandidate = NON_DIRECTED; }

		return relationToCandidate;
	}

	inline KPDBloodType stringToBloodType(std::string bt) {
		KPDBloodType bloodtype = BT_O;
		if (bt.compare("A") == 0) { bloodtype = BT_A; }
		else if (bt.compare("B") == 0) { bloodtype = BT_B; }
		else if (bt.compare("AB") == 0) { bloodtype = BT_AB; }

		return bloodtype;
	}

	inline KPDRace stringToRace(std::string raceString) {
		KPDRace race = WHITE;
		if (raceString.compare("BLACK") == 0) { race = BLACK; }
		else if (raceString.compare("HISPANIC") == 0) { race = HISPANIC; }
		else if (raceString.compare("OTHER_RACE") == 0) { race = OTHER_RACE; }

		return race;
	}

	inline KPDInsurance stringToInsurance(std::string insuranceString) {
		KPDInsurance insurance = PUBLIC;
		if (insuranceString.compare("PRIVATE") == 0) { insurance = PRIVATE; }
		else if (insuranceString.compare("OTHER_INSURANCE") == 0) { insurance = OTHER_INSURANCE; }

		return insurance;
	}

	inline KPDUnderlyingStatus stringToUnderlyingStatus(std::string underlyingStatus) {
		KPDUnderlyingStatus status = ACTIVE;
		if (underlyingStatus.compare("INACTIVE") == 0) { status = INACTIVE; }
		else if (underlyingStatus.compare("WITHDRAWN") == 0) { status = WITHDRAWN; }

		return status;
	}

	inline KPDTransplantationStatus stringToTransplantationStatus(std::string transplantationStatus) {
		KPDTransplantationStatus status = NOT_TRANSPLANTED;
		if (transplantationStatus.compare("IN_PROGRESS") == 0) { status = IN_PROGRESS; }
		else if (transplantationStatus.compare("TRANSPLANTED") == 0) { status = TRANSPLANTED; }

		return status;
	}

	inline KPDNodeType stringToNodeType(std::string nodeType) {
		KPDNodeType type = PAIR;
		if (nodeType.compare("NDD") == 0) { type = NDD; }
		else if (nodeType.compare("BRIDGE") == 0) { type = BRIDGE; }

		return type;
	}

	inline KPDCrossmatchResult stringToCrossmatchResult(std::string result) {
		KPDCrossmatchResult crossmatchResult = SUCCESSFUL_CROSSMATCH;
		if (result.compare("O_DONOR_TO_NON_O_CANDIDATE") == 0) { crossmatchResult = O_DONOR_TO_NON_O_CANDIDATE; }
		else if (result.compare("REQUIRES_DESENSITIZATION") == 0) { crossmatchResult = REQUIRES_DESENSITIZATION; }
		else if (result.compare("REQUIRES_DESENSITIZATION_AND_O_TO_NON_O") == 0) { crossmatchResult = REQUIRES_DESENSITIZATION_AND_O_TO_NON_O; }
		else if (result.compare("FAILED_CROSSMATCH_HLA") == 0) { crossmatchResult = FAILED_CROSSMATCH_HLA; }
		else if (result.compare("FAILED_CROSSMATCH_BT") == 0) { crossmatchResult = FAILED_CROSSMATCH_BT; }
		else if (result.compare("FAILED_CROSSMATCH_LAB") == 0) { crossmatchResult = FAILED_CROSSMATCH_LAB; }

		return crossmatchResult;
	}

	inline KPDOutputFileType stringToOutputFileType(std::string fileType) {
		KPDOutputFileType type = ARRANGEMENT_LIST;
		if (fileType.compare("TRANSPLANT_LIST") == 0) { type = TRANSPLANT_LIST; }
		else if (fileType.compare("SIMULATION_LOG") == 0) { type = SIMULATION_LOG; }
		
		return type;
	}

	// Other helper functions

	// Convert general bools to string
	inline std::string boolToString(bool boolToConvert) {
		if (boolToConvert) {
			return "Yes";
		}
		else {
			return "No";
		}
	}

	// Convert gender designation (bool) to string
	inline std::string genderToString(bool male) {
		if (male) {
			return "Male";
		}
		else {
			return "Female";
		}
	}

	// Convert int to string
	inline std::string intToString(int number) {
		std::stringstream stream;
		stream << number;
		return stream.str();
	}

	// Convert lab crossmatch result to string
	inline std::string labCrossmatchToString(bool result) {
		if (result) {
			return "Success";
		}
		else {
			return "Failure";
		}
	}

	inline std::string labCrossmatchToShortString(bool result) {
		if (result) {
			return "T";
		}
		else {
			return "F";
		}
	}


	// Retrieve the index associated with the discrete sampling of a vector
	inline int retrieveDiscreteSampleIndex(double prob, std::vector<double> & probs) {

		int n = (int)probs.size();
		int outcome = 0;

		while (outcome + 1 < n && prob > probs[outcome]) {
			prob -= probs[outcome];
			outcome++;
		}
		return outcome;
	}

	// Assign flags to each element of a vector
	inline int setFlags(int value, std::vector<int> &flagVector) {

		//Starts at index 1
		int position = 1;
		int numberOfFlags = 0;
		while (value != 0) {
			int flag = value % 2;
			if (flag == 1) {
				numberOfFlags++;
				flagVector[position] = 1;
			}
			value = value / 2;
			position++;
		}

		return numberOfFlags;
	}
	
	// Truncate value to within a certain range
	inline double truncateValue(double value, double min, double max) {

		double newValue = value;
		if (value > max) {
			newValue = max;
		}
		if (value < min) {
			newValue = min;
		}

		return newValue;
	}

}

#endif