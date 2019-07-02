#include <stdio.h>
#include <libxml/parser.h>

 
int is_leaf(xmlNode * node)
{
  xmlNode * child = node->children;
  while(child)
  {
    if(child->type == XML_ELEMENT_NODE) return 0;
 
    child = child->next;
  }
 
  return 1;
}
 
void print_attr(xmlNode * node)
{
    while(node)
    {
        if(node->type == XML_ELEMENT_NODE)
        {
			printf("key(%s):", node->name);
			if (is_leaf(node))
			{
				printf("value1(%s)\n", xmlNodeGetContent(node));
			}
			else
			{
				// means it had more childs
				printf("value2(%s)\n", xmlGetProp(node, "id"));
			}
        }
        print_attr(node->children);
        node = node->next;
    }
} 

void fetch_card (xmlNode * node)
{
	printf("key(%s):", node->name);
	if (is_leaf(node))
	{
		printf("value1(%s)\n", xmlNodeGetContent(node));
	}
	else
	{
		// means it had more childs
		printf("value2(%s)\n", xmlGetProp(node, "id"));
	}
	print_attr(node->children);
}

void print_xml(xmlNode * node)
{
    while(node)
    {
        if(node->type == XML_ELEMENT_NODE)
        {
			if ((!xmlStrcmp(node->name, (const xmlChar *)"card")))
			{
				printf("Fetching card properties\n");
				fetch_card(node);
			}
			if ((!xmlStrcmp(node->name, (const xmlChar *)"jbv")))
			{
				printf("Fetching jbv properties\n");
				fetch_card(node);
			}
			if ((!xmlStrcmp(node->name, (const xmlChar *)"print")))
			{
				printf("Fetching print properties\n");
				fetch_card(node);
			}
			if ((!xmlStrcmp(node->name, (const xmlChar *)"scan")))
			{
				printf("Fetching scan properties\n");
				fetch_card(node);
			}
        }
		print_xml(node->children);
        node = node->next;
    }
}


 
int main(int argc, char **argv)
{
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

	doc = xmlReadFile("config.xml", NULL, 0);

	if (doc == NULL) {
		printf("Could not parse the XML file");
	}

	root_element = xmlDocGetRootElement(doc);
	print_xml(root_element);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return 0;
}
