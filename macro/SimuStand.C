
//#define _COMPILE_MACRO_

#if defined(_COMPILE_MACRO_)

#include <TRint.h>
#include <TStopwatch.h>

#include "FairParRootFileIo.h"

#include "SpdRunSim.h"
#include "SpdMCEventHeader.h"

#include "SpdCommonGeoMapper.h"

#include "SpdFields.hh"
#include "SpdGenerators.hh"

#include "SpdCave.h"
#include "SpdPipe.h"

#include "SpdIts.h"
#include "SpdTsTB.h"
#include "SpdTsTEC.h"
#include "SpdEcalTB.h"
#include "SpdEcalTEC.h"
#include "SpdRsTB.h"
#include "SpdRsTEC.h"

#include "SpdItsGeoMapperX.h"
#include "SpdTsTBGeoBuilder.h"

#include "StandSts.h"

#endif

//_________________________________________________________________________
void SimuStand(Int_t nEvents = 1)
{
     TString outFile = "run_stand.root";    // Output data file name
     TString parFile = "params_stand.root"; // Output parameters file name

     SpdRunSim *run = new SpdRunSim();

     run->SetName("TGeant4");

     //gSystem->Setenv("GEOMPATH","/home/artur/Projects/1/");
     run->SetMaterials("media.geo");

     run->SetOutputFile(outFile);

     run->SetPythiaDecayer("DecayConfig.C");

     run->SetMCEventHeader(new SpdMCEventHeader);
     /*--------------------------------------------------*/
     /* +++++++++     GEOMETRY (STAND)      ++++++++ */
     /*--------------------------------------------------*/

     // to define cave parameters
     SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();

     /* ++++++++++++++++++ CAVE ++++++++++++++++++ */

     FairModule *cave = new SpdCave("CAVE");
     //cave->SetGeometryFileName("cave.geo");
     //cave->SetGeometryFileName("cave_precise.geo");
     run->AddModule(cave);

     /* ++++++++++++++++++ SPD ++++++++++++++++++ */

     // SpdEcalTB *ecal_barrel = new SpdEcalTB(); /* +++++++++ ECALT (BARREL) ++++++++++ */
     // SpdEcalTEC *ecal_ecps = new SpdEcalTEC(); /* +++++++++ ECALT (ENDCAPS) +++++++++ */
     // SpdRsTB *rs_barrel = new SpdRsTB();       /* +++++++++ RST (BARREL) ++++++++++++ */
     // SpdRsTEC *rs_ecps = new SpdRsTEC();       /* +++++++++ RST (ENDCAPS) +++++++++++ */
     // SpdTsTB *ts_barrel = new SpdTsTB();       /* +++++++++ TST (BARREL) ++++++++++++ */
     // SpdTsTEC *ts_ecps = new SpdTsTEC();       /* +++++++++ TST (ENDCAPS) +++++++++++ */

     // run->AddModule(ts_barrel);
     // run->AddModule(ecal_barrel);
     // run->AddModule(rs_barrel);
     // run->AddModule(ts_ecps);
     // run->AddModule(ecal_ecps);
     // run->AddModule(rs_ecps);

     /* ===== STS ===== */

     StandSts *sts = new StandSts();
     run->AddModule(sts);

     /*--------------------------------------------------*/
     /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
     /*--------------------------------------------------*/

     // /* --- const field --- */
     // SpdConstField *MagField = new SpdConstField();
     // MagField->SetField(0., 0., 8.0); //  kG

     // /* === define field region === */
     // SpdRegion *reg = 0;

     // reg = MagField->CreateFieldRegion("box");
     // reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000); // (X,Y,Z)_(min,max), cm

     // run->SetField(MagField);

     // MagField->Print();

     /*--------------------------------------------------*/
     /* ++++++++++ DEFINE PRIMARY GENERATORS +++++++++++ */
     /*--------------------------------------------------*/

     SpdPrimaryGenerator *primGen = new SpdPrimaryGenerator();

     SpdIsotropicGenerator *gen = new SpdIsotropicGenerator();

     //--------------------------------------------------------------------------------
     // Initialization parameters:
     //
     // [1] pdg number,
     // [2] kinetic energy (GeV),
     // [3] particles per event (with option = 1, 2) or DENSITY LEVEL[1,7] (with option = -1, -2)
     //
     // Options (via SetVGenOpt), default option = 2 :
     //     1: isotropic (oridinary):  theta[min,max], phi[min,max] via SetSpherical
     //     2: isotripic (solid angle):  theta[min,max], solid angle [phi,theta] via SetSolid
     //    -1: pseudoisotropic (oridinary):  theta[min,max], phi[min,max] via SetSpherical
     //    -2: pseudoisotropic (solid angle):  theta[min,max], solid angle [phi,theta] via SetSolid
     //
     // PSEUDOISOTROPIC DISTRIBUTION (option = -1, -2):
     //
     //     Particles: 12, 42, 162, 642, 2562, 10241, 40962 (v[i] = 4*v[i-1] - 6)
     //     Angular resolution: a = 63.435, a/2, a/4, a/8, a/16, a/32, a/64 (~1 deg)
     //--------------------------------------------------------------------------------

     gen->SetVerboseLevel(1); // 0 ("minimal" printing) or 1

     //gen->Initialize(211,1,10000);
     gen->Initialize(211, 1, 5);

     /* ---- 1 ---- */

     gen->SetSpherical(0, 5, 0, 360); // theta[min,max], phi[min,max]
     gen->SetVGenOpt(1);
     //    gen->SetVGenOpt(-1);

     /* ---- 2 ---- */
     //    gen->SetSolid(30, 60, 225, 45); // theta[min,max], [phi,theta] for solid angle
     //    //gen->SetVGenOpt(2);
     //    gen->SetVGenOpt(-2);

     gen->SetKeepEvent(1); // keep particles in the generator

     primGen->AddGenerator(gen);

     primGen->SetVerboseLevel(1);
     primGen->SetVerbose(1);

     //============================

     run->SetGenerator(primGen);

     primGen->SetBeam(0.1, 0.1, 0.1, 0.1); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
     primGen->SmearVertexXY(kTRUE);
       //primGen->SmearGausVertexXY(kTRUE);
     //
     // primGen->SetTarget(0., 5.); // Z- position, 2*delta or sigma [cm]
     // primGen->SmearVertexZ(kTRUE);
     //   //primGen->SmearGausVertexZ(kTRUE);

     /* ------------------------------------------------ */
     /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
     /* ------------------------------------------------ */

     run->SetStoreTraj(kTRUE);

     //SpdCommonGeoMapper::Instance()->SaveEmptyHits();

     //run->ForceParticleLifetime(-211, 26.033/5.); // pdg, life time [ns]
     //run->ForceParticleLifetime( 211, 26.033/5.); // pdg, life time [ns]

     //SpdCommonGeoMapper::Instance()->UnsetMaterials(true);

     /* ----------------------------------------------------------------------- */
     /* >>>>>>>>>>>>>>>>>>>>>>>>>>> INITALIZE RUN <<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
     /* ----------------------------------------------------------------------- */

     cout << "\n\t"
          << "++++++++++++++++++++++++++++++++++++++++++++" << endl;
     cout << "\t"
          << "+                                          +" << endl;
     cout << "\t"
          << "+           Init Run (start)               +" << endl;
     cout << "\t"
          << "+                                          +" << endl;
     cout << "\t"
          << "++++++++++++++++++++++++++++++++++++++++++++\n"
          << endl;

     run->Init();

     cout << "\n\t"
          << "++++++++++++++++++++++++++++++++++++++++++++" << endl;
     cout << "\t"
          << "+                                          +" << endl;
     cout << "\t"
          << "+           Init Run (finish)              +" << endl;
     cout << "\t"
          << "+                                          +" << endl;
     cout << "\t"
          << "++++++++++++++++++++++++++++++++++++++++++++\n"
          << endl;

     /*--------------------------------------------------*/
     /* +++++++++++++ CREATE RUN PARAMETERS  +++++++++++ */
     /*--------------------------------------------------*/

     Bool_t MergePars = kFALSE;
     FairParRootFileIo *parOut = new FairParRootFileIo(MergePars);
     if (MergePars)
          parOut->open(parFile.Data());
     else
          parOut->open(parFile.Data(), "RECREATE");

     FairRuntimeDb *rtdb = run->GetRuntimeDb();
     rtdb->setOutput(parOut);

     /* ----------------------------------------------------------------------- */
     /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> RUN <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
     /* ----------------------------------------------------------------------- */

     TStopwatch timer;
     timer.Start();

     run->Run(nEvents);

     timer.Stop();

     /*--------------------------------------------------*/
     /* ++++++++++++  SAVE RUN PARAMETERS  +++++++++++++ */
     /*--------------------------------------------------*/

     rtdb->saveOutput();

     /*--------------------------------------------------*/

     Double_t rtime = timer.RealTime();
     Double_t ctime = timer.CpuTime();

     cout << endl
          << endl;
     cout << "Macro finished succesfully." << endl;
     cout << "Output file is             " << outFile << endl;
     cout << "Parameter file is          " << parFile << endl;
     cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
     cout << endl;

     /*--------------------------------------------------*/

     //SpdCommonGeoMapper::Instance()->PrintGeometryList();
     SpdCommonGeoMapper::Instance()->PrintGeometry();

     //SpdItsGeoMapperX::Instance()->PrintVolPars();
     //SpdItsGeoMapperX::Instance()->PrintGeoTable();
     //SpdItsGeoMapperX::Instance()->Print("");

     /*--------------------------------------------------*/

     gApplication->Terminate();
}
