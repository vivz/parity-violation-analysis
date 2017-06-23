//Script to read in (several) ROOT files from the magnet off runs of mu+
//The trees in the files can be combined into a TChain structure for analysis of all data.

using namespace std;

#include "TFile.h"
#include "TTree.h"
#include "TAxis.h"


void bg(){
	gROOT->Reset();	//this clears any objects from a previous run of this script

	TChain hits("T");	//define the TChain (using the tree 'T' in the file)
	hits.Add("May30MagOn.root");
	hits.Add("May17MagOn.root");

	TChain bg("T");
	bg.Add("NoTargetJune6.root");

	//variables
	int eventNum=0;
	int UNIXtime=0;
	float tdc[2]={-10,-10};

	//add 'branches' to the tree
	hits.SetBranchAddress("eventNum",&eventNum);	//branch for the event number
    hits.SetBranchAddress("UNIXtime",&UNIXtime);	//branch for the time
	hits.SetBranchAddress("tdc",&tdc);			//branch for the tdc values (in ns)

	//histograms
	TH1D *bgHits = new TH1D("bgHits","Upward Decays",25,0,5);

	//subtracting the background data
	int eventNum_bg=0;
	int UNIXtime_bg=0;
	float tdc_bg[2]={-10,-10};

	Int_t nevent_bg = bg.GetEntries();

	bg.SetBranchAddress("eventNum",&eventNum_bg);	//branch for the event number
    bg.SetBranchAddress("UNIXtime",&UNIXtime_bg);	//branch for the time
	bg.SetBranchAddress("tdc",&tdc_bg);			//branch for the tdc values (in ns)

	for (Int_t i=0;i<nevent_bg;i++) {
		bg.GetEntry(i); 
		if(tdc_bg[0]>0){
			bgHits->Fill(tdc_bg[0]/1000);
		}
		else if(tdc_bg[1]>0){
			bgHits->Fill(tdc_bg[1]/1000);
		}
	}

	//exopnential fit function
	TF1 *myfunUp = new TF1("myfunUp","[0]*exp([1]*x)+[2]");
	myfunUp->SetParLimits(0,10,1e5); 
	myfunUp->SetParLimits(2,0,50);

	bgHits->Fit(myfunUp,"L","",0,5);

	//draw
	TCanvas * Clife = new TCanvas("Clife","Up and Down Lifetime",0,0,800,400);
	Clife->cd(1);

	bgHits->SetTitle("Background Event distribution");
	gPad->SetLogy();
	bgHits->GetXaxis()->SetTitle("Decay Time (#mu s)");
	bgHits->GetXaxis()->SetTitleSize(0.055);
	bgHits->GetXaxis()->SetTitleOffset(0.9);
	bgHits->GetXaxis()->SetLabelSize(0.055);
	bgHits->GetXaxis()->CenterTitle();
	bgHits->GetYaxis()->SetTitle("Logged Event Count");
	bgHits->GetYaxis()->SetTitleSize(0.055);
	bgHits->GetYaxis()->SetTitleOffset(0.9);
	bgHits->GetYaxis()->SetLabelSize(0.055);
	bgHits->GetYaxis()->CenterTitle();
	bgHits->Draw();

	double slopeUp = myfunUp->GetParameter(1);
	double slopeErrUp = myfunUp->GetParError(1);
	double offsetUp = myfunUp->GetParameter(2);

	cout << "********************************************************************************"<<endl;
	cout << "The measured slope from the Up distribution is " << slopeUp << endl;
	cout << "The Up lifetime is " << (-1/slopeUp) << " +/- " << (1/(slopeUp-slopeErrUp) - (1/(slopeUp+slopeErrUp)))/2 << " us" <<endl;
	cout << "The Up offset is "<< offsetUp <<endl;
	cout << "********************************************************************************"<<endl;


}
