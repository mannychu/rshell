#include <iostream>

#include "Connect.h"
#include "Legacy.h"
#include "Test.h"

//Empty Constructor
Connector::Connector() {}

//Constructor
Connector::Connector(Legacy* leftChild, Legacy* rightChild) 
{
    this->leftChild = leftChild;
    this->rightChild = rightChild;
}
