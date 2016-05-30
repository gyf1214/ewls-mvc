#include "main.hpp"

#define rep(i, a, b) for (int _a = (a), _b = (b), i = _a; i <= _b; ++i)
#define clr(i, a) memset(i, (a), sizeof(i))
#define infi 0x7FFFFFFF

using namespace std;

namespace EWLS {
    inline Edge *pre(Edge *e) {
        Iter x = e -> eL;
        --x;
        return *x;
    }

    inline Edge *next(Edge *e) {
        Iter x = e -> eL;
        ++x;
        return *x;
    }

    void Solver::updateDScore(int u) {
        maxDScore = max(maxDScore, make_pair(dScore[u], u));
    }

    void Solver::updateDScore() {
        maxDScore = make_pair(-infi, 0);
        rep(i, 1, vNum) if (vC[i]) updateDScore(i);
    }

    void Solver::add(int u) {
        assert(!vC[u]);
        vC[u] = true;
        dScore[u] = -dScore[u];
        updateDScore(u);

        rep(i, 0, vEdge[u].size() - 1) {
            Edge *e = vEdge[u][i].second;
            int v = vEdge[u][i].first;

            if (!e -> cnt) {
                dScore[v] -= e -> w;
                l.erase(e -> eL);
                e -> eL = l.end();
            } else {
                dScore[v] += e -> w;
                updateDScore(v);
            }

            ++e -> cnt;
        }
    }

    void Solver::remove(int u) {
        assert(vC[u]);
        vC[u] = false;
        dScore[u] = -dScore[u];
        if (u == maxDScore.second) updateDScore();

        rep(i, 0, vEdge[u].size() - 1) {
            Edge *e = vEdge[u][i].second;
            int v = vEdge[u][i].first;

            --e -> cnt;

            if (!e -> cnt) {
                l.push_front(e);
                e -> eL = l.begin();
                dScore[v] += e -> w;
            } else {
                dScore[v] -= e -> w;
                if (v == maxDScore.second) updateDScore();
            }
        }
    }

    void Solver::updateAns() {
        ub = cSize + l.size();
        rep(i, 1, vNum) ansVC[i] = vC[i];
        if (!l.empty()) coverRest();
        printBest();
    }

    void Solver::coverRest() {
        temp2.assign(l.begin(), l.end());
        if (temp2.empty()) return;

        temp.resize(vNum);

        while (!temp2.empty()) {
            for (Iter i = temp2.begin(); i != temp2.end(); ++i) {
                Edge *e = *i;
                temp[e -> u] = temp[e -> v] = 0;
            }
            for (Iter i = temp2.begin(); i != temp2.end(); ++i) {
                Edge *e = *i;
                ++temp[e -> u];
                ++temp[e -> v];
            }
            pair<int, int> best = make_pair(-infi, 0);
            for (Iter i = temp2.begin(); i != temp2.end(); ++i) {
                int u = (*i) -> u;
                int v = (*i) -> v;
                best = max(best, make_pair(temp[u], u));
                best = max(best, make_pair(temp[v], v));
            }

            int u = best.second;
            ansVC[u] = true;
            for (Iter i = temp2.begin(); i != temp2.end();) {
                Edge *e = *i;
                if (e -> u == u || e -> v == u) {
                    i = temp2.erase(i);
                } else ++i;
            }
        }
    }

    void Solver::pickRemove(int u) {
        if (find(temp.begin(), temp.end(), u) != temp.end()) return;
        if (u == tabuAdd) return;
        temp.push_back(u);
        if (dScore[u] + maxDScore.first <= 0) {
            rep(i, 0, vEdge[u].size() - 1) {
                int v = vEdge[u][i].first;
                Edge *e = vEdge[u][i].second;
                if (!vC[v] || v == tabuRemove) continue;
                if (dScore[u] + dScore[v] + e -> w <= 0) continue;
                temp3.push_back(make_pair(u, v));
            }
        } else {
            int v = rand() % vNum + 1;
            int iV = v;
            for (v = v % vNum + 1; v != iV; v = v % vNum + 1) {
                if (!vC[v] || v == tabuRemove) continue;
                int w = vvEdge[u][v] ? vvEdge[u][v] -> w : 0;
                if (w + dScore[u] + dScore[v] <= 0) continue;
                temp3.push_back(make_pair(u, v));
                return;
            }
        }
    }

    pair<int, int> Solver::pickExchange(int u1, int u2) {
        temp3.clear();
        pickRemove(u1);
        pickRemove(u2);
        if (temp3.empty()) return make_pair(0, 0);
        return temp3[rand() % temp3.size()];
    }

    void Solver::updateWeight() {
        for (Iter i = l.begin(); i != l.end(); ++i) {
            Edge *e = *i;
            ++e -> w;
            ++dScore[e -> u];
            ++dScore[e -> v];
        }
    }

