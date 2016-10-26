#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xmlreader.h>

#include "xml_vatic_pascal.h"
#include "ReadFile.h"

#pragma comment(lib,"iconv.lib")
#pragma comment(lib,"zdll.lib")
#pragma comment(lib,"libxml2.lib")

xmlDocPtr getdoc(const wchar_t *docname)
{
	std::vector<char> buf = ReadFile(docname);

	xmlDocPtr doc;
	//doc = xmlParseFile(docname);
	doc = xmlReadMemory(buf.data(), buf.size(), "vatic_pascal.xml", NULL, 0);

	if (doc == NULL) {
		fprintf(stderr, "Document not parsed successfully. \n");
		return NULL;
	}

	return doc;
}

void putdoc(const wchar_t *docname, xmlDocPtr doc)
{
	xmlChar *buf;
	int size;
	xmlDocDumpMemory(doc, &buf, &size);

	std::FILE *file = _wfopen(docname, L"wb");
	std::fwrite(buf, 1, size, file);
	std::fclose(file);
}

xmlXPathObjectPtr getNodeSet(xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) {
		printf("Error in xmlXPathEvalExpression\n");
		return NULL;
	}
	if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
		xmlXPathFreeObject(result);
		printf("No result\n");
		return NULL;
	}
	return result;
}

xmlNodePtr find1stChild(xmlNodePtr node, const char *name)
{
	xmlNodePtr ptr = node->children;
	while (ptr != nullptr)
	{
		if (xmlStrcmp(ptr->name, (xmlChar*)name) == 0)
		{
			return ptr;
		}
		ptr = ptr->next;
	}
}

static std::string get1stChildContent(xmlNodePtr node, const char *name)
{
	xmlNodePtr target = find1stChild(node, name);
	return std::string((const char*)target->children->content);
}

static vatic_object toObj(const xmlNodePtr & obj)
{
	std::string name = get1stChildContent(obj, "name");
	std::string difficult = get1stChildContent(obj, "difficult");
	std::string occluded = get1stChildContent(obj, "occluded");
	std::string pose = get1stChildContent(obj, "pose");
	std::string truncated = get1stChildContent(obj, "truncated");

	const xmlNodePtr bndBox = find1stChild(obj, "bndbox");
	std::string xmax = get1stChildContent(bndBox, "xmax");
	std::string xmin = get1stChildContent(bndBox, "xmin");
	std::string ymax = get1stChildContent(bndBox, "ymax");
	std::string ymin = get1stChildContent(bndBox, "ymin");

	vatic_object vObj;
	vObj.name = name;
	vObj.xmax = atoi((const char*)xmax.c_str());
	vObj.xmin = atoi((const char*)xmin.c_str());
	vObj.ymax = atoi((const char*)ymax.c_str());
	vObj.ymin = atoi((const char*)ymin.c_str());
	vObj.difficult = atoi((const char*)difficult.c_str());
	vObj.occluded = atoi((const char*)occluded.c_str());
	vObj.pose = pose;
	vObj.truncated = atoi((const char*)truncated.c_str());
	return vObj;
}

std::vector<vatic_object> xml_vatic_pascal_parse(std::wstring filename)
{
	std::vector<vatic_object> result;	

	xmlDocPtr doc = getdoc(filename.c_str());
	xmlChar *xpath = (xmlChar*) "//annotation/object";
	xmlXPathObjectPtr objects = getNodeSet(doc, xpath);
	if (objects)
	{
		xmlNodeSetPtr nodeset = objects->nodesetval;
		for (int i = 0; i < nodeset->nodeNr; i++)
		{
			const xmlNodePtr &obj = nodeset->nodeTab[i];
			vatic_object vObj = toObj(obj);
			result.push_back(vObj);
		}
		xmlXPathFreeObject(objects);
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();

	return result;
}

// convert obj to a xmlNode
static xmlNodePtr toXml(const vatic_object &obj)
{
	xmlNodePtr root = xmlNewNode(NULL, (const xmlChar*)"object");
	xmlNewChild(root, NULL, (const xmlChar*)"name", (const xmlChar*)obj.name.c_str());

	xmlNodePtr bndbox = xmlNewChild(root, NULL, (const xmlChar*)"bndbox", NULL);
	xmlNewChild(bndbox, NULL, (const xmlChar*)"xmax", (const xmlChar*)std::to_string(obj.xmax).c_str());
	xmlNewChild(bndbox, NULL, (const xmlChar*)"xmin", (const xmlChar*)std::to_string(obj.xmin).c_str());
	xmlNewChild(bndbox, NULL, (const xmlChar*)"ymax", (const xmlChar*)std::to_string(obj.ymax).c_str());
	xmlNewChild(bndbox, NULL, (const xmlChar*)"ymin", (const xmlChar*)std::to_string(obj.ymin).c_str());

	xmlNewChild(root, NULL, (const xmlChar*)"difficult", (const xmlChar*)std::to_string(obj.difficult).c_str());
	xmlNewChild(root, NULL, (const xmlChar*)"occluded", (const xmlChar*)std::to_string(obj.occluded).c_str());
	xmlNewChild(root, NULL, (const xmlChar*)"pose", (const xmlChar*)obj.pose.c_str());
	xmlNewChild(root, NULL, (const xmlChar*)"truncated", (const xmlChar*)std::to_string(obj.truncated).c_str());

	return root;
}

// update all objects in filename with objs
void xml_vatic_pascal_modifyObjects(std::wstring filename,
	const std::vector<vatic_object> &objs)
{
	xmlDocPtr doc = getdoc(filename.c_str());
	xmlChar *xpath = (xmlChar*) "//annotation/object";
	xmlXPathObjectPtr objects = getNodeSet(doc, xpath);
	if (objects)
	{
		xmlNodeSetPtr nodeset = objects->nodesetval;
		for (int i = 0; i < nodeset->nodeNr; i++)
		{
			const xmlNodePtr &obj = nodeset->nodeTab[i];
			xmlUnlinkNode(obj);
			xmlFreeNode(obj);			
		}
		xmlXPathFreeObject(objects);
	}

	xpath = (xmlChar*)"//annotation/filename";
	objects = getNodeSet(doc, xpath);
	if (objects)
	{
		xmlNodePtr cur = objects->nodesetval->nodeTab[0];
		for (auto obj : objs)
		{
			xmlNodePtr xml = toXml(obj);
			cur = xmlAddNextSibling(cur, xml);
		}
		xmlXPathFreeObject(objects);
	}
	
	putdoc(filename.c_str(), doc);

	xmlFreeDoc(doc);
	xmlCleanupParser();	
}