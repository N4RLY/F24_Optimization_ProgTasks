import numpy as np

# Initial data
# Coefficients of the objective function
print("Enter the coefficients of the objective function: ", end="")
C = np.array(list(map(int, input().split()))) 

# Number of constraints
print("Enter the number of constraints: ", end="")
n = int(input()) 

A = np.zeros((n, C.size))
print("Enter the coefficients of the constraints: ")
# Constraints
for i in range(n):
    A[i]=(list(map(int, input().split())))
    
# Right-hand side of the constraints
print("Enter the right-hand side of the constraints: ", end="")
b = np.array(list(map(int, input().split()))) 

# Creating the simplex method table
nvars = C.size 
nconstraints = b.size 

# Variables and table
varias = np.zeros(nvars)
id = np.identity(nconstraints)  
table = np.hstack((A, id, b.reshape(-1, 1)))

# Objective function
upline = np.hstack((-C, np.zeros(nconstraints + 1)))
table = np.vstack((upline, table))

# Variable for approximation accuracy
print("Enter the accuracy of the solution: ", end="")
approx_accuracy = float(input())  # accuracy (e.g., 0.000001)

# Simplex method
while np.min(table[0, :-1]) < -approx_accuracy:  
    mincol = np.argmin(table[0, :-1])  # index of the minimum element (pivot column)

    # Calculating the ratio b / A to choose the pivot row
    ratios = table[1:, -1] / table[1:, mincol]
    # Select only positive ratios
    positive_ratios = np.where(ratios > 0, ratios, np.inf)  

    # Check if there are positive ratios
    if np.all(positive_ratios == np.inf):
        print("The method is not applicable")
        break

    # Index of the minimum positive ratio
    minrow = np.argmin(positive_ratios) + 1  

    # Normalizing the pivot row
    table[minrow] /= table[minrow, mincol]

    # Updating all other rows
    for i in range(table.shape[0]):
        if i != minrow:
            table[i] -= table[i, mincol] * table[minrow]

# Updating basic variables
for i in range(nvars):
    for j in range(n+1):
        if table[j, i] == 1:
            flag = 1
            for k in range(n+1):
                if k != j:
                    if table[k, i] != 0:
                        flag = 0
                        break
            if flag:
                varias[i] = table[j, -1]

# Check for completion and output the result
if np.min(table[0, :-1]) >= -approx_accuracy:
    print("Final simplex table:")
    print(table)
    print("Optimal value of the objective function:", table[0, -1])
    print("Values of the variables:", varias[:nvars])
else:
    print("The method is not applicable")