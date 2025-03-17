//
//  main.cpp
//  minesweep
//
//  Created by Jeffery Suddeth on 3/16/25.
//

#include <iostream>
#include <ctime>
#include <string>
#include <queue>
#include <tuple>

using namespace std;

class Cell
{
private:
    int neighborMines;
    bool stepped;
    bool isMine;
   
public:
    Cell()
    {
        isMine = false;
        neighborMines = 0;
        stepped = false;
    }
    
    int getNeighborMines(){ return neighborMines; }
    void setNeighborMines(int mines) { neighborMines = mines; }
    
    bool getStepped() { return stepped; }
    void setStepped(bool isStep) { stepped = isStep; }
    
    bool getIsMine() { return isMine; }
    void setIsMine(bool mine) { isMine = mine; }
    
    void incrementNeighborMines()
    {
        neighborMines++;
    }
};


class GameBoard
{
public:
    static const int ROWS = 5;
    static const int COLS = 5;
    static const int NUMBER_OF_MINES = 5;
    
private:
    
    Cell cells[ROWS][COLS];
    
    
public:
    
    GameBoard(){
        
        
    }
    
    void showHint()
    {
        // find the mines:
        
        cout << "Mines are at: ";
        
        for (int i=0; i<ROWS; i++)
        {
            for (int j=0; j<COLS; j++)
            {
                if (cells[i][j].getIsMine())
                {
                    cout << "(" << i << ", " << j << "), ";
                }
            }
        }
        
        cout << endl;
    }
    
    void initializeBoard()
    {
        for (int i=0; i<NUMBER_OF_MINES; i++)
        {
            
            int row = rand() % 5;
            int col = rand() % 5;
            
            // keep doing this until we get one that is not already a mine
            while (cells[row][col].getIsMine())
            {
                row = rand() % 5;
                col = rand() % 5;
            }
            
            cells[row][col].setIsMine(true);
        }
        
        
        // update each cell with its neighboring mine count
        for (int row=0; row<ROWS; row++)
        {
            for (int col=0; col<COLS; col++)
            {
                countMines(row, col);
            }
        }
    }
    
    void printBoard()
    {
        for (int i=0; i<ROWS; i++)
        {
            for (int j=0; j<COLS; j++)
            {
                if (cells[i][j].getStepped() && !cells[i][j].getIsMine())
                {
                    cout << cells[i][j].getNeighborMines() << " ";
                }
                else if (cells[i][j].getStepped() && cells[i][j].getIsMine())
                {
                    cout << "! ";
                }
                else
                {
                    cout << "* ";
                }
                
            }
            cout << endl;
        }
    }
    
    bool step(int x, int y)
    {
        // step on it
        cells[x][y].setStepped(true);
        
        // check if we stepped on a mine
        if (cells[x][y].getIsMine())
        {
            return true;
        }
        
        // otherwise, we check the neighbors and mark them as stepped
        // spreading until we hit a cell whose mine count is greater than 0
        
        // add starting point for the cascade
        cascadeCells(x, y);
        
        return false;
    }
    
    void reveal(int row, int col)
    {
        if (row < 0 || row > ROWS || col < 0 || col > COLS) return;
        
        if (cells[row][col].getIsMine()) return;
        
        cells[row][col].setStepped(true);
    }
    
    
    // this fills the member set with the neighbors of for a given cell
    vector<tuple<int, int>> getNeighbors(int row, int col)
    {
        
        vector<tuple<int, int>> data;
        for (int rowOffset=-1; rowOffset<=1; rowOffset++)
        {
            for (int colOffset=-1; colOffset<=1; colOffset++)
            {
                int r = row+rowOffset;
                int c = col+colOffset;
                
                if (r >= 0 && r < ROWS && c >=0 && c < COLS)
                {
                    if (rowOffset != 0 || colOffset != 0)
                    {
                        if (!cells[r][c].getStepped())
                        {
                            data.push_back(make_tuple(r, c));
                        }
                    }
                }
            }
        }
        
        return data;
    }
    
