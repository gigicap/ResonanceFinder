#include <iostream>
#include <iomanip>
#include <math.h>
#include <vector>

// Root includes 
#include "TAxis.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TStyle.h"
#include "TPad.h"
#include "TROOT.h"
#include "TColor.h"
#include "TGFrame.h"
#include "TVirtualPad.h"
#include "TFile.h"
#include "TH1D.h"
#include "TRandom.h"
#include "TF1.h"
#include "TLegend.h"
#include "TLine.h"

#include "ResonanceSimulator.h"



using namespace std;

void ResonanceFinder(bool IsSimulation, bool IsVerbose){

int N_channels = 0;
int N_scans = 0;

if (IsVerbose) std::cout<<"---Start resonance finder"<<std::endl;

std::vector<std::vector<double> > ComptonSpectra;
std::vector<double> NRScounts;

if(!IsSimulation){
	std::cout<<"WORK in progress. Up to now only simulations available. Sorry!"<<std::cout;
	return;
}

else{
	bool use_bkg_file = false;

	if (IsVerbose) std::cout<<"---Initialising simulator"<<std::endl;
	ResonanceSimulator *theSimulator = new ResonanceSimulator(IsVerbose, use_bkg_file);
	if (IsVerbose) std::cout<<"---Running the simulator"<<std::endl;
	theSimulator->RunTheSimulator();

	if (IsVerbose) std::cout<<"---Getting outputs"<<std::endl;

	ComptonSpectra = theSimulator->GetSpectra();
	NRScounts = theSimulator->GetNRSCounts();
	N_channels = theSimulator->ADC_channels;
	delete theSimulator;
}

if (IsVerbose) std::cout<<"---->Ready"<<std::endl;

N_scans = NRScounts.size();

for (size_t i = 0; i < N_scans; ++i) {
	std::cout<<"Scan "<<i<<" - NRS = \t"<<NRScounts[i]<<std::endl;
}

double scan_matrix[N_scans][N_channels];
double error_matrix[N_scans][N_channels];


for (int i = 0; i < N_scans; i++){
	for(size_t j = 0; j<N_channels; j++){
		if (NRScounts[i] == 0){
		scan_matrix[i][j] = 0;
		error_matrix[i][j] = 0;
		}
		else{
		scan_matrix[i][j] = ComptonSpectra[i][j]/(NRScounts[i]);
		error_matrix[i][j] = scan_matrix[i][j]*(1/sqrt(ComptonSpectra[i][j]) + 1/sqrt(NRScounts[i]));
		}
	}
}

double delta[N_channels];
for (int j = 0; j < N_channels; ++j){
	double minval = 10e22;
	double maxval = -10e22;

	for (int i = 0; i < N_scans; ++i){
		if(scan_matrix[i][j] < minval) minval = scan_matrix[i][j];
		if(scan_matrix[i][j] > maxval) maxval = scan_matrix[i][j];
	}
	delta[j] = maxval - minval;
}

TCanvas *can2 = new TCanvas("can2","",1200,500);
can2->Divide(2,1);
can2->cd(1);
int n = N_channels;
double x[n];
for (int i = 0; i < n; ++i) x[i] = (double)i;
TGraph *gr = new TGraph(n,x,delta);
gr->SetTitle("Resonance finder function");
gr->GetXaxis()->SetTitle("ADC channel");
gr->GetYaxis()->SetTitle("max - min");
gr->Draw("ACP");

can2->cd(2);
TGraph *grS[N_scans];

for (int i = 0; i < N_scans; ++i){

double sm[N_channels];
for (int j = 0; j < N_channels; ++j)
{
sm[j] = scan_matrix[i][j];
}
grS[i] = new TGraph(n,x,sm);
}

grS[0]->SetTitle("scan functions");
grS[0]->GetXaxis()->SetTitle("ADC channel");
grS[0]->Draw("ACP");
for (int i = 1; i < N_scans; ++i) grS[i]->Draw("CP");



return;	
}


int main(int argc, char **argv){
   //check for a correct usage
   if(argc != 3){
   	std::cout<<"Usage of ResonanceFinder: ./ResonanceFinder IsSimulation IsVerbose"<<std::endl;
	std::cout<<"IsSimulation = 0 --> Use with data files / else run a simulator"<<std::endl;
	std::cout<<"IsVerbose = 0 --> No plots produced"<<std::endl;
   	return 1;
   }

   TApplication theApp("App", &argc, argv);

   bool IsSimulation = true;
   bool IsVerbose = true;

   if(strcmp(argv[1],"0")==0) IsSimulation = false;
   if(strcmp(argv[2],"0")==0) IsVerbose = false;

   ResonanceFinder(IsSimulation, IsVerbose);
   theApp.Run();

   gApplication->Terminate(0);
   exit(1);

   return 0;
}