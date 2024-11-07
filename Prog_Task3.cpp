#include <iostream>
#include <vector>
#include <numeric>
#include <iomanip>
#include <algorithm>

using namespace std;

const int INF = numeric_limits<int>::max();

pair<vector<int>, vector<int>> findDiff(vector<vector<int>>& grid) {
    vector<int> rowDiff;
    vector<int> colDiff;

    for (int i = 0; i < grid.size(); i++) {
        vector<int> arr = grid[i];
        sort(arr.begin(), arr.end());
        rowDiff.push_back(arr[1] - arr[0]);
    }

    for (int col = 0; col < grid[0].size(); col++) {
        vector<int> arr;
        for (int i = 0; i < grid.size(); i++) {
            arr.push_back(grid[i][col]);
        }
        sort(arr.begin(), arr.end());
        colDiff.push_back(arr[1] - arr[0]);
    }

    return make_pair(rowDiff, colDiff);
}

void Vogels(vector<int> supply, vector<int> demand, vector<vector<int>> grid) {
    int n = grid.size();
    int ans = 0;

    while (*max_element(supply.begin(), supply.end()) != 0 ||\
    *max_element(demand.begin(), demand.end()) != 0) {
        pair<vector<int>, vector<int>> diffs = findDiff(grid);
        vector<int> row = diffs.first;
        vector<int> col = diffs.second;

        int maxi1 = *max_element(row.begin(), row.end());
        int maxi2 = *max_element(col.begin(), col.end());

        if (maxi1 >= maxi2) {
            for (int ind = 0; ind < row.size(); ind++) {
                if (row[ind] == maxi1) {
                    int mini1 = *min_element(grid[ind].begin(), grid[ind].end());
                    for (int ind2 = 0; ind2 < grid[ind].size(); ind2++) {
                        if (grid[ind][ind2] == mini1) {
                            int mini2 = min(supply[ind], demand[ind2]);
                            ans += mini2 * mini1;
                            supply[ind] -= mini2;
                            demand[ind2] -= mini2;

                            if (demand[ind2] == 0) {
                                for (int r = 0; r < n; r++) {
                                    grid[r][ind2] = INF;
                                }
                            } else {
                                fill(grid[ind].begin(), grid[ind].end(), INF);
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        } else {
            for (int ind = 0; ind < col.size(); ind++) {
                if (col[ind] == maxi2) {
                    int mini1 = INF;
                    for (int j = 0; j < n; j++) {
                        mini1 = min(mini1, grid[j][ind]);
                    }

                    for (int ind2 = 0; ind2 < n; ind2++) {
                        if (grid[ind2][ind] == mini1) {
                            int mini2 = min(supply[ind2], demand[ind]);
                            ans += mini2 * mini1;
                            supply[ind2] -= mini2;
                            demand[ind] -= mini2;

                            if (demand[ind] == 0) {
                                for (int r = 0; r < n; r++) {
                                    grid[r][ind] = INF;
                                }
                            } else {
                                fill(grid[ind2].begin(), grid[ind2].end(), INF);
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    cout << "The basic feasible solution for Vogels' method is: " << ans << endl;
}

void Northwest(vector<int> supply, vector<int> demand, vector<vector<int>> grid) {
    int row = 0;
    int col = 0;
    int ans = 0;

    while (row != grid.size() && col != grid[0].size()) {
        if (supply[row] <= demand[col]) {
            ans += supply[row] * grid[row][col];
            demand[col] -= supply[row];
            row += 1;
        } else {
            ans += demand[col] * grid[row][col];
            supply[row] -= demand[col];
            col += 1;
        }
    }

    cout << "The basic feasible solution for Northwest corner method is: " << ans << endl;
}

void calculatePotentials(const vector<vector<int>>& grid, const vector<int>& supply, 
const vector<int>& demand, vector<int>& u, vector<int>& v) {
    int rows = grid.size();
    int cols = grid[0].size();

    fill(u.begin(), u.end(), -INF);
    fill(v.begin(), v.end(), -INF);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (supply[i] > 0 && demand[j] > 0) {
                u[i] = max(u[i], grid[i][j]);
                v[j] = max(v[j], grid[i][j]);
            }
        }
    }
}

pair<int, int> findMaxOpportunityCell(const vector<vector<int>>& grid,
const vector<int>& u, const vector<int>& v,
const vector<int>& supply, const vector<int>& demand) {
    int maxOpportunityCost = -1;
    pair<int, int> cell = {-1, -1};

    for (int i = 0; i < supply.size(); ++i) {
        if (supply[i] == 0) continue;
        for (int j = 0; j < demand.size(); ++j) {
            if (demand[j] == 0) continue;
            int opportunityCost = u[i] + v[j] - grid[i][j];
            if (opportunityCost > maxOpportunityCost) {
                maxOpportunityCost = opportunityCost;
                cell = {i, j};
            }
        }
    }
    return cell;
}

void Russels(vector<int> supply, vector<int> demand, vector<vector<int>> grid) {
    int totalCost = 0;

    int rows = grid.size();
    int cols = grid[0].size();
    vector<int> u(rows), v(cols);

    while (true) {
        calculatePotentials(grid, supply, demand, u, v);
        pair<int, int> maxCell = findMaxOpportunityCell(grid, u, v, supply, demand);

        int i = maxCell.first;
        int j = maxCell.second;

        if (i == -1 || j == -1) break;

        int allocation = min(supply[i], demand[j]);
        totalCost += allocation * grid[i][j];
        supply[i] -= allocation;
        demand[j] -= allocation;
    }

    cout << "The basic feasible solution for Russell's approximation method is: "\
    << totalCost << endl;
}

bool isBalanced(const vector<int>& supply,const vector<int>& demand) {
    int sum1 = accumulate(supply.begin(), supply.end(), 0);
    int sum2 = accumulate(demand.begin(), demand.end(), 0);

    return sum1 == sum2;
}

int checkNeget(const vector<vector<int>>& grid) {
    for(const auto& numbers : grid) {
        for(const auto& number : numbers) {
            if(number < 0) {
                return -1;
            }
        }
    }

    return 1;
}

void printTable(const vector<int>& supply, const vector<int>& demand,
const vector<vector<int>>& grid) {
    cout << "Input parameter table:" << endl;
    int rows = supply.size();
    int cols = demand.size();

    for (int j = 0; j < cols; ++j) cout << setw(5) << "D" + to_string(j + 1);
    cout << setw(4) << "S" << endl;

    for (int i = 0; i < rows; ++i) {
        cout << "S" << i + 1;
        for (int j = 0; j < cols; ++j) {
            cout << setw(4) << grid[i][j];
        }
        cout << setw(4) << supply[i] << endl;
    }

    cout << "D" << " ";
    for (const auto& d : demand) {
        cout << setw(4) << d;
    }

    cout << endl;
}

int main() {
    int rows, cols;
    cout << "Enter the number of supply sources: ";
    cin >> rows;
    cout << "Enter the number of demand destinations: ";
    cin >> cols;

    vector<int> supply(rows);
    vector<int> demand(cols);

    cout << "Enter the supply values for each source: ";
    for (int& s : supply) cin >> s;
    cout << "Enter the demand values for each destination: ";
    for (int& d : demand) cin >> d;

    vector<vector<int>> grid(rows, vector<int>(cols));

    cout << "Enter the cost matrix:" << endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cin >> grid[i][j];
        }
    }

    if(checkNeget(grid) < 0) {
        cout << "The method is not applicable!" << endl;
    } else if(!isBalanced(supply, demand)) {
        cout << "The problem is not balanced!" << endl;
    } else {
        printTable(supply, demand, grid);
        Northwest(supply, demand, grid);
        Vogels(supply, demand, grid);
        Russels(supply, demand, grid);
    }

    return 0;
}
