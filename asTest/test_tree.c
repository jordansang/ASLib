#include "asl_inc.h"
#include "asd_inc.h"

static s_int32_t tree_functional_test_cmp(void *a, void* b)
{
    int *aval = (int*)a;
    int *bval = (int*)b;
    /* Node must have data */
    if(aval == NULL || bval == NULL)
    {
        return -1;
    }
    if(*aval == *bval)
    {
        return 0;
    }
    return -1;
}
static void tree_functional_test_del(void *val)
{
    return;
}
static void tree_functional_test_display_node(tree *t, int *val)
{
    tree_item *treeNode;
    treeNode = asd_tree_get_item(t, val);
    if(treeNode != NULL)
    {
        if(treeNode->childList != NULL)
        {
            asl_print_dbg("Get Node with val:%2d, node has %2d leaves or branches", *val, treeNode->childList->count);
        }
        else
        {
            asl_print_dbg("Get Node with val:%2d, node has no leaves or branches", *val);
        }
    }
    else
    {
        asl_print_dbg("There is no Node with val:%2d", *val);
    }
}
static void tree_functional_test_delete_node(tree *t, int *val)
{
    int ret;
    ret = asd_tree_del(t, val);
    if(ret < 0)
    {
        asl_print_err("Delete Tree Node with val:%2d failed", *val);
    }
    asl_print_dbg("Delete Node %2d", *val);
}
void tree_functional_test_add_node(tree *t, int *parent, int *val)
{
    int ret;
    ret = asd_tree_add(t, parent, val);
    if(ret < 0)
    {
        if(parent != NULL)
        {
            asl_print_err("Add Tree Node with val:%2d to parent:%2d failed", *val, *parent);
        }
        else
        {
            asl_print_err("Add Tree Root Node with val:%2d failed", *val);
        }
    }
    if(parent != NULL)
    {
        asl_print_dbg("Add Node %2d to Branch %2d", *val, *parent);
    }
    else
    {
        asl_print_dbg("Add Root Node %2d to Tree", *val);
    }
}
static int tree_functional_test()
{
    int ret;
    tree *t;
    int vals[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    //int testVal;
    //struct tree_node *treeNode;
    t = tree_create();
    t->cmp = tree_functional_test_cmp;
    t->del = tree_functional_test_del;
    if(t == NULL)
    {
        asl_print_err("Create Tree failed");
        return FAILURE;
    }
    /* Add root node */
    asl_print_dbg("Add Nodes to Tree");
    tree_functional_test_add_node(t, NULL, &vals[0]);
    
    tree_functional_test_add_node(t, &vals[0],&vals[1]);
    tree_functional_test_add_node(t, &vals[0],&vals[2]);
    tree_functional_test_add_node(t, &vals[0],&vals[10]);
    tree_functional_test_add_node(t, &vals[1],&vals[3]);
    tree_functional_test_add_node(t, &vals[1],&vals[4]);
    tree_functional_test_add_node(t, &vals[2],&vals[5]);
    tree_functional_test_add_node(t, &vals[2],&vals[6]);
    tree_functional_test_add_node(t, &vals[2],&vals[7]);
    tree_functional_test_add_node(t, &vals[3],&vals[8]);
    tree_functional_test_add_node(t, &vals[4],&vals[11]);
    tree_functional_test_add_node(t, &vals[5],&vals[20]);
    tree_functional_test_add_node(t, &vals[6],&vals[17]);
    tree_functional_test_add_node(t, &vals[6],&vals[18]);
    tree_functional_test_add_node(t, &vals[7],&vals[19]);
    tree_functional_test_add_node(t, &vals[8],&vals[9]);
    tree_functional_test_add_node(t, &vals[11],&vals[12]);
    tree_functional_test_add_node(t, &vals[11],&vals[13]);
    tree_functional_test_add_node(t, &vals[11],&vals[14]);
    tree_functional_test_add_node(t, &vals[14],&vals[15]);
    tree_functional_test_add_node(t, &vals[15],&vals[16]);

    asl_print_dbg("Display some nodes' information");
    tree_functional_test_display_node(t, &vals[0]);
    tree_functional_test_display_node(t, &vals[3]);
    tree_functional_test_display_node(t, &vals[11]);
    tree_functional_test_display_node(t, &vals[13]);
    tree_functional_test_display_node(t, &vals[10]);
    tree_functional_test_display_node(t, &vals[6]);
    tree_functional_test_display_node(t, &vals[2]);

    asl_print_dbg("Delete some nodes");
    tree_functional_test_delete_node(t, &vals[10]);
    tree_functional_test_delete_node(t, &vals[8]);
    tree_functional_test_delete_node(t, &vals[14]);
    tree_functional_test_delete_node(t, &vals[18]);
    tree_functional_test_delete_node(t, &vals[17]);
    tree_functional_test_delete_node(t, &vals[5]);

    asl_print_dbg("Display some nodes' information after deletion");
    tree_functional_test_display_node(t, &vals[0]);
    tree_functional_test_display_node(t, &vals[3]);
    tree_functional_test_display_node(t, &vals[11]);
    tree_functional_test_display_node(t, &vals[13]);
    tree_functional_test_display_node(t, &vals[10]);
    tree_functional_test_display_node(t, &vals[6]);
    tree_functional_test_display_node(t, &vals[2]);

    asl_print_dbg("Add nodes to Tree");
    tree_functional_test_add_node(t, &vals[0],&vals[8]);
    tree_functional_test_add_node(t, &vals[0],&vals[5]);
    tree_functional_test_add_node(t, &vals[3],&vals[9]);
    tree_functional_test_add_node(t, &vals[3],&vals[10]);
    tree_functional_test_add_node(t, &vals[3],&vals[20]);
    tree_functional_test_add_node(t, &vals[4],&vals[14]);
    tree_functional_test_add_node(t, &vals[5],&vals[15]);
    tree_functional_test_add_node(t, &vals[5],&vals[16]);
    tree_functional_test_add_node(t, &vals[7],&vals[17]);
    tree_functional_test_add_node(t, &vals[7],&vals[18]);

    asl_print_dbg("Display some nodes' information after deletion");
    tree_functional_test_display_node(t, &vals[0]);
    tree_functional_test_display_node(t, &vals[3]);
    tree_functional_test_display_node(t, &vals[11]);
    tree_functional_test_display_node(t, &vals[13]);
    tree_functional_test_display_node(t, &vals[10]);
    tree_functional_test_display_node(t, &vals[6]);
    tree_functional_test_display_node(t, &vals[2]);

    tree_functional_test_display_node(t, &vals[4]);
    tree_functional_test_display_node(t, &vals[5]);
    tree_functional_test_display_node(t, &vals[7]);
    tree_functional_test_display_node(t, &vals[20]);

    ret = tree_destroy(t);
    asl_print_dbg("Destroy t ret:%d",ret);
    return ret;
}
static s_int32_t tree_performance_test_cmp(void *a, void *b)
{
    u_int64_t *aval = (u_int64_t*)a;
    u_int64_t *bval = (u_int64_t*)b;
    
    /* Node must have data */
    if(aval == NULL || bval == NULL)
    {
        return -1;
    }
    if(*aval == *bval)
    {
        return 0;
    }
    return -1;
}
static void tree_performance_test_del(void *val)
{
    u_int64_t *a = (u_int64_t*)val;
    asl_free(a);
}
static void tree_performance_test_delete_node(tree *t, u_int64_t *val)
{
    int ret;
    ret = asd_tree_del(t, val);
    if(ret < 0)
    {
        asl_print_err("Delete Tree Node with val:%lu failed", *val);
    }
    //asl_print_dbg("Delete Node %lu", *val);
}
static void tree_performance_test_add_node(tree *t, u_int64_t *parent, u_int64_t *val)
{
    int ret;
    ret = asd_tree_add(t, parent, val);
    if(ret < 0)
    {
        if(parent != NULL)
        {
            asl_print_err("Add Tree Node with val:%lu to parent:%lu failed", *val, *parent);
        }
        else
        {
            asl_print_err("Add Tree Root Node with val:%lu failed", *val);
        }
    }
    #if 0
    if(parent != NULL)
    {
        asl_print_dbg("Add Node %2d to Branch %lu", *val, *parent);
    }
    else
    {
        asl_print_dbg("Add Root Node %lu to Tree", *val);
    }
    #endif
}
static u_int64_t* tree_performance_gen_data(u_int64_t val)
{
    u_int64_t* data = (u_int64_t*)asl_malloc(sizeof(u_int64_t));
    if(data == NULL)
    {
        asl_print_err("Malloc for data failed");
    }
    else
    {
        *data = val;
    }
    return data;
}
static int tree_performance_test(u_int32_t maxBranch, u_int32_t maxLevel, u_int32_t testCount)
{
    u_int64_t branchIndex, levelIndex;
    u_int32_t testIndex = 1;
    int ret = FAILURE;
    u_int64_t val;
    u_int64_t levelBase;
    u_int64_t nodeCountOnLevel;
    u_int64_t levelCount;
    tree *t;
    u_int64_t startTime, endTime;
    startTime = asl_time_get_stamp();
    while(testIndex <= testCount)
    {
        //jprint_dbg("*******************    No.%02d test start    *******************",testIndex);
        t = tree_create();
        t->cmp = tree_performance_test_cmp;
        t->del = tree_performance_test_del;
        if(t == NULL)
        {
            asl_print_err("Create Tree failed");
            return FAILURE;
        }
        val = 1;
        /*
        ** maxLevel         test
        **    0         Only test Create and Destroy Tree with no nodes
        **    1         Test Tree only has root
        **    >1        Common Performance Test for Tree
        */
        if(maxLevel > 0)
        {
            tree_performance_test_add_node(t, NULL, tree_performance_gen_data(val));
            nodeCountOnLevel = 1;
            levelBase = val;
        }
        for(levelIndex = 2; levelIndex <= maxLevel; levelIndex++)
        {
            levelCount = nodeCountOnLevel;
            while(levelCount--)
            {
                for(branchIndex = 1; branchIndex <= maxBranch; branchIndex++)
                {
                    val++;
                    tree_performance_test_add_node(t, &levelBase, tree_performance_gen_data(val));
                }
                levelBase++;
            }
            levelBase = val - nodeCountOnLevel*maxBranch + 1;
            nodeCountOnLevel = nodeCountOnLevel*maxBranch;
        }
        ret = tree_destroy(t);
        if(ret < 0)
        {
            asl_print_err("Destroy Tree Failed");
        }
        t = NULL;
        //jprint_dbg("*******************No.%02d test over, ret:%d*******************",testIndex, ret);
        testIndex++;
    }
    endTime = asl_time_get_stamp();
    asl_print_dbg("Tree Performance Test Over, total test time: %lu s",endTime - startTime);
    return ret;
}
void test_tree()
{
    int ret;
    /* Functional Test for tree data structure */
    asl_print_dbg("-----------------Functional Test Start-----------------");
    ret = tree_functional_test();
    asl_print_dbg("-----------------Functional Test End  -----------------");

    asl_println("\n\n");
    asl_print_dbg("-----------------Performance Test Start-----------------");
    ret = tree_performance_test(5,5,100000);
    asl_print_dbg("-----------------Performance Test End-----------------");
    /*                          Test Result                               */
    /*              4,5,100000      5,5,100000        6,6,100     6,6,1000
    **  Recurse         32s             156s            44s          470s
    **  N_Recurse       32s             157s            44s          470s
    */
    /*              10,3,1000000    10,4,10000        10,5,100
    **  Recurse         39s             35s              66s         
    **  N_Recurse       s                s               s
    */
    return;
}

