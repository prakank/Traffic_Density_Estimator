#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include<cmath>
#include<boost/algorithm/string.hpp>
#include<boost/format.hpp> 

using namespace std;
#define d0(x) cout<<(x)<<" "
#define d1(x) cout<<(x)<<endl
#define d2(x,y) cout<<(x)<<" "<<(y)<<endl
#define d3(x,y,z) cout<<(x)<<" "<<(y)<<" "<<(z)<<endl
#define d4(a,b,c,d) cout<<(a)<<" "<<(b)<<" "<<(c)<<" "<<(d)<<endl
#define d5(a,b,c,d,e) cout<<(a)<<" "<<(b)<<" "<<(c)<<" "<<(d)<<" "<<(e)<<endl
#define d6(a,b,c,d,e,f) cout<<(a)<<" "<<(b)<<" "<<(c)<<" "<<(d)<<" "<<(e)<<" "<<(f)<<endl

vector<string> files = {"Density_Values_M3_1_86.csv", "Density_Values_M3_2_78.csv", "Density_Values_M3_3_111.csv", "Density_Values_M3_4_139.csv", "Density_Values_M3_5_170.csv", "Density_Values_M3_6_190.csv", "Density_Values_M3_7_197.csv", "Density_Values_M3_8_235.csv", "Density_Values_M4_1_87.csv", "Density_Values_M4_2_72.csv", "Density_Values_M4_3_89.csv","Density_Values_M4_4_93.csv","Density_Values_M4_5_136.csv","Density_Values_M4_6_145.csv","Density_Values_M4_7_171.csv","Density_Values_M4_8_230.csv"};
vector<float> queue;
vector<float> dynamic;
bool M3 = true;
int threads;
struct Comb{
    int Frame,Part,Sum,Area;
};
// struct den{
//     int Frame, density;
// };

bool Comparator(Comb a, Comb b){
    if(a.Frame < b.Frame) return true;
    if(a.Frame > b.Frame) return false;
    if(a.Part < b.Part) return true;
    return false;
}

int main(){
    
    string out;
    ifstream read("Density_Values_Baseline_Queue.csv");
    int t = 0;
    while (getline (read, out)) {
        if(t==0){t++;continue;}
        vector<string> temp;
        boost::split(temp,out,boost::is_any_of(","));
        queue.push_back(stof(temp[1]));
    }
    read.close();
    
    // ifstream read("Density_Values_Baseline_Dynamic.csv");
    // int t = 0;
    // while (getline (read, out)) {
    //     if(t==0){t++;continue;}
    //     vector<string> temp;
    //     boost::split(temp,out,boost::is_any_of(","));
    //     dynamic.push_back(stof(temp[1]));
    // }
    // read.close();


    string filename = "Error_Utility.csv";
    ofstream outputFile(filename);
    outputFile << "Method, Threads, Runtime, Error, Utility\n";
    
    
    for(int j=0;j<files.size();j++){
        vector<Comb> v;
        vector<float> fin;
        vector<string> v_;
        boost::split(v_,files[j],boost::is_any_of("_"));
        v_[2] == "M3" ? M3 = true : M3 = false;
        threads = stoi(v_[3]);
        int runtime = stoi(v_[4]);
        if(M3){
            vector<string> temp;
            ifstream MyReadFile(files[j]);
            int t = 0;
            while (getline (MyReadFile, out)) {
                if(t==0){t++;continue;}
                boost::split(temp,out,boost::is_any_of(","));
                Comb cd;
                // d1(out);
                cd.Frame = stoi(temp[1]);
                cd.Part = stoi(temp[2]);
                cd.Sum = stoi(temp[3]);
                cd.Area = stoi(temp[4]);
                v.push_back(cd);
            }
            sort(v.begin(),v.end(), Comparator);
            for(int i=0;i<v.size()/threads;i++){
                int sum=0;
                int area=0;
                for(int j=0;j<threads;j++){
                    int in = i*threads + j;
                    sum+=v[in].Sum;
                    area+=v[in].Area;
                }
                float dens = float((float)sum/(float)area);
                fin.push_back(dens);
            }
            MyReadFile.close();
        }
        else{
            vector<string> temp;
            ifstream MyReadFile(files[j]);
            int t=0;
            // d2(threads, M3==true);
            while (getline (MyReadFile, out)) {
                if(t==0){t++;continue;}
                // d1(out);
                boost::split(temp,out,boost::is_any_of(","));
                fin.push_back(stof(temp[2]));
                // cout << fin[fin.size()-1]  << "\b";
            }
            MyReadFile.close();
        }
        float err=0;
        float utility=0;
        for(int i=0;i<fin.size();i++){
            float val = (fin[i] - queue[i])*(fin[i] - queue[i]);
            err+=val;
        }
        err/=fin.size();
        err = sqrt(err);
        utility = log2(1/err);
        if(M3)outputFile << "3, ";
        else outputFile << "4, ";
        outputFile << threads << ", " << runtime << ", " << err << ", " << utility << "\n";
        // cout << "File:" << files[j].substr(15,files[j].size()-15) << ", Error:" << err << ", Utility:" << utility <<  "\n";
    }
    outputFile.close();

    return 0;
}