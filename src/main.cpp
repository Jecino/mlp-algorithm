#include "Data.h"
#include "AuxFun.h"
#include <iostream>
#include <vector>
#include <chrono>


//Globais
int maxIter = 10;
int maxIterIls;
int R_size = 26;

using namespace std;
using namespace std::chrono;

double calcularCusto(Data& data, vector<int>& v);
Solution construcao(Data& data, vector<double>& R);
void BuscaLocal(Solution& s, vector<vector<Subsequence>> &subseq_matrix, Data& data);
Solution ILS(int maxIter, int maxIterIls, int dimension, vector<double>& R, Data& data);
Solution Pertubacao(Solution& s, Data& data, int dimension);

//Comando para executar o codigo: ./mlp instances/"nome_da_instancia".tsp
//ex: ./mlp instances/teste.tsp
int main(int argc, char* argv[]) {

    srand(time(0));

    //Comandos necessarios para leitura da instancia
    auto data = Data(argc, argv[1]);        
    data.read();
    
    //data.getDimension() retorna quantos vertices sao presentes na instancia
    size_t dimension = data.getDimension();
    cout << "Dimensao: " << dimension << endl;

    maxIterIls = min(100, (int)dimension);

    vector<double> R(R_size);
    for(int i = 0; i < R_size; i++){
        R[i] = (double)i/100;
    }

    auto start = high_resolution_clock::now();
    Solution bestOfAll = ILS(maxIter, maxIterIls, dimension, R, data);
    auto stop = high_resolution_clock::now();
 
    auto duration = duration_cast<milliseconds>(stop - start);
    
    cout << "Tempo de execução do algoritmo: " << duration.count() << "ms\n";
    cout << "\nMelhor Sequência de todas | custo: "  << calcularCusto(data, bestOfAll.sequencia) << " | bestOfAll.custo: " << bestOfAll.custo << "\n";
    printVector(bestOfAll.sequencia);

    return 0;
}

double calcularCusto(Data& data, vector<int>& v){

    double custo = 0;
    
    //maneira de iterar sobre os valores do vector a partir de seu tamanho dado pela funcao size()
    for(int i = 0; i < v.size() - 1; i++){

        custo += data.getDistance(v[i], v[i+1]);
    }

    return custo;
}

Solution ILS(int maxIter, int maxIterIls, int dimension, vector<double>& R, Data& data){
    Solution bestOfAll;
    bestOfAll.custo = INFINITY;

    for(int i = 0; i < maxIter; i++){
        Solution s = construcao(data, R);
        Solution best = s;
        
/*         cout << "\nSequencia pós contrução" << "| custo: " << calcularCusto(data, s.sequencia) << " | s.custo: " << s.custo << endl;
        printVector(s.sequencia);
        cout << endl; */

        //Matriz de subsequencias
        vector<vector<Subsequence>> sub_matrix(dimension, vector<Subsequence>(dimension));
        updateSubSequence(best, sub_matrix, data);

        //DEBUG para checar os valores da matriz de subsequencias
/*         for (int i = 0; i < dimension; i++){
            for(int j = 0; j < dimension; j++){
                cout << "i: " << i << " | j: " << j << " | c: " << sub_matrix[i][j].C << endl;
            }
        }  */

/*         cout << "\nSequencia antes da busca local | custo: " << calcularCusto(data, s.sequencia)  << " | s.custo: " << s.custo << "\n";
        printVector(s.sequencia);
        cout << "\n"; */

        int iterIls = 0;
        while(iterIls < maxIterIls){
            BuscaLocal(s, sub_matrix, data);
            if(s.custo < best.custo){
                best = s;
                iterIls = 0;
            }

            //s = Pertubacao(best, data, dimension);
            iterIls++;
        }

        if(best.custo < bestOfAll.custo){
            bestOfAll = best;
        }
 
/*         cout << "Sequência após busca local | custo: "  << calcularCusto(data, s.sequencia) << " | s.custo: " << s.custo << "\n";
        cout << "\n";
        printVector(s.sequencia); */
    }

    return bestOfAll;
}

bool compCustoInsercao(InfoInsercao &x, InfoInsercao &y){
    return x.custo < y.custo;
}

Solution construcao(Data& data, vector<double> &R){

    int randAlpha = rand() % R_size;
    double alpha = R[randAlpha];
    Solution parcial;
    parcial.sequencia = criarSubtour(data);
    vector<int> CL = verticesRestantes(data, parcial.sequencia);
    parcial.custo = calcularCusto(data, parcial.sequencia);

    while(!CL.empty()){
        vector<InfoInsercao> custoInsercao = calcularCustoInsercao(data, parcial.sequencia, CL);
        sort(custoInsercao.begin(), custoInsercao.end(), compCustoInsercao);

        int selecionado = rand() % ((int) ceil(alpha * custoInsercao.size()));
        inserirNaSolucao(parcial, custoInsercao[selecionado]);
        removeVector(CL, custoInsercao[selecionado].noInserido);
    }

    return parcial;
}

