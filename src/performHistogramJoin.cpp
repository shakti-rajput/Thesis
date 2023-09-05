#include "config.h"
#include "timer.h"
#include "helper.h"

#include "performHistogramJoin.h"
#include <unordered_set>

using namespace std;

void makeHash(list<Sobit> &tables,
              bool index,
              unordered_map<long long int, list<Sobit>> &hashTable)
{
    Timer timer;
    timer.start();

    if (index)
    {
        for (auto x : tables)
        {
            hashTable[x.getObject()].push_back(x);
        }
    } // if index == 1 then custom key is second element
    else
    {
        for (auto x : tables)
        {
            hashTable[x.getSubject()].push_back(x);
        }
    }
    cout << tables.size() << endl;
    cout << "Hash Table size: " << hashTable.size() << endl;
    cout << "Hash Table making time --> " << timer.elapsed() << endl;
}

void reduceFrwd(list<Sobit> &currTable,
                bool currTableCommonVertex,
                list<Sobit> &rightTable,
                bool rightTableCommonVertex,
                unordered_map<long long int, list<Sobit>> &finalNextTableHash)
{
    unordered_map<long long int, list<Sobit>> hashTable;
    makeHash(rightTable, rightTableCommonVertex, hashTable);
    int tempKey;

    if (currTableCommonVertex)
    {
        for (auto x : currTable)
        {
            tempKey = x.getObject();
            if (hashTable.find(tempKey) != hashTable.end())
            {
                finalNextTableHash[tempKey] = hashTable[tempKey];
                hashTable.erase(tempKey);
            }
        }
    }
    else
    {
        for (auto x : currTable)
        {
            tempKey = x.getSubject();
            if (hashTable.find(tempKey) != hashTable.end())
            {
                finalNextTableHash[tempKey] = hashTable[tempKey];
                hashTable.erase(tempKey);
            }
        }
    }
}

void reduceCurrentTable(list<Sobit> &nextTable,
                        bool nextTableCommonVertex,
                        unordered_map<long long int, list<Sobit>> &finalRightTableHash)
{
    if (nextTableCommonVertex)
    {
        for (auto x = nextTable.begin(); x != nextTable.end();)
        {
            if (finalRightTableHash.find(x->getObject()) == finalRightTableHash.end())
            {
                x = nextTable.erase(x);
            }
            else
            {
                bool flag = false;
                for (auto entry : finalRightTableHash[x->getObject()])
                {
                    if (x->getSubject() == entry.getSubject())
                    {
                        flag = true;
                        break;
                    }
                }
                if (flag)
                    x++;
                else
                {
                    x = nextTable.erase(x);
                }
            }
        }
    }
    else
    {
        int count = 0;
        for (auto x = nextTable.begin(); x != nextTable.end();)
        {
            if (finalRightTableHash.find(x->getSubject()) == finalRightTableHash.end())
            {
                x = nextTable.erase(x);
            }
            else
            {
                bool flag = true;
                // int size = finalRightTableHash[x->getSubject()].size();
                // set<long long int> sObj;
                // set<long long int> sSub;

                for (auto entry : finalRightTableHash[x->getSubject()])
                {
                    // sObj.insert(entry.getObject());
                    // sSub.insert(entry.getSubject());
                    if (x->getObject() == entry.getObject())
                    {
                        flag = false;
                    }
                }
                // cout << endl;

                if (flag)
                {
                    x = nextTable.erase(x);
                }
                else
                {
                    // cout << "sObj: " << sObj.size() << endl;
                    // cout << "sSub: " << sSub.size() << endl;
                    // cout << "Size: " << size << endl;
                    // if (sObj.size() != size)
                    // {
                    //     cout << "Kuch to gdbd h Daya sObj !!!" << endl;
                    // }
                    // if (sSub.size() != 1)
                    // {
                    //     cout << "Kuch to gdbd h Daya sSub !!!" << endl;
                    // }

                    // for (auto entry : finalRightTableHash[x->getSubject()])
                    // {
                    //     cout << "(" << entry.getSubject() << "," << entry.getObject() << ") ";
                    // }

                    x++;
                }
            }
        }
    }
}

