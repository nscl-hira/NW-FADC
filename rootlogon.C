void rootlogon() {
  gSystem -> Load("lib/libDataStruct.so");

  gROOT -> ProcessLine(".x environment.C");
}



