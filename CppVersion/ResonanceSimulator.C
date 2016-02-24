#include "ResonanceSimulator.h"

# define PI  3.14159265358979323846  /* pi */

using namespace std;



ResonanceSimulator::ResonanceSimulator(bool verbo, bool use_bkg_file){
//get all the parameters from a cfg file and the background sample from ROOT file
DoVerbose = verbo; //if true, make all the plots...
UseBkg = use_bkg_file;
E_min = 2.0;
E_max = 2.4;
ADC_channels = 1024;
energy_per_channel = (E_max - E_min)/ADC_channels;


//Beam variables
width =  0.005; //percentile
run_time = 1000.0;	//beam spread, run time (in s)
N_total = 1.0e10;			//total number of gammas per bunch/second


//scanning related variables
scan_step = 2*0.005*(E_max + E_min)*0.1; 	//average step of the scans (10% of the whole energy spread)
scan_width = scan_step/10;	//scan energy fluctuations
n_scans = 10;	//number of scans 


//Target and detector variables
z_target = 0.15;
rho_target = 6.1e20;
Sigma_beam = 100;

detector_efficiency = 0.17; //set to have reasonable values (105cs/s of signal at resonance)
detector_Omega = 0.5;

//resonances
E_reso.push_back(2.2);
W_reso.push_back(6.2e-6);
I_reso.push_back(64); //in b 
/*
E_reso.push_back(2.24);
W_reso.push_back(8.2e-6);
I_reso.push_back(120); //in b 
*/

//cuts
e_cut = 1.8;

if(UseBkg){
TFile *f = new TFile("background.root","READ");
hbkg = (TH1D*)f->Get("hbackground");
}
else N_background = 110*run_time; //reasonable from CDR

}

void ResonanceSimulator::RunTheSimulator(TFile *f_output){

double E_central = (E_max + E_min)/2;

//define the energy scan range and check if it is inside the E_min,E_max range
double minimum_scan = E_central - scan_step*(int)n_scans/2;
TRandom rand0(0);
double rnd_minimum_scan = rand0.Gaus(minimum_scan,scan_width);
if(rnd_minimum_scan<E_min){
	std::cout<<"Warning : scan will start at E_min = "<<E_min<<std::endl;
	rnd_minimum_scan = E_min;
}

if (DoVerbose) std::cout<<"--Got first random energy"<<std::endl;

double en = 0; 
if(UseBkg) get_background();
resonanceIntegrals();
for (int j = 0; j < n_scans; ++j)
{
	if(j==0) en = rnd_minimum_scan;
	else{
		TRandom rand2(0);
		en = en +  rand2.Gaus(scan_step,scan_width);
	}

	if (DoVerbose) std::cout<<"-Energy = "<< en <<std::endl;


	//check if out of E_max
	if (en >= E_max){
		std::cout<<"STOP : out of range. Only "<<j<<" scans will be processed"<<std::endl;
		break;
	}
	energy_tracker.push_back(en);

	Beam = new TF1("Beam","[0]*TMath::Gaus(x,[1],[2])",E_min,E_max);
//randomize scan and use this kernel code:
	make_a_beam(en);
	hcounts.push_back(Compton_spectrum(j));
	N_NRS.push_back(resonanceCounter());
	delete Beam;
}

if(DoVerbose) PlotResults(f_output);

return;
}

//define a beam function
void ResonanceSimulator::make_a_beam(double E_central){

double central_energy_value = (E_min+E_max)/2;
double W_beam = central_energy_value*width;
double I_max = N_total/(W_beam/energy_per_channel*sqrt(2*PI));   //define the max beam intensity from brillance
double time_I = I_max*run_time;
	
Beam->SetParameters(time_I, E_central, W_beam);	

}

//extract a count spectrum from the beam function (as a TH1D)
TH1D* ResonanceSimulator::Compton_spectrum(int j){

TString thname = "hcounts";
thname.Append(Form("%d",j));

TH1D *temp_hcounts = new TH1D(thname,"Beam from Compton",ADC_channels,0,ADC_channels);

double energy = E_min;
for(int i = 0; i<ADC_channels; i++){
	energy = energy + energy_per_channel;
	temp_hcounts->Fill(i,Beam->Eval(energy));
	}
return temp_hcounts;	
}

void ResonanceSimulator::resonanceIntegrals(){
	std::cout<<"eresosize = "<<E_reso.size()<<std::endl;
for (size_t i = 0; i < E_reso.size(); ++i)
	{
	TF1 *Resonance = new TF1("Resonance","[0]*TMath::Gaus(x,[1],[2])",E_reso[i]-5*W_reso[i],E_reso[i]+5*W_reso[i]);
	Resonance->SetParameters(I_reso[i],E_reso[i],W_reso[i]);
	Inte.push_back(Resonance->Integral(E_reso[i]-5*W_reso[i],E_reso[i]+5*W_reso[i])*1.0e-24);
	delete Resonance;
	}
return;
}

