/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   newsimpletest.cpp
 * Author: michele
 *
 * Created on May 15, 2021, 6:30 PM
 */

#include <stdlib.h>
#include <iostream>


#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

  
class TestCase {
  public:
  int move;
  long player;
  long opponent;
  TestCase(int move, long player, long opponent) : move(move), player(player), opponent(opponent) {}
};
  
//  static {
//    try {
//      File input = new File("coefficients/moves.txt");
//      Scanner myReader = new Scanner(input);
//      while (myReader.hasNextLine()) {
//        String s = myReader.nextLine();
//        String[] splitS = s.split(" ");
//        tests.add(
//            new TestCase(Integer.parseInt(splitS[0]),
//                Long.parseLong(splitS[1]), Long.parseLong(splitS[2])));
//      }
//    } catch (FileNotFoundException ex) {
//      Logger.getLogger(GetMovesCache.class.getName()).log(Level.SEVERE, null, ex);
//    }
//  }
int main(int argc, char** argv) {
  std::vector<TestCase> tests;
  ifstream tests_file("../../../coefficients/moves.txt");
  std::string line;
  while (std::getline(tests_file, line)) {
    // Output the text from the file
    std::cout << line << "\n";
    break;
  }
  tests_file.close();
  return 0;
}