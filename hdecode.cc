//hdecode.cc
//Jason Wilkin
//Fall 2011
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <cassert>

using namespace std;

struct Hnode
//Huffman data structure that contains character and count
//contains left and right pointers for constructing a Huffman tree
{
  unsigned char ch;
  size_t count;
  Hnode * left;
  Hnode * right;
  Hnode(){
    left = NULL;
    right = NULL;
  }
  Hnode(Hnode* l, Hnode* r){ //internal node
    left = l;
    right = r;
  }
  Hnode(unsigned char c){ //leaf
    ch = c;
    left = NULL;
    right = NULL;
  }
};

string fileText = "";

void file2Str(std::string filename)
//adds each character in the file to fileText
{
  ifstream qq(filename.c_str());
  while(true){
    unsigned char temp;
    temp = qq.get();
    if (!qq.good()){
      break;
    }
    fileText+=temp;
  }
}


Hnode *tree_helper(stringstream & ss)
//Recursively creates a huffman tree from the overhead
{
  char temp;
  ss.get(temp);
  if (temp == 'I'){
    return new Hnode(tree_helper(ss), tree_helper(ss));
  }
  else if (temp == 'L'){
    char newChar;
    ss.get(newChar);
    return new Hnode(newChar);
  }
  return NULL;
}

int numBits;
int pos;

Hnode *tree_gen(string overhead)
//Returns a huffman tree from the overhead after setting numBits to the number
//of bits and pos to where the encoded text begins
{
  stringstream ss (stringstream::in | stringstream::out);
  ss << overhead;
  ss >> numBits;
  Hnode * tree = tree_helper(ss);
  pos = ss.tellg();
  return tree;
}


string codes[256];

void code_traverse(Hnode *tree, string code){
  //traverses huffman tree and sets the appropriate slot in codes to
  //a string containing a binary code
  if (!tree->left and !tree->right)
    codes[tree->ch] = code;
  
  if (tree->left)
    code_traverse(tree->left, code+"0");

  if (tree->right)
    code_traverse(tree->right, code+"1");
  
}

string char2BitStr(unsigned char c, string byte, size_t bitval)
//returns the a string of a binary code representing unsigned char c
{
  if (byte.size() == 8)
    return byte;
  else if (c >= bitval){
    byte+="1";
    return char2BitStr(c-bitval,byte,bitval/2);
  }
  else if (c < bitval){
    byte+="0";
    return char2BitStr(c,byte,bitval/2);
  }
  return "";
}
    
string makeBitStr(string filename)
//returns a string of all the binary codes in the order they appear
//in the file
{
  string bits = "";
  ifstream ff(filename.c_str());
  ff.seekg(pos);
  while (true){
    unsigned char temp;
    temp = ff.get();
    if (!ff.good())
      break;
    string byte = "";
    bits += char2BitStr(temp,byte,128);
  }
  return bits.substr(0, numBits);
}
    


int main(int argc, char ** argv)
{
  string file = argv[1];
  file2Str(file);
  Hnode *tree = tree_gen(fileText);
  string blank = "";
  code_traverse(tree, blank);
  string bitStr = makeBitStr(file);
  string name = file.substr(0, file.size()-4);

  //sets up a stringstream at the position of the first encoded character
  stringstream ss (stringstream::in | stringstream::out);
  ss << fileText;
  ss.seekg(pos-1); 
  ofstream out(name.c_str());
  Hnode *currentNode = tree;
  size_t i = 0;

  //goes through bitStr and traverses the tree to find each decoded character
  while (i<bitStr.size()){
    if (!currentNode->right and !currentNode->left){
      out << currentNode->ch;
      currentNode = tree;
    }
    else if (bitStr[i] == '0'){
      currentNode = currentNode->left;
      i++;
    }
    else if (bitStr[i] == '1'){
      currentNode = currentNode->right;
      i++;
    }
  }
  out << currentNode->ch; //writes the final decoded character to the file
  unlink(argv[1]);
}
