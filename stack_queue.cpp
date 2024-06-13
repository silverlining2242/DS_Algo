#include <iostream>
#include <string>
#include <sstream>
/*not allow to use STL stack and queue*/
using std::cin;
using std::cout;
using std::string;

class Queue
{
private:
  char *queueArr;
  int front;
  int back;
  int maxSize;

public:
  Queue(int max) : queueArr(NULL), front(-1), back(-1), maxSize(max)
  {
    queueArr = new char[maxSize];
  }
  ~Queue()
  {
    delete[] queueArr;
  }
  bool isEmpty()
  {
    return front == -1;
  }
  bool isFull()
  {
    return back == maxSize - 1;
  }
  char getFront()
  {
    if (isEmpty())
    {
      throw std::runtime_error("Queue is empty");
    }
    else
      return queueArr[front];
  }
  void enQueue(char newItem)
  {
    if (isFull())
      cout << "QUEUE FULL\n";
    else
    {
      if (front == -1)
        front = 0;
      back++;
      queueArr[back] = newItem;
    }
  }
  void deQueue()
  {
    if (isEmpty())
      cout << "QUEUE  EMPTY\n";
    else
    {
      front++;
      if (front > back)
        front = back = -1;
    }
  }
  void printQueue() // for check
  {
    if (isEmpty())
    {
      cout << "Queue is empty\n";
    }
    else
    {
      cout << "Queue : ";
      for (int i = front; i <= back; i++)
      {
        cout << queueArr[i] << " ";
      }
      cout << "\n";
    }
  }
};

class Stack // only operate single col
{
private:
  char *stackArr; // = NULL: we can't assign value in class member definition unless it's static
  int top;        // the stack index
  int maxSize;
  int index;

public:
  Stack() : stackArr(NULL), top(-1), maxSize(0), index(0) {} // constructor with member initializer list

  void init(int idx, int max)
  {
    index = idx;
    maxSize = max;
    stackArr = new char[maxSize];
  }
  ~Stack() // destructor
  {
    delete[] stackArr;
  }
  bool isFull()
  {
    return top == maxSize - 1;
  }

  bool isEmpty()
  {
    return top == -1;
  }

  int get_top() // get the top index
  {
    return top;
  }
  char getTopStack() // get the char of top stack // it has some bug
  {
    // if (isEmpty())
    //   cout << "EMPTY STACK\n";
    // return '\0'; // by pass that must return value, but it's not blank so be aware
    return stackArr[top];
  }
  char getIndexStack(int index) // change
  {
    // if (index < top)
    //   return '_';
    // else
    return stackArr[index];
  }
  // void change_top(int idx)
  // {
  //   top = idx;
  // }

  void push(char newItem)
  {
    if (isFull())
    {
      cout << "STACK FULL\n";
    }
    else
    {
      top++;
      stackArr[top] = newItem;
    }
  }
  void pushC(char newItem, int num)
  {
    if (isFull())
    {
      cout << "STACK FULL\n";
    }
    else
    {
      for (int i = 0; i < num; i++)
      {
        top++;
        stackArr[top] = newItem;
      }
    }
  }

  void pop()
  {
    if (isEmpty())
    {
      cout << "STACK EMPTY\n";
    }
    else
    {
      stackArr[top] = '_'; // problem here
      top--;
    }
  }
  // fix here diff= 0 need to consider hight
  void popB(int newtop) // deal with single stack // newtop is the bomb level
  {
    int diff = top - newtop;
    if (isEmpty())
    {
      // cout << "stack empty\n";
      return;
    }
    // update
    if (diff == 0) // the col is the same level with height >> use pop
    {
      pop();
      pop();
    }
    else if (diff > 1) // top is way above the bomb , no update top
    {
      if (top >= 3)
      {
        stackArr[newtop] = '_';
        stackArr[newtop - 1] = '_';
        stackArr[newtop + 1] = '_';
      }
      else if (top >= 2)
      {
        stackArr[newtop] = '_';
        stackArr[newtop + 1] = '_';
      }
      else if (top >= 1)
      {
        stackArr[newtop] = '_';
      }
    }
    else if (diff == 1) // hight is one higher than bomb >>use pop
    {
      pop();
      pop();
      pop();
    }
    else if (diff == -1) // bomb col itself or col is 1 less level than bomb
    {
      pop();
    }
    else if (diff < -1) // no impact at all
    {
      return;
    }
  }
  void printStack_index(int index)
  {
    if (isEmpty())
    {
      cout << "_ ";
    }
    else
    {
      cout << stackArr[index] << ' ';
    }
  }
};

