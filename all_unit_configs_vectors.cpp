#include<iostream>
#include<list>
#include<vector>
#include<algorithm>
using namespace std;


/*used to print a portion of a two dimensional array "arr" into the console. It is a helper function, mostly used for debugging right now
    "rs" is the row set to begin printing from "rs"
    "re" is the row set to end printing at "re"
    "cs" is the column set to begin printing from  "cs"
    "ce" is the column set to end printing at "ce"
    "dim" is the dimension of the square array beginning at *arr
    for example, voidPrint2DArray(*myarr, 2, 6, 1, 8, 10) would print the submatrix of the 10x10 matrix beginning at *myarr from row 2 to row 5 and column 1 to column 8*/
void Print2DArray(vector<vector<int>> mat, int rs, int re, int cs, int ce){
	for (int i = rs; i < re; i++) {
        for (auto it = mat[i].begin()+cs; it != mat[i].end()+ce-mat[i].size(); it++){
            cout << *it << " "<<flush;
        }
        cout << endl;
    }
}

/* calculates n!, that is, n*(n-1)*(n-2)*...*2*1
*/
int factorial(int n) {
  if(n > 1)
    return n * factorial(n - 1);
  else
    return 1;
}


/*calculates a divisor by which the final array should be divided by.
    As the goal is to calcualte the number of unique ways the ships can be positioned in the grid, there are many duplicate boards that are calculated.
    Namely, for every n ships of the same lenght, the recursive function will count n! times too many since there are n! ways that n units can be 
        placed in n spots, but, for our purposes, all those positions should be counted as the same. I have a different way in mind to fix this, but haven't
        quite gotten to it yet. I plan to have the iterative board only place ships of unique size in each postion when calculating total possible positioning for the units.
        It would then eliminate the need to divide by this divisor and, further, reduce the runtime significantly. Namely, by a factor of "divisor"*/
int find_divisor(vector<int> units_by_num){
    int divisor=1;
    for (int num_unit: units_by_num){
        divisor=factorial(num_unit)*divisor;
    } 
    return divisor;
}
/*Used as a helper function to retrieve the "nth" from a vector matrix "mat1" and place it in vector format*/
vector<int> get_column(vector<vector<int>> mat1, int n){
    vector<int> ret_vec = {};
    for(int i=0; i<mat1.size(); i++){
        ret_vec.push_back(mat1[i][n]);
    }
    return ret_vec;
}

/*Helper function used to add vectors element-wise*/
vector<int> add_vec_elts(vector<int> vec1, vector<int> vec2){
    transform(vec1.begin(), vec1.end(), vec2.begin(), vec1.begin(), plus<int>());
    return vec1;
}

/*Helper function used to add matrices element-wise*/
vector<vector<int>> add_matrices_elts(vector<vector<int>> mat1, vector<vector<int>> mat2){
    for(int i = 0; i < mat1.size(); i++){
        mat1[i]=add_vec_elts(mat1[i], mat2[i]);
    }
    return mat1;
}

/*Helper function used to divide every element of a matrix by a scalar "divisor"*/
vector<vector<int>> divide_matrix(vector<vector<int>> matrix, int divisor){
    for(int i = 0; i < matrix.size(); i++){
        transform(matrix[i].begin(), matrix[i].end(), matrix[i].begin(), [divisor](int &c){return c/divisor;});
    }
    return matrix;
}

/*Helper function used to return a sub-matrix of the matrix "mat" with rows going from rs(inclusive) to re(exclusive) and columns going from cs(inclusive) to ce(exclusive)*/
vector<vector<int>> splice_mat(vector<vector<int>> mat, int rs, int re, int cs, int ce){
    vector<vector<int>> submatrix;
    for (int i=rs; i < re; i++) {
        vector<int> v(mat[i].begin()+cs, next(mat[i].begin(),ce));
        submatrix.push_back(v);
    }
    return submatrix;
}



/* "recurse_units" is the bulk of the project. It is a recursive function used to calculate a matrix of values, where the value at (x,y) indicates how many grids,
out of every possible permutation of ships, have a unit partially covering the point (x,y). */
vector<vector<int>> recurse_units(vector<int> units, vector<vector<int>> grid){

    
    vector<vector<int>> totals(grid.size()-2, vector<int>(grid.size()-2,0));
    if (units.size()==0){
        return splice_mat(grid, 1, grid.size()-1, 1, grid[0].size()-1);
    }else{
        int unit_length = units[0];
        units.erase(units.begin());
        vector<vector<int>> temp_grid = grid;
        for (int x = 0; x < grid.size()-2; x++){
            for (int y = 0; y < grid.size()-2; y++){
                for (int i = 0; i < 2; i++){
                    if(i){
                        vector<int> unit_col=get_column(grid, x+1);

                        vector<int> grid_place(unit_col.begin()+y+1, next(unit_col.begin(),y+1+unit_length));
                        if(all_of(grid_place.begin(), grid_place.end(), [](int j) { return j==0; })){
                            
                            for (int k = 0; k<unit_length; k++){
                                grid[y+k+1][x+1]=1;
                            }

                            totals=add_matrices_elts(totals, recurse_units(units,grid));
                        }
                    }else{

                        vector<int> unit_row=grid[y+1];
                        vector<int> grid_place(unit_row.begin()+x+1, next(unit_row.begin(),x+1+unit_length));
                        if(all_of(grid_place.begin(), grid_place.end(), [](int j) { return j==0; })){
                            for (int k = 0; k<unit_length; k++){
                                grid[y+1][x+k+1]=1;
                            }
                            totals=add_matrices_elts(totals, recurse_units(units,grid));
                        }
                    }
                    grid=temp_grid;   
                }
            }
        }
        return totals;
    }    
}
/*main has many user-entered values and can be cleaned up a bit, but as this is in early stages, i left it how it was for the time being. The first
    four variables are to be entered manually where they represent the size of the grid, the number of units being placed of size 0, size 1, size 2 etc., 
    an array of the units by size, and the length of the largest unit.
    The rest of the function calls functions in the order they are to be used and allocates space for different necessary values, arrays, and matrices.
    Finally, the array of values "totals" is printed out. This array indicates how many units, out of all the iterations, partially covered that point.*/

int main() {

    int size = 4;
    vector<int> units_by_size={0,0,2,1};
    vector<int> all_units={};
    int num_units=0;
    for(int i=0; i<units_by_size.size(); i++){
        for (int j=0; j<units_by_size.at(i); j++ ){
            all_units.push_back(i);
            num_units+=1;
        }
    }

    vector<vector<int>> grid(size+2, vector<int>(size+2,0));
    vector<vector<int>> active_grid(size+2, vector<int>(size+2,0));


    for (int i=0; i<size+2; i++){
        for (int j=0; j<size+2; j++){
            if(!i||!j||i==size+1||j==size+1){
                grid[i][j]=1;
            }
        }
    }
    
    vector<vector<int>> totals=recurse_units(all_units, grid);
    totals=divide_matrix(totals, find_divisor(units_by_size));
    Print2DArray(totals, 0, size, 0, size);


    return 0;
    
}


/*FINAL COMMENTS
    I am aware the program is very messy and fairly inefficient at the moment, however i plan to fix this when i have more time available to me.
    I plan to find a faster way of calculating "totals" because even though the program works as intended, it is very slow as the grid and number of units
    gets larger. I will probably move alot of the values into vectors rather than memeory allocated arrays and find more intuitive ways of checking if units 
    overlap with the border or one another, probably by implementing large scale boolean comparisons of the vectors or arrays. */