
// -*- C++ -*-
//
// Package:    QCD/DeltaPhiQCD
// Class:      DeltaPhiQCD
// 

//
// Original Author:  Amin Ghiasi
//       Created:  Sun, 6 Sep 2015 20:08 GMT
//

#include <memory>
#include <vector>
#include "FWCore/Utilities/interface/InputTag.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include "TLorentzVector.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include <DataFormats/Math/interface/deltaR.h>
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/METReco/interface/MET.h"


//
// class declaration
//

class DeltaPhiQCD : public edm::EDProducer {
public:
      explicit DeltaPhiQCD ( const edm::ParameterSet& ) ;
      ~DeltaPhiQCD();
      
      static void fillDescriptions ( edm::ConfigurationDescriptions& descriptions ) ;
      
private:
      virtual void beginJob () ;
      virtual void produce(edm::Event&, const edm::EventSetup&);
      virtual void endJob () ;
      
      virtual void beginRun( edm::Run&, edm::EventSetup const& ) ;
      virtual void endRun( edm::Run&, edm::EventSetup const& ) ;
      virtual void beginLuminosityBlock( edm::LuminosityBlock&, edm::EventSetup const& ) ;
      virtual void endLuminosityBlock( edm::LuminosityBlock&, edm::EventSetup const& ) ;
      edm::InputTag JetTagRecoJets_ ;
      edm::InputTag JetTagGenJets_ ;
      std::string btagname_ ;      
      edm::InputTag GenParticleTag_ ;
      edm::InputTag metTag_ ;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//
//

//
// constructors and destructor
//
DeltaPhiQCD::DeltaPhiQCD ( const edm::ParameterSet& iConfig )
{


      //register your product
      JetTagRecoJets_ = iConfig.getParameter<edm::InputTag> ( "JetTagRecoJets" ) ;
      btagname_       = iConfig.getParameter<std::string>   ( "BTagInputTag"   ) ;
      JetTagGenJets_  = iConfig.getParameter<edm::InputTag> ( "JetTagGenJets"  ) ;
      GenParticleTag_ = iConfig.getParameter<edm::InputTag> ( "GenParticleTag" ) ;
      metTag_       = iConfig.getParameter<edm::InputTag> ( "GenMETTag"      ) ;


      produces < std::vector <TLorentzVector> > ( "RJetLorentzVector" ) ;
      produces < std::vector < double > > ( "RJetCSV" ) ;
      produces < std::vector < double > > ( "RJetPartonFlavor" ) ;
      produces < std::vector < double > > ( "RJetDeltaPhi" ) ;

      produces < double > ( "GenMHT" ) ;
      produces < double > ( "GenMHTphi" ) ;

      produces < double > ( "GenMET" ) ;
      produces < double > ( "GenMETphi" ) ;

      produces < std::vector <TLorentzVector> > ( "GenJetLorentzVector" ) ;
      produces < std::vector < double > > ( "GenJetCSV" ) ;
      produces < std::vector < double > > ( "GenJetPartonFlavor" ) ;
      produces < std::vector < double > > ( "GenJetDeltaPhi" ) ;

      produces < std::vector < TLorentzVector > > ( "NeutrinoLorentzVector" ) ;
      produces < std::vector < int > > ( "NeutrinoPdg" ) ;
      produces < std::vector < int > > ( "NeutrinoMotherPdg" ) ;


      produces < std::vector < double > >      ( "minDeltaPhiEta24" ) ;
      produces < std::vector < std::string > > ( "minDeltaPhiEta24Names" ) ;
      produces < std::vector < double > >      ( "minDeltaPhiEta5" ) ;
      produces < std::vector < std::string > > ( "minDeltaPhiEta5Names" ) ;

}


DeltaPhiQCD::~DeltaPhiQCD()
{
      
      // do anything here that needs to be done at desctruction time
      // (e.g. close files, deallocate resources etc.)
      
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void DeltaPhiQCD::produce ( edm::Event& iEvent, const edm::EventSetup& iSetup )
{

      using namespace edm;
      using namespace std;

      std::vector < TLorentzVector > JetVector;
      std::vector < double >  csv_vector ;
      std::vector < double >  partonflavor_vector ;
      std::vector < double >  deltaphi_vector ;

      JetVector        .clear() ;
      csv_vector       .clear() ;
      partonflavor_vector.clear() ;
      deltaphi_vector    .clear() ;

      edm::Handle < double > var ;

      std::vector < double >  minDeltaphi5,minDeltaphi24;
      std::vector < std::string >  minDeltaphi5Names, minDeltaphi24Names;

//      edm::InputTag MHT_Pt("MHT","Pt");
//      iEvent.getByLabel(MHT_Pt,var);
//      double mhtPt = *var;

      edm::InputTag MHT_Phi ( "MHT" , "Phi" ) ;
      iEvent.getByLabel ( MHT_Phi , var ) ;
      double mhtphi = *var ;

      double savephi [8][2], saveeta [8][2] ;

      edm::Handle < edm::View < pat::Jet > > src ;
      iEvent.getByLabel ( JetTagRecoJets_ , src ) ;
      double  recojetspt = -99. , recojetseta = -99. , recojetsphi = -99. , recojetscsv = -99. , deltaphi = -99. , partonflavor = 0. ;

      std::string pt, eta , phi, btag, dphi, partonflavor_str ;

      reco::MET::LorentzVector mhtLorentzstar ( 0,0,0,0 ) ;

      double mindeltaphi3 = -9, mindeltaphi4 = -9, mindeltaphi5 = -9 ;
      double mindeltaphistar = -9, deltaphistar = -9 ;
      double etacut ;
      int mindeltaphi3jetindex = -9, mindeltaphi4jetindex = -9, mindeltaphi5jetindex = -9, mindeltaphistarindex = -9 ; 
      std::string name ;


      for ( unsigned int ii = 0; ii < 2; ii++ )
      {
            if ( ii == 0 ) etacut = 5 ;
            if ( ii == 1 ) etacut = 2.4 ;
            if ( src.isValid() )
            {

            mindeltaphi3 = -9; mindeltaphi4 = -9; mindeltaphi5 = -9; mindeltaphi3jetindex = -9; mindeltaphi4jetindex = -9; mindeltaphi5jetindex = -9;

            unsigned int i = 0 ;
            for ( unsigned int index = 0; index < 8; ++index )
            {
                  if ( i < src -> size() )
                        while ( src -> at(i).pt() < 30 || src -> at(i).eta() > etacut || src -> at(i).eta() < (-1)*etacut ) 
                        {
                              i++ ;
                              if ( i >= src -> size())  break ;
                        }//while

                  if ( i < src -> size() )
                  {
                        recojetspt  = src -> at(i).pt() ;
                        recojetsphi = src -> at(i).phi() ;
                        recojetseta = src -> at(i).eta() ;
                        recojetscsv = src -> at(i).bDiscriminator(btagname_) ;
                        deltaphi = std::abs( reco::deltaPhi( src -> at(i).phi() , mhtphi ) ) ;
                        partonflavor = src -> at(i).partonFlavour();

                        if (std::abs( mindeltaphi3 ) > std::abs( deltaphi ) && index < 3 ) { mindeltaphi3 = deltaphi; mindeltaphi3jetindex = index+1; }
                        if (std::abs( mindeltaphi4 ) > std::abs( deltaphi ) && index < 4 ) { mindeltaphi4 = deltaphi; mindeltaphi4jetindex = index+1; }
                        if (std::abs( mindeltaphi5 ) > std::abs( deltaphi ) && index < 5 ) { mindeltaphi5 = deltaphi; mindeltaphi5jetindex = index+1; }
                        if ( etacut == 5 ) 
                        {
                              TLorentzVector dumb_vector;
                              dumb_vector.SetPtEtaPhiE( recojetspt, recojetseta, recojetsphi, src -> at(i).energy() );
                              JetVector.push_back( dumb_vector ) ;
                              csv_vector.push_back ( recojetscsv ) ;
                              partonflavor_vector.push_back ( partonflavor ) ;
                              deltaphi_vector.push_back ( deltaphi ) ;
                        }
                        
                  }//if i
                  else
                  {
                        recojetspt = -9 ;
                        recojetsphi = -9 ;
                        recojetseta = -9 ;
                        recojetscsv = -9 ;
                        deltaphi = -9 ;
                        partonflavor = -9 ;

                  }//else

                  savephi [index][ii] = recojetsphi ; 
                  saveeta [index][ii] = recojetseta ;
                  i++ ;

            }//index


            if ( etacut == 5 )
            {

                  std::auto_ptr < std::vector < TLorentzVector >  > JetVector2  ( new std::vector < TLorentzVector > (JetVector) ) ;
                  iEvent.put ( JetVector2 , "RJetLorentzVector" ) ;

                  std::auto_ptr < std::vector < double > > csv_vector2 ( new std::vector < double > ( csv_vector ) ) ;
                  iEvent.put ( csv_vector2 , "RJetCSV" ) ;

                  std::auto_ptr < std::vector < double > > partonflavor_vector2  ( new std::vector < double > ( partonflavor_vector ) ) ;
                  iEvent.put ( partonflavor_vector2 , "RJetPartonFlavor" ) ;

                  std::auto_ptr < std::vector < double > > deltaphi_vector2  ( new std::vector < double > ( deltaphi_vector ) ) ;
                  iEvent.put ( deltaphi_vector2 , "RJetDeltaPhi" ) ;

            }


            mindeltaphistar = -9; mindeltaphistarindex = -9;
            for ( unsigned int i = 0; i < src -> size(); i++ )
            {
                  mhtLorentzstar.SetPxPyPzE ( 0,0,0,0 ) ;
                  for( unsigned int j = 0; j < src -> size(); j++ )
                        if ( src -> at(j).pt() >= 30 && src -> at(j).eta() <= etacut && src -> at(j).eta() >= (-1) * etacut && j != i) mhtLorentzstar -= src->at(j).p4();
                  deltaphistar = std::abs( reco::deltaPhi( src->at(i).phi(), mhtLorentzstar.phi() ) );
                  if ( std::abs( mindeltaphistar ) > std::abs( deltaphistar ) ) { mindeltaphistar = deltaphistar; mindeltaphistarindex = i; }
            } //i

            }//if vsrc.isValid
            else 
                  std::cout << "Warning Reco Tag not valid: " << JetTagRecoJets_.label() << std::endl ;

            if ( etacut == 5 ) 
            { 

                  minDeltaphi5Names.push_back ( "RJetMinDeltaPhiStarEta5"        ) ; minDeltaphi5.push_back ( mindeltaphistar      ) ;
                  minDeltaphi5Names.push_back ( "RJetMinDeltaPhiStarIndexEta5"     ) ; minDeltaphi5.push_back ( mindeltaphistarindex ) ;
      
                  minDeltaphi5Names.push_back ( "RJetMinDeltaPhiEta5Njle3"       ) ; minDeltaphi5.push_back ( mindeltaphi3       ) ;
                  minDeltaphi5Names.push_back ( "RJetMinDeltaPhiJetIndexEta5Njle3" ) ; minDeltaphi5.push_back ( mindeltaphi3jetindex ) ;
      
                  minDeltaphi5Names.push_back ( "RJetMinDeltaPhiEta5Njle4"       ) ; minDeltaphi5.push_back ( mindeltaphi4       ) ;
                  minDeltaphi5Names.push_back ( "RJetMinDeltaPhiJetIndexEta5Njle4" ) ; minDeltaphi5.push_back ( mindeltaphi4jetindex ) ;
      
                  minDeltaphi5Names.push_back ( "RJetMinDeltaPhiEta5Njle5"       ) ; minDeltaphi5.push_back ( mindeltaphi5       ) ;
                  minDeltaphi5Names.push_back ( "RJetMinDeltaPhiJetIndexEta5Njle5" ) ; minDeltaphi5.push_back ( mindeltaphi5jetindex ) ;

            }


            if ( etacut == 2.4 ) 
            {

                  minDeltaphi24Names.push_back ( "RJetMinDeltaPhiStarEta24"        ) ; minDeltaphi24.push_back ( mindeltaphistar      ) ;
                  minDeltaphi24Names.push_back ( "RJetMinDeltaPhiStarIndexEta24"     ) ; minDeltaphi24.push_back ( mindeltaphistarindex ) ;

                  minDeltaphi24Names.push_back ( "RJetMinDeltaPhiEta24Njle3"       ) ; minDeltaphi24.push_back ( mindeltaphi3       ) ;
                  minDeltaphi24Names.push_back ( "RJetMinDeltaPhiJetIndexEta24Njle3" ) ; minDeltaphi24.push_back ( mindeltaphi3jetindex ) ;

                  minDeltaphi24Names.push_back ( "RJetMinDeltaPhiEta24Njle4"       ) ; minDeltaphi24.push_back ( mindeltaphi4       ) ;
                  minDeltaphi24Names.push_back ( "RJetMinDeltaPhiJetIndexEta24Njle4" ) ; minDeltaphi24.push_back ( mindeltaphi4jetindex ) ;

                  minDeltaphi24Names.push_back ( "RJetMinDeltaPhiEta24Njle5"       ) ; minDeltaphi24.push_back ( mindeltaphi5       ) ;
                  minDeltaphi24Names.push_back ( "RJetMinDeltaPhiJetIndexEta24Njle5" ) ; minDeltaphi24.push_back ( mindeltaphi5jetindex ) ;

            }


      }//ii










            









      std::vector < TLorentzVector > GenJetVector ;
      std::vector < double >  Gencsv_vector ;
      std::vector < double >  Genpartonflavor_vector ;
      std::vector < double >  Gendeltaphi_vector ;

      GenJetVector        .clear() ;
      Gencsv_vector       .clear() ;
      Genpartonflavor_vector.clear() ;
      Gendeltaphi_vector    .clear() ;



      reco::MET::LorentzVector genmhtLorentz    ( 0,0,0,0 ) ;
      reco::MET::LorentzVector genmhtLorentzstar( 0,0,0,0 ) ;

      std::string genname ;
      unsigned int  matchfound [1000] ;
      double genmindeltaphi3 = -9, genmindeltaphi4 = -9, genmindeltaphi5 = -9 ;
      double genmindeltaphistar = -9, gendeltaphistar = -9 ;
      double genetacut ;
      int genmindeltaphi3jetindex = -9, genmindeltaphi4jetindex = -9, genmindeltaphi5jetindex = -9, genmindeltaphistarindex = -9 ;

      double gendeltaphi = -9 ;
      edm::Handle < edm::View < reco::GenJet > > gensrc ;
      iEvent.getByLabel( JetTagGenJets_,gensrc ) ;
      double  genjetspt, genjetseta, genjetsphi, mindeltar ;
      int mindeltarindex ;

      for ( unsigned int l = 0 ; l < gensrc -> size() ; l++ )
            if ( gensrc -> at(l).pt() >= 30 && gensrc -> at(l).eta() <= 5 && gensrc -> at(l).eta() >= -5 ) genmhtLorentz -= gensrc -> at(l).p4() ;


      for ( unsigned int ii = 0; ii < 2; ii++ )
      {

            if ( ii == 0 ) genetacut = 5   ;
            if ( ii == 1 ) genetacut = 2.4 ;
            if ( gensrc.isValid() )
            {
            for ( int dumb = 0; dumb < 1000; dumb ++ ) matchfound [dumb] = 0 ;
            genmindeltaphi3 = -9; genmindeltaphi4 = -9; genmindeltaphi5 = -9 ; 
            genmindeltaphi3jetindex = -9; genmindeltaphi4jetindex = -9; genmindeltaphi5jetindex = -9;

            int onoff = 0 ;
            for ( unsigned int j = 0; j < 8; j++ )
            {
                  onoff = 0 ;
                  mindeltar = 100; mindeltarindex = -9;
                  for ( unsigned int i = 0; i < gensrc -> size(); ++i )
                  {
                        if ( deltaR ( gensrc -> at(i).eta() , gensrc -> at(i).phi() , saveeta[j][ii], savephi[j][ii] ) < mindeltar && matchfound [i] == 0 && onoff == 0 && saveeta[j][ii] != -9 && savephi[j][ii] != -9 )
                        {
                                    mindeltar = deltaR( gensrc -> at(i).eta(), gensrc -> at(i).phi(), saveeta[j][ii], savephi[j][ii] ) ;
                                    mindeltarindex = i ;
//                                    if ( mindeltar < .25 ) onoff = 1;

                        }//if deltaR
                  }//i

                  if ( mindeltarindex >= 0 && mindeltarindex < (int) gensrc -> size() )
                  {

                        matchfound [ mindeltarindex ] = 1 ;
                        genjetspt  = gensrc -> at( mindeltarindex ).pt()  ;
                        genjetsphi = gensrc -> at( mindeltarindex ).phi() ;
                        genjetseta = gensrc -> at( mindeltarindex ).eta() ;

                        gendeltaphi = std::abs( reco::deltaPhi ( gensrc -> at ( mindeltarindex ).phi(), mhtphi ) ) ;

                        if ( std::abs(genmindeltaphi3) > std::abs(gendeltaphi) && j < 3 ) { genmindeltaphi3 = gendeltaphi; genmindeltaphi3jetindex = j + 1; }
                        if ( std::abs(genmindeltaphi4) > std::abs(gendeltaphi) && j < 4 ) { genmindeltaphi4 = gendeltaphi; genmindeltaphi4jetindex = j + 1; }
                        if ( std::abs(genmindeltaphi5) > std::abs(gendeltaphi) && j < 5 ) { genmindeltaphi5 = gendeltaphi; genmindeltaphi5jetindex = j + 1; }
                        if ( genetacut == 5 )
                        {

                              TLorentzVector dumb_vector ;
                              dumb_vector.SetPtEtaPhiE( genjetspt,genjetseta,genjetsphi,gensrc -> at( mindeltarindex ).energy() ) ;
                              GenJetVector.push_back( dumb_vector ) ;
                              Gendeltaphi_vector.push_back ( gendeltaphi ) ;
      
                        } // if genetacut
                  } // if mindeltarindex
            } //j

            if ( genetacut == 5 )
            {
                  std::auto_ptr < double > genmhtpt2  (new double ( genmhtLorentz.pt()  ) ) ;
                  iEvent.put ( genmhtpt2 , "GenMHT" ) ;
                  std::auto_ptr < double > genmhtphi2 (new double( genmhtLorentz.phi() ) ) ;
                  iEvent.put ( genmhtphi2 , "GenMHTphi" ) ;
            }

            genmindeltaphistar = -9; genmindeltaphistarindex = -9;
            for( unsigned int k = 0; k < gensrc -> size(); k++ )
            {
                  for ( unsigned int l = 0 ; l < gensrc -> size() ; l++ )
                        if ( gensrc -> at(l).pt() >= 30 && gensrc -> at(l).eta() <= genetacut && gensrc -> at(l).eta() >= (-1) * genetacut && l != k ) genmhtLorentzstar -= gensrc->at(l).p4() ;
                  gendeltaphistar = std::abs( reco::deltaPhi( gensrc -> at(k).phi(), genmhtLorentzstar.phi() ) ) ;
                  if ( std::abs( genmindeltaphistar ) > std::abs( gendeltaphistar ) ) { genmindeltaphistar = gendeltaphistar; genmindeltaphistarindex = k; }
            }//k
            }//if vgensrc.isValid
            else 
                  std::cout << "Warning Gen Tag not valid: " << JetTagGenJets_.label() << std::endl ;
      

            if ( genetacut == 5 )
            {
                  std::auto_ptr < std::vector < TLorentzVector >  > GenJetVector2   ( new std::vector < TLorentzVector > (GenJetVector) ) ;
                  iEvent.put ( GenJetVector2 , "GenJetLorentzVector" ) ;

                  std::auto_ptr < std::vector < double > > Gencsv_vector2         ( new std::vector < double > ( Gencsv_vector ) ) ;
                  iEvent.put ( Gencsv_vector2 , "GenJetCSV" ) ;

                  std::auto_ptr < std::vector < double > > Genpartonflavor_vector2  ( new std::vector < double > ( Genpartonflavor_vector ) ) ;
                  iEvent.put ( Genpartonflavor_vector2 , "GenJetPartonFlavor" ) ;

                  std::auto_ptr < std::vector < double > > Gendeltaphi_vector2      ( new std::vector < double > ( Gendeltaphi_vector ) ) ;
                  iEvent.put ( Gendeltaphi_vector2 , "GenJetDeltaPhi" ) ;

            }


            if ( genetacut == 5 )
            {

                  minDeltaphi5Names.push_back ( "GenMinDeltaPhiStarEta5"        ) ; minDeltaphi5.push_back ( genmindeltaphistar      ) ;
                  minDeltaphi5Names.push_back ( "GenMinDeltaPhiStarIndexEta5"     ) ; minDeltaphi5.push_back ( genmindeltaphistarindex ) ;

                  minDeltaphi5Names.push_back ( "GenMinDeltaPhiEta5Njle3"       ) ; minDeltaphi5.push_back ( genmindeltaphi3       ) ;
                  minDeltaphi5Names.push_back ( "GenMinDeltaPhiJetIndexEta5Njle3" ) ; minDeltaphi5.push_back ( genmindeltaphi3jetindex ) ;

                  minDeltaphi5Names.push_back ( "GenMinDeltaPhiEta5Njle4"       ) ; minDeltaphi5.push_back ( genmindeltaphi4       ) ;
                  minDeltaphi5Names.push_back ( "GenMinDeltaPhiJetIndexEta5Njle4" ) ; minDeltaphi5.push_back ( genmindeltaphi4jetindex ) ;

                  minDeltaphi5Names.push_back ( "GenMinDeltaPhiEta5Njle5"       ) ; minDeltaphi5.push_back ( genmindeltaphi5       ) ;
                  minDeltaphi5Names.push_back ( "GenMinDeltaPhiJetIndexEta5Njle5" ) ; minDeltaphi5.push_back ( genmindeltaphi5jetindex ) ;

            }


            if ( genetacut == 2.4 )
            {

                  minDeltaphi24Names.push_back ( "GenMinDeltaPhiStarEta24"        ) ; minDeltaphi24.push_back ( genmindeltaphistar      ) ;
                  minDeltaphi24Names.push_back ( "GenMinDeltaPhiStarIndexEta24"     ) ; minDeltaphi24.push_back ( genmindeltaphistarindex ) ;

                  minDeltaphi24Names.push_back ( "GenMinDeltaPhiEta24Njle3"       ) ; minDeltaphi24.push_back ( genmindeltaphi3       ) ;
                  minDeltaphi24Names.push_back ( "GenMinDeltaPhiJetIndexEta24Njle3" ) ; minDeltaphi24.push_back ( genmindeltaphi3jetindex ) ;

                  minDeltaphi24Names.push_back ( "GenMinDeltaPhiEta24Njle4"       ) ; minDeltaphi24.push_back ( genmindeltaphi4       ) ;
                  minDeltaphi24Names.push_back ( "GenMinDeltaPhiJetIndexEta24Njle4" ) ; minDeltaphi24.push_back ( genmindeltaphi4jetindex ) ;

                  minDeltaphi24Names.push_back ( "GenMinDeltaPhiEta24Njle5"       ) ; minDeltaphi24.push_back ( genmindeltaphi5       ) ;
                  minDeltaphi24Names.push_back ( "GenMinDeltaPhiJetIndexEta24Njle5" ) ; minDeltaphi24.push_back ( genmindeltaphi5jetindex ) ;

            }

      }//ii

      std::auto_ptr < std::vector < double > > minDeltaphi24_2  ( new std::vector < double > ( minDeltaphi24 ) ) ;
      iEvent.put ( minDeltaphi24_2      , "minDeltaPhiEta24"      ) ;

      std::auto_ptr < std::vector < std::string > > minDeltaphi24Names_2  ( new std::vector < std::string > ( minDeltaphi24Names ) ) ;
      iEvent.put ( minDeltaphi24Names_2 , "minDeltaPhiEta24Names" ) ;

      std::auto_ptr < std::vector < double > > minDeltaphi5_2  ( new std::vector < double > ( minDeltaphi5 ) ) ;
      iEvent.put ( minDeltaphi5_2       , "minDeltaPhiEta5"       ) ;

      std::auto_ptr < std::vector < std::string > > minDeltaphi5Names_2  ( new std::vector < std::string > ( minDeltaphi5Names ) ) ;
      iEvent.put ( minDeltaphi5Names_2  , "minDeltaPhiEta5Names"  ) ;





      std::vector < TLorentzVector > neutrino_LVector;
      std::vector < int > neutrino_pdgid_vector, neutrino_mother_pdgid_vector ;

      edm::Handle < edm::View < reco::GenParticle > > genjets ;
      iEvent.getByLabel ( GenParticleTag_,genjets ) ;

      double neutrino_pt [6], neutrino_eta [6], neutrino_phi [6], neutrino_energy [6] ;
      int neutrino_pdgid [6], neutrino_mother_pdgid [6], place;
          
      for ( int i = 1; i < 5; i++ )
      {
            neutrino_pt         [i] = -9. ;
            neutrino_eta        [i] = -9. ;
            neutrino_phi        [i] = -9. ;
            neutrino_energy       [i] = -9. ;
            neutrino_pdgid      [i] = 0   ;
            neutrino_mother_pdgid [i] = 0   ;
      }//i

      if ( genjets.isValid () )
      {
            for( size_t i = 0; i < genjets -> size(); i++ )
            {
                  if( abs( genjets -> at(i).pdgId () ) == 12 || abs( genjets -> at(i).pdgId () ) == 14 || abs( genjets -> at(i).pdgId () ) == 16 )
                  {
                        place = 100 ;

                        for ( int j = 1; j < 5; j++ )
                              if ( genjets -> at(i).pt() > neutrino_pt [j] ) { place = j; break; }
      
                        if ( place < 5 )
                        {
                              for ( int k = 4; k >= place; k-- )
                              {
                                    neutrino_pt        [k + 1] = neutrino_pt        [k] ;
                                    neutrino_eta       [k + 1] = neutrino_eta       [k] ;
                                    neutrino_phi       [k + 1] = neutrino_phi       [k] ;
                                    neutrino_energy      [k + 1] = neutrino_energy      [k] ;
                                    neutrino_pdgid       [k + 1] = neutrino_pdgid       [k] ;
                                    neutrino_mother_pdgid[k + 1] = neutrino_mother_pdgid[k] ;
                              }//k

                              neutrino_pt         [place] = genjets -> at(i).pt();
                              neutrino_eta        [place] = genjets -> at(i).eta();
                              neutrino_phi        [place] = genjets -> at(i).phi();
                              neutrino_energy       [place] = genjets -> at(i).energy();
                              neutrino_pdgid      [place] = genjets -> at(i).pdgId();
                              neutrino_mother_pdgid [place] = genjets -> at(i).mother()->pdgId();
                        }//place
                  }//if abs
            }//i
      } //if
      else std::cout << "Warning Neutrino Tag not valid: " << GenParticleTag_.label() << std::endl ;

      
      for ( unsigned int i = 1; i < 5; i++ )
      {

            TLorentzVector dumb_vector ;
            dumb_vector.SetPtEtaPhiE( neutrino_pt[i], neutrino_eta[i], neutrino_phi[i], neutrino_energy[i] ) ;
                  
            neutrino_LVector.push_back ( dumb_vector ) ;
            neutrino_pdgid_vector.push_back ( neutrino_pdgid[i] ) ;
            neutrino_mother_pdgid_vector.push_back ( neutrino_mother_pdgid[i] ) ;
       }//i


      std::auto_ptr < std::vector < TLorentzVector > > eutrino_LVector2  ( new std::vector < TLorentzVector > ( neutrino_LVector ) ) ;
      std::auto_ptr < std::vector < int > > neutrino_pdgid_vector2  ( new std::vector < int > ( neutrino_pdgid_vector ) ) ;
      std::auto_ptr < std::vector < int > > neutrino_mother_pdgid_vector2  ( new std::vector < int > ( neutrino_mother_pdgid_vector ) ) ;


      iEvent.put ( eutrino_LVector2, "NeutrinoLorentzVector") ;
      iEvent.put ( neutrino_pdgid_vector2, "NeutrinoPdg") ;
      iEvent.put ( neutrino_mother_pdgid_vector2, "NeutrinoMotherPdg") ;




      double metpt_=-10., metphi_=-10.;

      edm::Handle < edm::View < pat::MET > > MET ;
      iEvent.getByLabel ( metTag_, MET ) ;

      reco::MET::LorentzVector metLorentz ( 0,0,0,0 ) ;

      if( MET.isValid() )
      {
            const pat::MET patMET( MET -> at (0) ) ;
            if( patMET.genMET() )
            {
                  const reco::GenMET* genMET( patMET.genMET () ) ;
                  metLorentz = genMET -> p4   () ;
                  metpt_     = metLorentz.pt  ();
                  metphi_    = metLorentz.phi ();

            } //if patMET      
      }//if MET

      else std::cout << "GenMETDouble::Invlide Tag: " << metTag_.label () << std::endl ;

      std::auto_ptr < double > htp( new double ( metpt_ ) ) ;
      iEvent.put ( htp, "GenMET" ) ;
      std::auto_ptr < double > htp2 ( new double ( metphi_ ) ) ;
      iEvent.put ( htp2, "GenMETphi" ) ;
      

}//void DeltaPhiQCD



// ------------ method called once each job just before starting event loop  ------------
void 
DeltaPhiQCD::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
DeltaPhiQCD::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
DeltaPhiQCD::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
DeltaPhiQCD::endRun(edm::Run&, edm::EventSetup const&)

{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
DeltaPhiQCD::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
DeltaPhiQCD::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DeltaPhiQCD::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
      //The following says we do not know what parameters are allowed so do no validation
      // Please change this to state exactly what you do use, even if it is no parameters
      edm::ParameterSetDescription desc;
      desc.setUnknown();
      descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DeltaPhiQCD);
