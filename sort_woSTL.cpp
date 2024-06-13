#include <iostream>
#include <string>
#include <vector>

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

  Person(int index, string name, int age, int money, int member, string join_date, int taken)
      : index(index), name(name), age(age), money(money), member(member), join_date(join_date), taken(taken) {}
};

void print_list(const vector<Person> &list);
void quicksort(vector<Person> &list, int low, int high, bool (*comp)(const Person &, const Person &));
int partition(vector<Person> &list, int low, int high, bool (*comp)(const Person &, const Person &));

bool compare(const Person &a, const Person &b)
{
  if (a.member != b.member)
    return a.member > b.member;
  if (a.join_date != b.join_date)
    return a.join_date < b.join_date;
  if (a.name != b.name)
    return a.name < b.name;
  return a.index < b.index;
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

  vector<Person> list;
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

  for (int i = 0; i < num_days; i++)
  {
    int p, x, k;
    cin >> p >> x >> k;
    while (p--)
    {
      cin.ignore();
      string name, date;
      int age, money, member;
      getline(cin, name);
      cin >> age >> money >> member >> date;
      list.push_back(Person(idx++, name, age, money, member, date, 0));
    }
    cout << "DAY #" << i + 1 << '\n';

    quicksort(list, 0, list.size() - 1, compare);

    int income = 0;
    while (x-- && !list.empty())
    {
      Person one = list.front();
      income += one.money;
      cout << one.name << ' ' << one.age << '\n';
      list.erase(list.begin());
    }

    quicksort(list, 0, list.size() - 1, compare2);

    int len = list.size();
    if (len > k)
    {
      list.erase(list.begin(), list.begin() + k);
    }
    else
    {
      list.clear();
    }

    income = (income / 10) * 9;
    cout << "INCOME TODAY: " << income << '\n';
  }
}

void quicksort(vector<Person> &list, int low, int high, bool (*comp)(const Person &, const Person &))
{
  if (low < high)
  {
    int pi = partition(list, low, high, comp);
    quicksort(list, low, pi - 1, comp);
    quicksort(list, pi + 1, high, comp);
  }
}

int partition(vector<Person> &list, int low, int high, bool (*comp)(const Person &, const Person &))
{
  Person pivot = list[high];
  int i = low - 1;

  for (int j = low; j < high; j++)
  {
    if (comp(list[j], pivot))
    {
      i++;
      swap(list[i], list[j]);
    }
  }
  swap(list[i + 1], list[high]);
  return i + 1;
}

void print_list(const vector<Person> &list)
{
  for (const Person &p : list)
  {
    cout << p.index << '\t' << p.name << '\t' << p.age << '\t' << p.money << '\t' << p.member << '\t' << p.join_date << '\t' << p.taken << '\n';
  }
}
