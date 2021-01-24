#ifndef ASD_TREE_H
#define ASD_TREE_H

#include "asd_list.h"
/* Tree Node can be branch or leaf, root has no parent */
struct asd_tree_item
{
    struct asd_tree_item *parent;
    struct asd_list *childList;
    
    void* data;
};


/* Basal Tree struct */
struct asd_tree
{
    struct asd_tree_item *root;

    cmp_func cmp;
    del_func del;
};


/* Prototypes */
struct asd_tree_item* asd_tree_get_item(struct asd_tree* tree, void* data);
result_t asd_tree_add(struct asd_tree* tree, void* parent, void* data);
result_t asd_tree_del(struct asd_tree* tree, void* data);
struct asd_tree* asd_tree_create();
result_t asd_tree_destroy(struct asd_tree* tree);


#endif /* ASD_TREE_H */