void buildSobitTableFromHashMap(list<Sobit> &nextTable,
                                bool nextTableCommonVertex,
                                unordered_map<long long int, list<Sobit>> &finalRightTableHash)
{
    for (const auto &entries : finalRightTableHash)
    {
        for (const auto &entry : entries.second)
        {
            nextTable.push_back(entry);
        }
    }
}

void reduceBckrwd(list<Sobit> &currTable,
                  bool currTableCommonVertex,
                  unordered_map<long long int, list<Sobit>> &finalRightTableHash)
{
    if (currTableCommonVertex)
    {
        for (auto x = currTable.begin(); x != currTable.end();)
        {
            if (finalRightTableHash.find(x->getObject()) == finalRightTableHash.end())
            {
                x = currTable.erase(x);
            }
            else
                x++;
        }
    }
    else
    {
        for (auto x = currTable.begin(); x != currTable.end();)
        {
            if (finalRightTableHash.find(x->getSubject()) == finalRightTableHash.end())
            {
                x = currTable.erase(x);
            }
            else
                x++;
        }
    }
}

void reduceFrwdHash(unordered_map<long long int, list<Sobit>> &currTableHash,
                    bool currTableCommonVertex,
                    list<Sobit> &nextTable,
                    bool nextTableCommonVertex,
                    unordered_map<long long int, list<Sobit>> &finalNextTableHash)
{

    unordered_map<long long int, list<Sobit>> hashTable;
    makeHash(nextTable, nextTableCommonVertex, hashTable);

    int tempKey;
    if (currTableCommonVertex)
    {
        for (auto it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                tempKey = it2->getObject();
                if (hashTable.find(tempKey) != hashTable.end())
                {
                    // count++;
                    finalNextTableHash[tempKey] = hashTable[tempKey];
                    hashTable.erase(tempKey);
                }
                if (finalNextTableHash.find(tempKey) == finalNextTableHash.end())
                {
                    // count++;
                    it2 = it->second.erase(it2);
                }
                else
                    it2++;
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
            }
            else
                it++;
        }
    }
    else
    {
        for (auto it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                tempKey = it2->getObject();
                if (hashTable.find(tempKey) != hashTable.end())
                {
                    finalNextTableHash[tempKey] = hashTable[tempKey];
                    hashTable.erase(tempKey);
                }
                if (finalNextTableHash.find(tempKey) == finalNextTableHash.end())
                {
                    it2 = it->second.erase(it2);
                }
                else
                    it2++;
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
            }
            else
                it++;
        }
    }
}

void reduceBckrwdHash(list<Sobit> &currTable,
                      unordered_map<long long int, list<Sobit>> &currTableHash,
                      bool currTableCommonVertex,
                      list<Sobit> &nextTable,
                      unordered_map<long long int, list<Sobit>> &nextTableHash,
                      bool nextTableCommonVertex)
{
    int flag = 0;

    if (currTableCommonVertex)
    {
        for (auto it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                int tempKey = it2->getObject();

                if (nextTableHash.find(tempKey) == nextTableHash.end())
                    it2 = it->second.erase(it2);
                else
                    it2++;
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
            }
            else
                it++;
        }

        for (auto x = currTable.begin(); x != currTable.end();)
        {
            if (currTableHash.find(x->getSubject()) != currTableHash.end() &&
                nextTableHash.find(x->getObject()) != nextTableHash.end())
            {
                x++;
            }
            else
                x = currTable.erase(x);
        }
    }
    else
    {
        for (auto it = currTableHash.begin(); it != currTableHash.end();)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end();)
            {
                int tempKey = it2->getSubject();

                if (nextTableHash.find(tempKey) == nextTableHash.end())
                    it2 = it->second.erase(it2);
                else
                    it2++;
            }
            if (it->second.empty())
            {
                it = currTableHash.erase(it);
            }
            else
                it++;
        }
        for (auto x = currTable.begin(); x != currTable.end();)
        {
            if (currTableHash.find(x->getObject()) != currTableHash.end() &&
                nextTableHash.find(x->getSubject()) != nextTableHash.end())
            {
                x++;
            }
            else
                x = currTable.erase(x);
        }
    }
    if (nextTableCommonVertex)
    {
        for (auto x = nextTable.begin(); x != nextTable.end();)
        {
            if (nextTableHash.find(x->getObject()) == nextTableHash.end())
            {
                x = nextTable.erase(x);
            }
            else
                x++;
        }
    }
    else
    {
        for (auto x = nextTable.begin(); x != nextTable.end();)
        {
            if (nextTableHash.find(x->getSubject()) == nextTableHash.end())
            {
                x = nextTable.erase(x);
            }
            else
                x++;
        }
    }
}

