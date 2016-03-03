#include "ReadConfig.h"

#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <vector>

using namespace xercesc;
using namespace std;

//XML PARSER STUFF
GetConfig::GetConfig()
{
   try
   {
      XMLPlatformUtils::Initialize();  // Initialize Xerces
   }
   catch( XMLException& e )
   {
      char* message = XMLString::transcode( e.getMessage() );
      cerr << "XML toolkit initialization error: " << message << endl;
      XMLString::release( &message );
   }


    TAG_root = XMLString::transcode("root");
    TAG_ADC = XMLString::transcode("ADC");
    TAG_BEAM = XMLString::transcode("BEAM");
    TAG_TARGET = XMLString::transcode("TARGET");
    TAG_DETECTOR = XMLString::transcode("DETECTOR");
    TAG_RESONANCES  = XMLString::transcode("RESONANCES");
    TAG_RESONANCE  = XMLString::transcode("RESONANCE");

    TAG_E_MIN  = XMLString::transcode("E_MIN");
    TAG_E_MAX  = XMLString::transcode("E_MAX");   
    TAG_ADC_CHANNELS  = XMLString::transcode("ADC_CHANNELS");   

    TAG_WIDTH  = XMLString::transcode("WIDTH");
    TAG_RUN_TIME  = XMLString::transcode("RUN_TIME");
    TAG_N_TOTAL  = XMLString::transcode("N_TOTAL");
    TAG_SIGMA_BEAM  = XMLString::transcode("SIGMA_BEAM");
    TAG_SCAN_STEP  = XMLString::transcode("SCAN_STEP");
    TAG_N_SCANS  = XMLString::transcode("N_SCANS");

    TAG_Z_TARGET  = XMLString::transcode("Z_TARGET"); 
    TAG_RHO_TARGET  = XMLString::transcode("RHO_TARGET"); 

    TAG_DETECTOR_EFFICIENCY  = XMLString::transcode("DETECTOR_EFFICIENCY");
    TAG_DETECTOR_OMEGA  = XMLString::transcode("DETECTOR_OMEGA"); 

    TAG_E_RESO  = XMLString::transcode("E_RESO");
    TAG_W_RESO  = XMLString::transcode("W_RESO");
    TAG_I_RESO  = XMLString::transcode("I_RESO");

    TAG_N_BACKGROUND  = XMLString::transcode("N_BACKGROUND"); 
    TAG_BACKGROUND_FILE  = XMLString::transcode("BACKGROUND_FILE"); 
    TAG_E_CUT  = XMLString::transcode("E_CUT");     
   
   m_ConfigFileParser = new XercesDOMParser;
}


GetConfig::~GetConfig()
{

   delete m_ConfigFileParser;

   try
   {
   XMLString::release( &TAG_root );
   
   XMLString::release( &TAG_ADC);
   XMLString::release( &TAG_BEAM);
   XMLString::release( &TAG_TARGET);
   XMLString::release( &TAG_DETECTOR);
   XMLString::release( &TAG_RESONANCES);
   XMLString::release( &TAG_RESONANCE);


   XMLString::release( &TAG_E_MIN);
   XMLString::release( &TAG_E_MAX);   
   XMLString::release( &TAG_ADC_CHANNELS);   

   XMLString::release( &TAG_WIDTH);
   XMLString::release( &TAG_RUN_TIME);
   XMLString::release( &TAG_N_TOTAL);
   XMLString::release( &TAG_SIGMA_BEAM);
   XMLString::release( &TAG_SCAN_STEP);
   XMLString::release( &TAG_N_SCANS);

   XMLString::release( &TAG_Z_TARGET); 
   XMLString::release( &TAG_RHO_TARGET); 

   XMLString::release( &TAG_DETECTOR_EFFICIENCY);
   XMLString::release( &TAG_DETECTOR_OMEGA); 

   XMLString::release( &TAG_E_RESO);
   XMLString::release( &TAG_W_RESO);
   XMLString::release( &TAG_I_RESO);

   XMLString::release( &TAG_N_BACKGROUND); 
   XMLString::release( &TAG_BACKGROUND_FILE); 
   XMLString::release( &TAG_E_CUT); 


   }
   catch( ... )
   {
      cerr << "Unknown exception encountered in TagNamesdtor" << endl;
   }

   // Terminate Xerces

   try
   {
      XMLPlatformUtils::Terminate();  // Terminate after release of memory
   }
   catch( xercesc::XMLException& e )
   {
      char* message = xercesc::XMLString::transcode( e.getMessage() );

      cerr << "XML ttolkit teardown error: " << message << endl;
      XMLString::release( &message );
   }
}


