#include <cstdlib>

#include <libxml/parser.h>
#include <iostream>
#include <string>

using namespace std;

class CReadConfig
{
public:
    CReadConfig();
    ~CReadConfig(){};

    bool ReadConfigFile();
    xmlNode *FetchNodeByKey (string nodeName);
    string FetchValueByKey (string nodeName);
    bool FetchAndSetEnvironment ();
    bool CloseConfigFile();
    bool SetCurrentNode (string nodeName);
    int is_leaf(xmlNode * node);
	string ShowCurrentNode ();
	string FetchValueByPath (string path);
	bool SetAboveParentToCurrentNode ();
	string FetchCurrentValue ();
        
private:
    xmlDoc *document;
    xmlNode *root;
    xmlNode *current;
    string filename = "config.xml";        
};

CReadConfig::CReadConfig()
{
    document = NULL;
    root = NULL;
	current = NULL;
}

string CReadConfig::ShowCurrentNode ()
{
	if (current == NULL)
	{
		return string("");
	}
	else
	{
		std::string sName((char*) current->name);
		return sName;
	}
}

string CReadConfig::FetchValueByPath (string path)
{
	string result = "";
	string temppath = path;
	xmlNode *temp = current;
	std::string delimiter = "/";
	cout << "CReadConfig::FetchValueByPath: trying to find path in current node " << path << endl;
	size_t pos = 0;
	std::string token;
	while ((pos = temppath.find(delimiter)) != std::string::npos) {
		token = temppath.substr(0, pos);
		if (token == "")
		{
			continue;
		}
		//std::cout << token << std::endl;
		if (SetCurrentNode(token))
		{
			cout << "CReadConfig::FetchValueByPath: Current node is changed to " << token << endl;
		}
		else
		{
			cout << "CReadConfig::FetchValueByPath: Current node cannot be changed to " << token << ". Path Not found (" << path << ") " << endl;
			current = temp;
			return result;
		}
		temppath.erase(0, pos + delimiter.length());
	}
	if (temppath != "")
	{
		if (SetCurrentNode(temppath))
		{
			cout << "CReadConfig::FetchValueByPath: Current node is changed to " << temppath << endl;
		}
		else
		{
			cout << "CReadConfig::FetchValueByPath: Current node cannot be changed to " << temppath << ". Path Not found (" << path << ") " << endl;
			current = temp;
			return result;
		}
	}
	result = FetchCurrentValue();
	cout << "CReadConfig::FetchValueByPath : Value fetched " << result << endl;
	current = temp;
	return result;
}

bool CReadConfig::ReadConfigFile ()
{
    cout << "CReadConfig::ReadConfigFile: parsing " << filename << " file" << endl;
    if (FILE *file = fopen(filename.c_str(), "r")) {
        fclose(file);
        cout << "CReadConfig::ReadConfigFile: file " << filename << " found" << endl;
    } else {
        cout << "CReadConfig::ReadConfigFile: file " << filename << " not found" << endl;
        return false;
    } 
    
    document = xmlReadFile(filename.c_str(), NULL, 0);

    if (document == NULL)
    {
        cout << "CReadConfig::ReadConfigFile: not able to parse " << filename << " file" << endl;
        return false;
    }
    root = xmlDocGetRootElement(document);
    current = root;
	cout << "1): " << ShowCurrentNode() << endl;
    if (FetchAndSetEnvironment())
    {
        cout << "CReadConfig::ReadConfigFile: Successfully able to parse " << filename << " file" << endl;
        // by default ssbt_configuration tag will be root
//        // fetching for ssbt_configuration tag
//        root = FetchNodeByKey(config_tag);
//        if (!root)
//        {
//            cout << "CReadConfig::ReadConfigFile: not able to parse after fetch FetchNodeByKey for " << config_tag << " node" << endl;
//            return false;
//        }
        return true;
    }
    else
    {
        cout << "CReadConfig::ReadConfigFile: not able to parse after Environment fetch" << filename << " file" << endl;
        return false;
    }
}

bool CReadConfig::CloseConfigFile ()
{
    cout << "CReadConfig::CloseConfigFile: closing " << filename << " file" << endl;
    if (document)
    {
        xmlFreeDoc(document);
    }
}


int CReadConfig::is_leaf(xmlNode * node)
{
    xmlNode * child = node->children;
    while(child)
    {
        if(child->type == XML_ELEMENT_NODE) return 0;
        child = child->next;
    }
    return 1;
}

string CReadConfig::FetchCurrentValue ()
{
    xmlNode * node = current;
    while(node)
    {
		//cout << "CReadConfig::FetchCurrentValue Running for current " << node->name << endl;
        if(node->type == XML_ELEMENT_NODE)
        {
            if (is_leaf(node))
            {
                cout << "CReadConfig::FetchCurrentValue: Successfully able to fetch value for " << node->name << endl;
                return string((char *)xmlNodeGetContent(node));
            }
        }
        node = node->next;
    }
    cout << "CReadConfig::FetchCurrentValueByString: not able to fetch value for current node" << endl;
    return string("");
}

