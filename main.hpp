#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>

namespace EWLS {
    const int maxV = 2000;
    const int maxE = 500000;

    struct Edge;

    typedef std::list<Edge *>::iterator Iter;

    struct Edge {
        int u, v, w, cnt;
        Iter eL;
    };

    const int maxStep = 40000000;
    const int delta = 2;

    class Solver {
        int vNum, eNum;

        Edge edge[maxE];

        int dScore[maxV];
        std::pair<int, int> maxDScore;

        std::vector<std::pair<int, Edge *> > vEdge[maxV];
        Edge *vvEdge[maxV][maxV];

        int cSize;
        bool vC[maxV], ansVC[maxV];

        std::list<Edge *> l, temp2;

        int tabuAdd, tabuRemove;

        std::vector<int> temp;
        std::vector<std::pair<int, int> > temp3;

        int ub;

        clock_t start;
        int step;

        void readEdge(std::istream &);
        void initSolution(void);
        void updateDScore(int);
        void updateDScore(void);
        void updateAns(void);
        void coverRest(void);
        void add(int);
        void remove(int);
        void pickRemove(int);
        std::pair<int, int> pickExchange(int, int);
        void updateWeight(void);
        void updateSize(void);
        void printBest(void);
        bool checkAns(void);
    public:
        void init(std::istream &);
        void cover(void);
        void printAns(void);
    };
}
