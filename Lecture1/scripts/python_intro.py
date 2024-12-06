import ROOT

x = ROOT.RooRealVar("x", "x", -10, 10)
mean = ROOT.RooRealVar("mean", "mean of gaussian", 1, -10, 10)
sigma = ROOT.RooRealVar("sigma", "wdth of gaussian", 2, 0.1, 10)

gauss = ROOT.RooGaussian("gauss", "gaussian PDF", x, mean, sigma)

xframe = x.frame(Title="Gaussian pdf")

gauss.plotOn(xframe)
