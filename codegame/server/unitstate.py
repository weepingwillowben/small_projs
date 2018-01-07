import copy

class UnitState:
    def __init__(self,stats,command):
        self.stats = stats
        self.curstate = copy.deepcopy(stats)

        self.command = command

    #def save(self):
