// Bpn.cpp: implementation of the CBpn class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "Bpn.h"

//#include <windows.h>
//#include <crtdbg.h>


/******************************************************************************
        R A N D O M S   D R A W N   F R O M   D I S T R I B U T I O N S
 ******************************************************************************/
int RandomEqualINT(int Low, int High)
{
	return rand() % (High-Low+1) + Low;
}      
double RandomEqualREAL(double Low, double High)
{
	return ((double) rand() / RAND_MAX) * (High-Low) + Low;
}      


/******************************************************************************
               A P P L I C A T I O N - S P E C I F I C   C O D E
 ******************************************************************************/




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBpn::CBpn (int nInputUnits, int nHiddenLayers, int nHiddenUnits, int nOutputUnits) 
{
	if(MAX_LAYERS<=nHiddenLayers+2) ASSERT(0);

	m_Units[0] = nInputUnits;						// ÀÔ·Â À¯´Ö °¹¼ö.
	for(int i=1;i<=nHiddenLayers;i++) 
		m_Units[i] = nHiddenUnits;					// È÷µç ·¹ÀÌ¾îÀÇ À¯´Ö °¹¼ö.
	m_Units[nHiddenLayers+1] = nOutputUnits;		// Ãâ·Â ·¹ÀÌ¾îÀÇ À¯´Ö °¹¼ö.


	m_nLayers = nHiddenLayers+2;
	m_nInputUnits = nInputUnits;
	m_nOutputUnits = nOutputUnits;

	m_dblTestError = 0;

	srand(4711);		// INITIALIZE RANDOMS
	GenerateNetwork();

	RandomWeights();			
	InitializeApplication();

}

CBpn::~CBpn()
{
	FinalizeApplication();
}

void CBpn::InitializeApplication()
{
	m_net.Alpha = 0.5;
	m_net.Eta   = 0.05;
	m_net.Gain  = 1;
}


void CBpn::FinalizeApplication()
{
	int l,i;
	for (l=0; l<m_nLayers; l++) {
		if (l != 0) {
			for (i=1; i<=m_Units[l]; i++) {
				free(m_net.Layer[l]->Weight[i]);
				free(m_net.Layer[l]->WeightSave[i]);
				free(m_net.Layer[l]->dWeight[i]);
			}
		}
		free(m_net.Layer[l]->Output    );
		free(m_net.Layer[l]->Error     );
		free(m_net.Layer[l]->Weight    );
		free(m_net.Layer[l]->WeightSave);
		free(m_net.Layer[l]->dWeight   );
		free(m_net.Layer[l]);
	}
	free(m_net.Layer);
}


void CBpn::GenerateNetwork()
{
	int l,i;

	m_net.Layer = (LAYER**) calloc(m_nLayers, sizeof(LAYER*));

	for (l=0; l<m_nLayers; l++) {
		m_net.Layer[l] = (LAYER*) malloc(sizeof(LAYER));

		m_net.Layer[l]->Units      = m_Units[l];
		m_net.Layer[l]->Output     = (double*)  calloc(m_Units[l]+1, sizeof(double));
		m_net.Layer[l]->Error      = (double*)  calloc(m_Units[l]+1, sizeof(double));
		m_net.Layer[l]->Weight     = (double**) calloc(m_Units[l]+1, sizeof(double*));
		m_net.Layer[l]->WeightSave = (double**) calloc(m_Units[l]+1, sizeof(double*));
		m_net.Layer[l]->dWeight    = (double**) calloc(m_Units[l]+1, sizeof(double*));
		m_net.Layer[l]->Output[0]  = BIAS;
      
		if (l != 0) {
			for (i=1; i<=m_Units[l]; i++) {
				m_net.Layer[l]->Weight[i]     = (double*) calloc(m_Units[l-1]+1, sizeof(double));
				m_net.Layer[l]->WeightSave[i] = (double*) calloc(m_Units[l-1]+1, sizeof(double));
				m_net.Layer[l]->dWeight[i]    = (double*) calloc(m_Units[l-1]+1, sizeof(double));
			}
		}
	}
	m_net.InputLayer  = m_net.Layer[0];
	m_net.OutputLayer = m_net.Layer[m_nLayers - 1];
	m_net.Alpha       = 0.9;
	m_net.Eta         = 0.25;
	m_net.Gain        = 1;
}



