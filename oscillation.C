//Script to read in (several) ROOT files from the magnet off runs of mu+
//The trees in the files can be combined into a TChain structure for analysis of all data.

using namespace std;

#include "TFile.h"
#include "TTree.h"
#include "TAxis.h"


void oscillation(){
	gROOT->Reset();	//this clears any objects from a previous run of this script

	TChain hits("T");	//define the TChain (using the tree 'T' in the file)
	hits.Add("May17MagOn.root");

	//variables
	int eventNum=0;
	int UNIXtime=0;
	int leftBoundery = 2;
	int rightBoundery = 3;
	float tdc[2]={-10,-10};

	//add 'branches' to the tree
	T->SetBranchAddress("eventNum",&eventNum);	//branch for the event number
	T->SetBranchAddress("UNIXtime",&UNIXtime);	//branch for the time
	T->SetBranchAddress("tdc",&tdc);				//branch for the tdc values (in ns)

	//histograms
	TH1D *h = new TH1D("h","U-D",200,leftBoundery,rightBoundery);
	//loop over all data in chain
	Int_t nEvent = hits.GetEntries();	//get the number of entries in the TChain

	for (Int_t i = 0; i < nEvent; i++){
		hits.GetEntry(i); 
		if(tdc[0]>0){		//see if this is an 'up' decay
			h->Fill(tdc[0]/1000);	//convert to micro-seconds from ns
		}
		else if(tdc[1]>0){
			h->Fill(tdc[1]/1000, -1);
		}
	}

	TGraphErrors *p = new TGraphErrors();

	//scale bin by bin
	int nBin = h->GetSize(); //subtracting the overflow and underflow bins
	cout<<nBin<<endl;
	for(int i = 1; i < 200; i++){
		Double_t x = h->GetXaxis()->GetBinCenter(i);
		int scale = TMath::Exp(x/2.197);
		//int scale = 1;
		int y = h->GetBinContent(i)*scale;
		if (y==0)
			continue;
		cout<< x<<", "<< y<<endl;
		p->SetPoint(i-1,x,y);
		//p->SetPointError(i,0,sqrt(counts)/time)
	}

	TCanvas * Diff = new TCanvas("Diff","Up and Down Lifetime",0,0,800,400);
	Diff->cd();

	p->SetMarkerStyle(3);
	p->SetMarkerSize(0.5);
	p->Draw("AP");

	// h->SetTitle("Up Decays");
	// h->GetXaxis()->SetTitle("Decay Time (#mu s)");
	// h->GetXaxis()->SetTitleSize(0.055);
	// h->GetXaxis()->SetTitleOffset(0.9);
	// h->GetXaxis()->SetLabelSize(0.055);
	// h->GetXaxis()->CenterTitle();
	// h->GetYaxis()->SetTitle("Entries");
	// h->GetYaxis()->SetTitleSize(0.055);
	// h->GetYaxis()->SetTitleOffset(0.9);
	// h->GetYaxis()->SetLabelSize(0.055);
	// h->GetYaxis()->CenterTitle();
	// h->Draw();


}
