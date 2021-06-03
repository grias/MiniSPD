/**
 * \file BmnEnums.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 * \brief Define enumerations used in tracking.
 **/

#ifndef BMNENUMS_H_
#define BMNENUMS_H_

/**
 * \enum Define status code.
 */
enum BmnStatus {
    kBMNSUCCESS = 0, /**< Success. */
    kBMNERROR = 1, /**< Error. */
    kBMNTIMEOUT = 2,
    kBMNFINISH = 3
};

/**
 * \enum Define type of event
 */
enum BmnTriggerType {
    kBMNBEAM = 6, /**< "beam"  BC1 + BC2 + T0 + VC */
    kBMNMINBIAS = 1, /**< "min.bias" BC1 + BC2 + T0 + VC + FD */
};

/**
 * \enum Define type of event
 */
enum BmnEventType {
    kBMNPEDESTAL = 0, /**< pedestal event. */
    kBMNPAYLOAD = 1, /**< spill event. */
};

/**
 * \enum Define active detectors
 */
enum StandDetector {
    kTRIGGER,
    kSILICON,
    kGEM,
    kSTRAW,
    kCALORIMETER
};

#endif /* BMNENUMS_H_ */
