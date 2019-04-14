from subprocess import PIPE, Popen
import os

# return the bucket of flop, turn river
class computeBucket:
    def __init__(self):
        with open('Group32runtimeFlopBucket.txt', 'r') as f:
            self.flopBucket = eval("".join(f.readlines))
        
        def buildCPP():
            # make_process = Popen("make runtimeBucket")
            # if make_process.wait() != 0:
            #     assert(False)
            
            os.system("chmod 777 Group32runtimeBucket")
            return Popen('./Group32runtimeBucket', stdin=PIPE, stdout=PIPE, stderr=PIPE,
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
        
        def normalize(hand, comm):
            hand = list(map(lambda x: (x // 4, x % 4), hand))
            comm = list(map(lambda x: (x // 4, x % 4), comm))
            def rankCard(a):
                return a[0]
                
            hand = sorted(hand, key=rankCard)
            comm = sorted(comm, key=rankCard)
            
            arr = hand + comm
            suitmap = {}
            tot = 0
            for i in range(len(arr)):
                r, s = arr[i]
                if s not in suitmap:
                    suitmap[s] = tot
                    tot+= 1
                    
                arr[i] = r * 4 + suitmap[s]
                
            return arr[:2], arr[2:]
        
        hand, comm = normalize(hand, comm)    
        key = ''    
        for i in hand:
            key += encode(i)
        
        for i in comm:
            key += encode(i)
        
        return decode(self.flopBucket[key])
    
    def cppComputeBucket(self, hand, comm):
        inp = "%d %d\n" % hand, comm
        self.process.stdin.write(inp)
        return int(self.process.stdout.readline())
        
    # hand and comm are integer masks.
    def getBucket(self, hand, comm):
        hand_arr = [i for i in range(52) if (1 << i) & hand]
        comm_arr = [i for i in range(52) if (1 << i) & comm]
        
        if len(comm_arr) == 3:
            return self.flop(hand_arr, comm_arr)
        elif len(comm_arr) == 4 or len(comm_arr) == 5:
            return self.cppComputeBucket(hand, comm)
        
                