void CBpn::RandomWeights()
{
	int l,i,j;

	for (l=1; l<m_nLayers; l++) {
		for (i=1; i<=m_net.Layer[l]->Units; i++) {
			for (j=0; j<=m_net.Layer[l-1]->Units; j++) {
				m_net.Layer[l]->Weight[i][j] = RandomEqualREAL(-0.5, 0.5);
			}
		}
	}
}



void CBpn::SetInput(double * Input)
{
	int i;

	for (i=1; i<=m_net.InputLayer->Units; i++) {
		m_net.InputLayer->Output[i] = Input[i-1];
	}
}


void CBpn::GetOutput(double * Output)
{
	int i;

	for (i=1; i<=m_net.OutputLayer->Units; i++) {
		Output[i-1] = m_net.OutputLayer->Output[i];
	}
}


void CBpn::SaveWeights()
{
	int l,i,j;

	for (l=1; l<m_nLayers; l++) {
		for (i=1; i<=m_net.Layer[l]->Units; i++) {
			for (j=0; j<=m_net.Layer[l-1]->Units; j++) {
				m_net.Layer[l]->WeightSave[i][j] = m_net.Layer[l]->Weight[i][j];
			}
		}
	}
}



void CBpn::RestoreWeights()
{
	int l,i,j;

	for (l=1; l<m_nLayers; l++) {
		for (i=1; i<=m_net.Layer[l]->Units; i++) {
			for (j=0; j<=m_net.Layer[l-1]->Units; j++) {
				m_net.Layer[l]->Weight[i][j] = m_net.Layer[l]->WeightSave[i][j];
			}
		}
	}
}

/******************************************************************************
                     P R O P A G A T I N G   S I G N A L S
 ******************************************************************************/


void CBpn::PropagateLayer(LAYER * Lower, LAYER * Upper)
{
	int  i,j;
	double Sum;

	for (i=1; i<=Upper->Units; i++) {
		Sum = 0;
		for (j=0; j<=Lower->Units; j++) {
			Sum += Upper->Weight[i][j] * Lower->Output[j];
		}
		Upper->Output[i] = 1 / (1 + exp(-m_net.Gain * Sum));
	}
}



void CBpn::PropagateNet()
{
	int l;

	for (l=0; l<m_nLayers-1; l++) {
		PropagateLayer(m_net.Layer[l], m_net.Layer[l+1]);
	}
}


/******************************************************************************
                  B A C K P R O P A G A T I N G   E R R O R S
 ******************************************************************************/


void CBpn::ComputeOutputError(double * Target)
{
	int  i;
	double Out, Err;

	m_net.Error = 0;
	for (i=1; i<=m_net.OutputLayer->Units; i++) {
		Out = m_net.OutputLayer->Output[i];
		Err = Target[i-1]-Out;
		m_net.OutputLayer->Error[i] = m_net.Gain * Out * (1-Out) * Err;
		m_net.Error += 0.5 * sqr(Err);
	}
}



void CBpn::BackpropagateLayer(LAYER * Upper, LAYER * Lower)
{
	int  i,j;
	double Out, Err;

	for (i=1; i<=Lower->Units; i++) {
		Out = Lower->Output[i];
		Err = 0;
		for (j=1; j<=Upper->Units; j++) {
			Err += Upper->Weight[j][i] * Upper->Error[j];
		}
		Lower->Error[i] = m_net.Gain * Out * (1-Out) * Err;
	}
}



void CBpn::BackpropagateNet()
{
	int l;

	for (l=m_nLayers-1; l>1; l--) {
		BackpropagateLayer(m_net.Layer[l], m_net.Layer[l-1]);
	}
}


void CBpn::AdjustWeights()
{
	int  l,i,j;
	double Out, Err, dWeight;

	for (l=1; l<m_nLayers; l++) {
		for (i=1; i<=m_net.Layer[l]->Units; i++) {
			for (j=0; j<=m_net.Layer[l-1]->Units; j++) {
				Out = m_net.Layer[l-1]->Output[j];
				Err = m_net.Layer[l]->Error[i];
				dWeight = m_net.Layer[l]->dWeight[i][j];
				m_net.Layer[l]->Weight[i][j] += m_net.Eta * Err * Out + m_net.Alpha * dWeight;
				m_net.Layer[l]->dWeight[i][j] = m_net.Eta * Err * Out;
			}
		}
	}
}