double ResonanceSimulator::resonanceCounter(){


double N1 = 0;
	std::cout<<"intesize = "<<Inte.size()<<std::endl;
for (size_t i = 0; i < Inte.size(); ++i){
	double energy = E_reso[i];
	N1 = N1 + Inte[i]*Beam->Eval(energy); 
	if(DoVerbose) std::cout<<"Energy = "<<E_reso[i]<<" Inte [i] = "<<Inte[i]<<" Beam = "<<Beam->Eval(energy)<<" -> N1 = "<<N1<<std::endl;
}

//correct with target parameters
double N2 = N1*z_target*rho_target*Sigma_beam;
//correct with detector parameters
double N3 = N2*detector_efficiency*detector_Omega;

//add background 
double N_t = N3+random_background(N_background);

if(DoVerbose) std::cout<<"N2 = "<<N2<<"\t N3 = "<<N3<<"\t N_t = "<<N_t<<std::endl;
return N_t;
}

double ResonanceSimulator::random_background(double N_b){
//extract a random bkg to add to signal 
TRandom rand1(0);
return rand1.Gaus(N_b,sqrt(N_b));
}

//Get background counts on NRS from Geant4 simulation template (histogram)
void ResonanceSimulator::get_background(){
	TAxis *xaxis = hbkg->GetXaxis(); 
	int binx = xaxis->FindBin(e_cut);
	int binmax = xaxis->GetXmax();
	N_background = hbkg->Integral(binx,binmax);

return;
}

std::vector<std::vector<double> > ResonanceSimulator::GetSpectra(){

std::vector<std::vector<double> > final_vector;

for (size_t i = 0; i < hcounts.size(); ++i){
	std::vector<double> value_vec;
	for (int j = 0; j < hcounts[i]->GetSize()-2; ++j){
			double c_value = hcounts[i]->GetBinContent(j+1);
			value_vec.push_back(c_value);
		}
	final_vector.push_back(value_vec);
	}

return final_vector;
}

std::vector<double> ResonanceSimulator::GetNRSCounts(){
	std::vector<double> NN;
	for (size_t i = 0; i < N_NRS.size(); ++i)
		NN.push_back(N_NRS[i]);
	
	return NN;
}


void ResonanceSimulator::PlotResults(TFile *f_output){

if (DoVerbose) std::cout<<"---Make a canvas"<<std::endl;


//canvas 1: background
if (DoVerbose) std::cout<<"---c1"<<std::endl;
if(UseBkg){
TCanvas *c1 = new TCanvas("can1","",600,500);
hbkg->GetXaxis()->SetTitle("E [MeV]");
hbkg->GetYaxis()->SetTitle("Counts");
double hbkgmax = hbkg->GetMaximum();
TLine *ecutline = new TLine(e_cut,0,e_cut,hbkgmax);
hbkg->SetLineColor(0);
ecutline->SetLineColor(2);
TLegend *leg1 = new TLegend(0.1,0.7,0.48,0.9);
leg1->AddEntry("hbkg","background at NRS detector (simulation","l");
leg1->AddEntry("ecutline","energy cut","l");
hbkg->Draw();
ecutline->Draw("same");
leg1->Draw("same");
f_output->WriteTObject(c1);
}
//canvas2: Energy scanning
if (DoVerbose) std::cout<<"---c2"<<std::endl;

TCanvas *c2 = new TCanvas("can2","",600,500);

int n = (int)energy_tracker.size();
double x[n];
double et[n];

for (int i = 0; i < n; ++i){
 x[i] = (double)i;
 et[i] = energy_tracker[i];
} 
TGraph *grScan = new TGraph(n,x,et);
grScan->SetTitle("Simulated energy steps during scan");
grScan->GetXaxis()->SetTitle("Step");
grScan->GetYaxis()->SetTitle("Central energy[MeV]");
grScan->Draw("ACP");

f_output->WriteTObject(c2);

//canvas3: hcounts e risonanze
if (DoVerbose) std::cout<<"---c3"<<std::endl;

TCanvas *c3 = new TCanvas("can3","",600,500);

hcounts[0]->SetLineColor(2);
hcounts[0]->Draw();
for (size_t i = 1; i < hcounts.size(); ++i){
	hcounts[i]->SetLineColor(i+3);
	hcounts[i]->Draw("same");
	}
double hcmax = hcounts[0]->GetMaximum();
std::vector<TLine*> resolines;
for (size_t j = 0; j < hcounts.size(); ++j){
	double chen = (E_reso[j] - E_min)/energy_per_channel;
	TLine *resoline = new TLine(chen,0,chen,hcmax);
	resoline->SetLineColor(4);
	resolines.push_back(resoline);
}
for (size_t j = 0; j < hcounts.size(); ++j) resolines[j]->Draw("same");

f_output->WriteTObject(c3);

//canvas4: N_NRS for all the scans
if (DoVerbose) std::cout<<"---c4"<<std::endl;

TCanvas *c4 = new TCanvas("can4","",600,500);

double nnrss[n];
for (int i = 0; i < n; ++i){
 nnrss[i] = N_NRS[i];
} 


TGraph *grNRS = new TGraph(n,x,nnrss);
grNRS->SetTitle("NRS counts during scan");
grNRS->GetXaxis()->SetTitle("Step");
grNRS->GetYaxis()->SetTitle("NRS Counts");
grNRS->Draw("ACP");

f_output->WriteTObject(c4);


if (DoVerbose) std::cout<<"---exit"<<std::endl;
return;
}