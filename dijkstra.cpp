#include <iostream>
#include <vector>
#include <list>
#include <limits.h>
#include <queue>
#define MAX 1000000
#define ORDERMAX 101 // add 1
using namespace std;

typedef struct man
{
  int v_man;
  bool state; // T: free  F: assigned
  man(int v = 0, bool s = true) : v_man(v), state(s) {}
  friend class Graph;
} man;

struct ipair
{
  int s;
  int d;
  ipair(int s, int d) : s(s), d(d) {}
  friend class Order;
  friend class Graph;
};

class Order
{
public:
  enum Status
  {
    Sleep,
    Create,
    Confirm,          // control by Order
    Cancel,           // control by Order
    ToClient_confirm, // control by Drop
    TOClient_waiting, // control by Drop
    Complete          // control by Complete
  };

  // int o_id;
  Status status;
  int o_ts;
  int v_man;
  int v_src;
  int v_dst;
  int man_idx;
  int dist_man2src;
  vector<ipair> order_path; // create path so drop can release ts

  Order() : man_idx(-1), v_man(-1), v_src(-1), o_ts(-1), v_dst(-1), dist_man2src(MAX), status(Sleep)
  {
  }
  void print_a_order()
  {
    cout << status << '\t' << o_ts << '\t' << v_man << '\t' << v_src << '\t' << v_dst << '\t' << man_idx << '\t';
    for (auto i : order_path)
      cout << "(" << i.s << ", " << i.d << ") ";
    cout << endl;
  }
  void update_order(Status sts, int ts, int man, int src, int dst, int manidx)
  {
    if (ts != -1)
      o_ts = ts;
    if (src != -1)
      v_src = src;
    if (man != -1)
      v_man = man;
    if (dst != -1)
      v_dst = dst;
    if (manidx != -1)
      man_idx = manidx;
    if (sts != -1)
      status = sts;
  }
  // is 1-base vertex , so use vertex= 0 as initial state
  friend class Graph;
};

class Graph
{
public:
  struct edge
  {
    int v;  // adj vertex
    int w;  // edge weight
    int ts; // edge traffic capacity
    edge(int v, int w, int ts) : v(v), w(w), ts(ts) {}

    // Comparison operator for priority_queue
    bool operator>(const edge &other) const
    {
      return w > other.w;
    }
  };
  int V;                    // No. of vertices
  vector<vector<edge>> adj; // Adjacency list
  vector<vector<ipair>> path;

  Graph(int V) : V(V + 1), adj(V + 1), path(V + 1) // adjust 1-base
  {
    adj[0].push_back(edge(0, 0, 0)); // Dummy to adjust to vertex with 1-base index **?
  }

  void addEdge(int u, int v, int w, int ts)
  {
    // adj[u]: store all vertex has adj with vertex u. index of adj is each vertex
    adj[u].push_back(edge(v, w, ts));
    adj[v].push_back(edge(u, w, ts));
  }

  void update_edge_ts(int u, int v, int ts_incre)
  {
    for (auto i = adj[u].begin(); i != adj[u].end(); ++i)
    {
      if (i->v == v)
        i->ts += ts_incre;
      if (i->ts < 0) // edge case
        i->ts = 0;
    }
    for (auto j = adj[v].begin(); j != adj[v].end(); ++j)
    {
      if (j->v == u)
        j->ts += ts_incre;
      if (j->ts < 0) // edge case
        j->ts = 0;
    }
  }
  void clean_a_path(int idx) // clean path[i] and keep first element
  {
    if (!path[idx].empty()) // vec is path[i]
      path[idx].clear();    // Clear the vector
  }

