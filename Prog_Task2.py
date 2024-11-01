import numpy as np

def generate_initial_point(A: np.ndarray, b: np.ndarray) -> np.ndarray:
    n_vars = A.shape[1]
    
    # let first value be 1
    x_0 = np.ones(n_vars) 

    # choose slacks value
    while True:
        s_0 = b - A @ x_0
        if np.all(s_0 > 0) and np.all(x_0 > 0):
            break
    
    # return x_0 and s_0 as general initial solution
    return np.hstack((x_0, s_0))

def solve_by_interior_point(c: np.ndarray, A: np.ndarray, b: np.ndarray, alpha: float, eps: float, x_0: np.ndarray):
    # Add slack variables to the constraint matrix
    global optimal
    st_c = c
    slack_vars = np.eye(A.shape[0])
    A = np.hstack((A, slack_vars))
    c = np.hstack((c, np.zeros(slack_vars.shape[1])))

    # Check initial point feasibility for constraints
    if np.max(np.matmul(A, x_0) - b) > 0:
        print("The initial point does not satisfy the constraints.")
        raise ValueError("Initial point is not feasible.")

    x = x_0
    obj_max = -10
    solved = False
    x_max = []
    #general algorithm
    while not solved:
        D = np.diag(x)
        A_ = A @ D
        c_ = D @ c
        P = np.identity(len(c)) - A_.T @ np.linalg.inv(A_ @ A_.T) @ A_
            
        c_p = P @ c_
        
        v = -min([i for i in c_p if i < 0], default=0)
        x_ = np.ones(len(c)) + (alpha / v) * c_p
        x__ = D @ x_
        
        if -np.min(x__ - x) <= eps :
            solved = True
            break
        x = x__
        st_x = x[:len(st_c)]
        #in one test coef aplha jump over oprimal and then the solution is not on constraints
        # so we need be sure that our solution on border of optimal
        if (obj_max < sum(st_x*st_c) ) and obj_max <= optimal + eps:
            obj_max = sum(st_x*st_c)
            x_max = st_x
    # return max values that we have in iterations
    return (obj_max, x_max)

def solve_by_simplex(C:np.ndarray, A:np.ndarray, b:np.ndarray, approx_accuracy:float):
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

    # Simplex method
    while np.min(table[0, :-1]) < -approx_accuracy:  
        mincol = np.argmin(table[0, :-1])  # index of the minimum element (pivot column)

        # Calculating the ratio b / A to choose the pivot row
        ratios = table[1:, -1] / table[1:, mincol]
        # Select only positive ratios
        positive_ratios = np.where(ratios > 0, ratios, np.inf)  

        # Check if there are positive ratios
        if np.all(positive_ratios == np.inf):
            return "The method is not applicable"

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
        return varias[:nvars]
    
    return "The method is not applicable"
# For testing the function
if __name__ == "__main__":
    # Objective function coefficients
    print("Enter the coefficients of the objective function (space-separated): ", end="")
    C = np.array(list(map(float, input().split())))

    # Number of constraints
    print("Enter the number of constraints: ", end="")
    n = int(input())

    # Constraint coefficients
    A = np.zeros((n, C.size))
    print("Enter the coefficients of each constraint (space-separated): ")
    for i in range(n):
        A[i] = list(map(float, input().split()))

    # Right-hand side of constraints
    print("Enter the right-hand side of the constraints (space-separated): ", end="")
    b = np.array(list(map(float, input().split())))

    # Accuracy of solution
    print("Enter the accuracy of the solution: ", end="")
    eps = float(input())

    # Generate an initial feasible point
    x_0 = generate_initial_point(A, b)

    # Solve the problem
    print("Solution by simplex ")
    coef = solve_by_simplex(C, A, b, eps)
    
    print(f"Value : {sum(C * coef)} \nObjective coefficient : {coef}")
    
    print("Solution with alpha = 0.5:")
    optimal = 10000000
    ans = solve_by_interior_point(C, A, b, 0.5, eps, x_0)
    optimal = ans[0]
    print(f"Value : {ans[0]} \nObjective coefficient: {ans[1]}")
    
    print("Solution with alpha = 0.9:")
    ans = solve_by_interior_point(C, A, b, 0.9, eps, x_0)
    print(f"Value : {ans[0]} \nObjective coefficient: {ans[1]}")
    
    