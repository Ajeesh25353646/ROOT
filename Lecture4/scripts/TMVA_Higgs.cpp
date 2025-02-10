#include "TFile.h"
#include "TTree.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"

void TMVA_Higgs() {
    TMVA::Tools::Instance();

    TFile* resultsFile = TFile::Open("TMVA_Higgs_Results.root", "RECREATE");

    TMVA::Factory factory("TMVAClassification", resultsFile, "!V:!Silent:Color:DrawProgressBar");
    TMVA::DataLoader loader("dataset");

    TFile* sigFile = TFile::Open("atlas-higgs-challenge-2014-v2-sig.root");
    TFile* bkgFile = TFile::Open("atlas-higgs-challenge-2014-v2-bkg.root");

    TTree* sigTree = (TTree*)sigFile->Get("tree");
    TTree* bkgTree = (TTree*)bkgFile->Get("tree");

    loader.AddSignalTree(sigTree, 1.0);
    loader.AddBackgroundTree(bkgTree, 1.0);

    loader.AddVariable("mass_MMC", 'F');
    loader.AddVariable("mass_transverse", 'F');
    loader.AddVariable("mass_visible", 'F');
    loader.AddVariable("pt_higgs", 'F');
    loader.AddVariable("deltaeta_jets", 'F');
    loader.AddVariable("tau_pt", 'F');

    loader.SetSignalWeightExpression("Weight");
    loader.SetBackgroundWeightExpression("Weight");

    loader.PrepareTrainingAndTestTree("", "", "nTrain_Signal=10000:nTrain_Background=20000:nTest_Signal=0:nTest_Background=0:SplitMode=Random:NormMode=NumEvents:!V");

    factory.BookMethod(&loader, TMVA::Types::kCuts, "Cuts", "!H:!V:FitMethod=GA:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:VarProp=NotEnforced");

    factory.BookMethod(&loader, TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None");

    factory.BookMethod(&loader, TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS");

    factory.BookMethod(&loader, TMVA::Types::kBDT, "BDT", "!H:!V:NTrees=200:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20");

    factory.TrainAllMethods();
    factory.TestAllMethods();
    factory.EvaluateAllMethods();

    resultsFile->Close();

    TMVA::TMVAGui("TMVA_Higgs_Results.root");
}