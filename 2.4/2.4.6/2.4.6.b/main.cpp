#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

/* This program is partially broken.
 * The hard part: How do you represent a null string:
 * that is, <epsilon>, using a recursive descent
 * parser with an input, one-way stream with no memory?
 *
 * Here, the most logical method I can think of would
 * be to use some sort of "stack depth" so that
 * I remember how many ')' I need to record but
 * that kind of violates predictive parser rules, right?
 */

struct Node {
  std::vector<Node*> children;
  std::string data;
};


void print_tree(Node* head, int depth) {
  // If nullptr, stop.
  if (head == nullptr) { return; }
  
  // If not nullptr, print self, then print children
    // Print <depth> spaces before.
    for (auto i = 0; i < depth; ++i) { std::cout << " "; }
    // Print self.
    std::cout << head->data << std::endl;
    // Print left -> right
    for (auto it : head->children) {
      print_tree(it, depth+1);
    }
}

void println(const char* str) {
  std::cout << str << std::endl;
}

void println(std::string str) {
  println(str.c_str());
}

void match(char c, std::string& str, Node** headp) {
  // If match, add a Node to the tree.
  if (str.c_str()[0] == c) {
    std::cout << "matched: " << c << std::endl;

    (*headp)->children.push_back(nullptr);
    headp = &(*headp)->children[(*headp)->children.size()-1];

    *headp = new Node{ {}, std::string(1, c) };
    str.erase(str.begin());
  } else {
    throw("match(): syntax error");
  }
}

void ignore(char c, std::string& str) {
  // If match, keep going.
  while (str.c_str()[0] == c) {
    str.erase(str.begin());
  }
}

void epsilon(Node** headp) {
  // Expecting an empty string?
  // Add a node with <epsilon>.
  (*headp)->children.push_back(nullptr);
  headp = &(*headp)->children[(*headp)->children.size()-1];

  *headp = new Node{ {}, std::string("<epsilon>") };
}

void S(std::string& str, Node** headp) {
  // If at end of input, stop.
  if (str.empty()) { 
    if ((*headp)->children[(*headp)->children.size()-1]->data == ")") {
      epsilon(headp);
    }
    return;
  }

  std::cout << "S: " << str << std::endl;

  // Create new S node.
  (*headp)->children.push_back(new Node { {}, "rule:S" });
  headp = &(*headp)->children[(*headp)->children.size()-1];
  // Else, create a new root and match the first character.

  // Predictive parser with 1 lookahead character.
  switch (str.c_str()[0]) {

    case '(':
      epsilon(headp);
      match('(', str, headp);
      ignore(' ', str);
      S(str, headp);
      ignore(' ', str);
      match(')', str, headp);
      ignore(' ', str);
      S(str, headp);
      break;
    case ' ':
      ignore(' ', str);
      break;
    default:
      if (str.c_str()[0] == '(' || str.c_str()[0] == ')') {
        epsilon(headp);
      } else {
        throw("S(): syntax error");
      }
  }
}

Node* parse(std::string input) {
  Node* head = new Node{{}, std::string("root")};
  try {
    S(input, &head);
  } catch (const char* e) {
    std::cout << "===== EXCEPTION: " << e << " =====" << std::endl;
  }
  return head;
}

int main() {
  println("Please input a string which parses according to:");
  println("S --> S ( S ) S | <epsilon>");
  std::string input;
  std::getline(std::cin, input);
  std::cout << "INPUT: " << input << std::endl;

  auto tree = parse(input);
  std::cout << "<<<<< PRINTING PARSE TREE: >>>>>" << std::endl;
  print_tree(tree, 0);

  return 0;
}