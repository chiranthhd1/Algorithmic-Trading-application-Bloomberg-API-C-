
// bb_api_cpp_main.cpp
// Example Bloomberg API C++ Program (c)2014 Richard Holowczak
// Portions (c) Bloomberg, LLC

// Bloomberg API Includes
#include <blpapi_defs.h>
#include <blpapi_correlationid.h>
#include <blpapi_element.h>
#include <blpapi_event.h>
#include <blpapi_exception.h>
#include <blpapi_message.h>
#include <blpapi_session.h> 
#include <blpapi_subscriptionlist.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <sstream>

using  namespace BloombergLP; 
using  namespace blpapi; 
using namespace BloombergLP;
using namespace blpapi;


void test1(std::vector<double>& in_v, std::vector<std::string>& dates, std::vector<double>& entry_price){

    std::vector<double> gain; // setting up a vector to save the gains which is later used to calculate avergae gain.
    std::vector<double> loss; // setting up a vector to save the loss which is later used to calculate avergae loss.
    std::string inp;

    std::string ent_date, exit_Date,  position ; // Variables used to write to a file.

    double ent_pric = 0;
	double exit_pric = 0;
	double pl = 0;
	double cpl = 0;
    double size =10000;
    double change = 0;
    double avg_gain = 0;
    double avg_loss =0;
    double sum_gain = 0;
    double sum_loss =0;
    double RS = 0;
    double RSI = 0;
    double gain_1 = 0;
    double loss_1 = 0;

    int tradenumber = 0;
    int flag = 0;
    int trade_greater = 0;
    int first_trigger = 0;
 
    std::ofstream myfile("op.csv");

    myfile << " TRADENUMBER , ENTRY DATE, ENTRY PRICE, EXIT DATE, EXIT PRICE, POSITION, SIZE, TRADE P&L, CUMULATIVE P&L " << "\n";
	// This loop calculates the change, gain and  loss for first 14 values.
    if ( in_v.size() >  15){
        for (int i =1; i < 14; i++){
            change = in_v[i] - in_v[i-1];
            if (change >= 0){
                gain.push_back(change);
            }
            if (change < 0){
                change = fabs(change);
                loss.push_back(change);
            }
            change = 0;
        }
  
        for (int k = 0; k < gain.size(); k++){
            sum_gain = sum_gain + gain[k];
        }
        for (int k = 0; k < loss.size(); k++){
            sum_loss = sum_loss + loss[k];
        }
  
        for ( int j = 14; j < in_v.size(); j++){
            change = in_v[j] - in_v[j-1];
            if (change >= 0){
                gain.push_back(change);
                gain_1 = change;
				// The average is calculated differntly if the value belongs to first 14 or if the value is after 14.
                if (j==14){
                    avg_gain = sum_gain / 14;
                    avg_loss = sum_loss / 14;
                }
                else{
                    avg_gain = ((avg_gain * 13) + gain_1)/14;
                    avg_loss = ((avg_loss*13)+0)/14;
                }
            }
            if (change < 0){
                change = fabs(change);
                loss_1 = change;
                loss.push_back(change);
                if (j==14){
                    avg_gain = sum_gain / 14;
                    avg_loss = sum_loss / 14;
                }
                else{
                    avg_gain = ((avg_gain*13)+0)/14;
                    avg_loss = ((avg_loss * 13) + loss_1)/14;
                }           
            }
            RS = avg_gain/avg_loss;		// RS is calculated
            RSI = (100 - (100/(1+RS)));	// RSI is calculated
            if ( RSI > 60 && trade_greater == 0){
				// if RSI is greater than 60 then over bought and short.
                if ( flag == 0){
                    std::cout << " Over bought, short and entry price is :" << entry_price[j]  << "for date " << dates[j] << std::endl;
                    flag = 1;
                    dates[j].pop_back();
                    if (first_trigger == 0){
                        ent_pric = entry_price[j];
                        ent_date = dates[j];
                    }
                    else{
                        exit_Date = dates[j];
                        exit_pric = in_v[j];
                    }
                     
                     
                }
                else {
                    std::cout << " Over bought, short and exit price is :" << in_v[j]  << "for date " << dates[j] << std::endl;
                    flag = 0;
                    dates[j].pop_back();
                    exit_Date = dates[j];
                    exit_pric = in_v[j];
                    first_trigger = 1;
                     
                }
                trade_greater = 1;
                std::cout << std::endl;
                if(first_trigger == 1 ){
                    position = "long";
                    double profit = exit_pric - ent_pric;
                    if ( position == "short"){
                        pl = profit * size;
                        pl = -pl;
 
                    }
                    if ( position == "long"){
                        pl = profit * size;
                    }
                    cpl = cpl+pl;
                    tradenumber++;
 
                    std::cout << "table " <<tradenumber << " " << ent_date << " " << ent_pric << " " <<  exit_Date << " " <<  exit_pric << " " <<  position << " " << pl <<  " " << cpl<< std::endl; 
 
                    myfile << tradenumber << "," << ent_date << "," << ent_pric << "," <<  exit_Date << "," <<  exit_pric << "," <<  position << ","<< size << ","<<pl<< "," << cpl << "\n";
                    ent_date = exit_Date;
                    ent_pric = exit_pric;
                 
                }
                trade_greater = 1;
                 
                 
            } 
            if (RSI < 40 && trade_greater == 1 ){	// if RSI is greater than 40 then over sold and short.

                if ( flag == 0){
                    std::cout << " Over sold, long and entry price is :" << entry_price[j]  << "for date " << dates[j] << std::endl;
                    flag = 1;
                    dates[j].pop_back();
                    if (first_trigger == 0){
                        ent_pric = entry_price[j];
                         
                        ent_date = dates[j];
                         
                    }
                    else{
                        exit_Date = dates[j];
                        exit_pric = in_v[j];
                    }
                }
                else {
                    std::cout << " Over sold, long and exit price is :" << in_v[j]   << "for date " << dates[j] << std::endl;
                    flag = 0;
                    dates[j].pop_back();
                    exit_Date = dates[j];
                    exit_pric = in_v[j];
                    first_trigger = 1;  
                }
                std::cout << std::endl;
                if(first_trigger == 1){
                    position = "short";
                    double profit = exit_pric - ent_pric;
                    if ( position == "short"){
                        pl = profit * size;
                        pl = -pl;
 
                    }
                    if ( position == "long"){
                        pl = profit * size;
                    }
                    cpl = cpl+pl;
                    tradenumber++;
                    std::cout << "table " <<tradenumber << " " << ent_date << " " << ent_pric << " " <<  exit_Date << " " <<  exit_pric << " " <<  position << " " << pl << " " << cpl << std::endl;    
                    myfile << tradenumber << "," << ent_date << "," << ent_pric << "," <<  exit_Date << "," <<  exit_pric << "," <<  position << ","<< size << ","<<pl<< "," << cpl << "\n";
                    ent_date = exit_Date;
                    ent_pric = exit_pric;
                }
                trade_greater = 0;
                 
            }
        }
    }
}




