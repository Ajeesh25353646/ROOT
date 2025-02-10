#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooPlot.h"
using namespace RooFit;

void basics(){

    //setup model

    //Declare variables x, mean, sigma with associated name, title, intital value and allowed range
    RooRealVar x("x", "x", -10, 10);
    RooRealVar mean("mean", "mean of gaussian", 1, -10, 10);
    RooRealVar sigma("sigma", "width of gaussian", 1, 0.1, 10);

    // Build gaussian pdf in terms of x, mean and sigma
    RooGaussian gauss("gauss", "gaussian PDF", x, mean, sigma);

    //Construct plot frame in x
    RooPlot *frame = x.frame(Title("Gaussian pdf."));

    //Plot model and change parameter values

    // Plot gauss in frame (i.e. in x)
    gauss.plotOn(frame);

    // Draw pdf on canvas
    frame->Draw();

}