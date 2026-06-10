#ifndef REALTIMESTAMPCOUNTER_H
#define REALTIMESTAMPCOUNTER_H

#pragma once
#include "zftools_api.h"

#include <memory.h>


#ifndef WIN64

//Declare Real Time Stamp Counter Related Information
#define CPUID	__asm __emit 0fh __asm __emit 0a2h
#define RDTSC	__asm __emit 0fh __asm __emit 031h

#define RTSC_SUPPORT

class ZFTOOLS_API RealTimeStampCounter
{
	unsigned MHZ;
	unsigned Base;
	unsigned StartCyclesHigh;
	unsigned StartCyclesLow;
	unsigned EndCyclesHigh;
	unsigned EndCyclesLow;
	__int64	 ElapsedCycles;

public:

	RealTimeStampCounter(unsigned int MachineSpeedInMHZ=100);

	inline void start()
	{
		GetTimeStamp(&StartCyclesHigh,&StartCyclesLow);
	}

	inline void StopCounter()
	{
		GetTimeStamp(&EndCyclesHigh,&EndCyclesLow);

		unsigned __int64 TempCycles1=0, TempCycles2=0;

		TempCycles1 = ((unsigned __int64)StartCyclesHigh <<32) | StartCyclesLow;
		TempCycles2 = ((unsigned __int64)  EndCyclesHigh <<32) | EndCyclesLow;

		ElapsedCycles = TempCycles2 - TempCycles1 - Base;
	}

	inline __int64 elapsed()
	{
		return ElapsedCycles;
	}

	inline double GetElapsedSeconds()
	{
		return ((double)(ElapsedCycles)/(double)(MHZ * 1000000));
	}

private:
	inline void GetTimeStamp(unsigned *pCyclesHigh,unsigned *pCyclesLow)
	{
		__asm
		{
			pushad	;
			CPUID	;
			RDTSC	;
			mov ebx, pCyclesHigh;
			mov	[ebx], edx;
			mov ebx, pCyclesLow;
			mov	[ebx], eax;
			popad	;
		}
	}

	inline unsigned FindBase()
	{
		unsigned Base,BaseExtra=0;
		unsigned CyclesLow,CyclesHigh;
		__asm
		{
			pushad;
			CPUID;
			RDTSC;
			mov	CyclesHigh, edx;
			mov CyclesLow, eax;
			popad;
			pushad;
			CPUID;
			RDTSC;
			popad;

			pushad;
			CPUID;
			RDTSC;
			mov	CyclesHigh, edx;
			mov CyclesLow, eax;
			popad;
			pushad;
			CPUID;
			RDTSC;
			popad;

			pushad;
			CPUID;
			RDTSC;
			mov	CyclesHigh, edx;
			mov CyclesLow, eax;
			popad;
			pushad;
			CPUID;
			RDTSC;
			sub eax, CyclesLow;
			mov BaseExtra, eax;
			popad;

			pushad;
			CPUID;
			RDTSC;
			mov	CyclesHigh, edx;
			mov CyclesLow, eax;
			popad;
			pushad;
			CPUID;
			RDTSC;
			sub eax, CyclesLow;
			mov Base, eax;
			popad;
		}

		if(BaseExtra <  Base)
			Base = BaseExtra;

		return Base;
	}
};

#endif //WIN64
#endif //REALTIMESTAMPCOUNTER_H
