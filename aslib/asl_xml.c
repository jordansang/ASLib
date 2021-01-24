#include "asl_type.h"
#include "asl_stdio.h"
#include "asl_syscmd.h"
#include "asl_xml.h"

result_t asl_xml_save_file(aslXmlDoc doc, char* path)
{
    result_t ret = FAILURE;
    if(doc == NULL)
    {
        asl_print_err("Invalid XML File");
    }
    else if(path == NULL)
    {
        asl_print_err("Target XML File Name is NULL");
    }
    else if(xmlSaveFormatFileEnc(path, doc, ASL_XML_ENCODE_UTF8, ENABLE) < 0)
    {
        asl_print_err("Save XML File %s FAILURE", path);
    }
    else
    {
        //asl_print_dbg("Save XML File %s SUCCESS", path);
        return asl_syscmd_sync();
    }
    
    return ret;
}

aslXmlDoc asl_xml_open_file(char* path)
{
    aslXmlDoc doc;
    if(path == NULL)
    {
        asl_print_err("Target XML File Name is NULL");
        return NULL;
    }
    doc = xmlReadFile(path, ASL_XML_ENCODE_UTF8, XML_PARSE_NOBLANKS);
    return doc;
}
result_t asl_xml_close_file(aslXmlDoc doc)
{
    xmlFreeDoc(doc);
    return SUCCESS;
}
result_t asl_xml_release()
{
    xmlCleanupParser();
    xmlMemoryDump();

    return SUCCESS;
}
aslXmlDoc asl_xml_create_file(char* path)
{
    aslXmlDoc doc;
    
    if(path == NULL)
    {
        //asl_print_warn("Target XML File Name is NULL");
    }
    doc = xmlNewDoc(BAD_CAST "1.0");
    doc->standalone = 1;

    if(path != NULL && asl_xml_save_file(doc, path) != SUCCESS)
    {
        asl_xml_close_file(doc);
        doc = NULL;
    }
    return doc;
}

result_t asl_xml_free_node(aslXmlNode node)
{
    xmlFreeNode(node);
    return SUCCESS;
}
result_t asl_xml_del_node(aslXmlNode node)
{
    aslXmlNode cnode, tnode;
    cnode = node->xmlChildrenNode;
    /* Delete all children nodes belonged to node */
    while(cnode != NULL)
    {
        tnode = cnode->next;
        xmlUnlinkNode(cnode);
        xmlFreeNode(cnode);
        cnode = tnode;
    }
    /* Delete node */
    xmlUnlinkNode(node);
    xmlFreeNode(node);

    return SUCCESS;
}
aslXmlNode asl_xml_set_root(aslXmlDoc doc, char* name)
{
    aslXmlNode root;
    root = xmlNewNode(NULL, BAD_CAST name);
    if(root == NULL)
    {
        asl_print_err("Create Root Node FAILURE");
    }
    else if(xmlDocSetRootElement(doc, root) != 0)
    {
        asl_print_err("Insert Root Node FAILURE");
        asl_xml_free_node(root);
        root = NULL;
    }

    return root;
}

aslXmlNode asl_xml_get_root(aslXmlDoc doc)
{
    return (xmlDocGetRootElement(doc));
}

result_t asl_xml_set_prop(aslXmlNode node, char* propName, char* propVal)
{
    xmlNewProp(node, BAD_CAST propName, BAD_CAST propVal);
    return SUCCESS;
}

char* asl_xml_get_prop(aslXmlNode node, char* propName)
{
    return ((char*)(xmlGetProp(node, BAD_CAST propName)));
}

aslXmlNode asl_xml_new_child(aslXmlNode pnode, char* tag, char* content)
{
    return xmlNewChild(pnode, NULL, BAD_CAST tag, BAD_CAST content);
}

char* asl_xml_get_content(aslXmlNode pnode, char* tag)
{
    aslXmlNode node;
    if(tag == NULL)
    {
        return ((char*)xmlNodeGetContent(pnode));
    }
    FOREACH_XMLNODE(pnode, node, tag)
    {
        return ((char*)xmlNodeGetContent(node));
    }
    return NULL;
}
result_t asl_xml_set_content(aslXmlNode pnode, char* tag, char* content)
{
    aslXmlNode node;
    FOREACH_XMLNODE(pnode, node, tag)
    {
        xmlNodeSetContent(node, BAD_CAST content);
        return SUCCESS;
    }
    return (asl_xml_new_child(pnode, tag, content) == NULL ? FAILURE : SUCCESS);
}
u_int32_t asl_xml_get_uint(aslXmlNode pnode, char* tag)
{
    char* content;
    u_int32_t target;
    content = asl_xml_get_content(pnode, tag);
    if(content != NULL)
    {
        target = atoi(content);
        xmlFree(content);
        return target;
    }
    asl_print_err("Get <%s> -> <%s> Failed", pnode->name, tag);
    return FAILURE;
}
result_t asl_xml_set_uint(aslXmlNode pnode, char* tag, u_int32_t value)
{
    char valueStr[32] = {0};
    sprintf(valueStr, "%u", value);
    return (asl_xml_set_content(pnode, tag, valueStr));
}
result_t asl_xml_dump(aslXmlDoc doc)
{
    xmlSaveFormatFileEnc("-", doc, ASL_XML_ENCODE_UTF8, ENABLE);
    return SUCCESS;
}