// Establish a session
int main(){

SessionOptions sessionOptions;
sessionOptions.setServerHost("localhost"); 
sessionOptions.setServerPort(8194);
Session session(sessionOptions);
if (!session.start()) 
{
	std::cerr << "Failed to start session." << std::endl; 
	return 1; 
}

// Open Service

 if (!session.openService("//blp/refdata"))
 { 
	 std::cerr << "Failed to open //blp/refdata" << std::endl;
	 return 1;
 }
 //Service ref = session.getService("//blp/refdata");

// Send Request to Service

   Service ref = session.getService("//blp/refdata"); 
   Request request = ref.createRequest("HistoricalDataRequest");
   request.getElement("securities").appendValue("IBM US Equity");     
   request.getElement("fields").appendValue("PX_LAST");  
   request.getElement("fields").appendValue("PX_OPEN");    
   request.set("periodicityAdjustment", "ACTUAL");  
   request.set("periodicitySelection", "DAILY");  
   request.set("startDate", "20150809");  
   request.set("endDate", "20161031");  
   request.set("maxDataPoints", 10000);  
   session.sendRequest(request);

// Process Response
std::vector <double> values;
std::vector <std::string> dates;
std::vector <double> entry_price;
int i =0;
int done = 0;
  while (! done)
  {        
	  Event event = session.nextEvent();     
	  MessageIterator msgIter(event);         
	  while (msgIter.next())
	  {          
		  Message msg = msgIter.message();  
		  if (Event::RESPONSE== event.eventType() ||  Event::PARTIAL_RESPONSE == event.eventType())
		  {                
			  if(msg.hasElement("securityData")) {  
				  Element secs = msg.getElement("securityData");    
				  std::cout << "\n"     
					  << secs.getElementAsString("security")  
					  << std::endl;   
					Element flds = secs.getElement("fieldData"); 

				  // We get an array of value for the historical request. 

				  std::cout << "Date\t\tPX_LAST\tPX_OPEN"            
					  << std::endl;               
				 for(int i = 0; i < flds.numValues(); ++i) {  
					  Element f = flds.getValueAsElement(i); 
					  // value are stored in seperate vectors
					  std::cout << f.getElementAsString("date")   
						  << "\t"                         
						  << f.getElementAsFloat64("PX_LAST")     
						  << "\t"                     
						  << f.getElementAsFloat64("PX_OPEN")  
						  << std::endl; 
					  values.push_back(f.getElementAsFloat64("PX_LAST"));
					  dates.push_back(f.getElementAsString("date"));
					  entry_price.push_back(f.getElementAsFloat64("PX_OPEN"));
				  }
					//  i++;
			  }
			   done = 1;
		  } 
		  else {              
			  std::cout << msg << std::endl;      
		  }        
	  }  
	 
  }

 std::cout << "number of values " << values.size() << std::endl;
 test1(values,dates,entry_price);

  int in=0;
  std::cout << std::endl;
  std::cout << " Enter any key and press enter to exit" << std::endl;
  // Clean up
  std::cin >> in; 
  exit(0);
  session.stop();
}