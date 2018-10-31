// Bpn.h: interface for the CBpn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPN_H__2B4E8BA6_412C_11D2_A56F_CABD267E9030__INCLUDED_)
#define AFX_BPN_H__2B4E8BA6_412C_11D2_A56F_CABD267E9030__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <afxtempl.h>

#define MIN_REAL      -HUGE_VAL
#define MAX_REAL      +HUGE_VAL


#define sqr(x)        ((x)*(x))


struct LAYER {	                    /* A LAYER OF A NET:                     */
	int				Units;			/* - number of units in this layer       */
	double*         Output;			/* - output of ith unit                  */
	double*         Error;			/* - error term of ith unit              */
	double**        Weight;			/* - connection weights to ith unit      */
	double**        WeightSave;		/* - saved weights for stopped training  */
	double**        dWeight;		/* - last weight deltas for momentum     */
};

struct NET{							/* A NET:                                */
	LAYER**       Layer;			/* - layers of this net                  */
	LAYER*        InputLayer;		/* - input layer                         */
	LAYER*        OutputLayer;		/* - output layer                        */
	double        Alpha;			/* - momentum factor                     */
	double        Eta;				/* - learning rate                       */
	double        Gain;				/* - gain of sigmoid function            */
	double        Error;			/* - total net error                     */
} ;



typedef CArray<double,double> CDblArray;


#define MAX_LAYERS			10			// HIDDEN, IN, OUT 다포함한 갯수.

class CBpn  
{
private :
	NET			m_net;
	int			m_nLayers;
	int			m_Units[MAX_LAYERS];
	int			m_nInputUnits;
	int			m_nOutputUnits;
	enum		{BIAS=1};	
	double		m_dblTestError;

public:
	void RestoreWeights();
	void SaveWeights();

private :
	void RandomWeights();
	void InitializeApplication();
	void GenerateNetwork();
	void FinalizeApplication();
	void SimulateNet(double* Input, double* Output, double* Target, bool Training);
	void AdjustWeights();
	void BackpropagateNet();
	void BackpropagateLayer(LAYER* Upper, LAYER* Lower);
	void ComputeOutputError(double* Target);
	void PropagateNet();
	void PropagateLayer(LAYER* Lower, LAYER* Upper);
	void GetOutput(double* Output);
	void SetInput(double* Input);
public:
	bool LoadFromFile(const char* szFileName);
	void SaveToFile(const char* szFileName);
	void PrintNet();
	double GetTestError();
	void TrainNet(CDblArray &arrIn, CDblArray &arrOut, CDblArray &arrTarget ,int nTrainNum);
	void TestNet (CDblArray &arrIn, CDblArray &arrOut, CDblArray &arrTarget);
	// nInput : 입력 유닛의 갯수, nHiddenLayers : 숨겨진 레이어의 수
	// nHiddenUnits : 숨겨진 레이어의 유닛수(전부 일괄적이다.
	// nOutputUnits : 출력 유닛의 갯수.
	CBpn(int nInputUnits, int nHiddenLayers, int nHiddenUnits, int nOutputUnits);
	virtual ~CBpn();

};

#endif // !defined(AFX_BPN_H__2B4E8BA6_412C_11D2_A56F_CABD267E9030__INCLUDED_)
