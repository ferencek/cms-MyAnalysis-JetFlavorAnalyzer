from FWCore.ParameterSet.VarParsing import VarParsing

###############################
####### Parameters ############
###############################

options = VarParsing ('python')

options.register('reportEvery', 10,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.int,
    "Report every N events (default is N=1)"
)
options.register('wantSummary', False,
    VarParsing.multiplicity.singleton,
    VarParsing.varType.bool,
    "Print out trigger and timing summary"
)

## 'maxEvents' is already registered by the Framework, changing default value
options.setDefault('maxEvents', 1000)

options.parseArguments()


import FWCore.ParameterSet.Config as cms

process = cms.Process("USER")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = options.reportEvery
process.MessageLogger.cerr.default.limit = 10

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(options.wantSummary) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        # Pythia6:
        # /TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/Summer12_DR53X-PU_S10_START53_V7C-v1/AODSIM
        #'/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/AODSIM/PU_S10_START53_V7C-v1/00000/FE7C71D8-DB25-E211-A93B-0025901D4C74.root'
        # /QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM
        #'/store/mc/Summer12_DR53X/QCD_Pt-15to3000_TuneZ2star_Flat_8TeV_pythia6/AODSIM/PU_S10_START53_V7A-v1/0000/004CB136-A1D3-E111-B958-0030487E4B8D.root'

        # Pythia8:
        # /GluGluToHToBB_M-125_8TeV-powheg-pythia8/Summer12_DR53X-PU_S10_START53_V7C-v1/AODSIM
        #'/store/mc/Summer12_DR53X/GluGluToHToBB_M-125_8TeV-powheg-pythia8/AODSIM/PU_S10_START53_V7C-v1/10000/06C64678-3FA1-E211-91A3-002618943970.root'

        # Herwig++:
        # /ZH_ZToLL_HToBB_M-125_8TeV-powheg-herwigpp/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM
        '/store/mc/Summer12_DR53X/ZH_ZToLL_HToBB_M-125_8TeV-powheg-herwigpp/AODSIM/PU_S10_START53_V7A-v1/0000/00B4DEBF-CEF5-E111-A2EE-00215E20452C.root'

        # Herwig6:
        # /TT_CT10_AUET2_8TeV-powheg-herwig/Summer12_DR53X-PU_S10_START53_V19-v1/AODSIM
        #'/store/mc/Summer12_DR53X/TT_CT10_AUET2_8TeV-powheg-herwig/AODSIM/PU_S10_START53_V19-v1/10000/0039166F-C9CA-E211-9B10-0026189438E0.root'
    )
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('histos.root')
)

#-------------------------------------------------------------------------
# AK5 jets
#-------------------------------------------------------------------------
from PhysicsTools.JetMCAlgos.HadronAndPartonSelector_cfi import selectedHadronsAndPartons
process.selectedHadronsAndPartons = selectedHadronsAndPartons.clone(particles=cms.InputTag("genParticles"))

from PhysicsTools.JetMCAlgos.AK5PFJetsMCFlavourInfos_cfi import ak5JetFlavourInfos
process.jetFlavourInfosAK5PFJets = ak5JetFlavourInfos.clone(jets=cms.InputTag("ak5PFJets"))
#-------------------------------------------------------------------------

process.jetFlavorAnalyzer = cms.EDAnalyzer('JetFlavorAnalyzer',
    src = cms.InputTag("genParticles"),
    jetFlavourInfos = cms.InputTag("jetFlavourInfosAK5PFJets"),
    jetPtMin = cms.double(30.)
)

process.p = cms.Path(
    process.selectedHadronsAndPartons
    *process.jetFlavourInfosAK5PFJets
    *process.jetFlavorAnalyzer
)
