#include <RooRealVar.h>
#include <RooExponential.h>
#include <RooGaussian.h>
#include <RooAddPdf.h>
#include <RooDataSet.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>

void B0_LHCb_decay() {
    int nbins = 25;
    double xmin = 5100;
    double xmax = 5500;


    RooRealVar x{"x", "m(p-pbar)", xmin, xmax, "MeV/c²"};
    x.setBins(nbins);
    auto data = *RooDataSet::read("rarest_b0_decay.dat", x, "D");

    RooRealVar c{"c", "exponential par", -0.0003, -1, -0.0};
    RooExponential b{"bkg", "Comb. bkg.", x, c};

    RooRealVar m0{"m0", "B0 mass", 5279, 5220, 5320};
    RooRealVar s0{"s0", "B0 width", 10, 2, 50};
    RooGaussian g0{"g0", "B0 peak", x, m0, s0};

    RooRealVar m1{"m1", "B0s mass", 5380, 5320, 5420};
    RooRealVar s1{"s1", "B0s width", 10, 2, 50};
    RooGaussian g1{"g1", "B0s peak", x, m1, s0};

    RooRealVar n0{"n0", "fraction of B0", 100, 0, 1000};
    RooRealVar n1{"n1", "fraction of Bs", 800, 0, 1000};
    RooRealVar nbck{"nB", "n events of bck", 20, 0, 1000};

    RooAddPdf model{"model", "model", RooArgList{g0, g1, b}, RooArgList{n0, n1, nbck}};

    auto fit_res = model.fitTo(data, RooFit::Save());

    auto xframe = x.frame();
    data.plotOn(xframe);
    model.plotOn(xframe);
    model.plotOn(xframe, RooFit::Components("bkg"), RooFit::LineColor(kGreen));
    model.plotOn(xframe, RooFit::Components("g0"), RooFit::LineColor(kRed));
    model.plotOn(xframe, RooFit::Components("g1"), RooFit::LineColor(kBlue));

    xframe->Draw();
    

    fit_res->Print();

}