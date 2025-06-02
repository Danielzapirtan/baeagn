import sys

def pr(p):
    k = 3
    while k <= p // k:
        if p % k == 0:
            return False
        k += 2
    return True

def prtab(l):
    p = 3
    tabpr = [2]
    while p < l:
        if pr(p):
            tabpr.append(p)
        p += 2
    return tabpr

def longpr(p, tab):
    for k in tab:
        if k == 509:
            if pow(3, p-1, p) != 1:
                return False
        if k > p // k:
            return True
        if p % k == 0:
            return False
    return True 

def gbn(n, tab):
    p = 3
    while p <= n - p:
        if pr(p):
            if longpr(n - p, tab):
               return True
        p += 2
    return False
