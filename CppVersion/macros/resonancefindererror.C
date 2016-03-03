#define  N_FILES 6;

void resonancefindererror(){
	
TString cond[N_FILES]={"nb0","nb30","nb50","nb80","nb105","nb300"};
int value[N_FILES]={0,30,50,80,105,300};
double d_value[N_FILES]={0,30,50,80,105,300};

TH1I *h[N_FILES];

gStyle->SetOptStat("");

double mean[N_FILES];
double sigma[N_FILES];

TCanvas *c1 = new TCanvas("c1","Found resonance channel (1000 scans)",800,600);


for (int i = 0; i < N_FILES; ++i)
{
	TString filename;
	filename.Form("../logs/channeloutput%d.txt",value[i]);
	ifstream f(filename);
		std::cout<<filename<<std::endl;

	int val;
	h[i] = new TH1I(cond[i],"Found resonance channel (1000 scans)",40,490,530);
	h[i]->SetLineColor(i+2);
	if (f.is_open()){
		while (f >> val){	
    		h[i]->Fill(val);
		}
		f.close();

		
		if(i==0) h[i]->Draw();
		else h[i]->Draw("same");
		TF1 *func = new TF1("fit","gaus",490,530);
		h[i]->Fit("fit");
		mean[i] = func->GetParameter(1);
		sigma[i] = func->GetParameter(2);
	}
	else{
		cout << "Unable to open file";
  		return;
	}


TCanvas *c2 = new TCanvas("c2","Reconstructed - Real resonance channel",800,600);

TGraphErrors *gr = new TGraphErrors(N_FILES,d_value,mean,0,sigma);
gr->SetTitle("Reconstructed - Real resonance channel");
gr->SetMarkerStyle(23);
gr->SetLineColor(4);
gr->SetMarkerColor(4);
gr->Draw("APM");

TLine *l = new TLine(0,512,330,512);
l->SetLineColor(2);
l->SetLineWidth(2);
l->Draw("same");

}
return;
}