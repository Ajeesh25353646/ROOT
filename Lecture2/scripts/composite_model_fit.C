#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooWorkspace.h"
#include "RooMinimizer.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "TFile.h"

using namespace RooFit;

void composite_model_fit() {
    // Create observable
    RooRealVar x("x", "x", -20, 20);
    
    // Create parameters
    RooRealVar mean("mean", "mean", 0);
    RooRealVar s1("s1", "sigma1", 3.2);
    RooRealVar s2("s2", "sigma2", 4, 3, 6);
    RooRealVar f("f", "coefficient", 0.5, 0, 1);

    // Create PDF components
    RooGaussian gauss1("gauss1", "gaussian1", x, mean, s1);
    RooGaussian gauss2("gauss2", "gaussian2", x, mean, s2);

    // Create composite model
    RooAddPdf model("model", "model", RooArgList(gauss1, gauss2), RooArgList(f));

    // Generate dataset
    RooDataSet* data = model.generate(x, 1000);

    // Create workspace and save components
    RooWorkspace w("w", "workspace");
    w.import(model);
    w.import(*data);
    w.writeToFile("workspace_model.root");

    // Construct likelihood
    RooAbsReal* nll = model.createNLL(*data);

    // Minimize nllw.r.t its parameters
    RooMinimizer minim(*nll) ; 
    minim.setVerbose(true);
    minim.migrad();
    
    // Print initial parameters
    f.Print();
    mean.Print();
    s1.Print();
    s2.Print();

    // HESSE errors
    minim.setVerbose(false);
    minim.hesse();
    
    // Print parameters with HESSE errors
    f.Print();
    mean.Print();
    s1.Print();

    // MINOS errors for s2
    minim.minos(s2);
    s2.Print();

    // Save and visualize results
    RooFitResult* result = minim.save();
    result->Print("v");

    // Correlation matrix visualization
    TCanvas* c_corr = new TCanvas("c_corr", "Correlation Matrix", 800, 600);
    gStyle->SetPalette(1);
    result->correlationHist()->Draw("colz");
    c_corr->SaveAs("correlation_matrix.png");


    
    // Create contour plot (f vs s2)
    TCanvas* c_counter = new TCanvas("c_counter", "Contour Plot", 600, 600);
    RooPlot* frame = minim.contour(f, s2, 1, 2, 3);  // 1σ, 2σ, 3σ
    frame->SetTitle("Confidence Contours (f vs s2)");
    frame->Draw();
    c_counter->SaveAs("contour_plot.png");

    // Print final parameters
    f.Print();
    mean.Print();
    s1.Print();
    s2.Print();

    // /// ... [previous code remains unchanged]

    // // Save fit results
    // RooFitResult* result = minim.save();
    // result->Print("v");  // Verbose printout as required

    // // Visualize correlation matrix
    // TCanvas* c1 = new TCanvas("c1", "Correlation Matrix", 800, 600);
    // gStyle->SetPalette(1);  // REQUIRED: Set color palette
    // result->correlationHist()->Draw("colz");
    // c1->SaveAs("correlation_matrix.png");

    // // Save correlation matrix to ROOT file
    // TFile output_file("correlation_matrix.root", "RECREATE");
    // result->correlationHist()->Write("correlationMatrix");
    // output_file.Close();

    // // 7. Contour Plot ========================================================
    // minim.SetErrorDef(0.5);   // 0.5 is for likelihood
    
    // TCanvas* c_counter = new TCanvas("c2", "Contour Plot", 800, 600);
    // RooPlot* frame = minim.contour(f, s2, 1, 2, 3);  // 1σ, 2σ, 3σ
    
    // // Configure plot appearance
    // frame->SetTitle("Confidence Contours (f vs s2);f;s2 [MeV]");
    // frame->SetLineWidth(2);
    // frame->GetYaxis()->SetTitleOffset(1.2);
    
    // frame->Draw();
    // c_counter ->SaveAs("contour_plot.png");

}

