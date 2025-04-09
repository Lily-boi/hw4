#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkDepth(Node * node, int depth, int& leaf_depth) {
  if (node == nullptr) {
    return true;
  }
  if (node->left == nullptr && node->right == nullptr) {
    if (leaf_depth == -1) {
      leaf_depth = depth;
    }
    return depth == leaf_depth;
  }
  

  return (checkDepth(node->left, depth + 1, leaf_depth) &&
         checkDepth(node->right, depth + 1, leaf_depth));

}

bool equalPaths(Node * root)
{
  if (root == nullptr) {
    return false;
  }
  
  int leaf_depth = -1;
  return checkDepth(root, 0, leaf_depth);

}

