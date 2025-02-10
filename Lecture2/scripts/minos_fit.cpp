#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooDataHist.h>
#include <RooHistFunc.h>
#include <RooFormulaVar.h>
#include <RooGenericPdf.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <TCanvas.h>
#include <iostream>

void minos_fit() {
    // Define Variables
    RooRealVar energy("energy", "Neutrino Energy", 0.5, 14, "GeV");
    energy.setBins(27);

    // Read Data
    RooDataSet* data = RooDataSet::read("minos_2013_data.dat", energy, "v");

    // Create Non-Oscillated Model
    RooDataSet* mcdata = RooDataSet::read("minos_2013_mc.dat", energy, "v");
    RooDataSet* reduced_mcdata = (RooDataSet*)mcdata->reduce(RooArgSet(energy));
    RooDataHist* dh_mc_noosc = reduced_mcdata->binnedClone();
    RooHistFunc func_noosc("func_mc_noosc", "No oscillation", energy, *dh_mc_noosc);

    // Define Oscillation Probability
    RooRealVar mixing("mixing", "Mixing", 0.9, 0.0, 1.0);
    RooRealVar dm2("dm2", "#nu square mass difference", 2.4e-3, 0.0, 5e-3, "eV^{2}"); // Note unit fix
    RooFormulaVar prob_osc("prob_osc", "1 - @0 * sin(1.267 * @1 * 735 / @2)^2", RooArgList(mixing, dm2, energy));

    // Combine with Non-Oscillated Model (FIXED)
    RooAbsReal* integral_noosc = func_noosc.createIntegral(energy);
    RooGenericPdf model("model", "model", "@0 * @1 / @2", 
                        RooArgList(prob_osc, func_noosc, *integral_noosc));

    // Fit the Model to Data
    RooFitResult* fitResult = model.fitTo(*data, RooFit::Save());

    // Plot Data and Model
    RooPlot* frame = energy.frame(RooFit::Title("MINOS Data Fit"));
    data->plotOn(frame);
    model.plotOn(frame);

    // Save the Plot
    TCanvas* c = new TCanvas("c", "MINOS Fit", 800, 600);
    frame->Draw();
    c->SaveAs("minos_data.png");

    // Build likelihood function
    RooAbsReal* nll = model.createNLL(*data);
    
    // Configure MINUIT
    RooMinimizer m(*nll);
    m.setVerbose(true);
    
    // Minimize likelihood
    m.migrad();
    std::cout << "\n[A] MIGRAD results:\n";
    mixing.Print();
    dm2.Print();

    // HESSE errors
    m.setVerbose(false);
    m.hesse();
    std::cout << "\n[B] HESSE results:\n";
    mixing.Print();
    dm2.Print();

    // MINOS errors
    m.minos(dm2);
    std::cout << "\n[C] MINOS results:\n";
    dm2.Print();

     // MINOS errors
    m.minos(dm2);
    std::cout << "\n[C] MINOS results:\n";
    dm2.Print();

    // Save final state
    RooFitResult* res = m.save();
    std::cout << "\n[D] Full results:\n";
    res->Print("v");

    // Confidence contours
    TCanvas* c2 = new TCanvas("c2", "Contours", 800, 600);  
    RooPlot* contour_frame = m.contour(dm2, mixing, 1, 2, 3);

    // Configure axes properly
    contour_frame->SetTitle("Neutrino Oscillation Contours;Δm^{2} [eV^{2}];sin^{2}(2θ)");  
    contour_frame->GetYaxis()->SetRangeUser(mixing.getMin(), mixing.getMax());  

    // Style adjustments
    contour_frame->SetLineWidth(2);
    contour_frame->SetFillStyle(0);  

    // Draw and save
    contour_frame->Draw();
    c2->SaveAs("minos_contours.png");  
}              