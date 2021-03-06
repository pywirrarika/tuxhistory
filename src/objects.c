#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<mxml.h>

#include "tuxhistory.h"
#include "fileops.h"
#include "objects.h"

static int init_obj_hash(void);

static int init_obj_hash(void)
{

    obj_table_hash = make_hashtable(hashtable_default_hash, NUM_OBJECTS+NUM_OF_TYPES+1);

    if(obj_table_hash == NULL)
        return 1;

    hashtable_add(obj_table_hash, "FOREST_MIXED", FOREST_MIXED);
    hashtable_add(obj_table_hash, "FOREST_TROPICAL", FOREST_TROPICAL);
    hashtable_add(obj_table_hash, "FOREST_CONIFER", FOREST_CONIFER);
    hashtable_add(obj_table_hash, "FOREST_SCRUB", FOREST_SCRUB);
    hashtable_add(obj_table_hash, "FOREST_BOREAL", FOREST_BOREAL);
    hashtable_add(obj_table_hash, "FOREST_WETLAND", FOREST_WETLAND);
    hashtable_add(obj_table_hash, "FOREST_RAIN", FOREST_RAIN);
    hashtable_add(obj_table_hash, "VILLAGER_MILKMAID", VILLAGER_MILKMAID);
    hashtable_add(obj_table_hash, "HOUSE", HOUSE);
    hashtable_add(obj_table_hash, "HUT", HUT);
    hashtable_add(obj_table_hash, "VILLAGE_CENTER", VILLAGE_CENTER);
    hashtable_add(obj_table_hash, "REC_GOLD_1", REC_GOLD_1);
    hashtable_add(obj_table_hash, "REC_STONE_1", REC_STONE_1);
    hashtable_add(obj_table_hash, "FARM", FARM);

    hashtable_add(obj_table_hash, "FOREST", FOREST);
    hashtable_add(obj_table_hash, "GOLD", GOLD);
    hashtable_add(obj_table_hash, "STONE", STONE);
    hashtable_add(obj_table_hash, "BUILDING", BUILDING);
    hashtable_add(obj_table_hash, "UNIT", UNIT);

    return 0;
}

