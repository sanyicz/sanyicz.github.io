from random import randint
import numpy as np
import tkinter as tk

class MineSweeper(tk.Frame): #class inheritance
    def __init__(self, parentWindow):
        tk.Frame.__init__(self, parentWindow) #?
        self.window = parentWindow #?

        self.gameState = '0' #1 if a game is started (New game), else 0
        self.colors = {'' : 'black', 0 : 'black', 1 : 'blue', 2 : 'green', 3 : 'orange', 4 : 'red', 5 : 'violet red'}
        
        self.menu = tk.Frame(self.window, borderwidth=2, relief='ridge', width=100) #frame for the settings menu
        self.menu.grid(row=0, column=0)
        
        tk.Label(self.menu, text='Number of mines').grid(row=0, column=0) #number of mines to place on the playing field
        self.numberOfMines = tk.IntVar()
        self.numberOfMines.set(20)
        tk.Entry(self.menu, textvariable=self.numberOfMines, width=10).grid(row=0, column=1)
        
        tk.Label(self.menu, text='Field width').grid(row=1, column=0) #width parameter of the playing field (number of blocks horizontally)
        self.fieldWidth = tk.IntVar()
        self.fieldWidth.set(10)
        tk.Entry(self.menu, textvariable=self.fieldWidth, width=10).grid(row=1, column=1)
        
        tk.Label(self.menu, text='Field height').grid(row=2, column=0) #height parameter of the playing field (number of blocks vertically)
        self.fieldHeight = tk.IntVar()
        self.fieldHeight.set(10)
        tk.Entry(self.menu, textvariable=self.fieldHeight, width=10).grid(row=2, column=1)

        tk.Button(self.menu, text='New game', command=self.newGame).grid(row=0, column=3) #button to start a game

        tk.Button(self.menu, text='Quit', command=self.quitGame).grid(row=1, column=3) #button to exit the program

    def newGame(self): #method to start a game
        try: #destroy the playing field if there is one
            self.gamefield.destroy()
        except: #only for the first new game after program start
            pass
        self.gamefield = tk.Frame(self.window, relief='ridge') #create frame for the playing field
        self.gamefield.grid(row=1, column=0)
        self.gameState = '1'
        
        grid_i, grid_j = self.fieldHeight.get(), self.fieldWidth.get() #gather the necessary data from the menu field settings
        n_mines = self.numberOfMines.get()
        
        if n_mines <= grid_i * grid_j: #if there are less mines than tiles on the field
            print('New game')
            
            self.notMines = grid_i * grid_j - n_mines #number of mineless tiles (needed to calculate win condition)
            
            self.minesLeft = tk.IntVar()
            self.minesLeft.set(n_mines)
            tk.Label(self.menu, text='Mines left').grid(row=4, column=0) #number of mines left to mark
            self.minesLeftLabel = tk.Label(self.menu, textvariable=self.minesLeft)
            self.minesLeftLabel.grid(row=4, column=1)

            self.tilesClicked = tk.IntVar()
            self.tilesClicked.set(0)
