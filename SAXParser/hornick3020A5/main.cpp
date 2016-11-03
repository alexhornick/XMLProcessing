/*************
Alex Hornick
CSCI 3020 Section 11
Fall 2016
Assignment 5

Programmed on Windows 10 using Visual Studio Community 2013
This program reads in one XML file, parses it using a SAX Parser, and then writes to 3 separate 
XML files using the data structures from the parser.
**************/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace std;
using namespace xercesc;

struct Country {
	string countryName;
	vector<string> cities;
	vector<string> religions;
};

class MySaxHandler : public DefaultHandler {
private:
	bool inCountry, inCountryName;
	bool inCity, inCityName;
	bool inReligion;
	string countryName = "";
	string cityName = "";
	vector<Country> *countries;
	vector<string> cities;
	vector<string> religions;
	string countryNameBuffer, cityNameBuffer, religionNameBuffer;
public:
	MySaxHandler(vector<Country> *countryList) {		
		inCountry, inCity, inReligion, inCountryName, inCityName = false;
		countries = countryList;
	}

	void startElement(
		const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname,
		const Attributes& attrs
		);

	void characters(
		const XMLCh* const chars,
		const XMLSize_t length
		);

	void endElement(
		const XMLCh* const uri,
		const XMLCh* const localname,
		const XMLCh* const qname
		);

	void fatalError(const SAXParseException&);
};

void MySaxHandler::startElement(
	const XMLCh* const uri,
	const XMLCh* const localname,
	const XMLCh* const qname,
	const Attributes& attrs
	) {
	char* message = XMLString::transcode(localname);
	if (strcmp(message, "country") == 0) {
		inCountry = true;
		countryName = "";
	}
	else if (strcmp(message, "name") == 0 && !inCity)
	{
		countryNameBuffer = "";
		inCountryName = true;
	}
	else if (inCountry && strcmp(message, "city") == 0) {
		inCity = true;
		cityName = "";
	}
	else if (strcmp(message, "name") == 0 && inCity)
	{
		inCityName = true;
		cityNameBuffer = "";
	}
	
	else if (strcmp(message, "religions") == 0)
	{
		inReligion = true;
		religionNameBuffer = "";
	}

	XMLString::release(&message);
}

void MySaxHandler::characters(
	const XMLCh* const chars,
	const XMLSize_t length
	) {

	if (inCountry && inCountryName && !inCity && !inReligion && countryNameBuffer == "") {
		char *message = new char[length + 1];
		XMLString::transcode(chars, message, length);
		countryNameBuffer += message;
		if (countryName == "")
		{
			countryName = countryNameBuffer;
		}

		delete[] message;
	}
	
	else if (inCountry && inCity && inCityName)
	{
		char *message = new char[length + 1];
		XMLString::transcode(chars, message, length);
		cityNameBuffer += message;
		if (cityName == "")
		{
			cityName = cityNameBuffer;
		}

		delete[] message;
	}

	else if (inCountry && !inCity && inReligion)
	{
		char *message = new char[length + 1];
		XMLString::transcode(chars, message, length);
		religionNameBuffer += message;
		delete[] message;

	}

}

void MySaxHandler::endElement(
	const XMLCh* const uri,
	const XMLCh* const localname,
	const XMLCh* const qname
	) {
	char* message = XMLString::transcode(localname);

	
	if (strcmp(message, "city") == 0) {
		inCity = false;
	}
	else if (strcmp(message, "name") == 0 && !inCity && inCountry && inCountryName)
	{
		inCountryName = false;
	}
	else if (strcmp(message, "name") == 0 && inCity && inCityName)
	{	
		cities.push_back(cityName);
		inCityName = false;
	}
	else if (strcmp(message, "religions") == 0 && inReligion) {
		inReligion = false;
		religions.push_back(religionNameBuffer);
	}
	else if (strcmp(message, "country") == 0)
	{
		inCountry = false;

		Country country;
		country.cities = cities;
		country.religions = religions;
		country.countryName = countryName;
		
		countries->push_back(country);
		cities.clear();
		religions.clear();
	}

	XMLString::release(&message);
}

void MySaxHandler::fatalError(const SAXParseException& exception) {
	char* message = XMLString::transcode(exception.getMessage());
	cout << "XML ERROR OCCURRED: " << message << endl;
	XMLString::release(&message);
}

int main() {
	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException &exception) {
		cout << "ERROR: unable to initialize the parser\n";
		system("pause");
		return 1;
	}

	string xmlFilename = "mondial-3.0.xml";
	vector<Country> countries;

	SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
	parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
	parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);

	MySaxHandler* handler = new MySaxHandler(&countries);
	parser->setContentHandler(handler);
	parser->setErrorHandler(handler);

	try {
		parser->parse(xmlFilename.c_str());
	}
	catch (const XMLException& exception1) {
		char* message = XMLString::transcode(exception1.getMessage());
		cout << "XML ERROR OCCURRED: " << message << endl;
		XMLString::release(&message);
		system("pause");
		return 1;
	}
	catch (const SAXParseException& exception2) {
		char* message = XMLString::transcode(exception2.getMessage());
		cout << "PARSE ERROR OCCURRED: " << message << endl;
		XMLString::release(&message);
		system("pause");
		return 1;
	}
	catch (...) {
		cout << "UNEXPECTED ERROR\n";
		system("pause");
		return 1;
	}

	delete parser;
	delete handler;

	//SENT DATA TO 3 XML FILES
	string xmlComment = "<!-- \n Alex Hornick CSCI 3020 Section 11 \n Fall 2016 \n Assignment 5 \n -->";

	ofstream countryXML, citiesXML, religionsXML;

	//COUNTRY.XML
	countryXML.open("country.xml");
	countryXML << "<?xml version='1.0' encoding='UTF-8'?>" << endl << xmlComment << endl; 

	countryXML << "<countries>" << endl;
	for (int i = 0; i < countries.size(); i++)
	{
		countryXML << "\t<country>" << countries.at(i).countryName << "</country>" << endl;
	}
	countryXML << "</countries>";
	countryXML.close();


	//CITIES.XML
	citiesXML.open("cities.xml");
	citiesXML << "<?xml version='1.0' encoding='UTF-8'?>" << endl << xmlComment << endl;;

	citiesXML << "<cities>" << endl;
	for (int i = 0; i < countries.size(); i++)
	{
		for (int j = 0; j < countries.at(i).cities.size(); j++)
		{
			citiesXML << "\t<city>" << endl
				<< "\t\t<city-name>" << countries.at(i).cities.at(j) << "</city-name>" << endl
				<< "\t\t<country-name>" << countries.at(i).countryName << "</country-name>" << endl
				<< "\t</city>" << endl;
		}
	}
	citiesXML << "</cities>";
	citiesXML.close();

	//RELIGIONS.XML
	religionsXML.open("religions.xml");
	religionsXML << "<?xml version='1.0' encoding='UTF-8'?>" << endl << xmlComment << endl;;

	religionsXML << "<religions>" << endl;
	for (int i = 0; i < countries.size(); i++)
	{
		religionsXML << "\t<country>" << endl
			<< "\t\t<country-name>" << countries.at(i).countryName << "</country-name>" << endl
			<< "\t\t<religion-list>" << endl;
		
		for (int j = 0; j < countries.at(i).religions.size(); j++)
		{
			religionsXML << "\t\t\t<religion>" << countries.at(i).religions.at(j) << "</religion>" << endl;
		}
		religionsXML << "\t\t</religion-list>" << endl << "\t</country>" << endl;
	}
	religionsXML << "</religions>";
	religionsXML.close();

	system("Pause");
	return 0;
}