  // prints shortest path from vertex_s
  int shortestPath(int src, int dst, int o_ts, int mode_update, int mode_find, vector<man> &men_arr, Order &theorder) // from src to all the other vertices
  {
    vector<int> dist(V, MAX);
    priority_queue<edge, vector<edge>, greater<edge>> pq;
    // init v_man for max for initial each time
    int v_man = MAX;
    // int v_dst = MAX; // problem
    vector<ipair> dist_man(V, ipair(0, -1)); //(1: has available man, man_idx)
    int size = men_arr.size();
    if (size == 0 && mode_find == 0)
      return MAX; // edge case
    for (int i = 0; i < size; i++)
    {
      if (men_arr[i].state == 1)
      {
        int v = men_arr[i].v_man;
        if (v != -1 && v != MAX)
        {
          dist_man[v].s = 1; // d use as true false
          dist_man[v].d = i; // s use as i-th man (man_idx)
        }
      }
    }
    // reserve release, clean all previous stored
    int path_len = path.size();
    for (int i = 0; i < path_len; i++)
      clean_a_path(i);

    // create source vertex
    pq.push(edge(src, 0, 0)); // src to src has w and ts = 0
    dist[src] = 0;

    int min_dst_man = MAX;
    int man_idx = MAX;
    // int prev_u = -1;

    // Looping till priority queue becomes empty (all vertex are included to set)
    while (!pq.empty())
    {
      // u is prev included vertex (smallest for each iteration)
      int u = pq.top().v;
      pq.pop(); // pop u!
      // get smallest, once select don't change unless wt is smaller
      // if (u == dst)
      //   v_dst = u;
      // get man
      if (mode_find == 0 && dist_man[u].s == 1 && dist[u] < min_dst_man) //**change
      {
        v_man = u;
        min_dst_man = dist[u];
        man_idx = dist_man[u].d; // **change
        dst = v_man;             // update dst = v_man so we can use it to reserve
      }
      // // edge case fail
      // if (mode_find == 0 && dist_man[u].s == 1 && dist[u] == min_dst_man && u > prev_u)

      // relax
      vector<edge>::iterator i; // 'i' is edge
      // pq: all adj vertex of u, that is relaxed
      for (i = adj[u].begin(); i != adj[u].end(); ++i) // all adj vertices of u
      {
        int v = (*i).v;                                        // adj vertex of u // can also i->v;
        int weight = ((*i).ts < o_ts) ? (*i).w + MAX : (*i).w; // not enough ts > increase wt + MAX

        // relax: relax all adj vertices of u (prev included in set)
        if (dist[v] > dist[u] + weight) // need relax v
        {
          dist[v] = dist[u] + weight;
          pq.push(edge(v, dist[v], o_ts));
          // push all vertex has relax (means not in the set and adj to u)

          // reserve release
          clean_a_path(v);
          path[v] = path[u];
          path[v].push_back(ipair(u, v));
        }
      }
      // edge case fail
      // prev_u = u;
    }

    // directly reserve or release after finding shortest path
    if (dist[dst] < MAX) // there is path to dst
    {
      // cout << "check final dist[" << dst << ']' << dist[dst] << endl; // check
      if (mode_update != -1) // -1 means no update path
      {
        for (auto i = path[dst].begin(); i != path[dst].end(); ++i) // update the path ts
        {
          int s = i->s;
          int d = i->d;
          if (mode_update) // mode ==1 >> reserve , mode ==0 >> release
            update_edge_ts(s, d, -o_ts);
          else
            update_edge_ts(s, d, o_ts);
        }
      }
      // Order and Drop all need to save path for future release
      if (!theorder.order_path.empty())
        theorder.order_path.clear(); // as Drop need to clear prev
      theorder.order_path = path[dst];

      // update man_related
      if (mode_find == 0)
      {
        theorder.dist_man2src = dist[dst];
        theorder.v_man = v_man;
        // update man
        theorder.man_idx = man_idx;
        men_arr[man_idx].state = false;
      }
      return (mode_find == 0) ? v_man : dist[dst]; // Order return v_man/ Drop return dist
    }
    else
      return MAX;
  }
  void update_ts_usingPath(Order &theorder, int o_ts, int mode_update)
  {
    for (auto i : theorder.order_path) // update the path ts
    {
      int s = i.s;
      int d = i.d;
      if (mode_update == 1) // mode ==1 >> reserve , mode ==0 >> release
        update_edge_ts(s, d, -o_ts);
      else if (mode_update == 0)
        update_edge_ts(s, d, o_ts);
    }
  }

  void print_arr(vector<int> &arr, int src)
  {
    cout << "src - " << src << " \n";
    int len = arr.size();
    for (int i = 1; i < len; i++)
      cout << i << '\t';
    cout << endl;
    for (int i = 1; i < len; i++)
      cout << arr[i] << '\t';
    cout << endl;
  }

  void print_edges()
  {
    cout << "v_s" << '\t' << "v_d" << '\t' << "wt" << '\t' << "ts" << endl;
    for (int i = 1; i < adj.size(); i++)
    {
      for (auto &e : adj[i])
        cout << i << '\t' << e.v << '\t' << e.w << '\t' << e.ts << endl;
    }
  }
  void print_paths()
  {
    for (size_t i = 0; i < path.size(); ++i)
    {
      cout << "path[" << i << "]: ";
      for (auto p : path[i])
      {
        cout << "(" << p.s << ", " << p.d << ") ";
      }
      cout << endl;
    }
    cout << endl;
  }
};

