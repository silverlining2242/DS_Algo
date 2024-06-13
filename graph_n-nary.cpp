#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits.h>
#define SMALLEST LLONG_MIN

using namespace std;

class Node
{
public:
  long long int id;
  long long int value;
  vector<Node *> children;

  Node(long long int id, long long int value) : id(id), value(value) {}
};

template <class T>
class Sum
{
public:
  T rootOfMaxPath = -1;
  map<T, Node *> nodes; 
  Node *dummy;          
  Sum()
  {
    dummy = new Node(-1, 0); 
  }

  T MaxPathSum()
  {
    if (dummy->children.empty())
      return 0;
    Node *root = dummy->children[0];
    T res = SMALLEST;
    MaxPathSumUtil(root, res);
    return res;
  }

  T MaxPathSumUtil(Node *node, T &res)
  {
    // base case
    if (!node)
      return 0;

    // Check if node is a leaf node, return the value of itself
    if (node->children.empty())
    {
      if (node->value > res)
      {
        res = node->value;
        rootOfMaxPath = node->id;
      }
      return node->value;
    }
    // recursive
    vector<T> childSums;
    T maxChildSum = SMALLEST;
    T maxTwoChildSum = SMALLEST; 

    // as long as node has children, recursive to the bottom
    for (Node *child : node->children)
    {
      T childSum = MaxPathSumUtil(child, res);
      childSums.push_back(childSum);
      maxChildSum = max(maxChildSum, childSum);
    }

    // get two max child + node itself
    if (childSums.size() > 1)
    {
      sort(childSums.begin(), childSums.end(), greater<T>());
      maxTwoChildSum = childSums[0] + childSums[1] + node->value; 
    }
    // itself v.s itself + the maxChild(subtree)
    T nodeMax = max(node->value + maxChildSum, node->value);
    // v.s itself + two of maxChild
    nodeMax = max(nodeMax, maxTwoChildSum);

    // Update the root ID when res is updated
    if (nodeMax > res) // res is previous calculating result,
    {
      res = nodeMax;
      rootOfMaxPath = node->id;
    }

    return node->value + (maxChildSum > 0 ? maxChildSum : 0); // ( ) is necessary !!
  }
  void Delete(T id)
  {
    auto it = nodes.find(id);
    if (it == nodes.end())
    {
      return; 
    }
    // above is passed, so nodeToDelete must have value
    Node *nodeToDelete = nodes[id]; 

    // Find parent of the node to delete
    Node *parent = NULL;
    for (auto &pair : nodes)
    {
      Node *potentialParent = pair.second; 
      auto &children = potentialParent->children;
      auto it = find(children.begin(), children.end(), nodeToDelete); 
      if (it != children.end())                                       
      {
        parent = potentialParent;
        break;
      }
    }
    if (!parent)
      return; 

    // Append nodeToDelete's children to its parent's children
    parent->children.insert(parent->children.end(), nodeToDelete->children.begin(), nodeToDelete->children.end()); 

    auto &children = parent->children;
    children.erase(remove(children.begin(), children.end(), nodeToDelete), children.end());
    nodes.erase(id);
  }

  // Function to add or retrieve a node
  Node *getOrAddNode(long long int id, long long int value)
  {
    if (nodes.find(id) == nodes.end()) // ensure id is unique
      nodes[id] = new Node(id, value);
    return nodes[id];
  }
  // Preorder traversal 
  void Preorder(Node *node)
  {
    if (node)
    {
      cout << node->value << " "; // Process the current node
      for (Node *child : node->children)
      { // Process all children
        Preorder(child);
      }
    }
  }
  // get nodes by id
  Node *getNodePointer(T id)
  {
    auto it = nodes.find(id);
    if (it != nodes.end())
      return it->second; // The key exists, return the associated Node pointer
    else
      return NULL; // The key does not exist, return NULL
  }
  void AddNodes(T parentId, T childId, T childValue)
  {
    Node *p = getNodePointer(parentId);
    Node *c = getNodePointer(childId);
    if (p && !c)
    {
      Node *childNode = getOrAddNode(childId, childValue);
      p->children.push_back(childNode);
    }
    else if (!p && !c)
    {
      Node *parentNode = getOrAddNode(parentId, 0); 
      Node *childNode = getOrAddNode(childId, childValue);
      parentNode->children.push_back(childNode);
    }
    else if (p && c)
    {
      c->value = childValue;
      if (find(p->children.begin(), p->children.end(), c) == p->children.end())
      {
        p->children.push_back(c); 
      }
    }
    else if (!p && c)
    {
      Node *parentNode = getOrAddNode(parentId, 0); // 0 is temp will be replace
      parentNode->children.push_back(c);
      c->value = childValue;
    }
    p = c = NULL; //**
  }
};

int main()
{
  long long int N, O; // Number of nodes
  cin >> N >> O;

  long long int rootId, rootValue;
  cin >> rootId >> rootValue;

  Sum<long long int> calculator;
  Node *root = calculator.getOrAddNode(rootId, rootValue);
  calculator.dummy->children.push_back(root);

  for (int i = 0; i < N; i++)
  {
    long long int parentId, childId, childValue;
    cin >> parentId >> childId >> childValue;
    calculator.AddNodes(parentId, childId, childValue);
  }

  while (O--)
  {
    string command;
    cin >> command;
    if (command == "Check")
    {
      long long int maxSum = calculator.MaxPathSum();
      cout << "Maximum Value: " << maxSum << '\n';
      cout << "Root of the Path: " << calculator.rootOfMaxPath << '\n';
    }
    else if (command == "Add")
    {
      long long int parentId, childId, childValue;
      cin >> parentId >> childId >> childValue;
      calculator.AddNodes(parentId, childId, childValue);
    }
    else if (command == "Delete")
    {
      long long int id;
      cin >> id;
      calculator.Delete(id);
      long long int maxSum = calculator.MaxPathSum();
    }
  }
  // final print
  long long int maxSum = calculator.MaxPathSum();
  cout << "Final Root: " << calculator.rootOfMaxPath << '\n';
  // check
  // cout << "Preorder: ";
  // calculator.Preorder(root);
  // cout << '\n';

  return 0;
}
