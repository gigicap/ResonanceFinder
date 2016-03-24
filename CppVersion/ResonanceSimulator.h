#ifndef   RESONANCESIMULATOR_H
#define   RESONANCESIMULATOR_H

#include <iostream>
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
#include "ReadConfig.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooCBShape.h"
#include "RooDataHist.h"
#include "RooPlot.h"

using namespace std;
using namespace RooFit; 


class ResonanceSimulator{

	bool DoVerbose;
	bool UseBkg;
	bool UseCompton;

	std::vector<double> energy_tracker; //keep track of the energy steps

	public:
		//input vars
		//Histogram related variables
		double E_min, E_max;
		int ADC_channels;
		double energy_per_channel;

		//scanning related variables
		double scan_step; 	//average step of the scans
		double scan_width;	//scan energy fluctuations
		int n_scans;		//number of scans 

		//Beam variables
		double width, run_time;	//beam spread, run time (in s)
		double N_total;			//total number of gammas per bunch/second

		//Target and detector variables
		double z_target, rho_target, Sigma_beam;
		double detector_efficiency, detector_Omega;

		//resonances
		std::vector<double> E_reso;
		std::vector<double> W_reso;
		std::vector<double> I_reso;

		//cuts
		double e_cut;

		//intermediate
		std::vector<double> Inte;
		double N_background;

		TF1 *Beam;
		TH1D *hbkg;
		TH1D *hh;

		ResonanceSimulator(bool,bool,bool);
		~ResonanceSimulator();

		void make_a_beam(double);					// generate a gaussian beam profile
		TH1D*  Compton_spectrum(int,double);		//	generate a gaussian compton spectrum at enetgy E
		void resonanceIntegrals(void);
		double resonanceCounter(void);
		double random_background(double);
		void get_background();

		double generate_compton_template();			//read a comptoon spectrum and store into a CB template
		void generate_compton_at_E(double ,TH1D*, double);	//tranlate the CB and generate a new compton spectrum at enetgy E 
		RooCBShape *cball;							//crystalball for compton template
		//Roofit stuff
		RooRealVar *x;
		RooRealVar *cbmean;
		RooRealVar *cbsigma;
		RooRealVar *alpha;
		RooRealVar *n;
		RooDataHist *dh;
		double template_sigma;

		//get outputs
		std::vector<std::vector<double> > GetSpectra();
		std::vector<double> GetNRSCounts();
		void PlotResults(TFile*);

		void RunTheSimulator(TFile*);	//run a scan

	
		//output vars
		std::vector<TH1D*> hcounts; //compton spectrum      //!!!!!!!
		std::vector<double> N_NRS;	//counts at resonance

		GetConfig *ConfigurationFile;



};

#endif
