#include <iostream>
#include <string>
#include <algorithm>
#include <queue>
/*using deque for pop front */

using namespace std;

class Person
{
public:
  int index;
  string name;
  int age;
  int money;
  int member;
  string join_date;
  int taken;

  Person(int index, string name, int age, int money, int member, string join_date, int taken) : index(index), name(name), age(age), money(money), member(member), join_date(join_date), taken(taken) {}
};

void print_list(deque<Person> &list);

bool compare(const Person &a, const Person &b)
{
  // a is with higher priority
  if (a.member != b.member)
    return a.member > b.member; // Members first
  if (a.join_date != b.join_date)
    return a.join_date < b.join_date; // earlier first
  if (a.name != b.name)
    return a.name < b.name; // ascending
  return a.index < b.index; //  last is index
}

bool compare2(const Person &a, const Person &b)
{
  if (a.money != b.money)
    return a.money > b.money;
  return a.index < b.index;
}

int main()
{
  int num_data, num_days;
  cin >> num_data >> num_days;
  cin.ignore();

  deque<Person> list;
  int idx = 0;

  while (num_data--)
  {
    string name, date;
    int age, money, member;
    getline(cin, name);
    cin >> age >> money >> member >> date;
    cin.ignore();
    list.push_back(Person(idx++, name, age, money, member, date, 0));
  }
  // print check

  // print_list(list);

  for (int i = 0; i < num_days; i++)
  {
    // input
    int p, x, k; // num of extra enters person, max of Panda accepts, max of Bear accepts
    cin >> p >> x >> k;
    // executing

    // add extra
    while (p--)
    {
      cin.ignore(); //**must
      string name, date;
      int age, money, member;
      getline(cin, name);
      // cin.ignore();
      cin >> age >> money >> member >> date;
      // cin.ignore();
      list.push_back(Person(idx++, name, age, money, member, date, 0));
    }
    cout << "DAY #" << i + 1 << '\n';

    // Pick for Panda, picked then print and pop out
    int income = 0; // income list
    sort(list.begin(), list.end(), compare);
    int len_list = list.size();
    while (x--)
    {
      if (list.empty()) // original list
        break;

      Person one = list.front();
      income += one.money;
      cout << one.name << ' ' << one.age << '\n';
      list.pop_front();
    }
    // Pick for Bear (not print out)

    sort(list.begin(), list.end(), compare2);
    // while (k--)
    // {
    //   if (list.empty())
    //     break;
    //   Person one = list.front();
    //   list.pop_front();
    // }
    int len = list.size();
    if (len >= k)
      list.erase(list.begin(), list.begin() + k);
    else
      list.erase(list.begin(), list.begin() + len);
    // print here
    income = (income / 10) * 9; // avoid using fp
    // std::cout<< std::fixed; // not print scientific notation
    cout << "INCOME TODAY: " << income << '\n';

    // print_list(list);
  }
}

void print_list(deque<Person> &list)
{
  for (auto i : list)
  {
    cout << i.index << '\t' << i.name << '\t' << i.age << '\t' << i.money << '\t' << i.member << '\t' << i.join_date << '\t' << i.taken << '\n';
  }
}