int Get_highestIndex(Stack *stackArr, int coln)
{
  // clear all '_' we create because of C
  for (int i = 0; i < coln; i++)
  {
    while (stackArr[i].getTopStack() == '_')
      stackArr[i].pop();
  }
  int highest = -1; // change
  for (int i = 0; i < coln; i++)
  {
    if (stackArr[i].get_top() > highest)
      highest = stackArr[i].get_top();
  }
  return highest;
}
void printFINAL(Stack *stackArr, int coln)
{
  cout << "FINAL MAP:\n";
  int highest = Get_highestIndex(stackArr, coln);
  if (highest == -1)
  {
    // cout << '\n';
    return;
  }
  for (int j = highest; j >= 0; j--) // print from top of the stack, all lvl 5 > then lvl 4
  {
    for (int i = 0; i < coln; i++)
    {
      if (stackArr[i].get_top() < j || stackArr[i].isEmpty())
        cout << "_ ";
      else
        stackArr[i].printStack_index(j);
    }
    cout << '\n';
  }
}
void printALLstack(Stack *stackArr, int col, int row); // this is for check
void printstackBag(Stack *stackBag)
{
  cout << "FINAL BAG:\n";
  if (stackBag->isEmpty())
  {
    cout << '\n';
    return;
  }
  int n = stackBag->get_top();
  for (int i = 0; i <= n; i++)
  {
    cout << stackBag->getIndexStack(i) << ' ';
  }
  cout << '\n';
}
// stackArr is passed from a single stack!
void Dig(int col, Stack *stackArr, Stack *stackbag, Queue *queuebag, int coln)
{
  // cout << item << col << '\n'; //print check
  if (stackArr[col].isEmpty())
    return;
  // because of 'C', if there is _ on top and top is point to it >> pop out
  while (stackArr[col].getTopStack() == '_')
  {
    stackArr[col].pop();
  }
  // **if empty, return
  if (stackArr[col].isEmpty())
    return;

  char item = stackArr[col].getTopStack();

  stackArr[col].pop(); // no matter what it dig, we should pop
  int Cnum = 3;        // num of rows to add if dig C
  if (item == 'D' || item == 'G')
  {
    stackbag->push(item);
  }
  else if (item == 'F' || item == 'M')
  {
    queuebag->enQueue(item);
    // queuebag->printQueue(); // for check
  }
  else if (item == 'B') // bomb just destroyed no activate the item
  {
    int newtop = stackArr[col].get_top() + 1; // one below the bomb, but i want the place of bomb
    for (int i = col - 1; i <= col + 1; i++)
    {
      if (i < 0 || i >= coln)
        continue;
      else
      {
        stackArr[i].popB(newtop);
      }
    }
  }
  else if (item == 'C')
  {
    if (stackbag->isEmpty())
      return;
    char Additem = stackbag->getTopStack();

    int newtop = 0; // highest among 5 cols
    for (int i = col - 2; i <= col + 2; i++)
    {
      if (i < 0 || i >= coln)
        continue;
      else
      {
        if (stackArr[i].get_top() > newtop)
          newtop = stackArr[i].get_top();
      }
    }

    for (int i = col - 2; i <= col + 2; i++)
    {
      if (i < 0 || i >= coln)
        continue;
      else
      {
        // only if stack is below the col with C, fill the _ and change it
        while (stackArr[i].get_top() - newtop < 0)
        {
          stackArr[i].pushC('_', 1);
        }
        stackArr[i].pushC(Additem, Cnum);
      }
    }
  }
  else if (item == 'P')
  {
    if (stackbag->isEmpty())
      return;
    while (stackbag->getTopStack() != 'D' && stackbag->get_top() >= 0) // **need to stop when top= -1
      stackbag->pop();
  }
}
void Use(Stack *stackArr, Stack *stackbag, Queue *queuebag, int coln) // use is for all cols
{
  if (queuebag->isEmpty())
    return;
  char item = queuebag->getFront();
  queuebag->deQueue();

  if (item == 'M')
  {
    for (int i = 0; i < coln; i++)
    {
      Dig(i, stackArr, stackbag, queuebag, coln); // need to trigger effect, use Dig func
    }
  }
  else if (item == 'F')
  {
    int highest = Get_highestIndex(stackArr, coln);
    if (highest == -1)
    {
      cout << "MINE LEVEL:1\n";
    }
    else
    {
      cout << "MINE LEVEL:" << highest + 1 << '\n';
    }
    for (int i = 0; i < coln; i++)
    {
      if (stackArr[i].get_top() < highest || stackArr[i].get_top() == -1)
        cout << "_ ";
      else
        cout << stackArr[i].getTopStack() << ' ';
    }
    cout << '\n';
  }
}
int main()
{
  // input
  int col, row, N; // col , rows, # of input
  cin >> col >> row >> N;
  // max number of the stack
  int nG, nD, nC, nINV = 0;
  // input - map
  char **map;
  map = new char *[row];
  for (int j = 0; j < col; j++)
    map[j] = new char[col];

  for (int i = 0; i < row; i++) // read map
  {
    for (int j = 0; j < col; j++)
    {
      cin >> map[i][j]; // directly read to transpose map
      if (map[i][j] == 'G')
        nG++;
      else if (map[i][j] == 'D')
        nD++;
      else if (map[i][j] == 'C')
        nC++;
      else if (map[i][j] == 'F' || map[i][j] == 'M')
        nINV++;
    }
  }
  // create transpose and horizontal opposite mapT
  char **mapT;
  mapT = new char *[col];
  for (int j = 0; j < col; j++)
    mapT[j] = new char[row];

  for (int i = 0; i < col; i++)
  {
    for (int j = 0; j < row; j++)
    {
      mapT[i][j] = map[row - j - 1][i];
    }
  }

  // creat queue - inventory
  Queue queueINV(nINV);

  // create stack - bag
  int maxBag = nD + nG + nC * 15;
  Stack stackBag;
  stackBag.init(0, maxBag); // only one

  // create stack - cols
  Stack *stackArr = new Stack[col]; // can't do this in C++: stackArr[0](0,7) > use .init
  int maxstack = row + nC * 3;
  for (int j = 0; j < col; j++)
  {
    stackArr[j].init(j, maxstack);
    for (int i = 0; i < row; i++)
    {
      stackArr[j].push(mapT[j][i]);
    }
  }
  // print check each stack after created:
  // cout << "stack initiated:\n";
  // printALLstack(stackArr, col, row);

  // executing n time
  cin.ignore(); // use to flush the input buffer
  while (N--)
  {
    string cmd;
    std::getline(cin, cmd);
    if (cmd.compare(0, 3, "DIG") == 0) // substr will modify the original to DIG
    {
      int x; // std::stoi(cmd.substr(4)); //stoi need higher version
      std::istringstream(cmd.substr(4)) >> x;
      // s to int // substr(it will truncate the original string) from 4 till the end
      Dig(x, stackArr, &stackBag, &queueINV, col);
    }
    else if (cmd.compare(0, 3, "USE") == 0)
    {
      Use(stackArr, &stackBag, &queueINV, col);
    }
    // print check for each cmd
    // printALLstack(stackArr, col, row);
    // printstackBag(&stackBag);
  }
  // final output
  printstackBag(&stackBag);
  printFINAL(stackArr, col);

  // delete
  // for (int i = 0; i < row; i++)
  //   delete[] map[i];
  // delete[] map;

  // for (int i = 0; i < col; i++)
  //   delete[] mapT[i];
  // delete[] mapT;

  // delete[] stackArr;

  return 0;
}
void printALLstack(Stack *stackArr, int col, int row) // it shows where the top is
{
  // get the highest level among all col;
  for (int j = 0; j < col; j++)
  {
    int toplevel = stackArr[j].get_top() + 1; // top is index, + 1 is length
    if (toplevel > row)
    {
      for (int i = 0; i < toplevel; i++) // row = height of stack
      {
        stackArr[j].printStack_index(i);
      }
    }
    else
    {
      for (int i = 0; i < row; i++) // row = height of stack
      {
        stackArr[j].printStack_index(i);
      }
    }
    cout << '\n';
  }
}