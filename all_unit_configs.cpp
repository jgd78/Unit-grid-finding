#include<iostream>
#include<list>
using namespace std;


/*class Point used to succinctly describe a point on a grid/matrix where "xcoord" represents the row of the matrix the point is on and "ycoord" represents the column */
class Point{
    public:
        int x;
        int y;
        Point(int xcoord, int ycoord){
            x=xcoord;
            y=ycoord;
        }
};

/*Simply used to make a copy of a square array "arr2" of side-length "length" into the space with memory address beginning with *arr1 that has been previously allocated*/
void CopyArray(int *arr1, int *arr2, int length){
    for(int i = 0; i<length*length; i++){
        *(arr1+i)=*(arr2+i);

    }
}

/*used to print a portion of a two dimensional array "arr" into the console. 
    "rs" is the row set to begin printing from including "rs"
    "re" is the row set to end printing from not including "re"
    "cs" is the column set to begin printing from including "cs"
    "ce" is the column set to end printing from not including "ce"
    "dim" is the dimension of the square array beginning at *arr
    for example, voidPrint2DArray(*myarr, 2, 6, 1, 8, 10) would print the submatrix of the 10x10 matrix beginning at *myarr from row 2 to row 5 and column 1 to column 8*/
void Print2DArray(int *arr, int rs, int re, int cs, int ce, int dim){
	for(int i=rs;i<re;++i){
		for(int j=cs;j<ce;++j){
			cout<<*(arr+dim*i+j)<<' ';
		}	
		cout<<endl;
	}
	cout<<endl;
}

//Unused test function when starting out with set grid size, just used for reference at this point

/*void calculate_grids_3(int *grid, int *totals, Point *dirs, int unit_length, int size){
    bool valid;
    for (int x = 1; x < size+1; x++){
        for (int y = 1; y < size+1; y++){
            for (int i = 0; i < 2; i++){
                valid = true;
                int dirx = dirs[i].x;
                int diry = dirs[i].y;
                for (int len=0; len<unit_length; len++){
                    valid = valid && !*(grid + (size+2)*(x+dirx*len) + (y+diry*len));                    
                }

                if (valid){
                    for (int len=0; len<unit_length; len++){
                        *(totals + size*(x+dirx*len-1) + (y+diry*len-1)) += 1;
                    }
                }                
            }
        }
    }    
}*/
/* calculates n!, that is, n*(n-1)*(n-2)*...*2*1*/
int factorial(int n) {
  if(n > 1)
    return n * factorial(n - 1);
  else
    return 1;
}

/* calculates the number of units in the present iteration by taking the array of units by length at "*units_by_num" and adding all the values present*/
int find_num_units(int *units_by_num, int longest){
    int num_units=0;
    for (int i=0; i<longest+1; i++){
        num_units=num_units+*(units_by_num+i);
    }
    return num_units;
}

/*calculates a divisor by which the final array should be divided by.
    As the goal is to calcualte the number of unique ways the ships can be positioned in the grid, there are many duplicate boards that are calculated.
    Namely, for every n ships of the same lenght, the recursive function will count n! times too many since there are n! ways that n units can be 
        placed in n spots, but, for our purposes, all those positions should be counted as the same. I have a different way in mind to fix this, but haven't
        quite gotten to it yet. I plan to have the iterative board only place ships of unique size in each postion when calculating total possible positioning for the units.
        It would then eliminate the need to divide by this divisor and, further, reduce the runtime as the recursive portion of the file will run fewer times.*/
int find_divisor(int *units_by_num, int longest){
    int divisor=1;
    for (int i=0; i<longest+1; i++){
        divisor=factorial(*(units_by_num+i))*divisor;
    } 
    return divisor;
}

/* the bulk of the runtime of the program, recurse units calculates every possible placement of "units" on "grid" of size "size" and returns a similar size grid with entries
    indicating in how many of those iterations a unit was found partially in the associated Point on grid. Dirs is a list of "Point" indicating which direction each ship can be pointing in, 
    either down or right. Up and left are not included as it would just produce more duplicates and take longer to run. It takes into account that the unit must be fully present on the grid, that is, for example, if the unit is in the lower-left most corner, it cannot be
    pointing downward, as it would then go off of the grid. Additionally, units cannot overlap one another.*/