    void Solver::updateSize() {
        int d = cSize - ub + delta;
        cSize = ub - delta;

        rep(i, 1, d) {
            pair<int, int> best = make_pair(-infi, 0);
            rep(u, 1, vNum) if (vC[u]) best = max(best, make_pair(dScore[u], u));
            remove(best.second);
        }
    }

    void Solver::printBest() {
        clock_t finish = clock();
        cout << "optimal mvc: " << ub << " ";
        cout << "in " << step << " steps, ";
        cout << (double)(finish - start) / CLOCKS_PER_SEC << " seconds" << endl;
    }

    void Solver::init(istream &in) {
        char tmp1[10], tmp2[10];
        in >> tmp1 >> tmp2 >> vNum >> eNum;
        ub = vNum;

        rep(i, 1, vNum) vEdge[i].clear();
        rep(i, 1, vNum) rep(j, 1, vNum) vvEdge[i][j] = NULL;
        readEdge(in);
        l.clear();
    }

    void Solver::readEdge(istream &in) {
        rep(i, 0, eNum - 1) {
            char tmp;
            int u, v;
            in >> tmp >> u >> v;

            edge[i].u = u;
            edge[i].v = v;
            vEdge[u].push_back(make_pair(v, edge + i));
            vEdge[v].push_back(make_pair(u, edge + i));
            vvEdge[u][v] = vvEdge[v][u] = edge + 1;
        }
    }

    void Solver::initSolution() {
        rep(i, 1, vNum) {
            vC[i] = false;
            dScore[i] = 0;
        }
        rep(i, 0, eNum - 1) {
            edge[i].w = 1;
            edge[i].cnt = 0;
            edge[i].eL = l.end();
            ++dScore[edge[i].u];
            ++dScore[edge[i].v];
            l.push_front(edge + i);
            edge[i].eL = l.begin();
        }
        cSize = ub - delta;
        maxDScore = make_pair(-infi, 0);
        int i;
        for (i = 0; i < cSize && !l.empty(); ) {
            int best = -1;
            rep(u, 1, vNum) if (!vC[u]) {
                if (dScore[u] > best) {
                    temp.clear();
                    temp.push_back(u);
                    best = dScore[u];
                } else if (dScore[u] == best) {
                    temp.push_back(u);
                }
            }
            if (!temp.empty()) {
                add(temp[rand() % temp.size()]);
                ++i;
            }
        }
        tabuAdd = tabuRemove = 0;
        cSize = i;
        step = 0;
        updateAns();
    }

    void Solver::cover() {
        initSolution();
        Iter now = l.end();
        while (step < maxStep) {
            while (l.empty()) {
                if (cSize < ub) updateAns();
                updateSize();
            }
            temp.clear();
            Iter check = l.end();
            --check;
            pair<int, int> ans = pickExchange((*check) -> u, (*check) -> v);
            if (ans.first) {
                now = l.end();
            } else {
                for (--now; now != l.end(); --now) {
                    ans = pickExchange((*now) -> u, (*now) -> v);
                    if (ans.first) {
                        for (--now; now != l.end() ;--now) {
                            Edge *e = *now;
                            if (e -> u != ans.first && e -> v != ans.first) break;
                        }
                        break;
                    }
                }
            }
            if (!ans.first) {
                // cout << step << ": random" << endl;
                updateWeight();
                Iter check = l.begin();
                int r = rand() % l.size();
                rep(i, 1, r - 1) ++check;
                ans.first = (rand() & 1) ? (*check) -> u : (*check) -> v;
                do { ans.second = rand() % vNum + 1; } while (!vC[ans.second]);
                now = l.end();
            } else {
                // cout << step << ": exchange" << endl;
            }
            add(ans.first);
            remove(ans.second);
            ++step;
            tabuAdd = ans.second;
            tabuRemove = ans.first;

            if (cSize + l.size() < ub) updateAns();
        }
    }

    void Solver::printAns() {
        clock_t finish = clock();
        cout << "finish in " << step << " steps, ";
        cout << (double)(finish - start) / CLOCKS_PER_SEC << " seconds" << endl;
        int ans = 0;
        cout << "mis:";
        rep(i, 1, vNum) if (!ansVC[i]) {
            ++ans;
            cout << " " << i;
        }
        cout << endl;
        cout << "mis size: " << ans << endl;
        if (!checkAns()) cout << "not a mis!!!" << endl;
    }

    bool Solver::checkAns() {
        temp.clear();
        rep(i, 1, vNum) if (!ansVC[i]) temp.push_back(i);
        rep(i, 1, temp.size() - 1) rep(j, 1, temp.size() - 1) {
            if (vvEdge[temp[i]][temp[j]]) return false;
        }
        return true;
    }
}

EWLS::Solver s;

int main(int argc, char **argv) {
    assert(argc == 2);

    ifstream fin;
    fin.open(argv[1], ios::in);

    int seed = time(NULL);
    cout << "seed: " << seed << endl;

    srand(seed);

    s.init(fin);
    s.cover();
    s.printAns();

    return 0;
}
