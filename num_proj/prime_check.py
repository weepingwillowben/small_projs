'''
This requires the gmpy2 python library.

This in turn depends on gmp (mac, linux) or mpir (windows), and also
mpc, and mpir being installed on your system.
On Unix systems these should be readily avaliable
through your system's package manager (brew, apt-get or dnf).

I have no idea how to install mpir on windows, but apparently it can be done.

Once these are installed, gmpy2 can be installed with pip as below:

pip install gmpy2
'''

import collections
import gmpy2
import random as rand
import time
from configparser import ConfigParser

InfPoint = collections.namedtuple("InfPoint",[])
Point = collections.namedtuple("Point",["x","y"])
Curve = collections.namedtuple("Curve",["A","B","N"])

def is_inf(P):
    return type(P) == InfPoint

def unsafe_add(P,L,curve):
    if P.x != L.x:
        m = gmpy2.divm((P.y - L.y),P.x - L.x,curve.N)
    else:
        if P.y == L.y:
            m = gmpy2.divm(3*P.x**2 + curve.A,2*P.y,curve.N)
        else:
            return InfPoint()

    m %= curve.N

    xnew = (m*m - P.x - L.x) % curve.N
    ynew = -(P.y + m*(xnew - P.x)) % curve.N
    return Point(x=xnew,y=ynew)

def add(P,L,curve):
    if is_inf(P):
        return L
    if is_inf(L):
        return P
    try:
        return unsafe_add(P,L,curve)
    except ZeroDivisionError:
        return InfPoint()

def double(P,curve):
    return add(P,P,curve)

def mul(t,P,curve):
    if t == 0:
        return InfPoint()
    if t == 1:
        return P
    assert(t > 0)
    # Multiplication by doubling
    return (double(mul(t//2,P,curve),curve) if t % 2 == 0
      else add(P,mul(t-1,P,curve),curve))

def divides(n,m):
    return n % m == 0

def size_condition_holds(q,n):
    #q > n^(1/4)+1
    rootval, isexact = gmpy2.iroot(n,4)
    return q > rootval + 1

def check_prime(p,q,L,curve):
    #suppose q is prime, check primality of p using L and curve
    assert(not is_inf(L))
    assert(size_condition_holds(q,curve.N))

    qL = mul(q,L,curve)
    try:
        qL = mul(q,L,curve)
    except BaseException as e:
        print("exceptoin raised")
        return False

    return is_inf(qL)

'''
This section is for generating the primes.
Here, unike above efficiency is ignored in favor of simplicity
'''

def legendre_n(n):
    res = [-1]*n
    res[0] = 0
    for x in range(1,n):
        res[(x**2)%n] = 1
    return res

def curve_x_val(x,curve):
    return (x**3 + curve.A*x + curve.B)%curve.N

def count_points(curve):
    count = 0
    ledg = legendre_n(curve.N)
    for x in range(0,curve.N):
        val = curve_x_val(x,curve)
        count += ledg[val]+1
    return count + 1#includes the point at infinity

def determinant(E):
    return 4*E.A**3 + 27*E.B**2

def ord_in_bounds(ord,n):
    proot = gmpy2.isqrt(n)
    return n+1-proot <= ord <= n+1+proot

def square_root(x,n):
    for y in range(n):
        if (y*y)%n == x:
            return y
    return None

rand_state = gmpy2.random_state(int(time.clock()))
def generate_curve(n):
    while True:
        curve = Curve(A=gmpy2.mpz_random(rand_state,n),B = gmpy2.mpz_random(rand_state,n),N=n)
        if gmpy2.gcd(determinant(curve),n) != 1:
            continue
        ord_e = count_points(curve)
        L = gen_rand_point(curve)
        assert(curve_x_val(L.x,curve) == L.y**2%curve.N)
        #print(L)
        nL = L
        count = 0
        while not is_inf(nL):
            nL = add(nL,L,curve)
            count += 1
        #print(mul(ord_e,L,curve))
        if not ord_in_bounds(ord_e,n):
            continue
        if ord_e % 2 != 0:
            continue
        q = ord_e // 2
        if gmpy2.is_prime(q):#probabalistic primality test
            return curve,q

def gen_rand_point(curve):
    while True:
        x = gmpy2.mpz_random(rand_state,curve.N)
        xval = curve_x_val(x,curve)
        sign_y = rand.randint(0,1)*2 - 1
        y = square_root(xval,curve.N)
        if y != None:
            yrandsign = (y*sign_y)%curve.N
            L =  Point(x=x,y=yrandsign)
            return L


def find_point(q,curve):
    while True:
        L = gen_rand_point(curve)
        if is_inf(mul(q,L,curve)):
            return L


def sieve_of_Eratosthenes_prime_check(prime):
    max = 123123
    is_composite = [False]*max
    for x in range(2,max):
        if not is_composite[x]:
            for y in range(x*2,max,x):
                is_composite[y] = True

    return not is_composite[prime]

lowprimes = [2,3,5,7,11,13,17,19,23,29,31,37]
def generate_certificate(n):
    #following from algorithm in
    #http://theory.stanford.edu/~dfreeman/cs259c-f11/finalpapers/primalityproving.pdf
    i = 0
    primes = []
    p_i =n
    lowerbound = 37
    while p_i > lowerbound:
        curve,q = generate_curve(p_i)
        L = find_point(q,curve)
        primes.append((curve,L,q))
        p_i = q
    return primes


def check_certificate(prime,cert):
    for curve,L, q in cert:
        if not check_prime(prime, q ,L, curve):
            return False
        prime = q
    return (prime in lowprimes)

def gen_and_check_certificate(certp):
    certif = generate_certificate(certp)

    print("Checking prime = ",certp)
    print("Python representation of certificate:")
    print(certif)
    print("Certificate is valid = ",check_certificate(certp,certif))

gen_and_check_certificate(103)
gen_and_check_certificate(1003)
gen_and_check_certificate(10007)

if __name__ == "__main__":
    while True:
        gen_and_check_certificate(int(input()))
