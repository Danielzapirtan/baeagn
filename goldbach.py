import sys

def pr(p):
    k = 3
    while k <= p // k:
        if p % k == 0:
            return False
        k += 2
    return True

def gbn(n):
    p = 3
    while p <= n - p:
        if prime(p):
            if prime(n - p):
               return True
        p += 2
    return False