void BuscaLocal(Solution& s, vector<vector<Subsequence>> &subseq_matrix, Data& data){
    vector<int> opcao = {1,2,3,4,5};
    bool melhorou = false;

    while(!opcao.empty()){
        int n = rand() % opcao.size();

        switch(opcao[n]){
            case 1:
                melhorou = bestImprovementSwap(s, subseq_matrix, data);
                break;
            case 2:
                //melhorou = bestImprovement20pt(s, subseq_matrix, data);
                break;
            case 3:
                //melhorou = bestImprovement0r0pt(s, subseq_matrix, data, 1);
                break;
            case 4:
                //melhorou = bestImprovement0r0pt(s, subseq_matrix, data, 2);
                break;
            case 5:
                //melhorou = bestImprovement0r0pt(s, subseq_matrix, data, 3);
                break;
        }

        if(melhorou == true){

            opcao = {1,2,3,4,5};
        }
        else{

            opcao.erase(opcao.begin() + n);
        }
    }
}

Solution Pertubacao(Solution& s, Data& data, int dimension){
    Solution newS = s;

    int size_i = rand() % ((int) floor(dimension / 10)) + 2;

    int size_j = rand() % ((int) floor(dimension / 10)) + 2;

    int i = rand() % (dimension) + 1;
    //Loop para definir novo valor de i até ele não ocorrer overflow
    while((i + size_i) > dimension - 1){
        i = rand() % (dimension) + 1;
    }

    int j = rand() % (dimension) + 1;
    //Checa se o segmento j contem o segmento i, ou se o segmento i contem o segmento j e gera um j novo até não, ou quando faz um overflow
    while( (i >= j && i <= j + size_j) || (i + size_i >= j && i+size_i <= j + size_j) || (j >= i && j <= i + size_i) || (j + size_j >= i && j+size_j <= i + size_i) || j < 1 || j + size_j >= dimension){
        j = rand() % (dimension) + 1;;
    }

    int vi = newS.sequencia[i];
    int vi_prev = newS.sequencia[i-1];
    int vi_last = newS.sequencia[i + size_i - 1];
    int vi_block_next = newS.sequencia[i+size_i];

    int vj = newS.sequencia[j];
    int vj_prev = newS.sequencia[j-1];
    int vj_last = newS.sequencia[j + size_j - 1];
    int vj_block_next = newS.sequencia[j+size_j];

    double delta = -data.getDistance(vi_prev, vi) - data.getDistance(vi_last, vi_block_next) - data.getDistance(vj_prev, vj) - data.getDistance(vj_last, vj_block_next) + data.getDistance(vi_prev, vj) + data.getDistance(vj_last, vi_block_next) + data.getDistance(vj_prev, vi) + data.getDistance(vi_last, vj_block_next);

    //Caso onde I está atrás de J
    if(i < j){
        //Inserir I no J
        newS.sequencia.insert(newS.sequencia.begin() + j + size_j, newS.sequencia.begin() + i, newS.sequencia.begin() + i + size_i);

        //Inserir J no I
        newS.sequencia.insert(newS.sequencia.begin() + i + size_i, newS.sequencia.begin() + j + size_j, newS.sequencia.begin() + j + size_j * 2);

        //Apaga J
        newS.sequencia.erase(newS.sequencia.begin() + j + size_j, newS.sequencia.begin() + j + size_j * 2);
        
        //Apaga I
        newS.sequencia.erase(newS.sequencia.begin() + i, newS.sequencia.begin() + i + size_i);
    }
    //Caso onde J está atrás de I
    else{
        
        //Inserir J no I
        newS.sequencia.insert(newS.sequencia.begin() + i + size_i, newS.sequencia.begin() + j, newS.sequencia.begin() + j + size_j);

        //Inserir I no J
        newS.sequencia.insert(newS.sequencia.begin() + j + size_j, newS.sequencia.begin() + i + size_i, newS.sequencia.begin() + i + size_i * 2);
        
        //Apaga I
        newS.sequencia.erase(newS.sequencia.begin() + i + size_i, newS.sequencia.begin() + i + size_i * 2);

        //Apaga J
        newS.sequencia.erase(newS.sequencia.begin() + j, newS.sequencia.begin() + j + size_j);
    }

    newS.custo += delta;
    
    return newS;
}
