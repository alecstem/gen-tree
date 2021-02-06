#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>
#include <math.h>

using namespace std;

int XSIZE = 600;
int YSIZE = 150;
float SCALE = 0.3;

float treeLength = 2;
int initialStringSize = 25;

int n = 20;
int ITERATIONS = 3;
int MUTATION_RATE = 10;

string dna = "F---F-+++++-+[F[[[[[[]]]]]]]";
string mutatedDna = "F-+";

HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD dwWritten;
HWND myconsole = GetConsoleWindow();
HDC dc = GetDC(myconsole);

class Tree
{
public:
    float x;
    float y;
    float angle;
    float startingAngle;
    float height;
    float left;
    float right;

    Tree()
    {
        x = XSIZE/2;
        y = YSIZE;
        angle = 3.14159265358979323846;
        startingAngle = angle;
        height = 0;
        left = 0;
        right = 0;
    }

};

void DDA(int X0, int X1, int Y0, int Y1, char c)
{
    /*
        Implementation of DDA c/o GeeksForGeeks
        https://www.geeksforgeeks.org/dda-line-generation-algorithm-computer-graphics/
    */

    int dx = X1 - X0;
    int dy = Y1 - Y0;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float Xinc = dx/(float) steps;
    float Yinc = dy/(float) steps;
    float X = X0;
    float Y = Y0;
    for (int i = 0; i <= steps; i++)
    {
        FillConsoleOutputCharacter(hOutput, c, 1, {X*SCALE,Y*SCALE}, &dwWritten);
        X += Xinc;
        Y += Yinc;
    }
}

void drawTree(string lsystem, Tree *tree, float &maxLength, float &maxY, float deltaTree, bool draw)
{
    vector<vector<float>> pos(500);
    int depth = 0;
    int lastY = 0;

    bool f = false;
    char c;

    if (tree->y > YSIZE) { return; }

    for (int i = 0; i < lsystem.length(); i++)
    {
        if (tree->angle == tree->startingAngle && depth < 4) { c = '|'; } else { c = '%'; }
        if (tree->y <= 0) { f = true; break; }
        if (tree->y > 150.0f) { break; }

        maxY = min(maxY, tree->y);

        if (lsystem[i]=='F' && tree->y <= YSIZE)
        {
            if (draw) { DDA(tree->x, tree->x+(treeLength*sin(tree->angle)), tree->y, tree->y+(treeLength*cos(tree->angle)), c); }
            tree->x = tree->x+(treeLength*sin(tree->angle));
            tree->y = tree->y+(treeLength*cos(tree->angle));
            if (tree->x < XSIZE/2)
            {
                tree->left++;
            } else if (tree->x > XSIZE/2)
            {
                tree->right++;
            }
        }
        if (lsystem[i]=='f' && tree->y <= YSIZE)
        {
            tree->x = tree->x+(treeLength*sin(tree->angle));
            tree->y = tree->y+(treeLength*cos(tree->angle));
        }
        if (lsystem[i]=='-') { tree->angle+=deltaTree; }
        if (lsystem[i]=='+') { tree->angle-=deltaTree; }
        if (lsystem[i]=='[') { pos[depth] = {tree->x, tree->y, tree->angle};depth+=1; }
        if (lsystem[i]==']')
        {
            depth-=1;
            tree->x = pos[depth][0];
            tree->y = pos[depth][1];
            tree->angle = pos[depth][2];
        }
    }
    if (f) { maxLength = YSIZE; return; }
    maxLength = (YSIZE+1) - maxY;
}

vector<pair<string, float>> genpop(int n)
{
    vector<pair<string,float>> initialPop(n);
    int check[initialStringSize] = { };
    string tmp = "";
    float stringDelta = 0.00f;

    for (int i = 0; i < n; i++)
    {
        while (tmp.length() < initialStringSize)
        {
            int randChar = rand()%dna.length();
            if (dna[randChar] == '[' && tmp.length() != initialStringSize-1)
            {
                int bracketLength = rand()%abs(int(tmp.length())-initialStringSize)-1;
                tmp+=dna[randChar];
                for (int k = 0; k < bracketLength; k++)
                {
                    string modifiedDna = "Ff+-";
                    tmp+=modifiedDna[rand()%modifiedDna.length()];
                }
                tmp+=']';
            } else if (dna[randChar] != ']' && dna[randChar] != '[')
            {
                tmp+=dna[randChar];
            }
        }
        stringDelta = 0.1f + static_cast<float>(rand())/( static_cast<float>(RAND_MAX/(3.0f-0.1f)));
        initialPop[i] = make_pair(tmp, stringDelta);
        tmp = "";
    }
    return initialPop;
}

