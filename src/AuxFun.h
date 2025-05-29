#ifndef AUXFUN_H
#define AUXFUN_H

#include "Data.h"
#include <vector>
#include <algorithm>

using namespace std;

struct Subsequence{
    double T, C;
    int W;
    int first, last;
    
    inline static Subsequence Concatenate (Subsequence &sigma_1, Subsequence &sigma_2, Data &data){
        Subsequence sigma;
        
        //Tempo entre ultimo vertice da subseq1 e primeiro da subseq2
        double temp = data.getDistance(sigma_1.last, sigma_2.first);

        //Custo de atraso
        sigma.W = sigma_1.W + sigma_2.W;

        //Custo Atrelado
        sigma.C = sigma_1.C + sigma_2.W * (sigma_1.T + temp) + sigma_2.C;

        //Duração
        sigma.T = sigma_1.T + temp + sigma_2.T;

        sigma.first = sigma_1.first;
        sigma.last = sigma_2.last;

/*          cout << "|| Concatenação ||" << endl;
        cout << "| sigma1 | w: " << sigma_1.W << " C: " << sigma_1.C << " T: " << sigma_1.T << endl;
        cout << "| sigma2 | w: " << sigma_2.W << " C: " << sigma_2.C << " T: " << sigma_2.T << endl;
        cout << "| sigma | w: " << sigma.W << " C: " << sigma.C << " T: " << sigma.T << endl; */
        return sigma;
    }
};

struct Solution{
    vector<int> sequencia;
    double custo;
};

struct InfoInsercao{
    int noInserido;
    int arestaRemovida;
    double custoAcumulado;
};

//Genericas
vector<int> criarSubtour(Data& data);
void printVector(vector<int>& vector);
bool contem(vector<int>& vector, int target);
void updateSubSequence(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &data);

//Construcao
vector<int> verticesRestantes(Data& data, vector<int>& subtour);
vector<InfoInsercao> calcularCustoInsercao(Data& data, vector<int>& sequencia, vector<int>& CL);
void ordenarCrescente(vector<InfoInsercao>& vector);
void inserirNaSolucao(Solution& s, InfoInsercao& noInserido);
void removeVector(vector<int>& vector, int& target);

//BuscaLocal
bool bestImprovementSwap(Solution& s, vector<vector<Subsequence>> &subseq_matrix, Data& data);
bool bestImprovement20pt(Solution& s, vector<vector<Subsequence>> &subseq_matrix, Data& data);
bool bestImprovement0r0pt(Solution& s, vector<vector<Subsequence>> &subseq_matrix, Data& data, int block);

#endif