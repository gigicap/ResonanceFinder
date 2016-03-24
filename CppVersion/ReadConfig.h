#ifndef ReadConfig_h
#define ReadConfig_h

//paths to be changed 
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <string>
#include <stdexcept>
#include <vector>

// Error codes

using namespace xercesc;


enum {
   ERROR_ARGS = 1, 
   ERROR_XERCES_INIT,
   ERROR_PARSE,
   ERROR_EMPTY_DOCUMENT
};


//to read the xml configuration files
class GetConfig
{
public:
   GetConfig();
  ~GetConfig();
   void readConfigFile(std::string&) throw(std::runtime_error);
 

   double rc_E_min, rc_E_max;
   int rc_ADC_channels;
   
   //scanning related variables
   double rc_scan_step;    //average step of the scans
   int rc_N_scans;      //number of scans 

   double rc_width, rc_run_time; //beam spread, run time (in s)
   double rc_N_total;         //total number of gammas per bunch/second
   char *rc_compton_file;

   //Target and detector variables
   double rc_z_target, rc_rho_target, rc_sigma_beam;
   double rc_detector_efficiency, rc_detector_omega;

   //resonances
   std::vector<double> rc_E_reso;
   std::vector<double> rc_W_reso;
   std::vector<double> rc_I_reso;

      //cuts
   double rc_E_cut;
   char *rc_background_file;
   double rc_N_background;


private:
   xercesc::XercesDOMParser *m_ConfigFileParser;
        
   char* m_E_MIN;
   char* m_E_MAX;
   char* m_ADC_CHANNELS;

   char* m_WIDTH;
   char* m_RUN_TIME;
   char* m_N_TOTAL;
   char* m_SIGMA_BEAM;
   char* m_SCAN_STEP;
   char* m_N_SCANS;
   char* m_COMPTON_FILE;

   char* m_Z_TARGET; 
   char* m_RHO_TARGET; 

   char* m_DETECTOR_EFFICIENCY;
   char* m_DETECTOR_OMEGA; 

   char* m_E_RESO;
   char* m_W_RESO;
   char* m_I_RESO;

   char* m_N_BACKGROUND; 
   char* m_BACKGROUND_FILE; 
   char* m_E_CUT;

   XMLCh* TAG_root;
   XMLCh* TAG_ADC;
   XMLCh* TAG_BEAM;
   XMLCh* TAG_TARGET;
   XMLCh* TAG_DETECTOR;
   XMLCh* TAG_BACKGROUND;
   XMLCh* TAG_RESONANCES;
   XMLCh* TAG_RESONANCE;


   XMLCh* TAG_E_MIN;
   XMLCh* TAG_E_MAX;   
   XMLCh* TAG_ADC_CHANNELS;   

   XMLCh* TAG_WIDTH;
   XMLCh* TAG_RUN_TIME;
   XMLCh* TAG_N_TOTAL;
   XMLCh* TAG_SIGMA_BEAM;
   XMLCh* TAG_SCAN_STEP;
   XMLCh* TAG_N_SCANS;
   XMLCh* TAG_COMPTON_FILE; 

   XMLCh* TAG_Z_TARGET; 
   XMLCh* TAG_RHO_TARGET; 

   XMLCh* TAG_DETECTOR_EFFICIENCY;
   XMLCh* TAG_DETECTOR_OMEGA; 

   XMLCh* TAG_E_RESO;
   XMLCh* TAG_W_RESO;
   XMLCh* TAG_I_RESO;

   XMLCh* TAG_N_BACKGROUND; 
   XMLCh* TAG_BACKGROUND_FILE; 
   XMLCh* TAG_E_CUT; 
   
};

#endif