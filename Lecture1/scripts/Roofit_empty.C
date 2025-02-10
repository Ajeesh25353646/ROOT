#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "TAxis.h"
using namespace RooFit;

void Roofit_empty() {
    // Define variables
    RooRealVar x("x", "x", -10, 10);
    RooRealVar mean("mean", "mean of gaussian", 1, -10, 10);
    RooRealVar sigma("sigma", "width of gaussian", 1, 0.1, 10);

    // Create a Gaussian PDF
    RooGaussian gauss("gauss", "gaussian PDF", x, mean, sigma);

    // Construct the frame and plot the Gaussian PDF
    RooPlot* xframe = x.frame(Title("Gaussian PDF"));
    gauss.plotOn(xframe);

    
    sigma.setVal(4);
    gauss.plotOn(xframe, LineColor(kRed));

    // canvas to display the plot
    TCanvas* canvas1 = new TCanvas("canvas1", "Gaussian PDF", 800, 600);
    xframe->Draw();
    canvas1->Update(); 

    // Generate a dataset of 10000 events from the Gaussian PDF
    std::unique_ptr<RooDataSet> data{gauss.generate({x}, 10000)};

    // Create a new frame and plot the data
    RooPlot* xframe2 = x.frame(Title("Gaussian PDF with Data"));
    data->plotOn(xframe2);

    // Fit the Gaussian to the data and print results
    gauss.fitTo(*data, PrintLevel(-1));
    mean.Print();
    sigma.Print();

    // Draw the data plot with the fitted curve
    TCanvas* canvas2 = new TCanvas("canvas2", "Gaussian PDF with Data", 800, 600);
    gauss.plotOn(xframe2, LineColor(kBlue)); 
    xframe2->Draw();
    canvas2->Update();
}