string CReadConfig::FetchValueByKey (string nodeName)
{
    xmlNode * node = current->children;
    while(node)
    {
		//cout << "CReadConfig::FetchValueByKey Running for current " << node->name << endl;
        if(node->type == XML_ELEMENT_NODE)
        {
            if ((!xmlStrcmp(node->name, (const xmlChar *)nodeName.c_str()) && is_leaf(node)))
            {
                cout << "CReadConfig::FetchValueByKey: Successfully able to fetch value for " << nodeName << endl;
                return string((char *)xmlNodeGetContent(node));
            }
        }
        node = node->next;
    }
    cout << "CReadConfig::FetchValueByKey: not able to fetch value for " << nodeName << endl;
    return string("");
}

xmlNode *CReadConfig::FetchNodeByKey (string nodeName)
{
	cout << "CReadConfig::FetchNodeByKey finding " << nodeName << endl;
    xmlNode * child = current->children;
    while (child)
    {
		cout << "CReadConfig::FetchNodeByKey Running for child " << child->name << endl;
        if ((!xmlStrcmp(child->name, (const xmlChar *)nodeName.c_str())))
        {
            cout << "CReadConfig::FetchNodeByKey: Successfully able to find node " << nodeName << " in config file" << endl;
            return child;
        }
        child = child->next;
    }
    cout << "CReadConfig::FetchNodeByKey: not able to find node " << nodeName << " in config file" << endl;
    return NULL;
}


bool CReadConfig::SetCurrentNode (string nodeName)
{
    xmlNode * temp;
    temp = FetchNodeByKey(nodeName);
    if (temp == NULL)
    {
        cout << "CReadConfig::SetCurrentNode: not able to get tag in config file (" << filename << ") for (" << nodeName << ") nodeName" << endl;
        return false;
    }
    else
    {
		cout << "CReadConfig::SetCurrentNode: Successfully able to get tag in config file (" << filename << ") for (" << nodeName << ") nodeName" << endl;
        current = temp;
		cout << "3): " << ShowCurrentNode() << endl;
    }
    return true;
}

bool CReadConfig::FetchAndSetEnvironment ()
{
   string env = "";
	if(getenv("SSBT_HAL_CONFIGURATION"))
	{
			
			env = getenv("SSBT_HAL_CONFIGURATION");
	}
    if (env == "")
    {
        // no environment is set, hence going to consider common environment only.
        env = "common";
    }
    cout << "CReadConfig::FetchAndSetEnvironment: parsing (" << filename << ") file for (" << env << ") Environment" << endl;
    current = FetchNodeByKey(env);
	cout << "2): " << ShowCurrentNode() << endl;
    if (current == NULL)
    {
        cout << "CReadConfig::FetchAndSetEnvironment: not able to parse config file (" << filename << ") for (" << env << ") Environment" << endl;
        return false;
    }
    return true;
}


bool CReadConfig::SetAboveParentToCurrentNode ()
{
	cout << "About to change from " << ShowCurrentNode() << endl;
    current = current->parent;
	cout << "About to change to " << ShowCurrentNode() << endl;
}

int main ()
{

    CReadConfig *pReadConfig = new CReadConfig();
    if (pReadConfig->ReadConfigFile())
    {
		cout << "===============================" << endl;
		cout << pReadConfig->FetchValueByPath("magtek_card/msrDirection/dev") << endl;
		cout << "===============================" << endl;
        if (pReadConfig->SetCurrentNode("magtek_card"))
       {
            cout << "CMagTek::CReadConfig: encoderType" << pReadConfig->FetchValueByKey("encoderType") << endl;
            cout << "CMagTek::CReadConfig: msrDirection" << pReadConfig->FetchValueByKey("msrDirection") << endl;
           cout << "CMagTek::CReadConfig: deviceOpenRetry" << pReadConfig->FetchValueByKey("deviceOpenRetry") << endl;
            cout << "CMagTek::CReadConfig: deviceReconnectCount" << pReadConfig->FetchValueByKey("deviceReconnectCount") << endl;
        }
		cout << "===============================" << endl;
		pReadConfig->SetAboveParentToCurrentNode();
		pReadConfig->SetAboveParentToCurrentNode();
		if (pReadConfig->SetCurrentNode("magtek_card"))
       {
            cout << "CMagTek::CReadConfig: encoderType" << pReadConfig->FetchValueByKey("encoderType") << endl;
            cout << "CMagTek::CReadConfig: msrDirection" << pReadConfig->FetchValueByKey("msrDirection") << endl;
           cout << "CMagTek::CReadConfig: deviceOpenRetry" << pReadConfig->FetchValueByKey("deviceOpenRetry") << endl;
            cout << "CMagTek::CReadConfig: deviceReconnectCount" << pReadConfig->FetchValueByKey("deviceReconnectCount") << endl;
        }
		cout << "===============================" << endl;
		pReadConfig->SetCurrentNode("dev");
				if (pReadConfig->SetCurrentNode("magtek_card"))
       {
            cout << "CMagTek::CReadConfig: encoderType" << pReadConfig->FetchValueByKey("encoderType") << endl;
            cout << "CMagTek::CReadConfig: msrDirection" << pReadConfig->FetchValueByKey("msrDirection") << endl;
           cout << "CMagTek::CReadConfig: deviceOpenRetry" << pReadConfig->FetchValueByKey("deviceOpenRetry") << endl;
            cout << "CMagTek::CReadConfig: deviceReconnectCount" << pReadConfig->FetchValueByKey("deviceReconnectCount") << endl;
        }
		cout << "===============================" << endl;
        pReadConfig->CloseConfigFile();
    }
    delete pReadConfig;
    return 0;
}
