#include "grid.h"
#include <iostream>
#include <cctype>
#include <cassert>
using namespace std;

const int HORIZ = 0;
const int VERT = 1;

const int FG = 0;
const int BG = 1;

bool plotLine(int r, int c, int distance, int dir, char plotChar, int fgbg);
void plotHorizontalLine(int r, int c, int distance, char plotChar, int fgbg);
void plotVerticalLine(int r, int c, int distance, char plotChar, int fgbg);
int performCommands(string commandString, char& plotChar, int& mode, int& badPos);
int processsteps(string commandString, int& r, int &c, int& pos, int dir, char& plotChar, int&mode, int&badPos);


int main()
{
    for (;;)
        {
            cout << "Enter the number of grid rows and columns (max 30 each): ";
            int nRows;
            int nCols;
            cin >> nRows >> nCols;
            cin.ignore(10000, '\n');
            if (nRows >= 1  &&  nRows <= MAXROWS  &&  nCols >= 1  &&  nCols <= MAXCOLS)
            {
                setSize(nRows, nCols);
                break;
            }
            cout << "The numbers must be between 1 and 30." << endl;
        }
        char currentChar = '*';
        int currentMode = FG;
        for (;;)
        {
            cout << "Enter a command string (empty line to quit): ";
            string cmd;
            getline(cin, cmd);
            if (cmd == "")
                break;
            int position;
            int status = performCommands(cmd, currentChar, currentMode, position);
            switch (status)
            {
              case 0:
                draw();
                break;
              case 1:
                cout << "Syntax error at position " << position << endl;
                break;
              case 2:
                if (!isprint(currentChar))
                    cout << "Current character is not printable" << endl;
                if (currentMode != FG  &&  currentMode != BG)
                    cout << "Current mode is " << currentMode << ", not FG or BG" << endl;
                break;
              case 3:
                cout << "Cannot perform command at position " << position << endl;
                break;
              default:
                  // It should be impossible to get here.
                cout << "performCommands returned " << status << "!" << endl;
            }
        }
    }
  

int performCommands(string commandString, char& plotChar, int& mode, int& badPos){
    int r = 1;
    int c = 1;
    char ch = plotChar;
    int fgbg = mode;
    int i = 0;
    int output = 0;
    while (i < commandString.size()){ //go through the letters in the string until you get to the end
        if (isprint(commandString.at(i)) && (mode == FG || mode == BG)){ //check to make sure that mode is valid and that all characters are printable
            switch(commandString.at(i)){
                case 'b':
                case 'B': //when the character is b, we know that in order to be valid, it can only have one character before the next letter
                    if (i+1<commandString.size()){//make sure that the length is right
                        if (isprint(commandString.at(i+1))){ //make sure the plotChar can be printed
                            mode = BG;
                            plotChar = commandString.at(i+1); // set the new plotted character
                            i+=2;//move i to the next char
                        }
                        else{
                            badPos = i + 1;
                            return 1;
                        }
                    }
                    else{
                        badPos = i;
                        return 1;
                    }
                    break;
                case 'f':
                case 'F':
                    if (i+1<commandString.size()){
                        if (isprint(commandString.at(i+1))){
                            mode = FG;
                            plotChar = commandString.at(i+1);
                            i+=2;
                        }
                        else{
                            badPos = i + 1;
                            return 1;
                        }
                    }
                    else{
                        badPos = i;
                        return 1;
                    }
                    break;
                case 'c':
                case 'C':
                    if (isalpha(commandString.at(i+1))){
                        cerr << "There is no issue with this clear command" << endl;
                        r = 1;
                        c = 1;
                        clearGrid();
                        plotChar = '*';
                        cerr << "There is no issue with this clear command" << endl;
                    }
                    else{
                        cerr << "There is a number or other invalid syntax after C" << endl;
                        badPos = i+1;
                        return 1;
                    }
                    i++;
                    break;
                case 'v':
                case 'V': //now to parse commands with vertical and horizontal
                    output = processsteps(commandString,r,c,i,VERT,plotChar,mode,badPos); //checks validity of command and plots if valid
                    if (output != 0){
                        return output;//when command isnt valid
                    }
                    
                    break;
                case 'h':
                case 'H':
                    output = processsteps(commandString,r,c,i,HORIZ,plotChar,mode,badPos);
                    if (output != 0){
                        return output;
                    }
                    break;
                default:
                    badPos = i;
                    return 1;}
        }
    else{
        return 2;
    }
}
    return 0;
}