    // Given the cell currently in the cascade set
    // we will reveal it and all of its neighbors, adding
    // the other neighbors until we hit one with a mine count > 0
    void cascadeCells(int row, int col)
    {
        queue<tuple<int, int>> cascade;
        
        // start with this one
        cascade.push(make_tuple(row, col));
                
        // we start with the one that was passed in.
        // pop that off the queue and reveal it. Then get its neigbors
        // for each neighbor if it has a neighbor mine count of 0 then add it to the queue for cascade effect.
        // if it does have a neighbor mine or if it is a mine then we just reveal it. If we call reveal on a cell
        // that has a mine it will not actually reveal.
        while(!cascade.empty())
        {

            // get the cell currently referenced by the iterator
            tuple<int, int> current = cascade.front();
            cascade.pop();
            int r = get<0>(current);
            int c = get<1>(current);
            
            // reveal this cell
            reveal(r, c);
            
            // get his neighbors into the cascade
            vector<tuple<int,int>> neighbors = getNeighbors(r, c);
            
            for (auto itr = neighbors.begin(); itr != neighbors.end(); itr++)
            {
                auto nbr = *itr;
                int nbr_r = get<0>(nbr);
                int nbr_c = get<1>(nbr);
                
                Cell cl = cells[nbr_r][nbr_c];
                if (cl.getNeighborMines() == 0 && !cl.getIsMine())
                {
                    cascade.push(make_tuple(nbr_r, nbr_c));
                }
                else
                {
                    reveal(nbr_r, nbr_c);
                }
            }
        }
    }
    
    bool checkForWinner()
    {
        // for each cell if we found one that is not stepped on but is also not a mine
        // then the game is not over.
        for (int i=0; i<ROWS; i++)
        {
            for (int j=0; j<COLS; j++)
            {
                if (!cells[i][j].getStepped() && !cells[i][j].getIsMine())
                {
                    return false;
                }
            }
        }
        
        return true;
    }
    
private:
    
    void countMines(int row, int col)
    {
        
        for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
        {
            for (int colOffset = -1; colOffset <= 1; colOffset++)
            {
                
                if (rowOffset == 0 && colOffset == 0)
                {
                    continue;
                }
                
                int rowValue = row + rowOffset;
                int colValue = col + colOffset;
                
                if (rowValue < 0 || rowValue >= ROWS || colValue < 0 || colValue >= COLS)
                {
                    continue;
                }
                
                if (cells[rowValue][colValue].getIsMine())
                {
                    cells[row][col].incrementNeighborMines();
                }
                
            }
        }
    }
};

int main(int argc, const char * argv[]) {
 
    GameBoard board;
 
    // seed the random number generator
    srand((int)time(0));
    
    board.initializeBoard();
    
    bool gameOver = false;
    bool loser = false;
    while (!gameOver)
    {
        int x = 0, y = 0;
    
        board.showHint();
        
        board.printBoard();
        
        
        cout << "Enter your row (0-" << GameBoard::ROWS-1 << "): ";
        cin >> x;
        while(x >= GameBoard::ROWS)
        {
            cout << "INVALID ENTRY. Enter a row from 0 to " << GameBoard::ROWS-1 << endl;
            cin >> x;
            
        }
        
        cout << "Enter your column (0-" << GameBoard::COLS-1 << ": ";
        cin >> y;
        while(y >= GameBoard::COLS)
        {
            cout << "INVALID ENTRY. Enter a row from 0 to " << GameBoard::COLS-1 << endl;
            cin >> y;
        }
        
        if (board.step(x, y))
        {
            loser = true;
            gameOver = true;
        }
        else
        {
            // check if we won
            if (board.checkForWinner())
            {
                loser = false;
                gameOver = true;
            }
        }
        
    }
    
    // print the board and the game over message
    board.printBoard();
    
    string msg = !loser ? "You won!" : "You stepped on a mine!";
    cout << "\n\n" << msg << endl;
    
    return 0;
}
