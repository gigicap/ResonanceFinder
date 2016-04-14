#include <iostream>
#include <iomanip>
#include <math.h>
#include <vector>

// Root includes 
#include "TAxis.h"
#include "TGraph.h"
#include "TGraphErrors.h"
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


void find_minima(int, double*, std::vector<int>*);

void ResonanceFinder(bool IsSimulation, bool IsCompton, bool IsVerbose, TFile *f_output){

int N_channels = 0;
int N_scans = 0;

//double NRS_CUT = 1000.0;
double NRS_CUT = 1000.0;
double tail_CUT = 0.05;


if (IsVerbose) std::cout<<"---Start resonance finder"<<std::endl;

std::vector<std::vector<double> > ComptonSpectra;
std::vector<double> NRScounts;

if(!IsSimulation){
	std::cout<<"WORK in progress. Up to now only simulations available. Sorry!"<<std::endl;
	return;
}

else{
	bool use_bkg_file = false;
	bool use_compton_file = IsCompton;


	if (IsVerbose) std::cout<<"---Initialising simulator"<<std::endl;
	ResonanceSimulator *theSimulator = new ResonanceSimulator(IsVerbose, use_bkg_file, use_compton_file);
	if (IsVerbose) std::cout<<"---Running the simulator"<<std::endl;
	theSimulator->RunTheSimulator(f_output);

	if (IsVerbose) std::cout<<"---Getting outputs"<<std::endl;

	ComptonSpectra = theSimulator->GetSpectra();
	NRScounts = theSimulator->GetNRSCounts();
	N_channels = theSimulator->ADC_channels;
	delete theSimulator;
}

if (IsVerbose) std::cout<<"---->Ready"<<std::endl;

N_scans = NRScounts.size();

if (IsVerbose){
for (size_t i = 0; i < N_scans; ++i) std::cout<<"Scan "<<i<<" - NRS = \t"<<NRScounts[i]<<std::endl;
}

double scan_matrix[N_scans][N_channels];
double error_matrix[N_scans][N_channels];

//fill scan matrix and check for scans to be skipped (too few NRS, too in tail)

int dontuse[N_scans];
for (int i = 0; i < N_scans; ++i)
	dontuse[i] = 0;

for (int i = 0; i < N_scans; i++){
	for(size_t j = 0; j<N_channels; j++){
		if (NRScounts[i] < NRS_CUT){  //too few NRS
		scan_matrix[i][j] = 0;
		error_matrix[i][j] = 0;
		dontuse[i] = 1; 		//skip this scan 
		}
		else{
		scan_matrix[i][j] = ComptonSpectra[i][j]/(NRScounts[i]);
		error_matrix[i][j] = scan_matrix[i][j]*(1/sqrt(ComptonSpectra[i][j]) + 1/sqrt(NRScounts[i]));
		}
	}
}

//too in tail 
for (int i = 0; i < N_scans; ++i)
{
	int j = 0;
	while(scan_matrix[i][j]<tail_CUT && j<N_channels)
		j++;
	if(j<N_channels-1) dontuse[i] = 1;
}


double delta[N_channels];
double deltaerr[N_channels];
/*
for (int j = 0; j < N_channels; ++j){
	double minval = 10e22;
	double maxval = -10e22;
	double error1 = 0;
	double error2 = 0;
	for (int i = 0; i < N_scans; ++i){
		if(scan_matrix[i][j] < minval){ 
			minval = scan_matrix[i][j];
			error1 = error_matrix[i][j];
			}
		if(scan_matrix[i][j] > maxval){ 
			maxval = scan_matrix[i][j];
			error2 = error_matrix[i][j];
		}
	}
	delta[j] = maxval - minval;
	deltaerr[j] = sqrt(error1*error1 + error2*error2);
}
*/

//average method 
for (int j = 0; j < N_channels; ++j){
int st_counts = 0;
double sum = 0;
	for (int s = 0; s < N_scans; ++s)
	{
		for (int t = s; t < N_scans; ++t)
		{
		 if(dontuse[s]==0 && dontuse[t]==0){
			sum = sum + abs(scan_matrix[s][j] - scan_matrix[t][j]);
			st_counts++;
			}
		}
	}
if (st_counts!=0) delta[j] = sum/st_counts;
else delta[j] = 0;
deltaerr[j] = 0;
}


std::vector<int> min_indices;
find_minima(N_channels, delta, &min_indices);


int n_min = (int)min_indices.size();
double x_min[n_min];
double y_min[n_min];
for (int i = 0; i < n_min; ++i){
	std::cout<<min_indices[i]<<"\t";
	x_min[i] = (double)min_indices[i];
	y_min[i] = delta[min_indices[i]];
}
std::cout<<std::endl;


if(IsVerbose){
TGraphErrors *gmin = new TGraphErrors(n_min,x_min,y_min,0,0);
gmin->SetMarkerStyle(23);
gmin->SetMarkerSize(1);
gmin->SetMarkerColor(2);


int n = N_channels;
double x[n];
for (int i = 0; i < n; ++i) x[i] = (double)i;
TGraphErrors *gr = new TGraphErrors(n,x,delta,0,deltaerr);
gr->SetTitle("Resonance finder function");
gr->GetXaxis()->SetTitle("ADC channel");
gr->GetYaxis()->SetTitle("max - min");
//gr->SetFillColor(5);
gr->SetFillColorAlpha(5,0.6);

TCanvas *can2 = new TCanvas("canvasout","",1200,500);
can2->Divide(2,1);
can2->cd(1);
TMultiGraph *mg0 = new TMultiGraph();
mg0->Add(gr,"l");
mg0->Add(gmin,"p");

mg0->Draw("a");


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

TMultiGraph *mg = new TMultiGraph();

grS[0]->SetTitle("scan functions");
grS[0]->GetXaxis()->SetTitle("ADC channel");
//grS[0]->Draw("AL");
for (int i = 0; i < N_scans; ++i){
	 mg->Add(grS[i],"l");
	}
mg->Draw("a");

f_output->WriteTObject(can2);
}


return;	
}


