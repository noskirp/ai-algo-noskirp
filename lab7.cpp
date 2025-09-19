#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <list>
using namespace std;

struct Edge {
  int from_number;  // номер узла, из которого выходит дуга
  int to_number; // номер узла, в который входит дуга

  int flow; // поток через дугу
  int flow_capacity; // пропускная способность дуги

  Edge(int from_number_, int to_number, int flow_, int flow_capacity_) :
    from_number(from_number_), to_number(to_number),
    flow(flow_), flow_capacity(flow_capacity_) {
  }

  int residual_flow() { // остаточный поток
    return flow_capacity - flow;
  }
};

struct Node {
  int number;
  char letter;
  vector<Edge*> adjacent; // список инцидентности

  Node(int number_) : number(number_) { }

  ~Node() {
    for (auto& edge : adjacent)
      delete edge;
  }
};


// поиск путей между src и dst в графе graph
// причем, путей, по которым можно еще что-то передать
// (т.е. не содержащих дуг где flow_capacity-flow == 0)
bool dfs(vector<Node*>& graph, int src, int dst, list<Edge*>& path, list<int>& visited) {
  if (src == dst)
    return true;

  if (find(visited.begin(), visited.end(), src) != visited.end()) {
    // если в этой вершине мы уже были
    return false;
  }

  auto& adjacent = graph[src]->adjacent;
  for (auto edge : adjacent) {
    if (edge->residual_flow() <= 0)
      continue;

    // если дуга не загружена - перехожу по ней
    visited.push_back(src);
    if (dfs(graph, edge->to_number, dst, path, visited)) {
      path.push_front(edge);
      return true;
    }
  }

  return false;
}

Edge* get_edge(vector<Node*>& graph, int src, int dst) {
  // граф задан списком инцидентности,
  // надо взять нужную вершину (src) и перебрать в ней все дуги, пока не попадется dst
  auto& adjacent = graph[src]->adjacent;

  for (auto edge : adjacent) {
    if (edge->to_number == dst)
      return edge;
  }
  return nullptr;
}

int ford_fulkerson(vector<Node*>& graph, int src, int dst) {
  int pathCount = 0;
  while (true) {
    list<Edge*> path;
    list<int> visited;

    // если найти путь уже нельзя - возвращаю текущее количество путей
    if (dfs(graph, src, dst, path, visited) == false)
      break;

    pathCount++;
    int min_flow = 1; // в классическом методе - обойти путь и найти в нем минимальную остаточную пропускную способность
    // у всех пройденных дуг увеличиваю поток на величину min_flow
    // а также нахожу антидугу и уменьшаю поток на ней
    for (auto edge : path) {
      edge->flow += min_flow;
      auto* antiEdge = get_edge(graph, edge->to_number, edge->from_number);
      antiEdge->flow -= min_flow;
    }
  }
  return pathCount;
}

int main() {
  ifstream ifst("input.txt");
  ofstream ofst("output.txt");

  // граф задан в виде списков инцидентности
  // граф двудольный - одна доля - буквы кубиков
  // другая - буквы имени
  // узел - строка таблицы
  vector<Node*> graph;
  string name;

  // в имени может быть несколько одинаковых символов, допустим "ANN"
  // если на кубике X есть буква N - она должна быть соединена дугой с обоими буквами имени
  // (так как может встать на любое место в имени)
  // чтобы делать это - будем хранить двумерный массив, где:
  // строка задает букву имени - например нулевая - букву А, вторая - B и так далее
  // в строке находятся идентификаторы узлов графа, соответствующие этой букве (имени)
  vector<vector<int>> char_to_id(256);

  size_t n;

  ifst >> n;
  ifst >> name;

  const auto NameLength = name.length();

  // всего в графе узлов столько, сколько в сумме в обоих долях
  graph.resize(n + NameLength);

  // первые узлы - символы имени
  for (size_t i = 0; i < NameLength; ++i) {
    graph[i] = new Node(i);
    graph[i]->letter = name[i];
    char_to_id[name[i]].push_back(i);
  }

  // дальше - кубики (им не задана буква, но заданы смежные узлы)
  for (size_t i = 0; i < n; ++i) {
    const int blockNodeId = NameLength+i;
    graph[blockNodeId] = new Node(blockNodeId);

    string line;
    ifst >> line;

    // если в кубике повторяются символы - удаляю их, так как кубик может быть исопльзован только 1 раз
    std::sort(line.begin(), line.end());
    auto last = unique(line.begin(),line.end());
    line.resize(std::distance(line.begin(), last));

    // перебираю оставшиеся буквы кубика
    for (size_t j = 0; j < line.size(); ++j) {
      const auto symbol = line[j];

      // буква кубика смежна каким-то буквам имени (возможно нескольким), выбираю номера узлов графа
      auto& name_letter_ids = char_to_id[symbol];

      for (size_t k = 0; k < name_letter_ids.size(); ++k) {
        auto nameNodeId = name_letter_ids[k];

        // для каждой такой пары устанавливаю связь
        graph[nameNodeId]->adjacent.push_back(new Edge(nameNodeId, blockNodeId, 0, 1));
        graph[blockNodeId]->adjacent.push_back(new Edge(blockNodeId, nameNodeId, 0, 0));
      }
    }
  }

  // добавляем в конец вершины стока и истока
  int srcNumber = graph.size();
  int dstNumber = srcNumber+1;

  graph.push_back(new Node(srcNumber));
  graph.push_back(new Node(dstNumber));

  // соединяю их
  for (size_t i = 0; i < NameLength; ++i) {
    graph[srcNumber]->adjacent.push_back(new Edge(srcNumber, i, 0, 1));
    graph[i]->adjacent.push_back(new Edge(i, srcNumber, 0, 0));
  }

  for (size_t i = NameLength; i < NameLength + n; ++i) {
    graph[i]->adjacent.push_back(new Edge(i, dstNumber, 0, 1));
    graph[dstNumber]->adjacent.push_back(new Edge(dstNumber, i, 0, 0));
  }

  int pathCount = ford_fulkerson(graph, srcNumber, dstNumber);
  if (pathCount == NameLength) {
    ofst << "YES" << endl;

    // перебираю узлы графа, соответствующие буквы имени
    for (size_t i = 0; i < NameLength; ++i) {
      if (i > 0)
        ofst << ' ';
      auto* node = graph[i];

      // ofst << endl << node->letter << " ";
      // для каждой буквы нахожу занятую дугу
      for (auto& edge : node->adjacent) {
        if (edge->flow == edge->flow_capacity) {
          // отнимаю NameLength, так как узлы кубиков в графе идут после узлов имени
          // +1 так как номерация кубиков начинается с 1, а узлов с 0
          ofst << (edge->to_number - NameLength + 1);
        }
      }
    }
  }
  else {
    ofst << "NO";
  }
}