int processsteps(string commandString, int& r, int &c, int& pos, int dir, char& plotChar, int&mode, int&badPos){
    int distance;
    pos++; //now pos is the position of the first character after the letter
    bool negative = false;
    if (pos >= commandString.size()){ // test whether or not it's an empty command
        cerr << "The command only has one letter" << endl;
        badPos = commandString.size();
        return 1;
    }

    
    
    if (commandString.at(pos) == '-'){
        negative = true;
        pos++;
    }
    else{
        negative = false;
    }
    
    //pos in the following statement is the character after H/V and if applicable, after -
    if (!isdigit(commandString.at(pos))){ //we know that pos has to exist, because the previous if statement would've gotten rid of it otherwise: this tests if we have a function like v* or h) and also would ignore negatives because of the previous if statement
        cerr << "The character after the letter is not valid" << endl;
        badPos = pos;
        return 1;
    }
    else{
        //pos is for sure the first digit of the command: now to figure out whether or not there are two digits or one digit
        if (pos + 1 < commandString.size() && isdigit(commandString.at(pos+1))){ //for two or more digits: H25(pos = 1, size = 3) is a valid command, H250 (pos = 1, size = 4) is not
            //now we know for sure that there are at least two digits in this equation, but what if there are 3 digits?
            if (pos + 2 < commandString.size() && isdigit(commandString.at(pos+2))){
                cerr << "There are more than 2 digits in this command" << endl;
                badPos = pos + 2;
                return 1;
            }
        
            if (negative == false){
                int onesdigit = commandString.at(pos+1) - '0';
                int tensdigit = commandString.at(pos) - '0';
                distance = 10*tensdigit + onesdigit;
                cerr << "positive 2 digit distance works! it is " << distance << endl;
                if (dir == HORIZ){
                    if (c+distance >= getCols() || c+distance <= 1){
                        badPos = pos - 1;
                        return 3;
                    }
                }
                else if (dir == VERT){
                    if (r + distance >= getRows() || r + distance <= 1){
                        badPos = pos - 1;
                        return 3;
                    }
                }
            }
            else {
                int onesdigit = commandString.at(pos+1) - '0';
                int tensdigit = commandString.at(pos) - '0';
                distance = -1*(10*tensdigit + onesdigit);
                cerr << "negative 2 digit distance works! it is " << distance << endl;
                if (dir == HORIZ){
                    if (c+distance > getCols() || c+distance < 1){
                        badPos = pos - 2;
                        return 3;
                    }
                }
                else if (dir == VERT){
                    if (r + distance > getRows() || r + distance < 1){
                        badPos = pos - 2;
                        return 3;
                    }
                }
            }
            pos = pos+2; //for two digits, we need to skip past the next two numbers
        }
        else{  //isdigit(commandString.at(pos)) && pos < commandString.size()){ the distance is one digit only and we know it's a valid digit
            if (negative == false){
                distance = commandString.at(pos) - '0';
                cerr << "positive 1 digit distance works! it is " << distance << endl;
                if (dir == HORIZ){
                    if (c+distance > getCols() || c+distance < 1){
                        badPos = pos - 1;
                        return 3;
                    }
                }
                else if (dir == VERT){
                    if (r + distance > getRows() || r + distance < 1){
                        badPos = pos - 1;
                        return 3;
                    }
                }
            }
            else{
                distance = -1*(commandString.at(pos) - '0');
                cerr << "negative 1 digit distance works! it is " << distance << endl;
                if (dir == HORIZ){
                    if (c+distance > getCols() || c+distance < 1){
                        badPos = pos - 2;
                        return 3;
                    }
                }
                else if (dir == VERT){
                    if (r + distance > getRows() || r + distance < 1){
                        badPos = pos - 2;
                        return 3;
                    }
                }
            }
           
            
            pos++;
        }
    }
    
    
    if (plotLine(r,c,distance,dir,plotChar,mode)){
        if (dir == HORIZ){
            c+=distance;
        }
        else if (dir == VERT){
            r+=distance;
        }
    }
    return 0;
}



bool plotLine(int r, int c, int distance, int dir, char plotChar, int fgbg){
    if (fgbg == 0 || fgbg == 1){// check that the mode is valid
        if (dir == 0){
            if (r <= getRows() && c <= getCols() && c + distance <= getCols() && isprint(plotChar) && c+distance >= 1 && r>=1 && c>=1){ //check to make sure that r and c are within all the boundaries, and if plotted, the grid will still be valid
            plotHorizontalLine(r,c,distance,plotChar, fgbg); //since we know dir == 0, we know direction is horizontal
            return true;
            }
            else{
            return false;
            }
        }
        
        else if (dir == 1){
            if (r <= getRows() && c <= getCols() && r + distance <= getRows() && isprint(plotChar) && r+distance >= 1 && r>=1 && c>=1){
            plotVerticalLine(r,c,distance,plotChar,fgbg);
            return true;
            }
            else{
            return false;
            }
        }
        else {//what happens when HORIZ or VERT aren't in the function
        return false;
        }
    }
    else{// what happens when fgbg isn't a valid
        return false;
    }
    

}

void plotHorizontalLine(int r, int c, int distance, char ch, int fgbg){
        if (distance < 0){ //this is what happens when negative distance
            distance = distance * -1;
            for (int i = 0; i <= distance; i++){
                if (getChar(r,c-i) == ' ' || fgbg == 0){
                    setChar(r,c-i,ch); //change the character at the spot to the intended distance
                }
            }
        }
        else{ //what happens when the distance provided is positive
            for (int i = 0; i <= distance; i++){
                if (getChar(r,c+i) == ' ' || fgbg == 0){
                    setChar(r,c+i,ch);
                }
            }
        }
    }


void plotVerticalLine(int r, int c, int distance, char ch, int fgbg){
    if (distance < 0){ //vertical plotting when it's negative
        distance = distance * -1;
        for (int i = 0; i <= distance; i++){
            if (getChar(r-i,c) == ' ' || fgbg == 0){
                setChar(r-i,c,ch);
            }
        }
    }
    else{
        for (int i = 0; i <= distance; i++){
            if (getChar(r+i,c) == ' ' || fgbg == 0){
                setChar(r+i,c,ch);
            }
        }
    }
}
