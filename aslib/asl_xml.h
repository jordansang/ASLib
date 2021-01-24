#ifndef ASL_XML_H
#define ASL_XML_H

#include <libxml/parser.h>
#include <libxml/tree.h>

#define ASL_XML_ENCODE_UTF8 "UTF-8"
#define ASL_XML_ROOT_NAME "root"

typedef xmlDocPtr aslXmlDoc;
typedef xmlNodePtr aslXmlNode;
typedef xmlChar aslXmlChar;

#define asl_xml_strcmp(SRC, DEST) xmlStrcmp(SRC, BAD_CAST DEST)
#define asl_xml_free(content) xmlFree(content)

#define FOREACH_XMLNODE(ROOT, NODE, KEY) \
    for(NODE = ROOT->xmlChildrenNode; NODE != NULL; NODE = NODE->next) \
        if(asl_xml_strcmp(NODE->name, KEY) == 0)


result_t asl_xml_save_file(aslXmlDoc doc, char* path);
aslXmlDoc asl_xml_open_file(char* path);
result_t asl_xml_close_file(aslXmlDoc doc);
result_t asl_xml_release();
aslXmlDoc asl_xml_create_file(char* path);
aslXmlNode asl_xml_new_node(aslXmlNode pnode, char* name);
result_t asl_xml_free_node(aslXmlNode node);
result_t asl_xml_del_node(aslXmlNode node);
aslXmlNode asl_xml_set_root(aslXmlDoc doc, char* name);
aslXmlNode asl_xml_get_root(aslXmlDoc doc);
result_t asl_xml_set_prop(aslXmlNode node, char* propName, char* propVal);
char* asl_xml_get_prop(aslXmlNode node, char* propName);
aslXmlNode asl_xml_new_child(aslXmlNode pnode, char* tag, char* content);
char* asl_xml_get_content(aslXmlNode pnode, char* tag);
result_t asl_xml_set_content(aslXmlNode pnode, char* tag, char* content);
u_int32_t asl_xml_get_uint(aslXmlNode pnode, char* tag);
result_t asl_xml_set_uint(aslXmlNode pnode, char* tag, u_int32_t value);
result_t asl_xml_dump(aslXmlDoc doc);

#endif /* ASL_XML_H */