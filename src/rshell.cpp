//
//
//          Assignment 1/2/3 - Design a 'bash' shell 
//
//              David Weber & Immanuel Chu
//                      5/10/17
//
//

#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <unistd.h>

#include "Legacy.h"
#include "Cmd.h"
#include "Connect.h"
#include "And.h"
#include "Or.h"
#include "Semicolon.h"
#include "Test.h"
#include "Parenth.h"
#include "Lt.h"
#include "Rt.h"
#include "Pipe.h"
#include "DubRt.h"

using namespace std;

void parseInput(string&, Legacy*&);
void testProtocol(string&, Legacy*&);
bool checkForSpace(const string& input);
void makeTree(Legacy*&, vector<char>&, vector<string>&, bool);
Legacy* treeUtility(Legacy* Fed, vector<char>& connectors, vector<string>& commands);

//checks input string for spaces in conjunction with #comments in other part
bool checkForSpace(const string& input) 
{
    for (unsigned i = 0; i < input.size(); i++) 
    {
        if(input.at(i) == '#') 
        {
            return true;
        }
        else if( (input.at(i) != ' ') && (input.at(i) != '#') )
        {
            return false;
        }
    }

    return true;
}

void testProtocol(string& input, Legacy* &inputs)
{
    cout << "yolo" << endl;
    return;
}

