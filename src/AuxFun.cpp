#include "AuxFun.h"

bool contem(vector<int>& vector, int target){
    for(int i = 0; i < vector.size(); i++){
        if (vector[i] == target){
            return true;
        }
    }
    return false;
}

void removeVector(vector<int>& vector, int& target){
    for(int i = 0; i < vector.size(); i++){
        if(vector[i] == target){
            vector.erase(vector.begin() + i);
        }
    }
}

void printVector(vector<int>& vector){
    bool first = true;
    for(int i = 0; i < vector.size(); i++){
        if(first){
            cout << vector[i];
            first = false;
        }
        else{
            cout << " -> " << vector[i];
        }
    }
    cout << "\n";
}

vector<int> criarSubtour(Data& data){
    int dimension = data.getDimension();
    vector<int> subtour = {1,1};

    for(int i = 1; i <= 3; i++){
        int arestaQualquer = rand() % (dimension + 1); //Primeiro +1 para corrigir o range

        while(contem(subtour, arestaQualquer) || arestaQualquer == 0) {
            arestaQualquer = rand() % (dimension + 1);
        }

        subtour.insert(subtour.begin() + i, arestaQualquer);
    }

    return subtour;
}

vector<int> verticesRestantes(Data& data, vector<int>& subtour){
    vector<int> CL;

    for(int i = 1; i <= data.getDimension(); i++){
        if(!contem(subtour, i)){
            CL.push_back(i);
        }
    }

    return CL;
}

vector<InfoInsercao> calcularCustoInsercao(Data& data, vector<int>& sequencia, vector<int>& CL){
    vector<InfoInsercao> custoInsercao((sequencia.size() - 1) * CL.size());

    int l = 0;
    for(int a = 0; a < sequencia.size() - 1; a++){
        int i = sequencia[a];
        int j = sequencia[a+1];
        int iteI = a, iteN = sequencia.size() - a; 
        double custoAcumuladoI = 0, custoAcumuladoInserido = 0;

        //Calcula o custo acumulado até o i, pois este não muda com a inserção
        for(int v = 0; v < a; v++){
            custoAcumuladoI += iteI * data.getDistance(sequencia[v], sequencia[v+1]);
            iteI -= 1;
        }

        for(auto k : CL){

            for(int m = 0; m < sequencia.size(); m++){
                
                //Nas duas iterações que envolve o nó k, necessita tratamento especial por não estar inserido na sequencia
                if(m == a){
                    custoAcumuladoInserido += iteN * data.getDistance(i, k);
                }

                else if(m == a + 1){
                    custoAcumuladoInserido += iteN * data.getDistance(k, j);
                }

                //Em valores de m menores que "a" calcula o custo remanescente
                else{

                    //Necessário reduzir o iterador pois a adição de uma operação irá desloca-lo fazendo que não represente mais a sequencia correta
                    if(m > a + 1){
                        custoAcumuladoInserido += iteN * data.getDistance(sequencia[m - 1], sequencia[m]);
                    }
                    else{
                        custoAcumuladoInserido += iteN * data.getDistance(sequencia[m], sequencia[m+1]);
                    }
                }
                //Apenas reduz o iterador se estiver calculando apos da posição do I para frente
                if( m >= a ) iteN -= 1;
            }

            custoInsercao[l].custoAcumulado = custoAcumuladoI + custoAcumuladoInserido;
            custoInsercao[l].noInserido = k;
            custoInsercao[l].arestaRemovida = i;
            l++;
        }
    }

    return custoInsercao;
}

void inserirNaSolucao(Solution& s, InfoInsercao& noInserido){
    for(int i = 0; i < s.sequencia.size() - 1; i++){
        if(s.sequencia[i] == noInserido.arestaRemovida){
            s.sequencia.insert(s.sequencia.begin() + i + 1, noInserido.noInserido);
            s.custo = noInserido.custoAcumulado;
            return;
        }
    }
}


bool bestImprovementSwap(Solution& s, vector<vector<Subsequence>> &subseq_matrix, Data& data){
    double bestDelta = s.custo;
    int best_i, best_j;
    int n = s.sequencia.size() - 1;

    for(int i = 1; i < n; i++){

        for(int j = i + 1; j < n - 1; j++){
            Subsequence sigma1, sigma2;

            //cout << "i: " << i << " j: " << j << endl;

            sigma1 = Subsequence::Concatenate(subseq_matrix[0][i - 1], subseq_matrix[j][i], data);
            sigma2 = Subsequence::Concatenate(sigma1, subseq_matrix[j + 1][n - 1], data);

            //cout << "s.custo: " << s.custo << " sigma2.C: " << sigma2.C << endl;
            if(sigma2.C < bestDelta){
                bestDelta = sigma2.C;
                best_i = i;
                best_j = j;
            }
        }
    }

    if(bestDelta < s.custo){
        swap(s.sequencia[best_i], s.sequencia[best_j]);
        s.custo = bestDelta;
        updateSubSequence(s, subseq_matrix, data);
        return true;
    }

    return false;
}

