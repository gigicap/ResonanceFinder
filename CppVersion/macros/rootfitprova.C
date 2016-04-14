
using namespace RooFit; 

void roofitprova(TString filename){
//read the Compton Sample file
TFile *f = new TFile(filename,"READ");

//read sample histogram 
TH1D *hh = (TH1D*)f->Get("eBeamStep5");
TH1D *hb = (TH1D*)f->Get("newEnergy");


int nentries = hh->Integral();
int size = hh->GetSize()-2;
cout<<nentries<<endl;

//get some starting values for the fit 
double hmin = hh->GetXaxis()->GetXmin();
double hmax = hh->GetXaxis()->GetXmax();
int binmax = hh->GetMaximumBin();
double maxval = hh->GetXaxis()->GetBinCenter(binmax);

double maximum = hh->GetMaximum();
double halfmax = maximum/2;

int i = binmax;
while(hh->GetBinContent(i)>=halfmax)
	i++;
int bin0 = i-1;
int bin1 = i;

double x_0 = hh->GetXaxis()->GetBinCenter(bin0);
double x_1 = hh->GetXaxis()->GetBinCenter(bin1);
double y_0 = hh->GetBinContent(bin0);
double y_1 = hh->GetBinContent(bin1);
double y_h = halfmax;
double x_h = x_0 + (y_h - y_0)*((x_1-x_0)/(y_1-y_0));


double start_sigma = 2*(x_h-maxval)/2.35;

cout<<"Start mean = "<<maxval<<" start sigma = "<<start_sigma<<endl;

RooRealVar x("x","x",hmin,hmax);
x.setBins(size);
RooRealVar cbmean("cbmean","cbmean",maxval,maxval-start_sigma,maxval+start_sigma);
RooRealVar cbsigma("cbsigma","cbsigma",start_sigma, 0,2*start_sigma);
RooRealVar alpha("alpha","alpha",0.5,-1.5,1.5);
RooRealVar n("n","n",40,0,100);

//get the sample histogram (hh) as data for CB PDF
RooDataHist dh("dh","dh",x,Import(*hh));

cout<<"Define a CB PDF"<<endl;
//define a pdf from a data fit
RooCBShape cball("cball", "crystal ball", x, cbmean, cbsigma, alpha, n);


cball.fitTo(dh);

//define a new randomized dataset
double energy_step = 0.3;

Double_t cbmean_shifted_val = cbmean.getValV()-energy_step;
RooRealVar cbmean_shifted("cbmean","cbmean",cbmean_shifted_val,cbmean_shifted_val-start_sigma,cbmean_shifted_val+start_sigma);


RooCBShape cball2("cball2", "crystal ball shifted", x, cbmean_shifted, cbsigma, alpha, n);


RooDataHist *data2 = cball2.generateBinned(x,nentries);


//now fit the beam
int binmaxB = hb->GetMaximumBin();
double maxvalB = hb->GetXaxis()->GetBinCenter(binmax);

std::cout<<"MAXVAL B = "<<maxvalB<<std::endl;


RooRealVar beammean("beammean","beammean",3.0,2.9,3.01);
RooRealVar beamsigma("beamsigma","beamsigma",0.01,0.0,0.5);
RooRealVar beamxi("beamxi","beamxi",0.,-10.,10.);					//peak asymmetry parameter 
RooRealVar beamrho1("beamrho1","beamrho1",0.,-1.,50.);				//left tail 
RooRealVar beamrho2("beamrho2","beamrho2",-50.,-120.,50.);			//right tail

RooDataHist dbeam("dbeam","dbeam",x,Import(*hb));
cout<<"Define a Bukin PDF"<<endl;

RooBukinPdf bukin("bukin","bukin",x, beammean, beamsigma, beamxi, beamrho1, beamrho2);
bukin.fitTo(dbeam);


//TH1* h2 = data2->createHistogram("dg2",x,Binning(size)); 
//RooDataHist dh2("dh2","dh2",x,Import(*h2));

//plot
RooPlot* xframe2 = x.frame() ;
dh.plotOn(xframe2) ;
cball.plotOn(xframe2) ;
data2->plotOn(xframe2,MarkerColor(2));
xframe2->Draw() ;

RooPlot* xframe3 = x.frame() ;
dbeam.plotOn(xframe3) ;
bukin.plotOn(xframe3) ;
xframe3->Draw() ;


return;
}




//DONT USE THE FOLLOWING 

void roofitprova2(TString filename){
//read the Compton Sample file
TFile *f = new TFile(filename,"READ");

//read sample histogram 
TH1D *hh = (TH1D*)f->Get("SignalMacroEBeam");


int nentries = hh->Integral();
int size = hh->GetSize()-2;
cout<<nentries<<endl;

//get some starting values for the fit 
double hmin = hh->GetXaxis()->GetXmin();
double hmax = hh->GetXaxis()->GetXmax();
int binmax = hh->GetMaximumBin();
double maxval = hh->GetXaxis()->GetBinCenter(binmax);

double maximum = hh->GetMaximum();
double halfmax = maximum/2;

int i = binmax;
while(hh->GetBinContent(i)>=halfmax)
	i++;
int bin0 = i-1;
int bin1 = i;

double x_0 = hh->GetXaxis()->GetBinCenter(bin0);
double x_1 = hh->GetXaxis()->GetBinCenter(bin1);
double y_0 = hh->GetBinContent(bin0);
double y_1 = hh->GetBinContent(bin1);
double y_h = halfmax;
double x_h = x_0 + (y_h - y_0)*((x_1-x_0)/(y_1-y_0));


double start_sigma = 2*(x_h-maxval)/2.35;

RooRealVar x("x","x",hmin,hmax);

cout<<"Start mean = "<<maxval<<" start sigma = "<<start_sigma<<endl;

double val;
TTree *t = new TTree("t","t");
t->Branch("Value",&val);

for(int i = 0; i< size; i++){
	int nen = hh->GetBinContent(i+1);
	for (int j = 0; j < nen; ++j)
	{
		hh->GetXaxis()->GetBinCenter(i);
		t->Fill();
	}
}

RooDataHist dh1("dh1","dh1",x,Import(*hh));
RooDataSet dh("dh","dh",x,Import(*t));

//define a pdf from a data fit
//RooCBShape cball("cball", "crystal ball", x, cbmean, cbsigma, alpha, n);

RooHistPdf ph("ph","ph",x,dh1,1);
RooKeysPdf kest3("kest3","kest3",x,dh,RooKeysPdf::MirrorBoth,2) ;
//cball.fitTo(dh);

//define a new randomized dataset
//double energy_step = 0.0;

//Double_t cbmean_shifted_val = cbmean.getValV()-energy_step;
//RooRealVar cbmean_shifted("cbmean","cbmean",cbmean_shifted_val,cbmean_shifted_val-start_sigma,cbmean_shifted_val+start_sigma);


//RooCBShape cball2("cball2", "crystal ball shifted", x, cbmean_shifted, cbsigma, alpha, n);


//RooDataHist *data2 = cball2.generateBinned(x,nentries);
RooDataHist *data2 = ph.generateBinned(x,nentries);

//TH1* h2 = data2->createHistogram("dg2",x,Binning(size)); 
//RooDataHist dh2("dh2","dh2",x,Import(*h2));

//plot
RooPlot* xframe2 = x.frame() ;
dh1.plotOn(xframe2) ;
dh.plotOn(xframe2) ;
ph.plotOn(xframe2) ;
kest3.plotOn(xframe2);
data2->plotOn(xframe2,MarkerColor(2));
xframe2->Draw() ;

return;
}
