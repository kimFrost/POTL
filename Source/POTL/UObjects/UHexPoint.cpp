// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UHexPoint.h"




UHexPoint::UHexPoint()
{
	Location = { -1, -1, -1 };
	Column = 0;
	Row = 0;
	IsCreator = false;
	Exits = false;
	Hex0Index = -1;
	Hex1Index = -1;
	Hex2Index = -1;
}


UHexPoint::~UHexPoint()
{

}