void recurse_units(int *units, int *grid,  int *totals, Point *dirs, int size, int units_num, int total_units){

    if (units_num==total_units){
        for (int x = 1; x < size+1; x++){
            for (int y = 1; y < size+1; y++){
                if (*(grid + (size+2)*x + y)){
                    *(totals + size*(x-1) + (y-1)) += 1;

                }
            }
        }
        return;
    }else{
        int unit_length = *(units+units_num);
        int *temp = (int*)calloc((size+2)*(size+2), sizeof(int));
        CopyArray(temp, grid, size+2);
        bool valid;
        for (int x = 1; x < size+1; x++){
            for (int y = 1; y < size+1; y++){
                for (int i = 0; i < 2; i++){
                    valid = true;
                    int dirx = dirs[i].x;
                    int diry = dirs[i].y;
                    
                    for (int len=0; len<unit_length; len++){
                        valid = valid && !*(grid + (size+2)*(x+dirx*len) + (y+diry*len));                    
                    }
                    if (valid){
                        for (int len=0; len<unit_length; len++){
                            *(grid + (size+2)*(x+dirx*len) + (y+diry*len))=1;
                        }

                        recurse_units(units, grid, totals, dirs, size, units_num+1, total_units);
                        

                    }
                    CopyArray(grid, temp, size+2);   
                }
            }
        }
        free(temp);
        return;
    }    
}
/*main has many user-entered values and can be cleaned up a bit, but as this is in early stages, i left it how it was for the time being. The first
    four variables are to be entered manually where they represent the size of the grid, the number of units being placed of size 0, size 1, size 2 etc., 
    an array of the units by size, and the length of the largest unit.
    The rest of the function calls functions in the order they are to be used and allocates space for different necessary values, arrays, and matrices.
    Finally, the array of values "totals" is printed out. This array indicates how many units, out of all the iterations, partially covered that point.*/

int main() {

    int size = 5;
    int units_by_size[]={0,0,2,1};
    int all_units[]={2,2,3};
    int longest=3;

    int *unit_sizes=units_by_size;
    int num_units = find_num_units(unit_sizes, longest);
    int divisor=find_divisor(unit_sizes, longest);
    int *units=all_units;   
    int *totals = (int*)calloc((size)*(size), sizeof(int));
    int *grid = (int*)calloc((size+2)*(size+2), sizeof(int));
    int *active_grid = (int*)calloc((size+2)*(size+2), sizeof(int));

    Point *dirs = (Point*)malloc(2*sizeof(Point));
    *dirs=Point(0,1);
    *(dirs+1)=Point(1,0);

    for (int i=0; i<size+2; i++){
        for (int j=0; j<size+2; j++){
            if(!i||!j||i==size+1||j==size+1){
                *(grid+(size+2)*i+j)=1;
            }
        }
    }
    
    /*Print2DArray(grid, 0, size+2,0, size+2, size+2);*/
    
    recurse_units(units, grid, totals, dirs, size, 0, num_units);
    for (int i=0; i<size; i++){
        for (int j=0; j<size; j++){
            *(totals+i*size+j)=*(totals+i*size+j)/divisor;
            
        }
    }
    Print2DArray(totals, 0, size, 0, size, size);
    free(totals);
    free(grid);
    free(dirs);
    free(units);

    return 0;
    
}


/*FINAL COMMENTS
    I am aware the program is very messy and fairly inefficient at the moment, however i plan to fix this when i have more time available to me.
    I plan to find a faster way of calculating "totals" because even though the program works as intended, it is very slow as the grid and number of units
    gets larger. I will probably move alot of the values into vectors rather than memeory allocated arrays and find more intuitive ways of checking if units 
    overlap with the border or one another, probably by implementing large scale boolean comparisons of the vectors or arrays. */