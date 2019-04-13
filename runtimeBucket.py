from subprocess import PIPE, Popen

# return the bucket of flop, turn river
class computeBucket:
    def __init__(self):
        self.flopBucket = {}
        
        with open('runtimeFlopBucket.txt', 'r') as f:
            eval("".join(f.readlines))
        
        def buildCPP():
            make_process = Popen("make runtimeBucket")
            if make_process.wait() != 0:
                assert(False)
            
            return Popen('./runtimeBucket', stdin=PIPE, stdout=PIPE, stderr=PIPE,
                universal_newlines=True, bufsize=1)

        self.process = buildCPP()
    
    
    # hand and comm are lists of integers represent the cards.
    # return the bucket id
    def flop(self, hand, comm):
        # int -> char
        def encode(i):
            return chr(ord('0') + i)
        
        # char -> int
        def decode(c):
            return ord(c) - ord('0')
            
        key = ''    
        for i in hand:
            key += encode(i)
        
        for i in comm:
            key += encode(i)
        
        return decode(self.flopBucket[key])
    
    def cppComputeBucket(self, hand, comm):
        inp = "%d %d\n" % hand, comm
        self.process.stdout.write(inp)
        return int(self.process.stdout.readline())
        
    
    def getBucket(self, hand, comm):
        hand_arr = [i for i in range(52) if (1 << i) & hand]
        comm_arr = [i for i in range(52) if (1 << i) & comm]
        
        if len(comm_arr) == 3:
            return self.flop(hand_arr, comm_arr)
        elif len(comm_arr) == 4 or len(comm_arr) == 5:
            return self.cppComputeBucket(hand, comm)
        
                