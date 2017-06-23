//Script to read in (several) ROOT files from the magnet off runs of mu+
//The trees in the files can be combined into a TChain structure for analysis of all data.

using namespace std;

#include "TFile.h"
#include "TTree.h"
#include "TAxis.h"


void lifetime(){
	gROOT->Reset();	//this clears any objects from a previous run of this script

	TChain hits("T");	//define the TChain (using the tree 'T' in the file)
	hits.Add("May30MagOn.root");
	hits.Add("May17MagOn.root");

	//variables
	int eventNum=0;
	int UNIXtime=0;
	float tdc[2]={-10,-10};

	//add 'branches' to the tree
	hits.SetBranchAddress("eventNum",&eventNum);	//branch for the event number
    hits.SetBranchAddress("UNIXtime",&UNIXtime);	//branch for the time
	hits.SetBranchAddress("tdc",&tdc);			//branch for the tdc values (in ns)

	//histograms
	TH1D *upHits = new TH1D("upHits","Upward Decays",50,0,5);
	TH1D *downHits = new TH1D("downHits","Downward Decays",50,0,5);

	//loop over all data in chain
	Int_t nevent = hits.GetEntries();	//get the number of entries in the TChain
	
	Int_t endTime = 0;
	Int_t startTime = 0;
	Int_t totalDataTime = 0;

	for (Int_t i=0;i<nevent;i++) {
		hits.GetEntry(i); 
		if(eventNum == 1){
			totalDataTime += endTime-startTime;
			startTime = UNIXtime;
		}
		else{
			endTime = UNIXtime;
		}

		if(tdc[0]>0){		//see if this is an 'up' decay
			upHits->Fill(tdc[0]/1000);	//convert to micro-seconds from ns
		}
		else if(tdc[1]>0){
			downHits->Fill(tdc[1]/1000);
		}
	}

	//exopnential fit function
	TF1 *myfunUp = new TF1("myfunUp","[0]*exp([1]*x)+[2]+[3]*exp(x/(-0.16))");
	myfunUp->SetParLimits(0,10,1e5); 
	myfunUp->SetParLimits(2,0,5);
	myfunUp->SetParLimits(3,10,1e5);

	upHits->Fit(myfunUp,"","",0,5);

	TF1 *myfunDown = new TF1("myfunDown","[0]*exp([1]*x)+[2]+[3]*exp(x/(-0.16))");
	myfunDown->SetParLimits(0,10,1e5);
	myfunDown->SetParLimits(2,0,100);
	myfunDown->SetParLimits(3,0,1e5);

	downHits->Fit(myfunDown,"","",0,5);

	//draw
	TCanvas * Clife = new TCanvas("Clife","Up and Down Lifetime",0,0,800,400);
	Clife->Divide(2,1);
	Clife->cd(1);

	upHits->SetTitle("Up Event TDC Distribution");
	upHits->GetXaxis()->SetTitle("Decay Time (#mu s)");
	upHits->GetXaxis()->SetTitleSize(0.055);
	upHits->GetXaxis()->SetTitleOffset(0.9);
	upHits->GetXaxis()->SetLabelSize(0.055);
	upHits->GetXaxis()->CenterTitle();
	upHits->GetYaxis()->SetTitle("Event Count");
	upHits->GetYaxis()->SetTitleSize(0.055);
	upHits->GetYaxis()->SetTitleOffset(0.9);
	upHits->GetYaxis()->SetLabelSize(0.055);
	upHits->GetYaxis()->CenterTitle();
	gStyle->SetOptFit(1);
	upHits->Draw();

	Clife->cd(2);

	downHits->SetTitle("Down Event TDC Distribution");
	downHits->GetXaxis()->SetTitle("Decay Time (#mu s)");
	downHits->GetXaxis()->SetTitleSize(0.055);
	downHits->GetXaxis()->SetTitleOffset(0.9);
	downHits->GetXaxis()->SetLabelSize(0.055);
	downHits->GetXaxis()->CenterTitle();
	downHits->GetYaxis()->SetTitle("Event Count");
	downHits->GetYaxis()->SetTitleSize(0.055);
	downHits->GetYaxis()->SetTitleOffset(0.9);
	downHits->GetYaxis()->SetLabelSize(0.055);
	downHits->GetYaxis()->CenterTitle();
	downHits->Draw();

	double slopeUp = myfunUp->GetParameter(1);
	double slopeErrUp = myfunUp->GetParError(1);
	double offsetUp = myfunUp->GetParameter(2);

	cout << "********************************************************************************"<<endl;
	cout << "The measured slope from the Up distribution is " << slopeUp << endl;
	cout << "The Up lifetime is " << (-1/slopeUp) << " +/- " << (1/(slopeUp-slopeErrUp) - (1/(slopeUp+slopeErrUp)))/2 << " us" <<endl;
	cout << "The Up offset is "<< offsetUp <<endl;
	cout << "********************************************************************************"<<endl;

	double slopeDown = myfunDown->GetParameter(1);
	double slopeErrDown = myfunDown->GetParError(1);
	double offsetDown = myfunDown->GetParameter(2);

	cout << "********************************************************************************"<<endl;
	cout << "The measured slope from the Down distribution is " << slopeDown<< endl;
	cout << "The Down lifetime is " << (-1/slopeDown) << " +/- " << (1/(slopeDown-slopeErrDown) - (1/(slopeDown+slopeErrDown)))/2 << " us" <<endl;
	cout << "The Down offset is "<< offsetDown <<endl;
	cout << "********************************************************************************"<<endl;

}