void dfsHash(long long int node,
             const Graph &g,
             unordered_set<long long int> &visited,
             unordered_set<long long int> &pathVisited,
             unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
             unordered_map<long long int,
                           unordered_map<long long int,
                                         unordered_map<long long int,
                                                       unordered_map<long long int, list<Sobit>>>>> &hashStore,
             list<Sobit> &currTable,
             unordered_map<long long int, list<Sobit>> &currTableHash,
             bool currTableCommonVertex)
{
    visited.insert(node);
    pathVisited.insert(node);
    cout << "Nodee: " << node << endl;
    // g.minVertexAdj;

    // 1 -- 2 -- 3 -- 4 -- 5 -- 6 -- 7 -- 8 -- 9
    //      C         C         C         C

    //      6 -- 4 -- 0 -- 1 -- 7 -- 5 -- 2
    for (auto table : g.VCTree.at(node))
    {
        for (auto v : table.second)
        {
            if (g.VCTree.find(v.second.first) != g.VCTree.end() && pathVisited.find(v.second.first) == pathVisited.end())
            {

                currTable;
                currTableHash;         // 1st table
                currTableCommonVertex; //

                long long int u1 = node;
                long long int v1 = v.first;
                long long int tableIndex1 = table.first;
                g.minVertexAdj.at(u1).at(tableIndex1).at(v1); // check direction of curr table
                sobitTables[tableIndex1];                     // 2nd Table
                bool firstTableCommonVertex = g.minVertexAdj.at(u1).at(tableIndex1).at(v1);

                int count;
                count = 0;

                reduceFrwdHash(currTableHash, // hash Table of table 1 is created
                               currTableCommonVertex,
                               sobitTables[u1][tableIndex1][v1],
                               !firstTableCommonVertex,
                               hashStore[u1][tableIndex1][v1]);

                long long int u2 = v.first;
                long long int v2 = v.second.first;
                long long int tableIndex2 = v.second.second;

                sobitTables[tableIndex2];                     // 3rd table
                g.minVertexAdj.at(v2).at(tableIndex2).at(u2); // check direction of second table
                bool secondTableCommonVertex = g.minVertexAdj.at(v2).at(tableIndex2).at(u2);

                reduceFrwdHash(hashStore[u1][tableIndex1][v1], // hash Table of table 2 is created
                               firstTableCommonVertex,
                               sobitTables[v2][tableIndex2][u2],
                               secondTableCommonVertex,
                               hashStore[v2][tableIndex2][u2]);

                dfsHash(v2, g, visited, pathVisited, sobitTables, hashStore,
                        sobitTables[v2][tableIndex2][u2], hashStore[v2][tableIndex2][u2], secondTableCommonVertex);

                reduceBckrwdHash(sobitTables[u1][tableIndex1][v1], // Table of Table 2 is reduced and hash of table 1 is reduced
                                 hashStore[u1][tableIndex1][v1],
                                 firstTableCommonVertex,
                                 sobitTables[v2][tableIndex2][u2],
                                 hashStore[v2][tableIndex2][u2],
                                 secondTableCommonVertex);

                reduceBckrwdHash(currTable, // Table of Table 1 is reduced and hash of curr Table is reduced
                                 currTableHash,
                                 currTableCommonVertex,
                                 sobitTables[u1][tableIndex1][v1],
                                 hashStore[u1][tableIndex1][v1], // firstTableHash,
                                 !firstTableCommonVertex);
            }
        }
    }
    pathVisited.erase(node);
}

