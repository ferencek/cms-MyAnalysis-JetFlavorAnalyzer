// -*- C++ -*-
//
// Package:    JetFlavorAnalyzer
// Class:      JetFlavorAnalyzer
// 
/**\class JetFlavorAnalyzer JetFlavorAnalyzer.cc MyAnalysis/JetFlavorAnalyzer/src/JetFlavorAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Dinko Ferencek
//         Created:  Fri Sep 19 16:22:48 CDT 2014
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "PhysicsTools/JetMCUtils/interface/CandMCTag.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourInfo.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourInfoMatching.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// class declaration
//

class JetFlavorAnalyzer : public edm::EDAnalyzer {
   public:
      explicit JetFlavorAnalyzer(const edm::ParameterSet&);
      ~JetFlavorAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      // ----------member data ---------------------------
      const edm::InputTag src_;
      const edm::InputTag jetFlavourInfos_;
      const double        jetPtMin_;

      edm::Service<TFileService> fs;

      TH1F *h1_status_StatusSelection;
      TH1F *h1_pdgId_StatusSelection;
      TH1F *h1_status_LastGeneration;
      TH1F *h1_pdgId_LastGeneration;
      TH1F *h1_jetFlavor;
};

//
// constructors and destructor
//
JetFlavorAnalyzer::JetFlavorAnalyzer(const edm::ParameterSet& iConfig) :

  src_(iConfig.getParameter<edm::InputTag>("src")),
  jetFlavourInfos_(iConfig.getParameter<edm::InputTag>("jetFlavourInfos")),
  jetPtMin_(iConfig.getParameter<double>("jetPtMin"))

{
   //now do what ever initialization is needed
   h1_status_StatusSelection = fs->make<TH1F>("h1_status_StatusSelection",";Status;",101,-0.5,100.5);
   h1_pdgId_StatusSelection = fs->make<TH1F>("h1_pdgId_StatusSelection",";PDG ID;",31,-0.5,30.5);
   h1_status_LastGeneration = fs->make<TH1F>("h1_status_LastGeneration",";Status;",101,-0.5,100.5);
   h1_pdgId_LastGeneration = fs->make<TH1F>("h1_pdgId_LastGeneration",";PDG ID;",31,-0.5,30.5);
   h1_jetFlavor = fs->make<TH1F>("h1_jetFlavor",";Jet flavor;",31,-0.5,30.5);
}


JetFlavorAnalyzer::~JetFlavorAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
JetFlavorAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   edm::Handle<edm::View<reco::GenParticle> > src;
   iEvent.getByLabel(src_, src);

   for(edm::View<reco::GenParticle>::const_iterator it = src->begin(); it != src->end(); ++it)
   {
     if( !(it->status()==2 || it->status()==71 || it->status()==72) ) continue; // only accept status==2,71,72 particles
     if( !CandMCTagUtils::isParton( *it ) ) continue;                           // skip particle if not a parton

     h1_status_StatusSelection->Fill(it->status());
     h1_pdgId_StatusSelection->Fill(abs(it->pdgId()));
   }

   for(edm::View<reco::GenParticle>::const_iterator it = src->begin(); it != src->end(); ++it)
   {
     if( it->status()==1 ) continue;                   // skip stable particles
     if( !CandMCTagUtils::isParton( *it ) ) continue;  // skip particle if not a parton

     // check if the parton has other partons as daughters
     unsigned nparton_daughters = 0;
     for(unsigned i=0; i<it->numberOfDaughters(); ++i)
     {
       if( CandMCTagUtils::isParton( *(it->daughter(i)) ) )
         ++nparton_daughters;
     }

     if( nparton_daughters==0 )
     {
       h1_status_LastGeneration->Fill(it->status());
       h1_pdgId_LastGeneration->Fill(abs(it->pdgId()));
     }
   }

   edm::Handle<reco::JetFlavourInfoMatchingCollection> theJetFlavourInfos;
   iEvent.getByLabel(jetFlavourInfos_, theJetFlavourInfos);

   for ( reco::JetFlavourInfoMatchingCollection::const_iterator j = theJetFlavourInfos->begin(); j != theJetFlavourInfos->end(); ++j )
   {
     const reco::Jet *aJet = (*j).first.get();
     reco::JetFlavourInfo aInfo = (*j).second;

     if( aJet->pt()<jetPtMin_ ) continue;

     h1_jetFlavor->Fill(abs(aInfo.getPartonFlavour()));
   }
}


// ------------ method called once each job just before starting event loop  ------------
void 
JetFlavorAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
JetFlavorAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
JetFlavorAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
JetFlavorAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
JetFlavorAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
JetFlavorAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
JetFlavorAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(JetFlavorAnalyzer);
