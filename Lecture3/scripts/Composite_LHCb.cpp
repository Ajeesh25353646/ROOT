#include "TFile.h"
#include "TCanvas.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooHist.h"
#include "RooCategory.h"
#include "RooSimultaneous.h"
#include "TStyle.h"

using namespace RooFit;

void Composite_LHCb() {
    // Observable 
    RooRealVar mass_obs("mass_obs", "Invariant Mass", 5080, 5550, "MeV/c²");
    mass_obs.setBins(25);
    
    // Load experimental measurements
    RooDataSet* exp_data = RooDataSet::read("rarest_b0_decay.dat", mass_obs, "D");

    // Background parameters
    RooRealVar bkg_param("bkg_param", "Background slope", -0.0001, -1, -0.000001);
    RooExponential bkg_model("bkg_model", "Background PDF", mass_obs, bkg_param);

    // B⁰ signal parameters
    RooRealVar B0_center("B0_center", "B⁰ mass", 5279, 5220, 5320);
    RooRealVar peak_sigma("peak_sigma", "Resolution", 10, 2, 50);
    RooGaussian B0_signal("B0_signal", "B⁰ PDF", mass_obs, B0_center, peak_sigma);

    // Bₛ⁰ signal parameters
    RooRealVar Bs0_center("Bs0_center", "Bₛ⁰ mass", 5380, 5320, 5420);
    RooGaussian Bs0_signal("Bs0_signal", "Bₛ⁰ PDF", mass_obs, Bs0_center, peak_sigma);

    // Composite Model 
    RooRealVar B0_count("B0_count", "B⁰ events", 100, 0, 1000);
    RooRealVar Bs0_count("Bs0_count", "Bₛ⁰ events", 800, 0, 1000);
    RooRealVar bkg_count("bkg_count", "Background events", 20, 0, 1000);

    RooAddPdf total_pdf("total_pdf", "Full model",
                       RooArgList(B0_signal, Bs0_signal, bkg_model),
                       RooArgList(Bs0_count, B0_count, bkg_count));

    // Primary Fit Procedure
    auto fit_outcome = total_pdf.fitTo(*exp_data, RooFit::Save());

    // Visualization Components 
    auto mass_frame = mass_obs.frame();
    exp_data->plotOn(mass_frame);
    total_pdf.plotOn(mass_frame);

    // Residual analysis components
    RooHist* res_plot = mass_frame->residHist();
    RooHist* pull_plot = mass_frame->pullHist();
    
    auto res_frame = mass_obs.frame();
    res_frame->addPlotable(res_plot, "P");
    
    auto pull_frame = mass_obs.frame();
    pull_frame->addPlotable(pull_plot, "P");

    // Graphical Output
    TCanvas main_canvas("main_canvas", "Analysis Results", 1200, 800);
    main_canvas.Divide(1,3);
    
    main_canvas.cd(1);
    mass_frame->Draw();
    
    main_canvas.cd(2);
    res_frame->Draw();
    
    main_canvas.cd(3);
    pull_frame->Draw();
    
    main_canvas.SaveAs("analysis_output.png");

    //  Validation Region Setup
    RooRealVar val_obs("val_obs", "Validation observable", 4000, 5000);
    RooExponential val_pdf("val_pdf", "Validation PDF", val_obs, bkg_param);
    auto val_data = val_pdf.generate(val_obs, 10000);

    // Combined Analysis 
    RooCategory data_cat("data_cat", "");
    data_cat.defineType("physics");
    data_cat.defineType("control");

    // Create combined dataset
    RooDataSet combData("combData", "Combined Dataset",
                       RooArgSet(mass_obs, val_obs),
                       Index(data_cat),
                       Import("physics", *exp_data),
                       Import("control", *val_data));

    //  Create simultaneous PDF
    RooSimultaneous simPdf("simPdf", "Simultaneous PDF", data_cat);
    simPdf.addPdf(total_pdf, "physics");
    simPdf.addPdf(val_pdf, "control");

    // Perform simultaneous fit
    simPdf.fitTo(combData);

    // Create validation plots
    TCanvas val_canvas("val_canvas", "Validation Results", 1200, 600);
    val_canvas.Divide(2,1);

    // Physics region plot
    auto physics_frame = mass_obs.frame(Title("Physics Region"));
    combData.plotOn(physics_frame, Cut("data_cat==data_cat::physics"));
    simPdf.plotOn(physics_frame, Slice(data_cat, "physics"), ProjWData(data_cat, combData));

    // Control region plot
    auto control_frame = val_obs.frame(Title("Control Region")); 
    combData.plotOn(control_frame, Cut("data_cat==data_cat::control"));
    simPdf.plotOn(control_frame, Slice(data_cat, "control"), ProjWData(data_cat, combData));

    // Draw and save
    val_canvas.cd(1);
    physics_frame->Draw();
    
    val_canvas.cd(2);
    control_frame->Draw();
    
    val_canvas.SaveAs("combined_fit_results.png");
}