void parseInput(string &input, Legacy* &inputs)
{
    //bool parenFound = false;
    int rightPar = 0;
    int leftPar = 0;
    bool commandPush = false;
    bool leftbracketFound = false;
    bool testSignaled = false;
    bool commentFound = false;

    vector<char> connectors;
    vector<string> commands;
    unsigned begin = 0; //beginning index. this will increment as the input is parsed to create the "new" input
    
    //check if '#' entered
    for(unsigned i = 0; i < input.size() && commentFound != true; ++i)  //run until end of input or until comment is found 
    {
        //cout << "1" << endl;
        if(input.at(i) == '#') //comment found
        {
            input = input.substr(begin, i - begin); //input is now substring from beginning until comment
            commentFound = true; //set comment flag to true
        }
        else if(input.at(i) == '(')
        {
            leftPar++;
        }
        else if(input.at(i) == ')')
        {
            rightPar++;
        }
    }
    
    string errmsg = "Error. Please check parentheses.";
    if(leftPar != rightPar)
    {
        throw errmsg;
    }
    
    //check if '[' entered
    for(unsigned i = 0; i < input.size() && leftbracketFound != true; ++i) //run until end of input or left bracket is found
    {
        //cout << "2" << endl;
        if(input.at(i) == '[') //bracket found
        {
            input.erase(0, 2);   //erase
            leftbracketFound = true;   //set bracket flag to true
        }
    }//END FOR
    
    
    
    
    // gets rid of whitespace
    if(input.at(input.size() - 1) == ' ') //if end of input is a whitespace
    {
        //cout << "3" << endl;
        unsigned i = input.size() - 1; //set i = last index of input
        
        while(input.at(i) == ' ')   //while last index of input is whitespace
        {
            i--;  //set new index of i to not have whitespace
        }
        
        input = input.substr(0, i + 1);   //new input is now input without whitespaces (takes on i index)
    }//END IF
    
    for(unsigned i = 0; i < input.size(); ++i)  //run through whle input
    {
        //cout << "4" << endl;
        if(commandPush) //if commandPush flag is true
        {
            //cout << "5" << endl;
            while(input.at(i) == ' ' && i < input.size())   //while input @ index i is whitespace and i < inputsize
            {
                if(i != input.size() - 1) //while i is not inputsize
                {
                    i++;   //increment whole array to make "new" input
                    begin++;  //by incrementing i and begin
                }
            }
            commandPush = false; //set commandPush flag to false
        }
        
        //check for semicolon
        if(input.at(i) == ';')   //if semicolon
        {
            //cout << "6" << endl;
            connectors.push_back(input.at(i));  //pushback ';' connector
            commands.push_back(input.substr(begin, i - begin));   //pushback commands using index i
            begin = i + 1; //set new being
            commandPush = true;  //command was pushed
        }
        
        //check for ending brackets
        //leftbracketFound is set above so need only check for right 
        else if(input.at(i) == ']' && leftbracketFound)  //if input at i is ']' and leftbracket exists              
        {
            //cout << "7" << endl;
            commands.push_back(input.substr(begin, i - begin));   //pushback input from begin to i - begin
            begin = i + 3; //increment begin to make "new" input
            testSignaled = true; //set testSignaled flag to true
            commandPush = true;  //set commandPush flag to true
        }  
        
        //check for ending parantheses
        //left paren is set above so need only check for right
        else if(input.at(i) == '(')
        {
            connectors.push_back(input.at(i));
            begin = i + 1;
        }
        else if(input.at(i) == ')')
        {
            connectors.push_back(input.at(i));
            
            if(input.at(begin) == '|' || input.at(begin) == '&')
            {
                begin = begin + 2;
            }
            
            string parenthCmds = input.substr(begin, i - begin);
            
            if(parenthCmds != "")
            {
                commands.push_back(parenthCmds);
                commandPush = true;
                begin = i + 1;
            }
        }
        
        //check for connecter '&'                    
        else if(input.at(i) == '&' && i < (input.size() - 1)) //if '&' is found and there is still some of input to parse
        {
            //cout << "8" << endl;
            if(input.at(i + 1) == '&')  //check for the second "&" ex. "&&" instead of just '&'
            {  
                //cout << "9" << endl;
                connectors.push_back(input.at(i)); //push back connector '&'
                commands.push_back(input.substr(begin, i - begin));  //pushback commands from begin to i - begin
                begin = i + 2;   //increment begin by 2 to make up for "&&"
                commandPush = true; //set commandPush to true
            }
        }
        
        //check for connector '|'
        else if(input.at(i) == '|' && i < (input.size() - 1))  //if '|' is found and there is still some of input to parse
        {
            //cout << "10" << endl;
            if(input.at(i + 1) == '|') //if second '|' exists
            {
                //cout << "11" << endl;
                connectors.push_back(input.at(i)); //push back command at index i
                string orCmd = input.substr(begin, i - begin);

                // commands.push_back(input.substr(begin, i - begin));  //pushback commands from begin to i - begin
                
                if (orCmd != "") 
                {
                    begin = i + 2;   //increment begin by 2 to make up for "||"
                    commandPush = true; //set commandPush flag to true
                    commands.push_back(orCmd);
                }
            }
        }
        //checking for pipe
        else if (input.at(i) == '|') {
            // push 'p' for pipe since or connector is taken already
            connectors.push_back('p');
            string pipeCmd = input.substr(begin, i - begin);
            if (pipeCmd != "") {
                begin = i + 1;
                commands.push_back(pipeCmd);
            }
        }
        // checks for the input redirection < connector
        else if (input.at(i) == '<') {
            connectors.push_back(input.at(i));
            string lefty = input.substr(begin, i - begin);
            if (lefty != "") {
                begin = i + 1;
                commands.push_back(lefty);
            }
        }
        // checks for the output redirection > connector and the >> connector
        else if (input.at(i) == '>') {
            // for the >> connector
            if (input.at(i + 1) == '>') {
                // a represents the >> connector
                connectors.push_back('a');
                string appendCmd = input.substr(begin, i - begin);
                if (appendCmd != "") {
                    begin = i + 2;
                    commands.push_back(appendCmd);
                }
            }
            // for the > connector
            // fixed the double read in with > and >>
            else if (input.at(i - 1) != '>') {
                connectors.push_back(input.at(i));
                string righty = input.substr(begin, i - begin);
                if (righty != "") {
                    begin = i + 1;
                    commands.push_back(righty);
                    //commandPushed = true;
                }
            }
            
        }


        //if connectors, commands are not empty, and no more input to parse, and last char is NOT ';', and testSignaled is false
        else if(!connectors.empty() && !commands.empty() && i == input.size() - 1 && input.at(input.size() - 1) != ';' && !testSignaled) 
        {
            //cout << "12" << endl;
            if (input.at(begin) == '|') {
                begin = begin + 2;
            }
            if (input.at(begin) == '&') {
                begin = begin + 2;
            }

            commands.push_back(input.substr(begin, input.size()));   //pushback the whole input. input has no connectors, simply one command
        }
    }//END FOR
    
    
    //last element of connectors is not empty and ';'
    if(!connectors.empty() && (connectors.back() == ';' || connectors.back() == ')') && commands.back() == "")
    {
        //cout << "13" << endl;
        connectors.pop_back();   //get rid of ';' in connectors
        commands.pop_back();  //get rid of last element in commands
    }//END IF
    
    
    
    

    //if connectors has nothing and testSignaled is false
    if(connectors.size() == 0 && !testSignaled) //single command
    {
        //cout << "14" << endl;
        commands.push_back(input.substr(begin, input.size() - begin));  //pushback single command
    }// END IF
    
    makeTree(inputs, connectors, commands, testSignaled);   //make tree with executes
    
    return;
}