int main(int argc, char **argv){
   //check for a correct usage
   //usage ./ResonanceFinder IsSimulation IsVerbose OutputFile ConfigurationFile
   if(argc != 5){
   	std::cout<<"Usage of ResonanceFinder: ./ResonanceFinder IsSimulation IsVerbose IsCompton output_file"<<std::endl;
	std::cout<<"IsSimulation = 0 --> Use with data files / else run a simulator"<<std::endl;
	std::cout<<"IsVerbose = 0 --> No plots produced"<<std::endl;
	std::cout<<"IsCompton = 0 --> No imput distribution from Compton spectrometer is given (fast gaussian beams are simulated)"<<std::endl;
	std::cout<<"Output file (give a fakename if IsVerbose = 0"<<std::endl;
   	return 1;
   }

   bool IsSimulation = true;
   bool IsVerbose = true;
   bool IsCompton = true;

   if(strcmp(argv[1],"0")==0) IsSimulation = false;
   if(strcmp(argv[2],"0")==0) IsVerbose = false;
   if(strcmp(argv[3],"0")==0) IsCompton = false;

   //TApplication theApp("App", &argc, argv);
   TFile *f_output;
   if(IsVerbose) f_output = new TFile(argv[4],"CREATE");

   ResonanceFinder(IsSimulation, IsCompton, IsVerbose, f_output);
   //theApp.Run();

   //gApplication->Terminate(0);
   //exit(1);
   if(IsVerbose) f_output->Close();
   return 0;
}



void find_minima(int N_channels, double *delta, std::vector<int> *min_indices){
for (int i = 1; i < N_channels-1; ++i)
{
	if((delta[i] < delta[i-1]) && (delta[i] < delta[i+1]))
		min_indices->push_back(i);
}
return;
}