#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooPlot.h"
#include "TCanvas.h"

void Root_empty() {
    // Define the observable
    RooRealVar energy("energy", "Reconstructed Neutrino Energy [GeV]", 0.5, 14);

    // Load observed data
    RooDataSet* data = RooDataSet::read("minos_2013_data.dat", RooArgSet(energy), "v");

    // Load Monte Carlo data
    RooDataSet* mc = RooDataSet::read("minos_2013_mc.dat", RooArgSet(energy), "v");

    // Create a binned dataset and build a PDF from the MC data
    RooDataHist mcHist("mcHist", "MC Histogram", energy, *mc);
    RooHistPdf mcPdf("mcPdf", "Non-Oscillated Neutrino Energy PDF", energy, mcHist);

    // Fit the model to the observed data
    mcPdf.fitTo(*data, RooFit::Save());

    // Visualize the results
    RooPlot* frame = energy.frame(RooFit::Title("MINOS Neutrino Energy Spectrum"));
    data->plotOn(frame);
    mcPdf.plotOn(frame);

    TCanvas* c = new TCanvas("c", "c", 800, 600);
    frame->Draw();
    c->SaveAs("/mnt/d/OneDrive - Alma Mater Studiorum Università di Bologna/ROOT/Lecture3/data/minos_energy_spectrum.png");
}
