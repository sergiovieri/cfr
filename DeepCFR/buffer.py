
import random 

class Buffer:
    def __init__(self, max_size):
        self.arr = []
        self.max_size = max_size
        self.inserted = 0
    
    def insert(self, elem):
        self.inserted += 1
        if len(self.arr) < self.max_size:
            self.arr.append(elem)
        else:
            s = int(random.random() * self.inserted)
            if s < self.max_size:
                self.arr[ s ] = elem
    
    def __iter__(self):
        self.iter = iter(self.arr)
        return self
    
    def __next__(self):
        return next(self.iter)
        