##            tk.Label(self.menu, text='Tiles clicked').grid(row=5, column=0) #number of tiles clicked (needed to calculate win condition)
##            self.tilesClickedLabel = tk.Label(self.menu, textvariable=self.tilesClicked)
##            self.tilesClickedLabel.grid(row=5, column=1)
                    
            self.field = np.zeros([grid_i, grid_j], dtype=int) #array to store tile values

            #generate places of mines
            mine_coordinates = []
            while len(mine_coordinates) < n_mines: #repeat randomization until there are enough mines at different places
                i, j = randint(0, grid_i-1), randint(0, grid_j-1)
                if (i, j) not in mine_coordinates:
                    mine_coordinates.append( (i, j) )

            #fill the array fo the tiles valus
            for i in range(grid_i):
                for j in range(grid_j):
                    self.field[i][j] = 0
                    for coord in mine_coordinates:
                        if i==coord[0] and j==coord[1]:
                            self.field[i][j] = -1 #tile's value is -1 if there is a mine in the given tile
            #print(self.field)
            #calculate how many mines are there in the adjacent tiles
            for i in range(grid_i):
                for j in range(grid_j):
                    if self.field[i][j] == -1:
                        for ii in range(i-1, i+2): #for one row up and down
                            for jj in range(j-1, j+2): #for one column left and right
                                if ii>=0 and jj>=0: #lower bound for the tile coordinates
                                    try: #upper bound for the tile coordinates: only if an adjacent tile exists
                                        if self.field[ii][jj] != -1:
                                            self.field[ii][jj] += 1
                                    except:
                                        pass
            #print(self.field)

            #create the playing field (takes too long!)
            self.tileLabels = [] #labels for the tiles with the mines and numbers on them
            self.buttonLabels = [] #buttons that hide the tiles
            for i in range(grid_i):
                self.tileLabels.append([])
                self.buttonLabels.append([])
                for j in range(grid_j):
                    if self.field[i][j] == -1: #for tiles with mines in them
                        symbol = 'M'
                        color = 'red'
                    else:
                        symbol = self.field[i][j]
                        if symbol == 0:
                            symbol = ''
                        else:
                            symbol = symbol
                        color = self.colors[symbol]
                    tileLabel = tk.Label(self.gamefield, text=symbol, font=('Helvetica 9 bold'), height=2, width=4, borderwidth=2, relief='ridge', fg=color)
                    tileLabel.grid(row=i, column=j)
                    self.tileLabels[i].append(tileLabel)
                    buttonLabel = tk.Label(self.gamefield, height=2, width=4, borderwidth=2, relief='raised')
                    buttonLabel.grid(row=i, column=j)
                    buttonLabel.bind('<Button-1>', self.buttonLeftClick)
                    buttonLabel.bind('<Button-3>', self.buttonRightClick)
                    self.buttonLabels[i].append(buttonLabel)
        else:
            print('More mines than tiles')

    def quitGame(self):
        self.window.destroy()

    def neighbourCells(self, row, col):
        #method for showing the neighbours of tiles with 0 in them
        for i in range(row-1, row+2):
            for j in range(col-1, col+2):
                if i>=0 and j>=0: #lower bound for the tile coordinates
                    try: #upper bound for the tile coordinates: only if an adjacent tile exists
                        if self.tileLabels[i][j]['text']=='' and self.buttonLabels[i][j].winfo_exists()==1:
                            self.buttonLabels[i][j].destroy()
                            self.tilesClicked.set(self.tilesClicked.get()+1) #increase the number of clicked on tiles by 1
                            self.neighbourCells(i, j) #if an adjacent tile also contains 0, call the method again
                        elif self.tileLabels[i][j]['text']!='M' and self.buttonLabels[i][j].winfo_exists()==1:
                            self.buttonLabels[i][j].destroy()
                            self.tilesClicked.set(self.tilesClicked.get()+1) #increase the number of clicked on tiles by 1
                    except:
                        pass

    def buttonLeftClick(self, event):
        #left click event: shows the tile under the bottom
        row, col = event.widget.grid_info()['row'], event.widget.grid_info()['column'] #get the coordinates of the widget that has been clicked on
        if self.gameState == '1' and self.buttonLabels[row][col]['text']!='M': #if a game is running and the tile is not marked for mine
            event.widget.destroy() #show the tile under the button
            if self.tileLabels[row][col]['text']=='M': #if a mine is clicked, the game is over, you lost
                self.gameOver()
            if self.tileLabels[row][col]['text']=='': #if an isolated tile is clicked
                self.neighbourCells(row, col)
            self.tilesClicked.set(self.tilesClicked.get()+1) #increase the number of clicked on tiles by 1
            if self.tilesClicked.get() == self.notMines and self.minesLeft.get() == 0: #win condition: every bomb is marked and every other tile is showed (left clicked on)
                self.gameWon()

    def buttonRightClick(self, event):
        #right click event: marks or unmarks the tile for mine (writes or erases 'M' on the button)
        if self.gameState == '1':
            row, col = event.widget.grid_info()['row'], event.widget.grid_info()['column'] #get the coordinates of the widget that has been clicked on
            if self.buttonLabels[row][col]['text']=='' and  self.minesLeft.get()>=1: #mark only if the button is not marked and there are still mines left to mark
                self.buttonLabels[row][col].configure(text = 'M', fg='red')
                self.minesLeft.set(self.minesLeft.get()-1)
            elif self.buttonLabels[row][col]['text']=='M': #unmark if the tile is already marked for mine
                self.buttonLabels[row][col].configure(text = '')
                self.minesLeft.set(self.minesLeft.get()+1)
            if self.tilesClicked.get() == self.notMines and self.minesLeft.get() == 0: #win condition: every bomb is marked and every other tile is showed (left clicked on)
                self.gameWon()
                
    def gameWon(self):
        #method called when the game is won
        #win condition: every bomb is marked and every other tile is showed (left clicked)
        print('You won')
        self.gameState = '0'
        gameWon = tk.Toplevel() #message window
        tk.Label(gameWon, text='You won').pack()
        gameWon.grab_set() #blocks the main window until message window is closed
        
    def gameOver(self):
        #method called when a mine is found (an 'M' is left clicked)
        print('Game over')
        self.gameState = '0'
        gameOver = tk.Toplevel() #message window
        tk.Label(gameOver, text='Game over').pack()
        for buttonList in self.buttonLabels:
            for button in buttonList:
                button.destroy()
        gameOver.grab_set() #blocks the main window until message window is closed


if __name__ == '__main__':
    root = tk.Tk() #create a window
    MineSweeper = MineSweeper(root).grid(row=0, column=0) #place the app in the window (as a tk.Frame)
    root.mainloop()
