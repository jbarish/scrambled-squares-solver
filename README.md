# scrambled-squares-solver
A c program to quickly (under 5 ms) solve a 3x3 scrambled squares problem  

Sample Scrambled Squares Problem:  

+--------+--------+--------+  
|3  R1   |9  Y1   |1  Y1   |  
|Y1    G0|G1    B0|B1    G0|  
|   B0   |   R0   |   R0   |  
+--------+--------+--------+  
|8  B1   |6  R1   |2  R1   |  
|R1    G0|G1    B0|B1    B0|  
|   Y0   |   Y0   |   Y0   |  
+--------+--------+--------+  
|5  Y1   |7  Y1   |4  Y1   |  
|R1    B0|B1    G0|G1    G0|  
|   G0   |   R0   |   B0   |  
+--------+--------+--------+  
Note that every tiles edges match with adjacent tiles. That is, a B always touches a B, a Y always touches a Y, (likewise for R and G).
Also, a 1 always touches a 0, and vice-verse.  

The problem becomes solving this puzzle, given the squares in a scrambled, random order.   
There are 95,126,814,720 possible orientations of tiles.   

Using a DFS-based recursive algorithm, the problem can be solved very efficiently.  

Tp use, make the program, and run with the 1st command argument being the input txt file of encoded tiles.  