void makeTree(Legacy*& inputs, vector<char>& connectors, vector<string>& commands, bool testSignaled)
{
    
    for(unsigned i = 0; i < commands.size(); i++) //runs through commands and removes empty commands
    {  
        if(commands.at(i) == "") //if empty command
        {
            commands.erase(commands.begin() + i - 1); //erase empty commands
        }
    }
    
    if((commands.size() == 1) && testSignaled)  //if command has only one element, and testsignaled = true
    {
        Legacy* in = new Test(commands.at(0));  //make new Test object
        inputs = in; //inputs 
        in = 0; //set in to 0
        return;
    }
    
    else if(commands.size() == 1) //if single command
    {
        Legacy* in = new CMD(commands.at(0));
        inputs = in;
        in = 0;   //reset in
        return;
    }

    Legacy* Fed = NULL;
    
    inputs = treeUtility(Fed, connectors, commands); //create executables
    
    while(!connectors.empty())
    {
        Fed = inputs;
        inputs = treeUtility(Fed, connectors, commands);
    }

    return;
    
}


Legacy* treeUtility(Legacy* Fed, vector<char>& connectors, vector<string>& commands) 
{
    if(commands.size() == 1 && connectors.empty())  // Base Legacy case, returns a Command
    { 
        // cout << "Legacy" << endl;
        return new CMD(commands.at(0));
    }

    if (connectors.back() == ';')   //if last element is ';'
    {
        connectors.pop_back();  //delete ';'
        Semicolon* connector = new Semicolon(); //create new Semicolon object

        if (Fed == NULL) 
        {
            connector->setrightChild(new CMD(commands.back()));
            // cout << "setright after" << endl;
            commands.pop_back();
        }
        else 
        {
            connector->setrightChild(Fed);
            Fed = NULL;
        }
        
        // cout << "setLeft before" << endl;
        connector->setleftChild(treeUtility(Fed, connectors, commands));
        // cout << "setLeft after" << endl;
        
        
        Fed = connector;
        // returns top node
        return Fed;
    }
        
        // connector->setrightChild(new CMD(commands.back())); //simultaneously create CMD using last element of commands and set it as connector's right child
        // commands.pop_back();  //delete commands last element
        // connector->setleftChild(treeUtility(Fed, connectors, commands)); //recursively call treeUtility using updated connectors and commands

        // return connector;  //input = connector
    // }

    if (connectors.back() == '&')   //if last element of connectors is '&', create AND object
    {
        connectors.pop_back();   //delete last object, in this case '&'. this is possible because there are two '&'
        AND* connector = new AND(); //creat new AND object
        
        if(Fed == NULL) 
        {
            connector->setrightChild(new CMD(commands.back()));
            commands.pop_back();
        }
        else
        {
            connector->setrightChild(Fed);
            Fed = NULL;
        }
        
        connector->setleftChild(treeUtility(Fed, connectors, commands));
        Fed = connector;
        
        // returns top node
        return Fed;

        // connector->setrightChild(new CMD(commands.back())); //simultaneously create new CMD of last element of commands and set it as connector's right child
        // commands.pop_back();  //delete commands last element
        // connector->setleftChild(treeUtility(Fed, connectors, commands)); //recursively call treeUtility using updated connectors and commands

        // return connector; //input = connector
    }

    if (connectors.back() == '|') //if last element is '|'
    {
        connectors.pop_back();   //delete last element of connectors
        OR* connector = new OR();   //create new OR object

        if (Fed == NULL) 
        {
            connector->setRightChild(new CMD(commands.back()));
            commands.pop_back();
        }
        else 
        {
            connector->setRightChild(Fed);
            Fed = NULL;
        }
        
        connector->setLeftChild(treeUtility(Fed, connectors, commands));
        Fed = connector;
        
        // returns top node
        return Fed;
        // connector->setRightChild(new CMD(commands.back())); //simultaneously create CMD using last element of commands and set it as connector's right child
        // commands.pop_back();  //delete commands last element 

        // connector->setLeftChild(treeUtility(Fed, connectors, commands)); //recursively call treeUtility using updated connectors and commands
        
        // return connector;  //input = connector
    }
    
    if(connectors.back() == ')')
    {
        stack<string> cmdStack;
        stack<char> conStack;
        
        vector<char> copyConnector;
        
        vector<string> copyCommand;
        
        int count = 1;
        
        connectors.pop_back();
        
        if(connectors.back() != ')')
        {
            cmdStack.push(commands.back());
            commands.pop_back();
        }
        
        while(count != 0)
        {
            if(connectors.back() == '(')
            {
                count--;
                conStack.push(connectors.back());
                connectors.pop_back();
            }
            else if(connectors.back() == ')')
            {
                count++;
                conStack.push(connectors.back());
                connectors.pop_back();
                if(connectors.back() != ')')
                {
                    cmdStack.push(commands.back());
                    commands.pop_back();
                }
            }
            else
            {
                conStack.push(connectors.back());
                connectors.pop_back();
                if(connectors.back() != ')')
                {
                    cmdStack.push(commands.back());
                    commands.pop_back();
                }
            }
        }
        
        conStack.pop();
        
        int conStackSize = conStack.size();
        int cmdStackSize = cmdStack.size();
        
        for(int i = 0; i < conStackSize; i++)
        {
            copyConnector.push_back(conStack.top());
            conStack.pop();
        }
        
        for(int i = 0; i < cmdStackSize; i++)
        {
            copyCommand.push_back(cmdStack.top());
            cmdStack.pop();
        }
        
        
        //finally create object
        Legacy* par = new Parenth(treeUtility(Fed, copyConnector, copyCommand));
        
        Fed = par;

        while (!connectors.empty()) 
        {
            Fed = treeUtility(Fed, connectors, commands);
        }
        
        return Fed;
        // return par;
    }

        // cout << "< in progress" << endl;
    if (connectors.back() == '<') 
    {
        Lt* lel = new Lt();

        lel->setRight(commands.back());
        commands.pop_back();
        connectors.pop_back();

        lel->setLeft(treeUtility(Fed, connectors, commands));

        Fed = lel;
        return Fed;
    }
    // cout << "> in progress"  << endl;
    if (connectors.back() == '>') 
    {
        Rt* red = new Rt();

        red->setRight(commands.back());
        commands.pop_back();
        connectors.pop_back();

        red->setLeft(treeUtility(Fed, connectors, commands));

        Fed = red;
        return Fed;
    }
    // cout << ">> in progress" << endl;
    if (connectors.back() == 'a') 
    {
        // cout << "READ APPPEND" << endl;
        DubRt* app = new DubRt();

        app->setRight(commands.back());
        commands.pop_back();
        connectors.pop_back();

        app->setLeft(treeUtility(Fed, connectors, commands));

        Fed = app;
        return Fed;
    }
    // cout << "| in progress" << endl;
    if (connectors.back() == 'p') 
    {
        Pipe* piper = new Pipe();

        piper->setRight(commands.back());
        commands.pop_back();
        connectors.pop_back();

        piper->setLeft(treeUtility(Fed, connectors, commands));

        Fed = piper;
        return Fed;
    }
    
    return 0;
}



int main()
{
    string entireLine;  //initialize string for input

    Legacy* inputs = 0;
    
    for ( ; ; )         //continute until...
    {   
        cout << "$ ";   //bash $
        getline(cin, entireLine);   //get user input
        
        if((entireLine == "") || (checkForSpace(entireLine))) 
        {
            continue;
        }
        try 
        {
            parseInput(entireLine, inputs); //parse
            inputs->execute();
        }
        catch(string s) 
        {
            cout << s << endl;
            continue;
        }
        
    }
    
    return 0;
}