void dfs(long long int node,
         const Graph &g,
         unordered_set<long long int> &visited,
         unordered_set<long long int> &pathVisited,
         unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
         unordered_map<long long int, string> &decodeStringToData,
         unordered_map<long long int,
                       unordered_map<long long int,
                                     unordered_map<long long int,
                                                   unordered_map<long long int, list<Sobit>>>>> &hashStore)
{
    visited.insert(node);
    pathVisited.insert(node);
    cout << "Nodee: " << node << endl;
    for (auto table : g.VCTree.at(node))
    {
        for (auto v : table.second)
        {
            if (g.VCTree.find(v.second.first) != g.VCTree.end() && pathVisited.find(v.second.first) == pathVisited.end())
            {
                int tableIndex1 = table.first;
                int u1 = node;
                int v1 = v.first;
                // sobitTables[tableIndex1];
                bool currTableCommonVertex = g.minVertexAdj.at(u1).at(tableIndex1).at(v1);

                int tableIndex2 = v.second.second;
                int u2 = v.first;
                int v2 = v.second.first;
                // sobitTables[v.second.second];
                bool nextTableCommonVertex = g.minVertexAdj.at(v2).at(tableIndex2).at(u2);

                cout << "tableIndex1: " << tableIndex1 << " tableIndex2: " << tableIndex2 << endl;

                cout << u1 << " " << tableIndex1 << " " << v1 << " " << currTableCommonVertex << endl;
                cout << v2 << " " << tableIndex2 << " " << u2 << " " << nextTableCommonVertex << endl;
                reduceFrwd(sobitTables[u1][tableIndex1][v1],
                           currTableCommonVertex,
                           sobitTables[v2][tableIndex2][u2],
                           nextTableCommonVertex,
                           hashStore[v2][tableIndex2][u2]);

                dfsHash(v2, g, visited, pathVisited, sobitTables, hashStore,
                        sobitTables[v2][tableIndex2][u2], hashStore[v2][tableIndex2][u2], !nextTableCommonVertex);

                reduceBckrwd(sobitTables[v2][tableIndex2][u2],
                             nextTableCommonVertex,
                             hashStore[v2][tableIndex2][u2]);

                reduceBckrwd(sobitTables[u1][tableIndex1][v1],
                             currTableCommonVertex,
                             hashStore[v2][tableIndex2][u2]);
            }
        }
    }
    pathVisited.erase(node);
}

void semiJoinOpConVertices(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                           const Graph &g,
                           unordered_map<long long int, string> &decodeStringToData,
                           unordered_map<long long int,
                                         unordered_map<long long int,
                                                       unordered_map<long long int,
                                                                     unordered_map<long long int, list<Sobit>>>>> &hashStore)
{
    unordered_set<long long> visited;
    unordered_set<long long> pathVisited;

    for (auto node : g.nodesTopoOrder)
    {
        if (g.VCTree.find(node) != g.VCTree.end() && visited.find(node) == visited.end())
        {
            dfs(node, g, visited, pathVisited, sobitTables, decodeStringToData, hashStore);
        }
    }
}

