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

std::vector<std::vector<int> > ComptonSpectra;
std::vector<int> NRScounts;

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


for (int i = 0; i < N_scans; ++i){
	for(size_t j = 0; j<N_channels; j++){
		scan_matrix[i][j] = (double)ComptonSpectra[i][j]/((double)NRScounts[i]);
		error_matrix[i][j] = scan_matrix[i][j]*(1/sqrt((double)ComptonSpectra[i][j]) + 1/sqrt((double)NRScounts[i]));
	}
}

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