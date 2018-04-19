void rootlogon() {
  gSystem -> Load("DataStruct.so");

  gROOT -> ProcessLine(".x environment.C");
}