struct PairHash
{
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

bool checkConvertex(const Graph &g, long long int u, long long int table, long long int v)
{
    if (g.VCTree.find(u) != g.VCTree.end() &&
        g.VCTree.at(u).find(table) != g.VCTree.at(u).end() &&
        g.VCTree.at(u).at(table).find(v) != g.VCTree.at(u).at(table).end())
    {
        return true;
    }

    return false;
}

void createVertexCoverConVertices(const Graph &g,
                                  unordered_set<pair<long long int, long long int>, PairHash> &vertexCoverSet,
                                  unordered_map<long long int, vector<pair<long long int, bool>>> &vertexCoverConVertices)
{
    for (const auto &u : g.minVertexAdj)
    {
        for (const auto &table : u.second)
        {
            for (const auto &v : table.second)
            {
                if (checkConvertex(g, u.first, table.first, v.first))
                {
                    vertexCoverSet.insert(make_pair(u.first, table.first));
                    vertexCoverConVertices[u.first].push_back(make_pair(table.first, !v.second));
                }
            }
        }
    }
}

void fetchTable(const unordered_map<long long int,
                                    unordered_map<long long int,
                                                  unordered_map<long long int,
                                                                unordered_map<bool,
                                                                              unordered_map<long long int, list<Sobit>>>>>> &
                    tempHashStore,
                long long int u,
                unordered_map<long long int, list<Sobit>> &hashTable)
{
    if (tempHashStore.find(u) != tempHashStore.end())
    {
        for (auto table : tempHashStore.at(u))
        {
            for (auto &v : table.second)
            {
                for (auto direction : v.second)
                {
                    hashTable = direction.second;
                    cout << "Yha Table Size: " << hashTable.size() << endl;
                    return;
                }
            }
        }
    }
}

void semiJoinOpNonConVertices(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                              const Graph &g, unordered_map<long long int, string> decodeStringToData)
{

    unordered_map<long long int,
                  unordered_map<long long int,
                                unordered_map<long long int,
                                              bool>>>
        nonConVertex;

    unordered_map<long long int,
                  unordered_map<long long int,
                                unordered_map<long long int,
                                              unordered_map<bool,
                                                            unordered_map<long long int, list<Sobit>>>>>>
        tempHashStore;

    unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, bool>>> conTableEdges;

    for (auto u : g.minVertexAdj)
    {
        for (auto table : u.second)
        {
            for (auto v : table.second)
            {
                if (!checkConvertex(g, u.first, table.first, v.first))
                {
                    nonConVertex[u.first][table.first][v.first] = v.second;
                    cout << "nonConVertex: " << u.first << " " << table.first << " " << v.first << " " << v.second << endl;
                }
            }
        }
    }
    for (auto u : g.VCTree)
    {
        for (auto table : u.second)
        {
            for (auto v : table.second)
            {
                cout << u.first << " " << table.first << " " << v.first << endl;
            }
        }
    }

    if (!g.VCTree.empty())
    {
        for (auto u : g.VCTree)
        {
            if (nonConVertex.find(u.first) != nonConVertex.end())
            {
                for (auto table : u.second)
                {
                    for (auto v : table.second)
                    {
                        bool direction = !g.minVertexAdj.at(u.first).at(table.first).at(v.first);
                        list<Sobit> tables = sobitTables[u.first][table.first][v.first];
                        makeHash(tables, direction, tempHashStore[u.first][table.first][v.first][direction]);
                        cout << "Hash: " << u.first << " " << table.first << " " << v.first << " " << direction << endl;
                        break;
                    }
                }
            }
        }
        for (auto u : nonConVertex)
        {
            unordered_map<long long int, list<Sobit>> hashTable;
            fetchTable(tempHashStore, u.first, hashTable);
            cout << "hashTable Size: " << hashTable.size();
            for (auto table : u.second)
            {
                for (auto v : table.second)
                {
                    const auto &currTableCommonVertex = !v.second;
                    reduceBckrwd(sobitTables[u.first][table.first][v.first],
                                 currTableCommonVertex,
                                 hashTable);
                }
            }
        }
    }
}

bool reduceLoopdfs(const Sobit &parentSobitEntry,
                   vector<long long int>::iterator iter,
                   const Graph &g,
                   Edge parentEdge,
                   const long long int &nodeO,
                   const long long int &nodeN,
                   unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                   const unordered_map<long long int, string> &decodeQueryTables,
                   const unordered_map<string, long long int> &dataTables,
                   unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>>> &hashStore,
                   unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>>> &updatedHashStore,
                   const Sobit &initialEntry,
                   unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, set<long long int>>>>> &dp,
                   const unordered_map<long long int, string> &decodeStringToData,
                   const unordered_map<string, long long int> &storeStringtoData,
                   unordered_set<long long int> &store)
{
    long long int u = *iter;
    long long int prevObj = parentSobitEntry.getObject();
    if (u == nodeN)
    {
        if (parentSobitEntry.getObject() == initialEntry.getSubject())
        {
            store.insert(parentSobitEntry.getObject());
            return true;
        }

        return false;
    }

    // if (!dp[parentEdge.u][parentEdge.table][parentEdge.v][prevObj].empty())
    // {
    //      return dp[parentEdge.u][parentEdge.table][parentEdge.v][prevObj].find(initialEntry.getSubject()) != dp[parentEdge.u][parentEdge.table][parentEdge.v][prevObj].end();
    // }

    long long int nextU = *next(iter);

    for (const auto &table : g.adj.at(u))
    {
        for (const auto &v : table.second)
        {
            if (v.first == nextU)
            {

                Edge currentEdge;

                if (parentEdge.table != -1)
                {
                    currentEdge.u = u;
                    currentEdge.table = table.first;
                    currentEdge.v = v.first;

                    if (g.minVertexAdj.find(u) == g.minVertexAdj.end())
                    {
                        swap(currentEdge.u, currentEdge.v);
                    }

                    if (hashStore[currentEdge.u][currentEdge.table].find(currentEdge.v) != hashStore[currentEdge.u][currentEdge.table].end()) // If the Hash was already calculated
                    {

                        // if (!dp[parentEdge.u][parentEdge.table][parentEdge.v][prevObj].empty())
                        // {
                        //      return dp[parentEdge.u][parentEdge.table][parentEdge.v][prevObj].find(initialEntry.getSubject()) != dp[parentEdge.u][parentEdge.table][parentEdge.v][prevObj].end();
                        // }

                        if (hashStore[currentEdge.u][currentEdge.table][currentEdge.v].find(prevObj) != hashStore[currentEdge.u][currentEdge.table][currentEdge.v].end())
                        {
                            bool check = false;

                            unordered_set<long long int> accumulateStore;
                            for (const auto &entry : hashStore[currentEdge.u][currentEdge.table][currentEdge.v][prevObj])
                            {
                                unordered_set<long long int> tempStore;
                                bool flag = reduceLoopdfs(entry, iter + 1, g, currentEdge, nodeO, nodeN, sobitTables, decodeQueryTables, dataTables, hashStore, updatedHashStore, initialEntry, dp, decodeStringToData, storeStringtoData, tempStore);
                                accumulateStore.insert(tempStore.begin(), tempStore.end());

                                // dp[currentEdge.u][currentEdge.table][currentEdge.v][prevObj].insert(store.begin(), store.end());
                                dp[currentEdge.u][currentEdge.table][currentEdge.v][prevObj].insert(tempStore.begin(), tempStore.end());

                                if (flag)
                                {
                                    check = true;
                                    updatedHashStore[currentEdge.u][currentEdge.table][currentEdge.v][prevObj].push_back(entry);
                                }
                            }
                            store.insert(accumulateStore.begin(), accumulateStore.end());

                            return check;
                        }
                        else
                            return false;
                    }
                    else
                    {
                        makeHash(sobitTables[currentEdge.u][currentEdge.table][currentEdge.v], 0, hashStore[currentEdge.u][currentEdge.table][currentEdge.v]);

                        if (hashStore[currentEdge.u][currentEdge.table][currentEdge.v].find(prevObj) != hashStore[currentEdge.u][currentEdge.table][currentEdge.v].end())
                        {
                            bool check = false;
                            unordered_set<long long int> accumulateStore;
                            for (const auto &entry : hashStore[currentEdge.u][currentEdge.table][currentEdge.v][prevObj])
                            {

                                unordered_set<long long int> tempStore;
                                bool flag = reduceLoopdfs(entry, iter + 1, g, currentEdge, nodeO, nodeN, sobitTables, decodeQueryTables, dataTables, hashStore, updatedHashStore, initialEntry, dp, decodeStringToData, storeStringtoData, tempStore);
                                accumulateStore.insert(tempStore.begin(), tempStore.end());
                                // dp[currentEdge.u][currentEdge.table][currentEdge.v][prevObj].insert(store.begin(), store.end());
                                dp[currentEdge.u][currentEdge.table][currentEdge.v][prevObj].insert(tempStore.begin(), tempStore.end());

                                if (flag)
                                {
                                    updatedHashStore[currentEdge.u][currentEdge.table][currentEdge.v][prevObj].push_back(entry);
                                    check = true;
                                }
                            }
                            store.insert(accumulateStore.begin(), accumulateStore.end());
                            return check;
                        }
                        else
                            return false;
                    }
                }
                else
                {
                    parentEdge.u = u;
                    parentEdge.table = table.first;
                    parentEdge.v = v.first;

                    if (g.minVertexAdj.find(parentEdge.u) == g.minVertexAdj.end()) // if Parent.u is not found in minvertex edge
                    {
                        swap(parentEdge.u, parentEdge.v);
                    }
                    int countKept = 0;
                    int countDeleted = 0;
                    for (auto entry = sobitTables[parentEdge.u][parentEdge.table][parentEdge.v].begin(); entry != sobitTables[parentEdge.u][parentEdge.table][parentEdge.v].end();)
                    {

                        // if (dp[parentEdge.u][parentEdge.table][parentEdge.v][(*entry).getObject()].find((*entry).getSubject()) != dp[parentEdge.u][parentEdge.table][parentEdge.v][(*entry).getObject()].end())
                        // {
                        //      countKept++;
                        //      entry++;
                        //      continue;
                        // }
                        unordered_set<long long int> tempStore;

                        bool flag = reduceLoopdfs(*entry, iter + 1, g, parentEdge, nodeO, nodeN, sobitTables, decodeQueryTables, dataTables, hashStore, updatedHashStore, *entry, dp, decodeStringToData, storeStringtoData, tempStore);

                        dp[parentEdge.u][parentEdge.table][parentEdge.v][(*entry).getObject()].insert(tempStore.begin(), tempStore.end());

                        if (!flag) // flag == false
                        {
                            countDeleted++;
                            entry = sobitTables[parentEdge.u][parentEdge.table][parentEdge.v].erase(entry);
                        }
                        else
                        {
                            countKept++;
                            entry++;
                        }
                    }
                    cout << "Kitne bache bhai: " << countKept << endl;
                }
            }
        }
    }
}

void reduceLoopTables(unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                      Graph &g,
                      long long int nodeO,
                      long long int nodeN,
                      unordered_map<long long int, string> &decodeQueryTables,
                      unordered_map<string, long long int> &dataTables,
                      unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>>> &hashStore,
                      unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>>> &updatedHashStore,
                      unordered_map<long long int, string> decodeStringToData,
                      unordered_map<string, long long int> storeStringtoData)
{
    vector<long long int> loopPath = g.loopTablePaths[nodeN];
    vector<long long int>::iterator iter = loopPath.begin();
    unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, set<long long int>>>>> dp;

    Timer timer;
    timer.start();

    unordered_set<long long int> store;
    reduceLoopdfs(Sobit(), iter, g, Edge(), nodeO, nodeN, sobitTables, decodeQueryTables, dataTables, hashStore, updatedHashStore, Sobit(), dp, decodeStringToData, storeStringtoData, store);
    cout << "Time elapsed for reduceLoopdfs: " << timer.elapsed() << endl;
    timer.start();
    for (auto u : updatedHashStore)
    {
        for (auto table : u.second)
        {
            for (auto v : table.second)
            {
                reduceCurrentTable(sobitTables[u.first][table.first][v.first], 0, updatedHashStore[u.first][table.first][v.first]);
            }
        }
    }
    cout << "Time elapsed for reduceCurrentTable: " << timer.elapsed() << endl;
}

void reduceLoopTables(Graph &g,
                      unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
                      unordered_map<long long int, string> &decodeQueryTables,
                      unordered_map<string, long long int> &dataTables,
                      unordered_map<long long int, string> &decodeStringToData,
                      unordered_map<string, long long int> &storeStringtoData)
{
    unordered_map<long long int,
                  unordered_map<long long int,
                                unordered_map<long long int,
                                              unordered_map<long long int, list<Sobit>>>>>
        hashStore3;
    unordered_map<long long int,
                  unordered_map<long long int,
                                unordered_map<long long int,
                                              unordered_map<long long int, list<Sobit>>>>>
        updatedHashStore;
    for (auto u : g.loopEdges)
    {
        cout << u.first << " --> ";
        for (auto table : u.second)
        {
            cout << table.first << "[ ";
            for (auto v : table.second)
            {
                cout << v.first << " " << v.second << endl;
                long long int nodeN = v.first;
                long long int nodeO = v.second;

                reduceLoopTables(sobitTables, g, nodeO, nodeN, decodeQueryTables, dataTables, hashStore3, updatedHashStore, decodeStringToData, storeStringtoData);
            }
            cout << "]" << endl;
        }
        cout << endl;
    }
}

// void detectTablesofVertex(long long int u,
//                           long long int table,
//                           long long int v,
//                           Graph g,
//                           long long int oldV,
//                           unordered_map<long long int, unordered_map<long long int, unordered_map<long long int, list<Sobit>>>> &sobitTables,
//                           unordered_map<long long int, string> decodeStringToData)
// {
//      list<Sobit> tableStoreO, tableStoreN;
//      unordered_map<long long int, list<Sobit>> hashTableN, hashTableO;
//      bool indexN, indexO;
//      bool flag = false;
//      cout << u << " " << table << " " << v << endl;
//      if (g.minVertexAdj.find(u) != g.minVertexAdj.end())
//      {
//           cout << "Here: ";
//           indexN = g.minVertexAdj[u][table][v];
//           flag = true;
//      }
//      else if (g.minVertexAdj.find(v) != g.minVertexAdj.end())
//      {
//           cout << "OR_HEREEE: ";
//           indexN = !g.minVertexAdj[v][table][u];
//      }
//      if (g.minVertexAdj.find(oldV) != g.minVertexAdj.end())
//      {
//           cout << "OR_HEREEE::::::::::: " << endl;
//           for (auto table1 : g.minVertexAdj[oldV])
//           {
//                for (auto v1 : table1.second)
//                {

//                     indexO = !g.minVertexAdj[oldV][table1.first][v1.first];
//                     tableStoreO = sobitTables[oldV][table1.first][v1.first];
//                     makeHash(tableStoreO, indexO, hashTableO);
//                     if (flag)
//                     {
//                          reduceBckrwd(sobitTables[u][table][v], indexN, hashTableO);
//                          makeHash(sobitTables[u][table][v], indexN, hashTableN);
//                     }
//                     else
//                     {
//                          reduceBckrwd(sobitTables[v][table][u], indexN, hashTableO);
//                          makeHash(sobitTables[v][table][u], indexN, hashTableN);
//                     }

//                     reduceBckrwd(sobitTables[oldV][table1.first][v1.first], indexO, hashTableN);
//                }
//           }
//      }
//      else
//      {
//           cout << "HEREEEEEEE: " << endl;
//           for (auto u1 : g.minVertexAdj)
//           {
//                for (auto table1 : u1.second)
//                {
//                     for (auto v1 : table1.second)
//                     {
//                          if (v1.first == oldV)
//                          {
//                               indexO = g.minVertexAdj[u1.first][table1.first][v1.first];
//                               tableStoreO = sobitTables[oldV][table1.first][v1.first];
//                               makeHash(tableStoreO, indexO, hashTableO);
//                               if (flag)
//                               {
//                                    reduceBckrwd(sobitTables[u][table][v], indexN, hashTableO);
//                                    makeHash(sobitTables[u][table][v], indexN, hashTableN);
//                               }
//                               else
//                               {
//                                    reduceBckrwd(sobitTables[v][table][u], indexN, hashTableO);
//                                    makeHash(sobitTables[v][table][u], indexN, hashTableN);
//                               }
//                               reduceBckrwd(sobitTables[oldV][table1.first][v1.first], indexO, hashTableN);
//                          }
//                     }
//                }
//           }
//      }
// }
