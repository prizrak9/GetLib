// GetLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include "getlib.h"

using namespace getlib;


int main()
{
	get.end = get.Return;
	get.HasPointMaxNumberCountLimit = true;
	get.afterPointMaxNumberCount = 0;
	get.CanEscape = true;
	get.type = Get::enterType::Double;

	char* str;
	double d;
	get >> d;
	
	//printf(d);

    return 0;
}

