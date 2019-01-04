/* ---------------------------------------------
DD-Enums-Functions.h
Defines a Set of Factor Variables and Functions 
For Use Throughout the Project
---------------------------------------------- */

#ifndef ENUMSFUNCTIONS_H
#define ENUMSFUNCTIONS_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <ctime>

/* Enums */

// Simulation Specifications
enum KPDOptimizationScheme { CYCLES_AND_CHAINS, CYCLES_AND_CHAINS_WITH_FALLBACKS, LOCALLY_RELEVANT_SUBSETS };
enum KPDUtilityScheme { UTILITY_TRANSPLANTS, UTILITY_FIVE_YEAR_SURVIVAL, UTILITY_TEN_YEAR_SURVIVAL, UTILITY_TRANSPLANT_DIFFICULTY, UTILITY_RANDOM };

// Characteristics
enum KPDBloodType { BT_O, BT_A, BT_B, BT_AB, BT_UNSPECIFIED };
enum KPDRace { RACE_WHITE, RACE_BLACK, RACE_HISPANIC, RACE_HAWAIIAN, RACE_NATIVE, RACE_ASIAN, RACE_MULTIRACIAL, RACE_OTHER, RACE_UNSPECIFIED };
enum KPDRelation { RELATION_PARENT, RELATION_NDD, RELATION_LIVING_DECEASED, RELATION_NON_DOMINO_THERAPEUTIC,
	RELATION_CHILD, RELATION_TWIN, RELATION_SIBLING, RELATION_HALF_SIBLING, RELATION_RELATIVE,
	RELATION_SPOUSE, RELATION_PARTNER, RELATION_PAIRED_DONATION, RELATION_OTHER_UNRELATED, RELATION_UNSPECIFIED };
enum KPDInsurance { INSURANCE_PUBLIC, INSURANCE_MEDICAID, INSURANCE_MEDICARE_PLUS, INSURANCE_MEDICARE, INSURANCE_PRIVATE, INSURANCE_PRIVATE_PLUS, 
	INSURANCE_OTHER, INSURANCE_UNSPECIFIED };

// Node Properties
enum KPDStatus{ STATUS_ACTIVE, STATUS_INACTIVE, STATUS_WITHDRAWN };
enum KPDTransplant { TRANSPLANT_YES, TRANSPLANT_NO, TRANSPLANT_IN_PROGRESS };
enum KPDNodeType { PAIR, NDD, BRIDGE };

// Match Properties
enum KPDCrossmatch { CROSSMATCH_SUCCESSFUL, CROSSMATCH_O_DONOR_TO_NON_O_CANDIDATE, 
	CROSSMATCH_REQUIRES_DESENSITIZATION, CROSSMATCH_REQUIRES_DESENSITIZATION_AND_O_TO_NON_O, CROSSMATCH_FAILED };


/* Functions */

namespace KPDFunctions {

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

	// Converts 'value' to binary, saving to vector and returning the number of bits
	inline int setFlags(int value, int startIndex, std::vector<int> &flagVector) {
		
		int position = startIndex;
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
		
	// String Functions

	inline std::string intToString(int number) {
		std::stringstream stream;
		stream << number;
		return stream.str();
	}

	std::string indent(int numberOfSpaces) {

		std::stringstream tab;

		for (int i = 1; i <= numberOfSpaces; i++) {
			tab << " ";
		}

		return tab.str();
	}

	inline std::string boolToYesNo(bool flag) {
		if (flag) {
			return "Yes";
		}
		else {
			return "No";
		}
	}

	inline std::string boolToMaleFemale(bool male) {
		if (male) {
			return "Male";
		}
		else {
			return "Female";
		}
	}

	inline std::string boolToTF(bool result) {
		if (result) {
			return "T";
		}
		else {
			return "F";
		}
	}

	
	// Enum Converters	

	inline std::string optimizationSchemeToString(KPDOptimizationScheme type) {
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
			return "Unspecified";
		}
	}

	inline std::string utilitySchemeToString(KPDUtilityScheme scheme) {
		if (scheme == UTILITY_TRANSPLANTS) {
			return "# Of Transplants";
		}
		else if (scheme == UTILITY_FIVE_YEAR_SURVIVAL) {
			return "5-Year Survival";
		}
		else if (scheme == UTILITY_TEN_YEAR_SURVIVAL) {
			return "10-Year Survival";
		}
		else if (scheme == UTILITY_TRANSPLANT_DIFFICULTY) {
			return "Difficult to Transplant Candidates";
		}
		else if (scheme == UTILITY_RANDOM) {
			return "Random Utility Assignment";
		}
		else {
			return "Unspecified";
		}
	}
	