bool bestImprovement20pt(Solution& s, vector<vector<Subsequence>> &subseq_matrix, Data& data){
    double bestDelta = 0;
    int best_i, best_j;

    for(int i = 1; i < s.sequencia.size() - 1; i++){
        int vi = s.sequencia[i];
        int vi_next = s.sequencia[i+1];
        int vi_prev = s.sequencia[i-1];

        for(int j = i + 2; j < s.sequencia.size() -1; j++){

            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j+1];
            int vj_prev = s.sequencia[j-1];

            double delta = - data.getDistance(vi, vi_next) - data.getDistance(vj, vj_next) + data.getDistance(vi_next, vj_next) + data.getDistance(vi, vj);  

            if(delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }
    //DEBUG: cout << bestDelta << " | " << best_i << " | " << best_j << "\n";
    if(bestDelta < 0){
        reverse(s.sequencia.begin() + best_i + 1, s.sequencia.begin() + best_j + 1);
        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

bool bestImprovement0r0pt(Solution& s, vector<vector<Subsequence>> &subseq_matrix, Data& data, int block){
    double bestDelta = 0;
    int best_i, best_j; 

        for(int i = 1; i < s.sequencia.size() - block; i++){
        int vi_block_first = s.sequencia[i];
        int vi_block_next = s.sequencia[i+block];
        int vi_block_last = s.sequencia[i+block-1];
        int vi_block_prev = s.sequencia[i-1];
        
        //cout << "\ni: " << i << " | block: " << block << " | sequencia.size: " << s.sequencia.size() << "\n";

        for(int j = 1; j < s.sequencia.size() - 1; j++){
            int vj = s.sequencia[j];
            int vj_next = s.sequencia[j+1];
            int vj_prev = s.sequencia[j-1];
 
            //cout << " | j: " << j;
            //Skip no loop caso j estiver conflitando com i
            if((j >= i && j <= i + block) || (j + 1 >= i && j + 1 <= i + block)){
                //cout << " Skip \n";
                continue;
            }

            double delta = -data.getDistance(vi_block_last, vi_block_next) -data.getDistance(vi_block_prev, vi_block_first) -data.getDistance(vj, vj_next) +data.getDistance(vj, vi_block_first) + data.getDistance(vi_block_last, vj_next) + data.getDistance(vi_block_prev, vi_block_next);

            //cout << " Calculado\n";
            if(delta < bestDelta){
                bestDelta = delta;
                best_i = i;
                best_j = j;
            }
            
        }
    }
    //DEBUG: cout << bestDelta << " | " << best_i << " | " << best_j << "\n";

    if(bestDelta < 0){
        //cout << "Sequencia antes de modificar | Vi = " << s.sequencia[best_i] << " | block: " << block << " | Vj = " << s.sequencia[best_j] << "\n";
        //printVector(s.sequencia);

        vector<int> temp(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + block);

        s.sequencia.insert(s.sequencia.begin() + best_j + 1, temp.begin(), temp.end());

        //Compensa o iterador quando ocorre inserção atrás do i escolhido
        //Pois se for inserido antes do i, o i será igual i + block
        if(best_i > best_j){
            best_i = best_i + block;
        }

        s.sequencia.erase(s.sequencia.begin() + best_i, s.sequencia.begin() + best_i + block);

        //cout << "Sequencia apos modificar: \n";
        //printVector(s.sequencia);

        s.custo = s.custo + bestDelta;
        return true;
    }

    return false;
}

void updateSubSequence(Solution &s, vector<vector<Subsequence>> &subseq_matrix, Data &data){
    int n = s.sequencia.size() - 1; // -1 pois a sequencia começa e termina no mesmo nó ( 1 -> ... -> 1 )

    //Unico nó
    for(int i = 0; i < n; i++){
        subseq_matrix[i][i].W = (i > 0);
        subseq_matrix[i][i].C = 0;
        subseq_matrix[i][i].T = 0;
        subseq_matrix[i][i].first = s.sequencia[i];
        subseq_matrix[i][i].last = s.sequencia[i];
    }

    //Concatenação normal
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j - 1], subseq_matrix[j][j], data);
        }
    }

    //Concatenação invertida (2-opt)
    for(int i = n - 1; i >= 0; i--){
        for(int j = i - 1; j >= 0; j--){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j + 1], subseq_matrix[j][j], data);
        }
    }
}