int objects_xml(FILE *fp)
{
    int i;
    int value;
    mxml_node_t *tree;
    mxml_node_t *node;
    mxml_node_t *inode;

    objects_hash = make_hashtable(hashtable_default_hash, NUM_OBJECT_HASH);
    if(objects_hash == NULL)
        return 1;

    tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
    if(init_obj_hash())
        return 1;

    i = 0;
    for(inode = mxmlFindElement(tree, tree, "object", 
                NULL, NULL, MXML_DESCEND);
            inode != NULL;
            inode = mxmlFindElement(inode, tree, "object",
                NULL, NULL, MXML_DESCEND))
    {
        if(i > MAX_DEF_OBJECTS)
        {
            printf("To many objects, the limit is %d", MAX_DEF_OBJECTS);
            return 1;
        }
        
        object[i].vision_range = 0;
        node = mxmlFindElement(inode, inode, "type",
                               NULL, NULL, MXML_DESCEND);
        if(node != NULL)
        {
            if(value != -1)
            {
                value = (int)hashtable_lookup(obj_table_hash, node->child->value.opaque);
                object[i].type = value;
            }
        }
        else
        {
            object[i].type = -1;
            printf("objects_xml: Error loading objects description file\n");
            return 1;
        }

        node = mxmlFindElement(inode, inode, "name",
                               NULL, NULL, MXML_DESCEND);
        if(node != NULL)
        {
            if((int)hashtable_lookup(objects_hash, node->child->value.opaque) != NULL)
            {
                printf("This element was already added to the hash table!\n");
                return 1;
            }
            value = (int)hashtable_lookup(obj_table_hash, node->child->value.opaque);
            if(value != -1)
            {
                object[i].name_enum = value;
                strcpy(object[i].name, node->child->value.opaque);    
            }
        }
        else
        {
            object[i].name_enum = -1;
            printf("objects_xml: Error loading objects description file.\n");
            return 1;
        }

        node = mxmlFindElement(inode, inode, "rname",
                               NULL, NULL, MXML_DESCEND);
        if(node != NULL)
        {
            strcpy(object[i].rname, node->child->value.opaque);
        }
        else
        {
            strcpy(object[i].rname, "");
            printf("objects_xml: Error loading objects description file.\n");
            return 1;
        }
        node = mxmlFindElement(inode, inode, "description",
                               NULL, NULL, MXML_DESCEND);
        if(node != NULL)
        {
            strcpy(object[i].description, node->child->value.opaque);
        }
        else
        {
            strcpy(object[i].rname, "");
            printf("objects_xml: Error loading objects description file.\n");
            return 1;
        }

        node = mxmlFindElement(inode, inode, "live",
                    NULL, NULL, MXML_DESCEND);
            
        if(atoi(node->child->value.opaque) >= 0)
        {
            object[i].live = atoi(node->child->value.opaque);
            object[i].actual_live = object[i].live;
        }
        else
        {
            object[i].live = -1;
            printf("objects_xml: Error loading objects description file.\n");
            return 1;
        }

        if(object[i].type != FOREST || 
            object[i].type != GOLD   ||
            object[i].type != STONE)
        {
            node = mxmlFindElement(inode, inode, "vision",
                    NULL, NULL, MXML_DESCEND);
            
            if(node != NULL)
            {
                if(atoi(node->child->value.opaque) >= 0)
                {
                    object[i].vision_range = atoi(node->child->value.opaque);
                }
                else
                {
                    object[i].vision_range = -1;
                    printf("objects_xml: Error loading objects description file.\n");
                    return 1;
                }
            }

            node = mxmlFindElement(inode, inode, "defence",
                    NULL, NULL, MXML_DESCEND);
            
            if(node != NULL)
            {
                if(atoi(node->child->value.opaque) >= 0)
                {
                    object[i].defence = atoi(node->child->value.opaque);
                }
                else
                {
                    object[i].defence = -1;
                    printf("objects_xml: Error loading objects description file.\n");
                    return 1;
                }
            }
            
            node = mxmlFindElement(inode, inode, "attack",
                    NULL, NULL, MXML_DESCEND);
            
            if(node != NULL)
            {
                if(atoi(node->child->value.opaque) >= 0)
                {
                    object[i].attack = atoi(node->child->value.opaque);
                }
                else
                {
                    object[i].attack = -1;
                    printf("objects_xml: Error loading objects description file.\n");
                    return 1;
                }
            }
        
            node = mxmlFindElement(inode, inode, "move",
                    NULL, NULL, MXML_DESCEND);
            
            if(node != NULL)
            {
                if(atoi(node->child->value.opaque) >= 0)
                {
                    object[i].move = atoi(node->child->value.opaque);
                }
                else
                {
                    object[i].move = -1;
                    printf("objects_xml: Error loading objects description file.\n");
                    return 1;
                }
            }
            node = mxmlFindElement(inode, inode, "food",
                    NULL, NULL, MXML_DESCEND);
            
            if(node != NULL)
            {
                if(atoi(node->child->value.opaque) >= 0)
                {
                    object[i].cost[REC_FOOD] = atoi(node->child->value.opaque);
                }
                else
                {
                    object[i].cost[REC_WOOD] = -1;
                    printf("objects_xml: Error loading objects description file.\n");
                    return 1;
                }
            }
            else
            {
                object[i].cost[REC_FOOD] = 0;
            }

            node = mxmlFindElement(inode, inode, "wood",
                    NULL, NULL, MXML_DESCEND);
            
            if(node != NULL)
            {
                if(atoi(node->child->value.opaque) >= 0)
                {
                    object[i].cost[REC_WOOD] = atoi(node->child->value.opaque);
                }
                else
                {
                    object[i].cost[REC_WOOD] = -1;
                    printf("objects_xml: Error loading objects description file.\n");
                    return 1;
                }
            }
            else
            {
                object[i].cost[REC_WOOD] = 0;
            }

            node = mxmlFindElement(inode, inode, "gold",
                    NULL, NULL, MXML_DESCEND);
            
            if(node != NULL)
            {
                if(atoi(node->child->value.opaque) >= 0)
                {
                    object[i].cost[REC_GOLD] = atoi(node->child->value.opaque);
                }
                else
                {
                    object[i].cost[REC_WOOD] = -1;
                    printf("objects_xml: Error loading objects description file.\n");
                    return 1;
                }
            }
            else
            {
                object[i].cost[REC_GOLD] = 0;
            }

            node = mxmlFindElement(inode, inode, "stone",
                    NULL, NULL, MXML_DESCEND);
            
            if(node != NULL)
            {
                if(atoi(node->child->value.opaque) >= 0)
                {
                    object[i].cost[REC_STONE] = atoi(node->child->value.opaque);
                }
                else
                {
                    object[i].cost[REC_WOOD] = -1;
                    printf("objects_xml: Error loading objects description file.\n");
                    return 1;
                }
            }
            else
            {
                object[i].cost[REC_STONE] = 0;
            }
         }
        else
        {
            object[i].defence = -1;
            object[i].attack = -1;
            object[i].move = -1;
        }

        //Initializing states

        object[i].state.state = INACTIVE;
        object[i].state.old_state = INACTIVE;
        object[i].state.count = 0;
        object[i].state.flag = 0;
        object[i].state.action_againts = INACTIVE;
        object[i].state.agains_flag = 0;
        object[i].state.path = NULL;
        object[i].state.path_count = 0; 
        object[i].state.path_flag = 0;
        object[i].state.carrying = 0;
        object[i].state.resource_type = REC_NONE;
        object[i].state.target_obj = NULL;
        object[i].state.target_point.x = 0;
        object[i].state.target_point.y = 0;
        object[i].state.rec_point_flag = 0;
        object[i].state.rec_point.x = 0;
        object[i].state.rec_point.y = 0;
        object[i].state.generate_flag = 0;
        object[i].state.generate_type = 0;
        object[i].state.generate_compl = 0;
        object[i].state.generate_count = 0;

        /* Debug: print the values of current object */
        printf("%d %s:%d(%s) %s lives: %d, def: %d, att: %d, mov: %d\n", 
                object[i].type,
                object[i].name,
                object[i].name_enum,
                object[i].rname,
                object[i].description,
                object[i].live,
                object[i].defence,
                object[i].attack,
                object[i].move);

        /* End of debug */
        hashtable_add(objects_hash, object[i].name, &object[i]);
        strcpy(object_names[object[i].name_enum],object[i].name);

        i++;
    }

    /* Use the objects_hash table this way *
    th_obj *db_obj;
    db_obj = hashtable_lookup(objects_hash, "FOREST_CONIFER");
    printf("%d %s:%d(%s) %s lives: %d\n", 
                db_obj->type,
                db_obj->name,
                db_obj->name_enum,
                db_obj->rname,
                db_obj->description,
                db_obj->live);
                */

    free_hashtable(obj_table_hash);

    mxmlDelete(inode);
    mxmlDelete(node);

    fclose(fp);

    object_counter = 0;

    return 0;
}

