#include "asl_stdio.h"
#include "asl_memory.h"
#include "asd_tree.h"
/************************************************************************* 
Function name: asd_tree_lookup

Function Description:
    Find leaf with target data, it's a recursion function.
    (The function will return the first node matched the rule, if there 
    are nodes which have the same data, it may cause bugs)

Parameters:
    tree    -- target tree pointer
    node    -- current pointer branch or leaf
    data    -- target data
    
Return:
    branch or leaf which has the same data with target
    if not find, return NULL

*************************************************************************/
static struct asd_tree_item* asd_tree_lookup(struct asd_tree *tree, struct asd_tree_item* node, void *data)
{
    //asl_print_dbg("Tree Lookup for val:%2d, current val:%2d", *((int*)(data)),*((int*)(node->data)));
    struct asd_list_item *listNode;
    struct asd_tree_item *treeNode;
    /* If not initialize root, node will be NULL when fisrt compare */
    if(tree->cmp == NULL || node == NULL)
    {
        return NULL;
    }
    if(tree->cmp(node->data, data) == 0)
    {
        return node;
    }
    if(node->childList != NULL)
    {
        listNode = node->childList->head;
        while(TRUE)
        {
            treeNode = asd_tree_lookup(tree, listNode->data, data);
            if(treeNode != NULL)
            {
                return treeNode;
            }
            if(listNode == node->childList->tail)
            {
                break;
            }
            listNode = listNode->next;
        }
    }
    return NULL;
}
/************************************************************************* 
Function name: tree_del_leaf

Function Description:
    Delete leaf from branch. The leaf should not have any subordinate 
    leaves or branches, or it may cause some bugs. This should be confirmed
    by outer functions.

Parameters:
    tree    -- target tree pointer
    branch  -- target branch that have the leaf or be NULL for root
    leaf    -- target leaf
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
static result_t asd_tree_del_leaf(struct asd_tree* tree, struct asd_tree_item* branch, struct asd_tree_item* leaf)
{
    struct asd_list_item *listNode;
    struct asd_tree_item *node;
    /* Delete the root */
    if(branch == NULL)
    {
        asl_free(leaf);
        tree->root = NULL;
        return SUCCESS;
    }
    /* Delete leaf on certain branch */
    ASD_LIST_LOOP(branch->childList, node, listNode)
    {
        if(node == leaf)
        {
            //asl_print_dbg("Delete node %2d on branch %2d", *((int*)(node->data)),*((int*)(branch->data)));
            asd_list_del_item(branch->childList, leaf);
            tree->del(leaf->data);
            asl_free(leaf);
            if(branch->childList->count == 0)
            {
                //asl_print_dbg("Branch %2d has no leaves, free the leaf container", *((int*)(branch->data)));
                asd_list_destroy(branch->childList);
                branch->childList = NULL;
            }
            return SUCCESS;
        }
    }
    
    return FAILURE;
}
/************************************************************************* 
Function name: tree_del_all_node

Function Description:
    Delete all branches and leaves in target branch, then delete the target
    branch. The target branch can be any branch in the tree or the root.
    This is a recursion function, so it may consume more cpu.

Parameters:
    tree    -- target tree pointer
    node    -- target branch to delete
    parent  -- parent of target branch, if branch is root, parent 
               shuold be NULL
    
Return:
    SUCCESS
    FAILURE

*************************************************************************/
static result_t asd_tree_del_all_node(struct asd_tree* tree, struct asd_tree_item* node, struct asd_tree_item* parent)
{
    while(node->childList != NULL)
    {
        asd_tree_del_all_node(tree, asd_list_head(node->childList), node);
    }
    return asd_tree_del_leaf(tree, parent, node);
}
/************************************************************************* 
Function name: tree_get_node

Function Description:
    Get leaf or branch with the target data. This is the interface for 
    outer functions.

Parameters:
    tree    -- target tree pointer
    data    -- target data
    
Return:
    branch or leaf which has the same data with target
    if not find, return NULL
    
*************************************************************************/
struct asd_tree_item* asd_tree_get_item(struct asd_tree* tree, void* data)
{
    //asl_print_dbg("Get node %2d", *((int*)(data)));
    return asd_tree_lookup(tree, tree->root, data);
}
/************************************************************************* 
Function name: tree_add_node

Function Description:
    Add a new leaf to target branch. The target branch contains the same 
    data with parent. This function can used to add root.

Parameters:
    tree    -- target tree pointer
    parent  -- target branch data, if add root, it shuold be NULL
    data    -- target data
    
Return:
    SUCCESS
    FAILURE
    
*************************************************************************/
result_t asd_tree_add(struct asd_tree* tree, void* parent, void* data)
{
    struct asd_tree_item* parentNode;
    struct asd_tree_item* newNode;
    if(tree->root == NULL)
    {
        parentNode = NULL;
    }
    else
    {
        parentNode = asd_tree_get_item(tree, parent);
        if(parentNode == NULL)
        {
            //asl_print_dbg("Can't find parent %2d",*((int*)(parent)));
            return FAILURE;
        }
    }
    if(parentNode != NULL)
    {
        //asl_print_dbg("tree add Node %2d to branch %2d", *((int*)(data)),*((int*)(parentNode->data)));
    }
    newNode = ASL_MALLOC_T(struct asd_tree_item);
    newNode->childList = NULL;
    newNode->parent = parentNode;
    newNode->data = data;
    if(tree->root == NULL)
    {
        tree->root = newNode;
    }
    else
    {
        if(parentNode->childList == NULL)
        {
            parentNode->childList = asd_list_create();
        }
        asd_list_add(parentNode->childList, newNode);
    }
    return SUCCESS;
}
/************************************************************************* 
Function name: tree_del_node

Function Description:
    Delete leaf or branch. If delete branch, the function will delete all 
    subordinate branches and leaves of the branch.

Parameters:
    tree    -- target tree pointer
    data    -- target data
    
Return:
    SUCCESS
    FAILURE
    
*************************************************************************/
result_t asd_tree_del(struct asd_tree* tree, void* data)
{
    //asl_print_dbg("del target val:%02d",*((int*)(data)));
    struct asd_tree_item* node = asd_tree_lookup(tree, tree->root, data);
    if(node == NULL)
    {
        return FAILURE;
    }
    //asl_print_dbg("del find node with val:%02d",*((int*)(node->data)));
    
    return asd_tree_del_all_node(tree, node, node->parent);
}
/************************************************************************* 
Function name: tree_create

Function Description:
    Create a new tree.

Parameters:
    cmp    -- comparation function for data
    del    -- deletation function for data
    
Return:
    tree
    NULL for FAILURE
    
*************************************************************************/
struct asd_tree* asd_tree_create()
{
    struct asd_tree* tree = ASL_MALLOC_T(struct asd_tree);
    if(tree == NULL)
    {
        return NULL;
    }
    tree->root = NULL;
    return (tree);
}
/************************************************************************* 
Function name: tree_destroy

Function Description:
    Destroy a tree. This function will also release all subordinate branches
    and leaves in the tree.

Parameters:
    tree    -- target tree pointer
    
Return:
    SUCCESS
    FAILURE
    
*************************************************************************/
#define USE_RECURSION_METHOD
result_t asd_tree_destroy(struct asd_tree* tree)
{
    if(tree == NULL)
    {
        return SUCCESS;
    }
#ifdef USE_RECURSION_METHOD
    /* Use the recursion function to delete all nodes in the tree */
    asd_tree_del_all_node(tree, tree->root, NULL);
#else
    int ret;
    struct asd_tree_item* branch;
    while(tree->root)
    {
        /*
        ** Everytime, I will get the head node of the child list at the deepest in one direction.
        ** I choose its parent as branch and do deletion. Then I will run the loop from root again.
        ** Finially, I will delete the root from the tree, and break the loop.
        */
        branch = tree->root;
        while(branch->childList != NULL)
        {
            branch = asd_list_head(branch->childList);
        }
        branch = branch->parent;
        /* Only root regards NULL as parent pointer */
        if(branch == NULL)
        {
            ret = asd_tree_del_leaf(tree, NULL, tree->root);
            break;
        }
        ret = asd_tree_del_leaf(tree, branch, asd_list_head(branch->childList));
        if(ret == FAILURE)
        {
            return ret;
        }
    }
#endif
    asl_free(tree);
    tree = NULL;
    return SUCCESS;
}

