//hencode.cc
//Jason Wilkin
//Fall 2011
#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <unistd.h>

using namespace std;

struct Hnode
//Huffman data structure that contains character and count
//contains left and right pointers for constructing a Huffman tree
{
  unsigned char ch;
  size_t count;
  Hnode * left;
  Hnode * right;
  Hnode();
  Hnode(Hnode* l, Hnode* r, size_t s){ //internal node
    left = l;
    right = r;
    count = s;
  }
  Hnode(unsigned char c,size_t s){ //leaf
    ch = c;
    count = s;
    left = NULL;
    right = NULL;
  }
};


struct Listnode
//data structure that contains an Hnode and a pointer to the next element
{
  Hnode *node;
  Listnode *next;
  Listnode(Hnode *n);
};

class List{ //Ordered list of Listnodes
public:
  size_t size;
  Listnode* front;
  List();
  ~List();

  void insert_ordered(Listnode*& front, Hnode*& nodey);
  void remove();

};


Listnode::Listnode(Hnode *n){ //Initializes Listnode with 
  node = n;
  next = NULL;
}

List::List(){ //List constructor
  size = 0;
  front = NULL;
}

List::~List(){ //List destructor
  while(front!=NULL){
    Listnode *p = front->next;
    delete front;
    front = p;
  }
}
 
void List::insert_ordered(Listnode*& front, Hnode*& nodey){
  //inserts Listnodes into a list ordered by count from least to greatest
  if (!front) {
    front = new Listnode(nodey);
    size++;
  }
  
  else if (nodey->count < front->node->count){
    Listnode *temp = front;
    front = new Listnode(nodey);
    front->next = temp;
    size++;
  }
  
  else 
    insert_ordered(front->next, nodey);
  
}


void List::remove(){
  // removes the first (smallest) element from the list
  Listnode *temp = front->next;
  delete front;
  front = temp;
  size--;
}

size_t counts[256]; //array of the number of each character found in the file
string codes[256]; //array of binary codes that represent original characters

void init()
//initializes each slot in counts to 0 and each slot in codes to
//an empty string
{
  for (size_t i=0;i<256;i++){
    counts[i] = 0;
    codes[i] = "";
  }
}



unsigned char bitStr2Char(string s)
//returns a new unsigned character based on a binary string
{
  char total = 0;
  char val = 1;
  for (int i=7;i>=0;i--){
    total += (s[i] - '0') * val;
    val = val *2;
  }
  return total;
}

void read_file(std::string filename)
//reads the file and sets slots in counts accordingly
{
  ifstream ff(filename.c_str());
  while (true){
    unsigned char temp;
    temp = ff.get();
    if (!ff.good())
      break;
    counts[temp]++;
  }
}

void code_traverse(Hnode *tree, string code){
  // traverses huffman tree and sets the appropriate slot in codes to
  //a string containing a binary code
  if (!tree->left and !tree->right)
    codes[tree->ch] = code;
 
  if (tree->left)
    code_traverse(tree->left, code+"0");
  
  if (tree->right)
    code_traverse(tree->right, code+"1");
}

void huff_tree(List & sorted){
  //Creates a huffman tree from an ordered List of Listnodes containing Hnodes

  //if file has 1 or less characters, warns the user
  if (sorted.size <= 1){
    cout << "Cannot encode file containing " << sorted.size <<
      "characters" << endl;
    return;
  }
  
  else {
    while (sorted.size > 1){
      Hnode *temp1 = sorted.front->node;
      Hnode *temp2 = sorted.front->next->node;
      
      sorted.remove();
      sorted.remove();
      Hnode *huff = new Hnode(temp1, temp2, temp1->count + temp2->count);
      sorted.insert_ordered(sorted.front, huff);
    }
    //Creates a string of a binary code for each character
    string blank = "";
    code_traverse(sorted.front->node, blank);
  }
}

size_t numBits = 0;

string encode(std::string filename)
//Creates one long bit string of all the binary codes in the order they
//appear in the file
//Sets numBits to the size of this new bit string
{
  string bits;
  ifstream gg(filename.c_str());
  while (true){
    unsigned char temp;
    temp = gg.get();
      if (!gg.good())
	break;
    bits += codes[temp];
    numBits += codes[temp].size();
  }					   
  return bits;
}

void printTree(Hnode *tree, ofstream & ostr)
//outputs the huffman tree to ofstream ostr
{
  assert(tree);
  if (!tree->right and !tree->left)
    ostr << 'L' << tree->ch;
  
  else if (tree->right and tree->left){
    ostr << 'I';
    printTree(tree->left, ostr);
    printTree(tree->right, ostr);
  }
} 
   
  

int main(int argc, char ** argv)
{
  init();
  read_file(argv[1]);
  List sortedChars;
  for (size_t i=0;i<256;i++){
    if (counts[i]>0){
      Hnode *nodey = new Hnode(i, counts[i]); //Creates Hnode for each char
      sortedChars.insert_ordered(sortedChars.front, nodey);
    }
  }
  huff_tree(sortedChars);
  string bits = encode(argv[1]);
  string newFile = argv[1];
  newFile = newFile + ".huf";
  ofstream out(newFile.c_str());
  out << numBits;
  printTree(sortedChars.front->node, out);
  while (bits.size() % 8 != 0)   //adds extra 0s to bits for complete bytes
    bits += '0';
  for (size_t i=0;i<bits.size();i+=8){
    out << bitStr2Char(bits.substr(i, 8));
  }
  unlink(argv[1]);
}
