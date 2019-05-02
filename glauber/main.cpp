#include <iostream>

#include "Fitter.h"
#include "FitterHelper.h"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TLegend.h"
#include "TH2.h"

using namespace Glauber;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Wrong number of parameters! Executable usage:" << std::endl;
        std::cout << "   ./glauber f0 k0" << std::endl;
        return -1;
    }
    const Float_t f0 = atof( argv[1]);
    std::cout << "f=" << f0 << std::endl;
    const Float_t f1 = atof( argv[2]);
    std::cout << "f=" << f1 << std::endl;
    const Int_t k0 = atoi( argv[3] );
    std::cout << "k0=" << k0 << std::endl;
    const Int_t k1 = atoi( argv[4] );
    std::cout << "k1=" << k1 << std::endl;
    
    // *****************************************
    // Modify this part according to your needs
    // *****************************************

    ///  |   mode    |   function for Na     |
    ///  |   Default | f*Npart + (1-f)*Ncoll |
    ///  |   PSD     |     f - NpartA         |
    ///  |   Npart   |     Npart^f           |
    ///  |   Ncoll   |     Ncoll^f           |
//    const TString mode = "Default";
    const TString mode = "PSD";
    
    const TString glauber_filename = "/home/vad/NIR_codes/Centrality/gmc-Au3Au3-snn23.6-md0.4-nd-1.0-rc1-smax99.0.root";   // input files
    const TString glauber_treename = "nt_Au3_Au3";
    const TString in_filename = "/home/vad/NIR_codes/QAHistoBuilder/QASelectedPT2Histos.root";

//    const TString histoname = "tracksMDC_selected";
//    const TString histoname = "hitsTOF_selected";
//    const TString histoname = "hitsRPC_selected";
//    const TString histoname = "hitsTOF+RPC_selected";
    const TString histoname = "FWSumChargeZ_selected";


    const Int_t min_bin = 20;      // not fitting low multiplicity region due to trigger bias, etc
    const Int_t max_bin = 80;   // very large number to fit the whole histo
    const Int_t nevents = 66502;

    const TString outdir = ".";
    // *****************************************
    // *****************************************

    std::unique_ptr<TFile> glauber_file{ TFile::Open(glauber_filename, "read") };
    std::unique_ptr<TTree> glauber_tree{ (TTree*) glauber_file->Get(glauber_treename) };

    std::unique_ptr<TFile> f{TFile::Open(in_filename)};    
    TH1F *hdata = (TH1F*)f->Get(histoname);
    
    Fitter fitter ( std::move(glauber_tree) );

    fitter.SetMode(mode);
    fitter.SetMassNumber(f0/2);
    fitter.SetInputHisto(*hdata);
    fitter.SetBinSize(1);
    fitter.Init(nevents);
    
    fitter.SetFitMinBin(min_bin);
    fitter.SetFitMaxBin(max_bin);
    fitter.SetOutDirName(outdir);

    float par[3];
//    const float chi2 = fitter.FitGlauber(par, f0, f1, k0, k1, nevents);
	
    float chi2=1e10;
    if ( mode == "PSD" ) chi2 = fitter.FitFW(par, f0, f1, k0, k1, nevents);
    else chi2 = fitter.FitGlauber(par, f0, f1, k0, k1, nevents);

    std::cout << "f = " << par[0] << "    mu = " << par[1] << "    k = " << par[2] << "    chi2 = " << chi2 << std::endl; 
    
//    Glauber::DrawHistos(fitter, true, true, true, true);
//    DrawHistos(fitter, true, true, true, true);

    if ( mode == "PSD" ) DrawHistos(fitter, true, true, true, false, true);
    else DrawHistos(fitter, true, true, true, true, false);

    const float range[2] = {300, 350.};
    std::unique_ptr<TH1F> hB(fitter.GetModelHisto (range, "B", par, 100000));
    hB->SaveAs( "b_test.root" );
    
    std::cout << "END!" << std::endl;

    return 0;
}
