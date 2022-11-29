//@HEADER
// ************************************************************************
//
//                      OPENMP TASK BENCH 1.0
//       Copyright 2022 National Technology & Engineering 
//                Solutions of Sandia, LLC (NTESS). 
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY NTESS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NTESS OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Jan Ciesko (jciesko@sandia.gov)
//
// ************************************************************************
//@HEADER

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

int max_height, max_childs, final_depth;
float creation_prob;

struct node_t
{
    int size;
    struct node_t **children;
};
int max(int x, int y)
{
    return x > y ? x : y;
}

int height_serial(struct node_t* node)
{
    if (node == 0)
        return 0;

    int i = 0;
    int max_height_childs = 0;
    while (i < node->size)
    {
        max_height_childs = max(max_height_childs, height_serial(node->children[i]));
        i++;
    }

    return max_height_childs + 1;
}

int height_ompss(struct node_t* node, int level)
{
    if (node == 0)
        return 0;

    int i = 0;
    int max_height_childs = 0;
    while (i < node->size)
    {
        #pragma omp task reduction(max: max_height_childs) final(final_depth < level)
        {
             max_height_childs = max(max_height_childs, height_ompss(node->children[i], level+1));
        }
        i++;
    }
#pragma omp taskwait

    return max_height_childs + 1;
}



struct node_t* new_tree_rec(int count)
{
    double double_rand = drand48();

    // do not create if the height of the current tree is equal to the max_height
    if (count == max_height
            // probability of create a new node 
            || double_rand > creation_prob)
    {
        return 0;
    }

    int num_childs = (rand() % max_childs) + 1;
    struct node_t* new_node = malloc(sizeof(struct node_t));
    struct node_t** children = malloc(sizeof(struct node_t*) * num_childs);
    new_node->size = num_childs;
    new_node->children = children;
    int i = 0;
    while (i < num_childs)
    {
        children[i] = new_tree_rec(count + 1);
        i++;
    }

    return new_node;
}

struct node_t* new_tree()
{
    return new_tree_rec(0);
}

void free_tree(struct node_t* node)
{
    if (node != 0)
    {
        int i = 0;
        while (i < node->size)
        {
            free_tree(node->children[i]);
            i++;
        }

        free(node->children);
        free(node);
    }
}


int main(int argc, char *argv[])
{
    if(argc != 6)
    {
        printf("./max_height_tree seed creation_prob max_height max_childs_per_node final_depth\n");
        printf("where:\n"
                "-seed: value used to initialize the GNA,\n"
                "-creation_prob: probability of create a new node (between 0..1),\n"
                "-max_height: value used to determine the max tree height,\n"
                "-max_childs_per_node: value used to determine the max number of childs per node,\n"
                "-final_depth: how many levels should be solved in parallel\n");
        exit(1);
    }
    int seed = atoi(argv[1]);
    creation_prob = atof(argv[2]);
    max_height = atoi(argv[3]);
    max_childs = atoi(argv[4]);
    final_depth= atoi(argv[5]);

    srand(seed);
    srand48(seed);

    struct node_t * root = new_tree();

    int real_height = height_serial(root);
    int computed_height = height_ompss(root, /* current level */ 0);
    free_tree(root);

    printf("real_height: %d == computed_height: %d ?\n", real_height, computed_height);
    assert(real_height == computed_height);
}
