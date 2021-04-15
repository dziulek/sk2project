import copy
import numpy as np

class Player:
    def __init__(self):
        self.nickname = 'Unnamed_cell'
        self.coordinates = np.array([])
        self.color = 0xffffffff
    def clear(self):
        self.nickname = 'Unnamed_cell'
        self.coordinates = np.array([])
    def setBlobsCoordinates(self, __coordinates):
        if type(__coordinates) != np.ndarray:
            coordinates = np.array(__coordinates)
        
        if len(np.shape(__coordinates)) == 2 and np.shape(__coordinates)[1] == 2:
            self.coordinates = copy.deepcopy(__coordinates)

    def addCoordinate(self, coord):
        self.coordinates = np.append(self.coordinates, coord)

    def addNickname(self, nick):
        self.nickname = nick


class GameState:
    def __init__(self):
        self.players = []
        self.map = {
            'minis' : np.array([]),
            'width' : 1,
            'height' : 1,
            'colors' : np.array([], int),
            'bombs' : np.array([]),
            'abandoned' : np.array([])
        }
        self.playerState = True
        self.view = np.array([])
    
    def clear(self):
        self.myPlayer = None
        self.players = []
        self.map = {'minis' : np.array([]), 'colors' : np.array([], int)}
        self.view = np.array([])
        
    def appendPlayer(self, player):
        self.players.append(copy.deepcopy(player))
    
    def appendMe(self, me):
        self.myPlayer = copy.deepcopy(me)

    def addMap(self, width, height, minis, colors):
        self.map['width'] = width
        self.map['height'] = height
        self.map['minis'] = copy.deepcopy(minis)
        self.map['colors'] = copy.deepcopy(colors)

    def addViewCoord(self, coord):
        self.view = np.append(self.view, coord)
    
    def setView(self, window):
        if type(window) != np.ndarray:
            window = np.array(window)
        if np.shape(window) == (2,2):
            self.view = copy.deepcopy(window)

class MyInfo:
    def __init__(self):
        self.attributes = {}
        self.attributes['waction'] = 0
        self.attributes['divide'] = 0
        self.attributes['mouse'] = [0, 0]
        self.attributes['state'] = 'wait'

    def addWAction(self, b):
        self.attributes['waction'] = b
    
    def addMousePosition(self, vector):
        self.attributes['mouse'] = copy.deepcopy(vector)
    
    def addDivideAction(self, b):
        self.attributes['divide'] = b

    def addState(self, state):
        self.attributes['state'] = state


def main():

    my = MyInfo()

    print(my.attributes['mouse'][0], my.attributes['mouse'][1])

    my.addMousePosition([4,5])
    print(my.attributes['mouse'][0], my.attributes['mouse'][1])

if __name__ == "__main__":

    main()


    