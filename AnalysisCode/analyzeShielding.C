// analyzeShielding.C
//
// RDataFrame + implicit multithreading analyzer for the B1_Gamma_Final
// shielding-simulation output (HDPE / Pb / Cu / inner-poly layered shield).
//
// Reads all files matching  <dir>/myEdep*.root  and produces log-binned
// TH1D spectra (500 bins, 1e-10 .. 1e4 MeV):
//
//   HdpeCount   -> HdpeKineticEnergy
//   LeadCount   -> LeadKineticEnergy
//   CopperCount -> CopperKineticEnergy
//   PolyCount   -> PolyKineticEnergy
//   TrackEdep   -> totalEdep
//
// The output file is named after the last subdirectory of the input path,
// e.g.  dir = /data/sim/Gamma1MeV  ->  Gamma1MeV.root  (in the CWD).
//
// Usage (ACLiC-compiled, recommended):
//   root -l -b -q 'analyzeShielding.C+("/data/sim/Gamma1MeV")'
// or interpreted:
//   root -l -b -q 'analyzeShielding.C("/data/sim/Gamma1MeV")'
//
// Memory safety: no naked new/delete. TChains and the output TFile are
// held by std::unique_ptr; the histograms are owned by RDataFrame's
// RResultPtr and are written before everything goes out of scope.

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RResultPtr.hxx>
#include <ROOT/RDFHelpers.hxx>   // ROOT::RDF::RunGraphs

#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

void analyzeShielding(std::string dirPath)
{
   // ------------------------------------------------------------------
   // 0. Start the clock; use all available cores
   // ------------------------------------------------------------------
   const auto tStart = std::chrono::steady_clock::now();

   ROOT::EnableImplicitMT();

   // ------------------------------------------------------------------
   // 1. Logarithmic binning: 500 bins from 1e-10 to 1e4 (MeV)
   // ------------------------------------------------------------------
   const int    nBins  = 500;
   const double loEdge = 1e-10;
   const double hiEdge = 1e4;

   std::vector<double> edges(nBins + 1);
   const double logLo = std::log10(loEdge);
   const double logHi = std::log10(hiEdge);
   for (int i = 0; i <= nBins; ++i)
      edges[i] = std::pow(10., logLo + (logHi - logLo) * i / nBins);

   // ------------------------------------------------------------------
   // 2. Input pattern and output-file name
   // ------------------------------------------------------------------
   while (!dirPath.empty() && dirPath.back() == '/')
      dirPath.pop_back();                       // strip trailing slashes

   if (dirPath.empty()) {
      std::cerr << "ERROR: empty directory path.\n";
      return;
   }

   const std::string pattern = dirPath + "/myEdep*.root";

   // last subdirectory ->  <lastSubdir>.root
   const auto        slash   = dirPath.find_last_of('/');
   const std::string outName =
      (slash == std::string::npos ? dirPath : dirPath.substr(slash + 1)) + ".root";

   // ------------------------------------------------------------------
   // 3. Book one histogram per (tree, branch)
   // ------------------------------------------------------------------
   struct Spec {
      const char *tree;
      const char *branch;
      const char *title;
   };
   const std::vector<Spec> specs = {
      {"HdpeCount",   "HdpeKineticEnergy",   "Kinetic energy entering HDPE;E [MeV];Entries"},
      {"LeadCount",   "LeadKineticEnergy",   "Kinetic energy entering Pb;E [MeV];Entries"},
      {"CopperCount", "CopperKineticEnergy", "Kinetic energy entering Cu;E [MeV];Entries"},
      {"PolyCount",   "PolyKineticEnergy",   "Kinetic energy entering polyethylene;E [MeV];Entries"},
      {"TrackEdep",   "totalEdep",           "Total energy deposited per track;E_{dep} [MeV];Entries"},
   };

   std::vector<std::unique_ptr<TChain>>          chains;   // must outlive the event loops
   std::vector<ROOT::RDF::RResultPtr<TH1D>>      results;
   std::vector<ROOT::RDF::RResultPtr<ULong64_t>> counts;   // lazy entry counters
   chains.reserve(specs.size());
   results.reserve(specs.size());
   counts.reserve(specs.size());

   for (const auto &s : specs) {
      auto chain = std::make_unique<TChain>(s.tree);
      const int nFiles = chain->Add(pattern.c_str());
      if (nFiles == 0) {
         std::cerr << "ERROR: no files match " << pattern << '\n';
         return;
      }
      std::cout << "Tree " << s.tree << ": chained " << nFiles << " file(s)\n";

      ROOT::RDataFrame df(*chain);
      const std::string hName = std::string("h_") + s.branch;

      // Book the entry count and the histogram on the same dataframe:
      // both are lazy, so they are filled in the SAME single event loop.
      counts.emplace_back(df.Count());

      // <double>: all five branches are stored as doubles by G4AnalysisManager
      results.emplace_back(
         df.Histo1D<double>({hName.c_str(), s.title, nBins, edges.data()}, s.branch));
      chains.emplace_back(std::move(chain));
   }

   // ------------------------------------------------------------------
   // 4. Run all five event loops concurrently (single call, all cores):
   //    counts + histograms are produced in one pass over the data
   // ------------------------------------------------------------------
   // NB: RResultHandle is not copy-assignable, so vector::insert() won't
   // compile; build the handle list with push_back (needs only construction).
   std::vector<ROOT::RDF::RResultHandle> handles;
   handles.reserve(results.size() + counts.size());
   for (auto &r : results) handles.push_back(r);
   for (auto &c : counts)  handles.push_back(c);
   ROOT::RDF::RunGraphs(handles);

   // Total number of entries processed per tree (from RDataFrame itself)
   ULong64_t grandTotal = 0;
   for (size_t i = 0; i < specs.size(); ++i) {
      std::cout << "Tree " << specs[i].tree << ": " << *counts[i]
                << " total entries\n";
      grandTotal += *counts[i];
   }
   std::cout << "Grand total entries processed: " << grandTotal << '\n';

   // ------------------------------------------------------------------
   // 5. Write everything out
   // ------------------------------------------------------------------
   std::unique_ptr<TFile> fout(TFile::Open(outName.c_str(), "RECREATE"));
   if (!fout || fout->IsZombie()) {
      std::cerr << "ERROR: cannot create output file " << outName << '\n';
      return;
   }

   for (auto &r : results) {
      fout->cd();
      r->Write();
      std::cout << "  wrote " << r->GetName()
                << "  (entries: " << static_cast<long long>(r->GetEntries()) << ")\n";
   }

   fout->Close();   // unique_ptr also deletes the TFile afterwards
   std::cout << "Output written to " << outName << '\n';

   // ------------------------------------------------------------------
   // 6. Stop the clock
   // ------------------------------------------------------------------
   const auto tEnd = std::chrono::steady_clock::now();
   const double seconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count() / 1000.;
   std::cout << "Total wall time: " << seconds << " s\n";
}