/******************************************************************************
                      S I M U L A T I N G   T H E   N E T
 ******************************************************************************/


void CBpn::SimulateNet(double * Input, double * Output, double * Target, bool Training)
{
	SetInput(Input);
	PropagateNet();
	GetOutput(Output);

	ComputeOutputError(Target);
	if (Training) {
		BackpropagateNet();
		AdjustWeights();
	}
}





void CBpn::TrainNet(CDblArray &arrIn, CDblArray &arrOut, CDblArray &arrTarget ,int nTrainNum)
{
	if(arrIn.GetSize()!=m_nInputUnits || arrOut.GetSize()!=m_nOutputUnits || arrTarget.GetSize()!=m_nOutputUnits ) 
		ASSERT(0);

	int i;
	for(i=0;i<arrIn.GetSize();i++) {
		if(arrIn[i]>1||arrIn[i]<0) ASSERT(0);
	}
	for(i=0;i<arrTarget.GetSize();i++) {
		if(arrTarget[i]>1||arrTarget[i]<0) ASSERT(0);
	}

	int    n;

	for (n=0; n<nTrainNum; n++) { 
		SimulateNet(&arrIn[0],&arrOut[0],&arrTarget[0],true);
	}
}

void CBpn::TestNet (CDblArray &arrIn, CDblArray &arrOut, CDblArray &arrTarget)
{
	if(arrIn.GetSize()!=m_nInputUnits || arrOut.GetSize()!=m_nOutputUnits || arrTarget.GetSize()!=m_nOutputUnits ) 
		ASSERT(0);

	SimulateNet(&arrIn[0],&arrOut[0],&arrTarget[0],false);

	printf("Input:%f Target:%f Output:%f Diff:%f Error is %f \n",
		arrIn[0], arrTarget[0],arrOut[0],fabs(arrTarget[0]-arrOut[0]),
		m_net.Error);
}

double CBpn::GetTestError()
{
	return m_net.Error;
//	return m_dblTestError;
}

void CBpn::PrintNet()
{
	int i,j,l;
	FILE	*out;
	out = fopen("NET.dat","w");
	if(out==NULL) return;

	for (l=1; l<m_nLayers; l++) {
		for (i=1; i<=m_net.Layer[l]->Units; i++) {
			for (j=0; j<=m_net.Layer[l-1]->Units; j++) {
				fprintf(out,"%f\t",m_net.Layer[l]->Weight[i][j]);
			}
			fprintf(out,"\n");
		}
		fprintf(out,"\n");
		fprintf(out,"\n");
	}
	fclose(out);
}

void CBpn::SaveToFile(const char * szFileName)
{
	int i,j,l;
	FILE	*out;
	out = fopen(szFileName,"w");
	if(out==NULL) return;

	for (l=1; l<m_nLayers; l++) {
		for (i=1; i<=m_net.Layer[l]->Units; i++) {
			for (j=0; j<=m_net.Layer[l-1]->Units; j++) {
				fprintf(out,"%10.20f\t",m_net.Layer[l]->Weight[i][j]);
//				fprintf(out,"%f\t",m_net.Layer[l]->WeightSave[i][j]);
//				fprintf(out,"%lf\t",m_net.Layer[l]->dWeight[i][j]);
			}
			fprintf(out,"\n");
		}
		fprintf(out,"\n");
	}
	fclose(out);
}

bool CBpn::LoadFromFile(const char * szFileName)
{
	int i,j,l;
	int ret;
	FILE	*in;
	in = fopen(szFileName,"r");
	if(in==NULL) return false;

	double f;

	for (l=1; l<m_nLayers; l++) {
		for (i=1; i<=m_net.Layer[l]->Units; i++) {
			for (j=0; j<=m_net.Layer[l-1]->Units; j++) {
				ret = fscanf(in,"%lf",&f);
				if(ret==0||ret==EOF) {
					fclose(in);
					return false;
				}
				m_net.Layer[l]->Weight[i][j] = f;
//				fscanf(in,"%f",&f);
//				m_net.Layer[l]->dWeight[i][j] = f;
			}
		}
	}
	fclose(in);
	return true;
}
