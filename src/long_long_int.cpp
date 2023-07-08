// #include "long_long_int.h"
// #include <iostream>
// #include <list>
// #include <string>
// #include <unordered_map>
// #include <vector>

// using namespace std;

// Sobit::Sobit(){};
// Sobit::Sobit(long long int subject, long long int object,
//              list<long long int> ss, list<long long int> so,
//              list<long long int> os, list<long long int> oo) {

//   subject = subject;
//   object = object;
//   ss = ss;
//   so = so;
//   os = os;
//   oo = oo;
// }

// void Sobit::setData(long long int subject, long long int object,
//                     list<long long int> ss = {}, list<long long int> so = {},
//                     list<long long int> os = {}, list<long long int> oo = {}) {

//   subject = subject;
//   object = object;
//   ss = ss;
//   so = so;
//   os = os;
//   oo = oo;
// }

// vector<list<long long int>> Sobit::getData() const {

//   list<long long int> sub;
//   list<long long int> obj;
//   sub.push_back(subject);
//   obj.push_back(object);
//   vector<list<long long int>> res;
//   res.push_back(sub);
//   res.push_back(obj);
//   res.push_back(ss);
//   res.push_back(so);
//   res.push_back(os);
//   res.push_back(oo);

//   return res;
// }

// void maptoSobits(list<pair<int, int>> table, list<Sobit> &sobitTable) {

//   for (auto x : table) {
//     Sobit s;
//     s.setData(x.first, x.second);
//     sobitTable.push_back(s);
//   }
// }