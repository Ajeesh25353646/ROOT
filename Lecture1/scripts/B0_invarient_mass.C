#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooBreitWigner.h>
#include <RooGaussian.h>
#include <RooPlot.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TList.h>
#include <TLegend.h>
#include <iostream>

using namespace std;

void B0_invarient_mass() {
    // Open the ROOT file
    TFile *file = TFile::Open("B0sInvariantMass.root");
    if (!file || file->IsZombie()) {
        cerr << "Error opening file." << endl;
        return;
    }

    // List all keys (objects) in the file to find the histogram name
    TList *keys = file->GetListOfKeys();
    cout << "Objects in the file:" << endl;
    for (auto key : *keys) {
        cout << "  - " << key->GetName() << " (type: " << ((TKey*)key)->GetClassName() << ")" << endl;
    }

    // Retrieve the first TH1 histogram in the file (replace name if needed)
    TH1 *h_mass = nullptr;
    for (auto key : *keys) {
        TObject *obj = file->Get(key->GetName());
        if (obj->InheritsFrom("TH1")) {
            h_mass = (TH1*)obj;
            cout << "\nUsing histogram: " << h_mass->GetName() << endl;
            break;
        }
    }

    if (!h_mass) {
        cerr << "No TH1 histogram found in the file." << endl;
        return;
    }

    // Define the invariant mass variable
    RooRealVar m("m", "Invariant Mass [MeV/c^{2}]", h_mass->GetXaxis()->GetXmin(), h_mass->GetXaxis()->GetXmax());

    // Convert TH1 to RooDataHist
    RooDataHist data_hist("data_hist", "B0s Mass Data", m, h_mass);

    // Breit-Wigner model
    RooRealVar m0_bw("m0_bw", "Mean BW", 5367, 5300, 5400);
    RooRealVar gamma("gamma", "Width", 10, 1, 100);
    RooBreitWigner bw("bw", "Breit-Wigner", m, m0_bw, gamma);
    bw.fitTo(data_hist, RooFit::PrintLevel(-1));

    // Gaussian model
    RooRealVar mean_gaus("mean_gaus", "Gaussian Mean", 5367, 5300, 5400);
    RooRealVar sigma("sigma", "Sigma", 100, 1, 5000);
    RooGaussian gaus("gaus", "Gaussian", m, mean_gaus, sigma);
    gaus.fitTo(data_hist, RooFit::PrintLevel(-1));

    // Plotting
    TCanvas *c1 = new TCanvas("c1", "Fit Comparison", 800, 600);
    RooPlot *frame = m.frame(RooFit::Title("B0s Invariant Mass Fits"));
    data_hist.plotOn(frame, RooFit::MarkerColor(kBlack), RooFit::LineColor(kBlack));
    bw.plotOn(frame, RooFit::LineColor(kRed));
    gaus.plotOn(frame, RooFit::LineColor(kBlue));
    frame->Draw();

    TLegend *leg = new TLegend(0.65, 0.7, 0.85, 0.85);
    leg->AddEntry(frame->findObject("data_hist"), "Data", "PE");
    leg->AddEntry(frame->findObject("bw_Norm[m]"), "Breit-Wigner", "L");
    leg->AddEntry(frame->findObject("gaus_Norm[m]"), "Gaussian", "L");
    leg->Draw();

    // Compare with PDG value (B0s mass = 5366.92 ± 0.10 MeV/c²)
    cout << "\nComparison with PDG value (5366.92 MeV/c²):" << endl;
    cout << "  Breit-Wigner mass: " << m0_bw.getVal() << " ± " << m0_bw.getError() << endl;
    cout << "  Gaussian mean:     " << mean_gaus.getVal() << " ± " << mean_gaus.getError() << endl;

    // Save plot
    c1->SaveAs("B0s_fits.png");
}