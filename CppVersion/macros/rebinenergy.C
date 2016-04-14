void rebinenergy(){
	
TFile *f1 = new TFile("Scan3MeV.root","READ"); //get the compton measurement

TH1D *hcompt = (TH1D*)f1->Get("eBeamStep5");
int nbins = hcompt->GetSize()-2;
double min = hcompt->GetXaxis()->GetBinCenter(1);
double max = hcompt->GetXaxis()->GetBinCenter(nbins);

TH1D *newEnergy = new TH1D("newEnergy","Energy",nbins,min,max);

TFile *f2 = new TFile("3MeVfitted.root","READ"); //get the beam spectrum

TH1D *hen = (TH1D*)f2->Get("Energy");
int enb = hen->GetSize()-2;


for(int i = 1; i<enb; i++){
	double number = (double)hen->GetBinContent(i);
	double energy = hen->GetXaxis()->GetBinCenter(i);

	newEnergy->Fill(energy,number);
}

for(int i = 1; i<nbins; i++){
	newEnergy->SetBinError(i,sqrt(newEnergy->GetBinContent(i)));
}

TFile *fout = new TFile("3MeVComptonSimulation.root","UPDATE");
fout->WriteTObject(hcompt);
fout->WriteTObject(newEnergy);


return;
}