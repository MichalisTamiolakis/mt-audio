/*
  Tda7313.cpp - Library for TDA7313.
  Created by Kolesnikov Anton, 19.11.2016.
*/

#include "Tda7313.h"

Tda7313::Tda7313(){	
	Wire.begin();
	Tda7313address = 0x88;
}
 
Tda7313::Tda7313(int address){	
	Wire.begin();
	Tda7313address = address;
}


void Tda7313::sla(int arg){
	switch(arg){
		case 0:
			iSelector |= (1<<3);
			iSelector |= (1<<4);
			_sla[iInput-1] = arg;
		break;
		case 1:
			iSelector &= ~(1<<3);
			iSelector |= (1<<4);
			_sla[iInput-1] = arg;
		break;
		case 2:
			iSelector |= (1<<3);
			iSelector &= ~(1<<4);
			_sla[iInput-1] = arg;
		break;
		case 3:
			iSelector &= ~(1<<3);
			iSelector &= ~(1<<4);
			_sla[iInput-1] = arg;
		break;
	}
	
	Wire.beginTransmission(Tda7313address);
	Wire.write(iSelector);
	Wire.endTransmission();
}

void Tda7313::sla(int val, int input)
{
	if(val < 0 || val > 3 || input < 1 || input > 3) return;
	// Update realtime val
	if(input == iInput)
	{
		Tda7313::sla(val);
	}

	// Just store value for next time
	else
	{
		_sla[input-1] = val;
	}
}

void Tda7313::loud(bool loudEnabled){
	if(loudEnabled) iSelector &= ~(1<<2); else iSelector |= (1<<2);
	_loud = loudEnabled;
	Wire.beginTransmission(Tda7313address);
	Wire.write(iSelector);
	Wire.endTransmission();
}

void Tda7313::input(int arg){
	switch(arg){
		case 1:
			iSelector &= ~(1<<0);
			iSelector &= ~(1<<1);
			iInput = arg;
		break;
		case 2:
			iSelector |= (1<<0);
			iSelector &= ~(1<<1);
			iInput = arg;
		break;
		case 3:
			iSelector &= ~(1<<0);
			iSelector |= (1<<1);
			iInput = arg;
		break;
		default:
			return;
	}
	
	Wire.beginTransmission(Tda7313address);
	Wire.write(iSelector);
	Wire.endTransmission();
	
	Tda7313::sla(_sla[arg-1]);
}

void Tda7313::volume(int arg){
	if(_mute) Tda7313::mute(false);
	if((arg < 0)||(arg > 17)) return;
	iVolume = arg;
	Wire.beginTransmission(Tda7313address);
	Wire.write(VOLUME_MASK[iVolume]);
	Wire.endTransmission(); 
}

void Tda7313::mute(bool muteEnabled){
	if(muteEnabled){
		_mute = true;
		Wire.beginTransmission(Tda7313address);
		Wire.write(0x9F);
		Wire.write(0xBF);
		Wire.write(0xDF);
		Wire.write(0xFF);
		Wire.endTransmission();
	}else{
		_mute = false;
		Tda7313::attLF(iAttLF);
		Tda7313::attRF(iAttRF);
		Tda7313::attLR(iAttLR);
		Tda7313::attRR(iAttRR);
	}
}

void Tda7313::bass(int arg){
	if((arg < 0)||(arg > 14)) return;
	iBass = arg;
	Wire.beginTransmission(Tda7313address);
	Wire.write(BASS_MASK[iBass]);
	Wire.endTransmission();
}

void Tda7313::treble(int arg){
	if((arg < 0)||(arg > 14)) return;
	iTreble = arg;
	Wire.beginTransmission(Tda7313address);
	Wire.write(TREBLE_MASK[iTreble]);
	Wire.endTransmission();
}

void Tda7313::attLF(int arg){
	if((arg < 0)||(arg > 13)||(_mute)) return;
	iAttLF = arg;
	Wire.beginTransmission(Tda7313address);
	Wire.write(ATT_LF_MASK[iAttLF]);
	Wire.endTransmission();
	
}

void Tda7313::attRF(int arg){
	if((arg < 0)||(arg > 13)||(_mute)) return;
	iAttRF = arg;
	Wire.beginTransmission(Tda7313address);
	Wire.write(ATT_RF_MASK[iAttRF]);
	Wire.endTransmission();
}

void Tda7313::attLR(int arg){
	if((arg < 0)||(arg > 13)||(_mute)) return;
	iAttLR = arg;
	Wire.beginTransmission(Tda7313address);
	Wire.write(ATT_LR_MASK[iAttLR]);
	Wire.endTransmission();
}

void Tda7313::attRR(int arg){
	if((arg < 0)||(arg > 13)||(_mute)) return;
	iAttRR = arg;
	Wire.beginTransmission(Tda7313address);
	Wire.write(ATT_RR_MASK[iAttRR]);
	Wire.endTransmission();
}


int Tda7313::sla(){ 
	return _sla[iInput-1];
}

bool Tda7313::loud(){
	return _loud;
}

int Tda7313::input(){ 
	return iInput;
}

int Tda7313::volume(){
	return iVolume;
}

bool Tda7313::mute(){
	return _mute;
}

int Tda7313::bass(){
	return iBass;
}

int Tda7313::treble(){
	return iTreble;
}

int Tda7313::attLF(){
	return iAttLF;
}

int Tda7313::attRF(){
	return iAttRF;
}

int Tda7313::attLR(){
	return iAttLR;
}

int Tda7313::attRR(){
	return iAttRR;
}

void Tda7313::sync()
{
	Tda7313::volume(Tda7313::volume());
	Tda7313::treble(Tda7313::treble());
	Tda7313::attLF(Tda7313::attLF());
	Tda7313::attRF(Tda7313::attRF());
	Tda7313::attLR(Tda7313::attLR());
	Tda7313::attRR(Tda7313::attRR());
	Tda7313::input(Tda7313::input());
	Tda7313::mute(Tda7313::mute());
	Tda7313::loud(Tda7313::loud());
	Tda7313::bass(Tda7313::bass());
	Tda7313::sla(Tda7313::sla());
}
