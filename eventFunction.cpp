#include "eventFunction.h"
#include "flags.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <chrono>

// pending task: ability to toggle and record flags.

const std::chrono::milliseconds TEXT_DELAY = std::chrono::milliseconds(500);
const std::string playerName_placeholder = "軟綿綿橘子貓"; // to be replaced with getter function.

// Should take MainCharacter as argument as well.
// It should update relevant variables through get/increment functions.
// I'll add it in once its ready.
bool triggerEvent(std::string eventFileName);
void parseAndOutputdialogue(std::ifstream& event);
void parseAndProcessBranchingChoice(std::ifstream& event);
void parseAndOutputSysMsg(std::ifstream& event);
void parseAndProcessStatChange(std::ifstream& event);
void parseAndProcessFlagUpdate(std::ifstream& event, FlagArray& flags);
void replacePlayerName(std::string& line, std::string playerName);
void delay_ms(std::chrono::milliseconds);

/*int main() {
	try{
		triggerEvent("event1.txt");
		triggerEvent("event2.txt");
		triggerEvent("event3.txt");
	}
	catch(std::invalid_argument const& e){
		std::cout << e.what();
	}
	return 0;
}*/

bool triggerEvent(std::string eventFileName, FlagArray& flags) {
	std::string path = "events/";
	path += eventFileName;
	
	std::ifstream event(path);
	
	if(!event.is_open()){
		std::string errMsg = "File name does not exist: " + eventFileName + '\n';
		throw std::invalid_argument(errMsg);
	}
	
	// output event title.
	std::string title = "";
	std::getline(event, title);
	std::cout << title << "\n";
	delay_ms(TEXT_DELAY);
	
	while(true){
		std::string input = "";
		std::getline(event, input);
		if(input == "dialogue"){
			parseAndOutputdialogue(event);
		}else if(input == "choice"){
			parseAndProcessBranchingChoice(event);
		}else if(input == "sysMsg"){
			parseAndOutputSysMsg(event);
		}else if(input == "endEvent"){
			break;
		}else if(input == "endBranch"){
			while(input != "endChoice"){
				std::getline(event, input);
			}
		}else if(input == "statChange"){
			parseAndProcessStatChange(event);
		}else if(input == "flag"){
			parseAndProcessFlagUpdate(event, flags);
		}else{
			throw std::logic_error("Invalid file formatting.");
		}

		std::getline(event, input); // skip line break.
		delay_ms(TEXT_DELAY);
	}
	
	event.close();
	return true;
}

void parseAndOutputdialogue(std::ifstream& event) {
	std::string input = "";

	std::getline(event, input);
	if(input != "player"){
		std::cout << input << "：";
	}

	std::getline(event, input);
	replacePlayerName(input, playerName_placeholder);
	std::cout << input << std::endl;

	return;
}

void parseAndProcessBranchingChoice(std::ifstream& event) {
	std::string input = "";
	std::getline(event, input);
	int branchCnt = std::stoi(input);

	std::string* choice = new std::string[branchCnt];
	for(int i = 0; i < branchCnt; ++i){
		std::getline(event, input);
		choice[i] = input;
		std::cout << "(" << i + 1 << ") " << input << std::endl;
	}

	std::getline(event, input);
	
	int playerChoice = 0;
	while(true){
		std::cout << "＞＞你的選擇：";
		std::cin >> input;
		try{
			playerChoice = std::stoi(input) - 1;
			if(playerChoice < 0 || playerChoice >= branchCnt){
				throw std::out_of_range("Choice out of range.");
			}
			break;
		}
		catch(std::invalid_argument const& e){
			std::cout << "＞＞請輸入有效數字！數字就好，括號不用～" << std::endl;
		}
		catch(std::out_of_range const& e){
			std::cout << "＞＞請輸入有效數字！選擇：（";
			std::cout << "1";
			for(int i = 1; i < branchCnt; ++i){
				std::cout << "," << i + 1;
			}
			std::cout << "）" << std::endl;
		}
	}

	std::string chosenBranch = "branch " + std::to_string(playerChoice);

	while(true){
		std::getline(event, input);
		if(input == chosenBranch){
			return;
		}
	}

	delete [] choice;
	return;
}

void parseAndOutputSysMsg(std::ifstream& event) {
	std::string input = "";

	std::getline(event, input);
	replacePlayerName(input, playerName_placeholder);
	std::cout << "＞＞ " << input << std::endl;
	return;
}

void parseAndProcessStatChange(std::ifstream& event) {
	std::string parameter = "";
	std::string value_string = "";

	std::getline(event, parameter);
	std::getline(event, value_string);

	int value = 0;
	try{
		value = stoi(value_string);
	}
	catch(std::invalid_argument const& e){
		throw std::invalid_argument("Invalid statChange value.");
	}

	// placeholder actions. to be replaced with increment functions.
	if(parameter == "ATK"){
		std::cout << "atk changed by " << value << std::endl;
	}else if(parameter == "DEF"){
		std::cout << "def changed by " << value << std::endl;
	}else if(parameter == "SPD"){
		std::cout << "spd changed by " << value << std::endl;
	}else if(parameter == "SKL"){
		std::cout << "skl changed by " << value << std::endl;
	}else if(parameter == "LUK"){
		std::cout << "luk changed by " << value << std::endl;
	}else if(parameter == "MAXSP"){
		std::cout << "maxsp changed by " << value << std::endl;
	}else if(parameter == "MNY"){
		std::cout << "mny changed by " << value << std::endl;
	}else{
		throw std::invalid_argument("Parameter does not exist.");
	}

	return;
}

void parseAndProcessFlagUpdate(std::ifstream& event, FlagArray& flags) {
	std::string flagName = "";
	bool status = false;

	event >> flagName >> status;
	flags.updateFlagStatus(flagName, status);

	std::getline(event, flagName); // clear \n for next .getline.
	return;
}

void replacePlayerName(std::string& line, std::string playerName) {
	std::size_t pos = 0;
	
	pos = line.find("player", pos);
	while(pos != std::string::npos){
		line.replace(pos, 6, playerName);
		pos = line.find("player", pos);
	}

	return;
}

void delay_ms(std::chrono::milliseconds ms){
	using namespace std::chrono;

	time_point<steady_clock> start = steady_clock::now();
	while(true){
		time_point<steady_clock> now = steady_clock::now();
		milliseconds duration = duration_cast<milliseconds>(now - start);
		if(duration >= ms){
			break;
		}
	}

	return;
}