void gotoxy( int x, int y )
{
    COORD p = { x, y };
    SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), p );
}

pair<string,string> selection(vector<pair<string, float>> initialPop, float &bestDelta)
{
    vector<pair<float,int>> heightValues(n);
    vector<pair<float,int>> symmetricFitness(n);

    float maxLength = 0.00f;
    float maxY = 10000000000.0f;
    float symmetry = 0;

    for (int h = 0; h < n; h++)
    {
        maxLength = 0.00f;
        maxY = 10000000000.0f;

        string lsystem = initialPop[h].first;
        float deltaTree = initialPop[h].second;
        string initsystem = lsystem;
        Tree *tree = new Tree();

        for (int i = 0; i < ITERATIONS; i++)
        {
            string tmp;
            for (int j = 0; j < lsystem.length(); j++)
            {
                if (lsystem[j]=='F')
                {
                    tmp+=initsystem;
                } else
                {
                    tmp+=lsystem[j];
                }
            }
            lsystem = tmp;
            drawTree(lsystem, tree, maxLength, maxY, deltaTree, false);
        }
        heightValues[h] = make_pair(round(maxLength),h);
        if (abs((tree->left-tree->right))/(tree->left+tree->right) > 0)
        {
            symmetry = abs((tree->left-tree->right))/(tree->left+tree->right);
        } else
        {
            symmetry = 100000.0f;
        }
        symmetricFitness[h] = make_pair(symmetry, h);

        delete tree;

    }
    sort(symmetricFitness.begin(), symmetricFitness.end());
    sort(heightValues.begin(), heightValues.end());
    cout << "The best height is " << heightValues[heightValues.size()-1].first
    << " at index " << heightValues[heightValues.size()-1].second << endl;
    cout << "The best symmetry is " << symmetricFitness[0].first
    << " at index " << symmetricFitness[0].second << endl;

    Tree *tree = new Tree();

    maxLength = 0.00f;
    maxY = 10000000000.0f;

    string lsystem = initialPop[symmetricFitness[0].second].first;
    float deltaTree = initialPop[symmetricFitness[0].second].second;
    string initsystem = lsystem;

    for (int i = 0; i < ITERATIONS; i++)
    {
        string tmp;
        for (int j = 0; j < lsystem.length(); j++)
        {
            if (lsystem[j]=='F') { tmp+=initsystem; }
            else { tmp+=lsystem[j]; }
        }
        lsystem = tmp;
        drawTree(lsystem, tree, maxLength, maxY, deltaTree, true);
    }
    bestDelta = deltaTree;
    return make_pair(initialPop[heightValues[heightValues.size()-1].second].first, initialPop[symmetricFitness[0].second].first);
}

vector<pair<string, float>> updatePop(string heightParent, string symmetryParent, float &bestDelta)
{
    vector<pair<string, float>> newPop(n);

    string tmp = "";

    for (int i = 0; i < n; i++)
    {
        tmp = symmetryParent;
        for (int j = 0; j < initialStringSize; j++)
        {
            int x = rand() % 101;
            int y = rand() % 101;
            if (tmp[j] != '[' && tmp[j] != ']' && heightParent[j] != '[' && heightParent[j] != ']' && x >= 50)
            {
                tmp[j] = heightParent[j];
            }
            if (y < MUTATION_RATE && tmp[j] != ']' && tmp[j] != '[') { tmp[j] = mutatedDna[rand()%mutatedDna.length()]; }
        }
        newPop[i].first = tmp;
        newPop[i].second = bestDelta;
        tmp = "";
    }
    return newPop;
}

int main()
{
    srand(time(NULL));
    
    vector<pair<string, float>> initialPop = genpop(n);
    float bestDelta = 0;

    while (true)
    {
        pair<string, string> t = selection(initialPop, bestDelta);
        gotoxy(0,2);
        for(int i = 0; i < 5; i++)
        {
            cout << initialPop[i].first << " " << initialPop[i].second << endl;
        }
        initialPop = updatePop(t.first, t.second, bestDelta);
        Sleep(100);
        FillConsoleOutputCharacter(hOutput, ' ', 1000000, {0,0}, &dwWritten); //clears the console window

    }
}
