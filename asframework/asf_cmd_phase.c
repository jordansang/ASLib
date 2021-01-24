#include "asl_type.h"
#include "asl_stdio.h"
#include "asl_xml.h"

result_t asf_cmd_phase_init(char* file)
{
    aslXmlDoc doc;
    aslXmlNode root;
    doc = asl_xml_open_file(file);
    if(doc == NULL)
    {
        asl_print_dbg("Open Command File %s Failed, Create New One", file);
        if((doc = asl_xml_create_file(file)) == NULL)
        {
            asl_print_err("Create Command File %s Failed", file);
            return FAILURE;
        }
        root = asl_xml_set_root(doc, "root");
        asl_xml_set_prop(root, "owner", "Router Manager");
    }
    root = asl_xml_get_root(doc);
    
    return asl_xml_save_file(doc, file);
}