// Graph operation
void print_man(const vector<man> &men_arr);
void print_order(vector<Order> &order_arr);
void Drop(int o_id, int v_dst, Order &order, vector<man> &men_arr, Graph &g);
int main()
{
  // input -0
  int v, e, d; // # of v, e, # of v has man
  cin >> v >> e >> d;

  // Initial create
  Graph g(v);
  // input -1 : man PLACE
  vector<man> men_arr;
  while (d--)
  {
    string cmd;
    cin >> cmd;
    if (cmd == "PLACE")
    {
      int v_id, num_men; // 2 1: 1 man stands on vertex 2
      cin >> v_id >> num_men;
      while (num_men--)
        men_arr.push_back(man(v_id, true));
    }
  }
  // cout << "initial men_arr\n";
  // print_man(men_arr); // check

  // input -2 : Edge
  for (int i = 0; i < e; i++)
  {
    string cmd;
    cin >> cmd;
    if (cmd == "EDGE")
    {
      int s, d, dis, e_ts; // ts:traffic space of e (# of men can use at the same time)
      cin >> s >> d >> dis >> e_ts;
      g.addEdge(s, d, dis, e_ts);
    }
  }
  // g.print_edges(); // check
  // cout << endl;    // question not requirement !

  // input -3: command
  int cmd_n;
  cin >> cmd_n;
  vector<Order> order_arr(ORDERMAX);
  // order_arr.push_back(Order(0, 0)); // order is 1-base push dummy[0]

  // 1. indicate the timing: receive order and try assign man
  while (cmd_n--)
  {
    string cmd;
    cin >> cmd;
    if (cmd == "Order")
    {
      int o_id, v_src, o_ts;
      cin >> o_id >> v_src >> o_ts;
      order_arr[o_id].update_order(Order::Create, o_ts, 0, v_src, 0, 0);

      // 1-1: find man
      int v_man = g.shortestPath(v_src, 0, o_ts, 1, 0, men_arr, order_arr[o_id]); // dst = 0!! **avoid RT error

      if (v_man < MAX)
        cout << "Order " << o_id << " from: " << v_man << endl;
      // 1-2: reserve v_man to v_src is in the function shortestPath
      else
      {
        cout << "Just walk. T-T\n";
        order_arr[o_id].status = Order::Cancel;
      }
    }
    // 2. indicate timing: man arrive v_src (= restaurant = pick up location)
    else if (cmd == "Drop")
    {
      int o_id, v_dst;
      cin >> o_id >> v_dst;
      Order order = order_arr[o_id]; // edge case
      if (order.status == Order::Sleep || order.status == Order::Cancel)
        continue;
      Drop(o_id, v_dst, order_arr[o_id], men_arr, g);
    }
    // 3. indicate timing: man arrive v_dst, complete order
    else if (cmd == "Complete")
    {
      int o_id;
      cin >> o_id;
      Order order = order_arr[o_id];
      if (order.status == Order::Sleep || order.status == Order::Cancel) // edge case
        continue;
      // 3-1: release v_src to v_dst (in 2-4)
      if (order.status == Order::ToClient_confirm) // edge case
      {
        g.update_ts_usingPath(order, order.o_ts, 0); // 0 is release mode

        // 3-2: complete the order, update man place/status and order status
        int man_idx = order_arr[o_id].man_idx;
        int v_dst = order_arr[o_id].v_dst;
        men_arr[man_idx].v_man = v_dst;
        men_arr[man_idx].state = true;
        order_arr[o_id].status = Order::Complete;
        // 3-2: perform drop with order is waiting (in ascending in order_arr)
        int len = order_arr.size();
        for (int i = 1; i < len; i++)
        {
          if (order_arr[i].status == Order::TOClient_waiting)
          {
            // cout << "the order is : " << i << endl;
            Drop(i, order_arr[i].v_dst, order_arr[i], men_arr, g);
          }
        }
      }
    }
  }
  // print_order(order_arr); // check
  // print_man(men_arr);     // check
  // g.print_edges();        // check

  return 0;
}

void Drop(int o_id, int v_dst, Order &order, vector<man> &men_arr, Graph &g)
{
  // 2-1: release v_man to v_src (in 1-2)
  if (order.status != Order::TOClient_waiting)   // other wise waiting will increase ts
    g.update_ts_usingPath(order, order.o_ts, 0); // 0 is release mode

  // 2-2: as man arrive update man_arr / update order_arr with v_dst
  int man_idx = order.man_idx;
  int v_src = order.v_src;
  int o_ts = order.o_ts;
  men_arr[man_idx].v_man = v_src;
  order.v_dst = v_dst;
  // 2-3: get shortest path
  int dist_man2src = order.dist_man2src;
  int dist_src2dst = g.shortestPath(v_src, v_dst, o_ts, 1, 1, men_arr, order); // mode 1 reserve/ 1 for Drop

  // check

  if (dist_src2dst < MAX)
  {
    cout << "Order " << o_id << " distance: " << dist_man2src + dist_src2dst << endl;
    order.status = Order::ToClient_confirm;
    // 2-4: reserve v_src to v_dst is in shortestPath func
  }
  else
  {
    cout << "No Way Home\n";
    order.status = Order::TOClient_waiting;
  }
}

void print_man(const vector<man> &men_arr)
{
  int size = men_arr.size();
  cout << "man_id | v_man | state\n";
  for (int i = 0; i < size; i++)
  {
    cout << i << '\t' << men_arr[i].v_man << "\t" << boolalpha << men_arr[i].state << endl;
  }
}

void print_order(vector<Order> &order_arr)
{
  cout << "o_id | status | o_ts | v_man | v_src | v_dst | assigned man | path\n";
  for (int i = 0; i < ORDERMAX; i++)
  {
    cout << i << '\t';
    order_arr[i].print_a_order();
  }
}