void GetConfig::readConfigFile(string& configFile)  throw( std::runtime_error )
{
   // Test to see if the file is ok.
	std::cout<<"--> In"<<std::endl;
   struct stat fileStatus;

   errno = 0;
   if(stat(configFile.c_str(), &fileStatus) == -1) // ==0 ok; ==-1 error
   {
       if( errno == ENOENT )     
          throw ( std::runtime_error("Path file_name does not exist, or path is an empty string.") );
       else if( errno == ENOTDIR )
          throw ( std::runtime_error("A component of the path is not a directory."));
       else if( errno == ELOOP )
          throw ( std::runtime_error("Too many symbolic links encountered while traversing the path."));
       else if( errno == EACCES )
          throw ( std::runtime_error("Permission denied."));
       else if( errno == ENAMETOOLONG )
          throw ( std::runtime_error("File can not be read\n"));
   }

   // Configure DOM parser.
	std::cout<<"--> Configure DOM parser."<<std::endl;

   m_ConfigFileParser->setValidationScheme( XercesDOMParser::Val_Never );
   m_ConfigFileParser->setDoNamespaces( false );
   m_ConfigFileParser->setDoSchema( false );
   m_ConfigFileParser->setLoadExternalDTD( false );

   try
   {
	std::cout<<"--> parse."<<std::endl;

      m_ConfigFileParser->parse( configFile.c_str() );

      DOMDocument* xmlDoc = m_ConfigFileParser->getDocument();
      
      DOMElement* elementRoot = xmlDoc->getDocumentElement();
      if( !elementRoot ) throw(std::runtime_error( "empty XML document" ));

      DOMNodeList* resonances_children = elementRoot->getElementsByTagName(TAG_RESONANCE);
      const  XMLSize_t nodeCount1 = resonances_children->getLength();

      std::cout<<"--> resonance_children lenght = "<<nodeCount1<<std::endl;

      int l_counter = 0;
      for( XMLSize_t xx = 0; xx < nodeCount1; ++xx )
      {      
      	DOMNode* node_resonance = resonances_children->item(xx);			//get every resonance
      	 //subnodes of the resonances 
      	 if( node_resonance->getNodeType() &&  node_resonance->getNodeType() == DOMNode::ELEMENT_NODE ) 
         			{
            		// Re-cast node as element
            		DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( node_resonance );
            		
					     const XMLCh* xmlch_E_RESO = currentElement->getAttribute(TAG_E_RESO);
        			   m_E_RESO = XMLString::transcode(xmlch_E_RESO);
        			   double tempereso = atof(m_E_RESO);
        
                const XMLCh* xmlch_W_RESO = currentElement->getAttribute(TAG_W_RESO);
                  m_W_RESO = XMLString::transcode(xmlch_W_RESO);
                  double tempwreso = atof(m_W_RESO);

                const XMLCh* xmlch_I_RESO = currentElement->getAttribute(TAG_I_RESO);
                  m_I_RESO = XMLString::transcode(xmlch_I_RESO);
                  double tempireso = atof(m_I_RESO);        
        
              rc_E_reso.push_back(tempereso);
              rc_W_reso.push_back(tempwreso);
              rc_I_reso.push_back(tempireso);
         			}
      }


      DOMNode* node_adc = elementRoot->getElementsByTagName(TAG_ADC)->item(0);
      if( node_adc->getNodeType() &&  node_adc->getNodeType() == DOMNode::ELEMENT_NODE ) 
              {
                // Re-cast node as element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( node_adc );

                const XMLCh* xmlch_E_MIN = currentElement->getAttribute(TAG_E_MIN);
                 m_E_MIN = XMLString::transcode(xmlch_E_MIN);
                 rc_E_min = atof(m_E_MIN);

                const XMLCh* xmlch_E_MAX = currentElement->getAttribute(TAG_E_MAX);
                 m_E_MAX = XMLString::transcode(xmlch_E_MAX);
                 rc_E_max = atof(m_E_MAX);

                const XMLCh* xmlch_ADC_CHANNELS = currentElement->getAttribute(TAG_ADC_CHANNELS);
                 m_ADC_CHANNELS = XMLString::transcode(xmlch_ADC_CHANNELS);
                 rc_ADC_channels = atof(m_ADC_CHANNELS);
              }

      DOMNode* node_beam = elementRoot->getElementsByTagName(TAG_BEAM)->item(0);
      if( node_beam->getNodeType() &&  node_beam->getNodeType() == DOMNode::ELEMENT_NODE ) 
              {
                // Re-cast node as element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( node_beam );

                const XMLCh* xmlch_WIDTH = currentElement->getAttribute(TAG_WIDTH);
                 m_WIDTH = XMLString::transcode(xmlch_WIDTH);
                 rc_width = atof(m_WIDTH);

                const XMLCh* xmlch_RUN_TIME = currentElement->getAttribute(TAG_RUN_TIME);
                 m_RUN_TIME = XMLString::transcode(xmlch_RUN_TIME);
                 rc_run_time = atof(m_RUN_TIME);

                const XMLCh* xmlch_N_TOTAL = currentElement->getAttribute(TAG_N_TOTAL);
                 m_N_TOTAL = XMLString::transcode(xmlch_N_TOTAL);
                 rc_N_total = atof(m_N_TOTAL);

                const XMLCh* xmlch_SIGMA_BEAM = currentElement->getAttribute(TAG_SIGMA_BEAM);
                 m_SIGMA_BEAM = XMLString::transcode(xmlch_SIGMA_BEAM);
                 rc_sigma_beam = atof(m_SIGMA_BEAM);

                const XMLCh* xmlch_SCAN_STEP = currentElement->getAttribute(TAG_SCAN_STEP);
                 m_SCAN_STEP = XMLString::transcode(xmlch_SCAN_STEP);
                 rc_scan_step = atof(m_SCAN_STEP);

                const XMLCh* xmlch_N_SCANS = currentElement->getAttribute(TAG_N_SCANS);
                 m_N_SCANS = XMLString::transcode(xmlch_N_SCANS);
                 rc_N_scans = atof(m_N_SCANS);
              }

      DOMNode* node_target = elementRoot->getElementsByTagName(TAG_TARGET)->item(0);
      if( node_target->getNodeType() &&  node_target->getNodeType() == DOMNode::ELEMENT_NODE ) 
              {
                // Re-cast node as element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( node_target );

                const XMLCh* xmlch_Z_TARGET = currentElement->getAttribute(TAG_Z_TARGET);
                 m_Z_TARGET = XMLString::transcode(xmlch_Z_TARGET);
                 rc_z_target = atof(m_Z_TARGET);

                const XMLCh* xmlch_RHO_TARGET = currentElement->getAttribute(TAG_RHO_TARGET);
                 m_RHO_TARGET = XMLString::transcode(xmlch_RHO_TARGET);
                 rc_rho_target = atof(m_RHO_TARGET);
               }

      DOMNode* node_detector = elementRoot->getElementsByTagName(TAG_DETECTOR)->item(0);
      if( node_detector->getNodeType() &&  node_detector->getNodeType() == DOMNode::ELEMENT_NODE ) 
              {
                // Re-cast node as element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( node_detector );

                const XMLCh* xmlch_DETECTOR_EFFICIENCY = currentElement->getAttribute(TAG_DETECTOR_EFFICIENCY);
                 m_DETECTOR_EFFICIENCY = XMLString::transcode(xmlch_DETECTOR_EFFICIENCY);
                 rc_detector_efficiency = atof(m_DETECTOR_EFFICIENCY);

                const XMLCh* xmlch_DETECTOR_OMEGA = currentElement->getAttribute(TAG_DETECTOR_OMEGA);
                 m_DETECTOR_OMEGA = XMLString::transcode(xmlch_DETECTOR_OMEGA);
                 rc_detector_omega = atof(m_DETECTOR_OMEGA);
               }

      DOMNode* node_background = elementRoot->getElementsByTagName(TAG_BACKGROUND)->item(0);
      if( node_background->getNodeType() &&  node_background->getNodeType() == DOMNode::ELEMENT_NODE ) 
              {
                // Re-cast node as element
                DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( node_background );

                const XMLCh* xmlch_N_BACKGROUND = currentElement->getAttribute(TAG_N_BACKGROUND);
                 m_N_BACKGROUND = XMLString::transcode(xmlch_N_BACKGROUND);
                 rc_N_background = atof(m_N_BACKGROUND);

                const XMLCh* xmlch_E_CUT = currentElement->getAttribute(TAG_E_CUT);
                 m_E_CUT = XMLString::transcode(xmlch_E_CUT);
                 rc_E_cut = atof(m_E_CUT);

                const XMLCh* xmlch_BACKGROUND_FILE = currentElement->getAttribute(TAG_BACKGROUND_FILE);
                 m_BACKGROUND_FILE = XMLString::transcode(xmlch_BACKGROUND_FILE);
                 rc_background_file = m_BACKGROUND_FILE;
               }
      
      
   }         
   catch( xercesc::XMLException& e )
   {
      char* message = xercesc::XMLString::transcode( e.getMessage() );
      ostringstream errBuf;
      errBuf << "Error parsing file: " << message << flush;
      XMLString::release( &message );
   }
   
}