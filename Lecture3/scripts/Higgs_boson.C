#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooCBShape.h"
#include "RooPolynomial.h"
#include "RooAddPdf.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/ProfileLikelihoodCalculator.h"
#include "RooStats/FeldmanCousins.h"
#include "RooStats/AsymptoticCalculator.h"

using namespace RooFit;
using namespace RooStats;

void Higgs_boson() {
    // Define the observable
    RooRealVar x("x", "invariant mass", 110, 135, "GeV");
    x.setBins(25);

    RooDataSet* data = RooDataSet::read("higgs_4l.dat", x);

    // Define a model: invarient mass (Crystal Ball distribution)
    RooRealVar mass("mass", "Higgs mass", 125, 110, 150);
    RooRealVar sigma("sigma", "Width", 4.1/2.35);
    RooRealVar alpha("alpha", "Alpha", 0.6);
    RooRealVar n("n", "Power", 20);
    RooCBShape signal("signal", "Signal PDF", x, mass, sigma, alpha, n);
    
    // Background model (2nd order polynomial)
    RooRealVar a1("a1", "a1", -160, -100, -200);
    RooRealVar a2("a2", "a2", 2.7, 2, 4);
    RooPolynomial background("background", "Background PDF", x, RooArgList(a1, a2));

    // Extended composite model
    RooRealVar nsig("nsig", "Signal events", 100, 0, 1000);
    RooRealVar nbkg("nbkg", "Background events", 1000, 0, 10000);
    RooAddPdf model("model", "Signal+Background", 
                   RooArgList(signal, background), 
                   RooArgList(nsig, nbkg));

    // Perform fit
    model.fitTo(*data, PrintLevel(-1));

    // Plot results
    RooPlot* frame1 = x.frame();
    data->plotOn(frame1);
    model.plotOn(frame1);
    model.plotOn(frame1, Components("signal"), LineColor(kRed));
    model.plotOn(frame1, Components("background"), LineColor(kGreen));

    TCanvas c1;
    frame1->Draw();
    c1.SaveAs("higgs_fit.png");

    // Create workspace and ModelConfig
    RooWorkspace w("w");
    w.import(model);
    w.import(*data);

    // Configure constants
    w.var("sigma")->setConstant(true);
    w.var("alpha")->setConstant(true);
    w.var("n")->setConstant(true);

    // ModelConfig for significance calculation
    RooStats::ModelConfig mc("ModelConfig", &w);
    mc.SetPdf(*w.pdf("model"));
    mc.SetParametersOfInterest(*w.var("nsig"));
    mc.SetObservables(*w.var("x"));
    w.defineSet("nuisParams", "nbkg,a1,a2");
    mc.SetNuisanceParameters(*w.set("nuisParams"));
    mc.SetSnapshot(*w.var("nsig"));
    w.import(mc);

    // Set mass as constant for mass vs significance
    w.var("mass")->setConstant(true);

    // ModelConfig for mass measurement
    RooStats::ModelConfig mc_mass("ModelConfig_mass", &w);
    mc_mass.SetPdf(*w.pdf("model"));
    mc_mass.SetParametersOfInterest(*w.var("mass"));
    w.defineSet("nuisParams2", "nbkg,a1,a2,nsig,sigma");
    mc_mass.SetNuisanceParameters(*w.set("nuisParams2"));
    mc_mass.SetSnapshot(*w.var("mass"));
    w.import(mc_mass);

    RooPlot* frame2 = w.var("x")->frame();
    data->plotOn(frame2);
    model.plotOn(frame2);
    model.plotOn(frame2, RooFit::Components("background"), RooFit::LineStyle(kDashed));

    TCanvas c2;
    frame2->Draw();
    c2.SaveAs("HiggsFit2.png");  


    w.writeToFile("HiggsModel.root");


    // Profile Likelihood Interval
    ProfileLikelihoodCalculator plc(*data, mc_mass);
    plc.SetConfidenceLevel(0.68);
    LikelihoodInterval* plInterval = plc.GetInterval();

    // LikelihoodIntervalPlot with the proper constructor
    LikelihoodIntervalPlot plPlot(plInterval);
    plPlot.SetRange(120, 128);  
    plPlot.SetMaximum(20);

    TCanvas c3("c3", "Profile Likelihood", 800, 600);
    plPlot.Draw();
    c3.SaveAs("profile_likelihood.png");


    // Feldman-Cousins Interval
    FeldmanCousins fc(*data, mc_mass);
    fc.SetConfidenceLevel(0.90);
    fc.UseAdaptiveSampling(true);
    fc.SetNBins(100);  
    fc.SetTestSize(0.10);

    // Calculate and retrieve interval
    PointSetInterval* fcInterval = fc.GetInterval();
}