	inline std::string bloodTypeToString(KPDBloodType bloodType) {
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
			return "Unspecified";
		}
	}

	inline std::string raceToString(KPDRace race) {
		if (race == RACE_WHITE) {
			return "White";
		}
		else if (race == RACE_BLACK) {
			return "Black";
		}
		else if (race == RACE_HISPANIC) {
			return "Hispanic";
		}
		else if (race == RACE_HAWAIIAN) {
			return "Hawaiian";
		}
		else if (race == RACE_NATIVE) {
			return "Native";
		}
		else if (race == RACE_ASIAN) {
			return "Asian";
		}
		else if (race == RACE_MULTIRACIAL) {
			return "Multi-Racial";
		}
		else if (race == RACE_OTHER) {
			return "Other";
		}
		else {
			return "Unspecified";
		}
	}
	
	inline std::string relationToString(KPDRelation relation) {
		if (relation == RELATION_PARENT) {
			return "Parent";
		}
		else if (relation == RELATION_NDD) {
			return "Non-Directed Donor";
		}
		else if (relation == RELATION_LIVING_DECEASED) {
			return "Living/Decased Donor";
		}
		else if (relation == RELATION_NON_DOMINO_THERAPEUTIC) {
			return "Non-Domino Therapeutic Donor";
		}
		else if (relation == RELATION_CHILD) {
			return "Child";
		}
		else if (relation == RELATION_TWIN) {
			return "Twin";
		}
		else if (relation == RELATION_SIBLING) {
			return "Sibling";
		}
		else if (relation == RELATION_HALF_SIBLING) {
			return "Half-Sibling";
		}
		else if (relation == RELATION_RELATIVE) {
			return "Relative";
		}
		else if (relation == RELATION_SPOUSE) {
			return "Spouse";
		}
		else if (relation == RELATION_PARTNER) {
			return "Partner";
		}
		else if (relation == RELATION_PAIRED_DONATION) {
			return "Paired Donation";
		}
		else if (relation == RELATION_OTHER_UNRELATED) {
			return "Other Unrelated Donor";
		}
		else {
			return "Unspecified";
		}
	}

	inline std::string insuranceToString(KPDInsurance insurance) {

		if (insurance == INSURANCE_PUBLIC) {
			return "Public Only";
		}
		else if (insurance == INSURANCE_MEDICAID) {
			return "Medicaid Only";
		}
		else if (insurance == INSURANCE_MEDICARE_PLUS) {
			return "Medicare + Other";
		}
		else if (insurance == INSURANCE_MEDICARE) {
			return "Medicaid Only";
		}
		else if (insurance == INSURANCE_OTHER) {
			return "Other Payment";
		}
		else if (insurance == INSURANCE_PRIVATE) {
			return "Private Only";
		}
		else if (insurance == INSURANCE_PRIVATE_PLUS) {
			return "Private + Other";
		}
		else {
			return "Unspecified";
		}
	}

	inline std::string statusToString(KPDStatus state) {
		if (state == STATUS_ACTIVE) {
			return "Active";
		}
		else if (state == STATUS_INACTIVE) {
			return "Inactive";
		}
		else if (state == STATUS_WITHDRAWN) {
			return "Withdrawn";
		}
		else {
			return "Unspecified";
		}
	}

	inline std::string transplantToString(KPDTransplant state) {
		if (state == TRANSPLANT_NO) {
			return "Not Transplanted";
		}
		else if (state == TRANSPLANT_IN_PROGRESS) {
			return "In Progress";
		}
		else if (state == TRANSPLANT_YES) {
			return "Transplanted";
		}
		else {
			return "Unspecified";
		}
	}

	inline std::string nodeTypeToString(KPDNodeType type) {
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
			return "Unspecified";
		}
	}
	
	inline std::string crossmatchToString(KPDCrossmatch result) {
		if (result == CROSSMATCH_SUCCESSFUL) {
			return "Successful Crossmatch";
		}
		else if (result == CROSSMATCH_O_DONOR_TO_NON_O_CANDIDATE) {
			return "O Donor to Non-O Candidate";
		}
		else if (result == CROSSMATCH_REQUIRES_DESENSITIZATION) {
			return "Requires Desensitization";
		}
		else if (result == CROSSMATCH_REQUIRES_DESENSITIZATION_AND_O_TO_NON_O) {
			return "Requires Desensitization and O Donor to Non-O Candidate";
		}
		else if (result == CROSSMATCH_FAILED) {
			return "Failed Crossmatch";
		}
		else {
			return "Unspecified";
		}
	}
	
	inline KPDOptimizationScheme stringToOptimizationScheme(std::string arrangementType) {
		KPDOptimizationScheme type = CYCLES_AND_CHAINS;
		if (arrangementType.compare("CYCLES_AND_CHAINS_WITH_FALLBACKS") == 0) { type = CYCLES_AND_CHAINS_WITH_FALLBACKS; }
		else if (arrangementType.compare("LOCALLY_RELEVANT_SUBSETS") == 0) { type = LOCALLY_RELEVANT_SUBSETS;  }
		
		return type;
	}

	inline KPDUtilityScheme stringToUtilityScheme(std::string utilityScheme) {
		KPDUtilityScheme scheme = UTILITY_TRANSPLANTS;
		if (utilityScheme.compare("UTILITY_FIVE_YEAR_SURVIVAL") == 0) { scheme = UTILITY_FIVE_YEAR_SURVIVAL; }
		else if (utilityScheme.compare("UTILITY_TEN_YEAR_SURVIVAL") == 0) { scheme = UTILITY_TEN_YEAR_SURVIVAL; }
		else if (utilityScheme.compare("UTILITY_TRANSPLANT_DIFFICULTY") == 0) { scheme = UTILITY_TRANSPLANT_DIFFICULTY; }
		else if (utilityScheme.compare("UTILITY_RANDOM") == 0) { scheme = UTILITY_RANDOM;  }

		return scheme;
	}
	
	inline KPDBloodType stringToBloodType(std::string bt) {
		KPDBloodType bloodtype = BT_UNSPECIFIED;		
		if (bt.compare("O") == 0) { bloodtype = BT_O; }
		else if (bt.compare("A") == 0) { bloodtype = BT_A; }
		else if (bt.compare("B") == 0) { bloodtype = BT_B; }
		else if (bt.compare("AB") == 0) { bloodtype = BT_AB; }

		return bloodtype;
	}
	
	inline KPDRace stringToRace(std::string raceString) {
		KPDRace race = RACE_UNSPECIFIED;
		if (raceString.compare("WHITE") == 0 || raceString.compare("White") == 0) { race = RACE_WHITE; }
		else if (raceString.compare("BLACK") == 0 || raceString.compare("Black") == 0) { race = RACE_BLACK; }
		else if (raceString.compare("HISPANIC") == 0 || raceString.compare("Hispanic") == 0) { race = RACE_HISPANIC; }
		else if (raceString.compare("HAWAIIAN") == 0 || raceString.compare("Hawaiian") == 0) { race = RACE_HAWAIIAN; }
		else if (raceString.compare("NATIVE") == 0 || raceString.compare("Native") == 0) { race = RACE_NATIVE; }
		else if (raceString.compare("ASIAN") == 0 || raceString.compare("Asian") == 0) { race = RACE_ASIAN; }
		else if (raceString.compare("MULTIRACIAL") == 0 || raceString.compare("Multiracial") == 0) { race = RACE_MULTIRACIAL; }
		else if (raceString.compare("OTHER") == 0 || raceString.compare("Other") == 0) { race = RACE_OTHER; }
		
		return race;
	}
		
	inline KPDRelation stringToRelation(std::string relation) {
		KPDRelation relationToCandidate = RELATION_UNSPECIFIED;
		if (relation.compare("Parent") == 0) { relationToCandidate = RELATION_PARENT; }
		else if (relation.compare("NDD") == 0) { relationToCandidate = RELATION_NDD; }
		else if (relation.compare("Living/Deceased") == 0) { relationToCandidate = RELATION_LIVING_DECEASED; }
		else if (relation.compare("Non-Domino Therapeutic") == 0) { relationToCandidate = RELATION_NON_DOMINO_THERAPEUTIC; }
		else if (relation.compare("Child") == 0) { relationToCandidate = RELATION_CHILD; }
		else if (relation.compare("Twin") == 0) { relationToCandidate = RELATION_TWIN; }
		else if (relation.compare("Sibling") == 0) { relationToCandidate = RELATION_SIBLING; }
		else if (relation.compare("Half-Sibling") == 0) { relationToCandidate = RELATION_HALF_SIBLING; }
		else if (relation.compare("Relative") == 0) { relationToCandidate = RELATION_RELATIVE; }
		else if (relation.compare("Spouse") == 0) { relationToCandidate = RELATION_SPOUSE; }
		else if (relation.compare("Partner") == 0) { relationToCandidate = RELATION_PARTNER; }
		else if (relation.compare("Paired Donation") == 0) { relationToCandidate = RELATION_PAIRED_DONATION; }
		else if (relation.compare("Other Unrelated") == 0) { relationToCandidate = RELATION_OTHER_UNRELATED; }

		return relationToCandidate;
	}

	inline KPDInsurance stringToInsurance(std::string insuranceString) {
		KPDInsurance insurance = INSURANCE_UNSPECIFIED;
		if (insuranceString.compare("Public only") == 0) { insurance = INSURANCE_PUBLIC; }
		if (insuranceString.compare("Medicaid only") == 0) { insurance = INSURANCE_MEDICAID; }
		else if (insuranceString.compare("Medicare(primary) + Other") == 0) { insurance = INSURANCE_MEDICARE_PLUS; }
		else if (insuranceString.compare("Medicare only") == 0) { insurance = INSURANCE_MEDICARE; }
		else if (insuranceString.compare("Other source of payment") == 0) { insurance = INSURANCE_OTHER; }
		else if (insuranceString.compare("Private only") == 0) { insurance = INSURANCE_PRIVATE; }
		else if (insuranceString.compare("Private only + Other") == 0) { insurance = INSURANCE_PRIVATE_PLUS; }

		return insurance;
	}
	
}

#endif