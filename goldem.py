#!/usr/bin/env python3
"""
Peano Arithmetic Prover
A top-down theorem prover for basic Peano arithmetic with induction.
Operators can be called as functions or represented as lists.
"""

import copy
from typing import Union, List, Any, Optional

# Global axioms and theorems storage
axioms = []
theorems = []

class PeanoTerm:
    """Base class for Peano arithmetic terms"""
    pass

def zero():
    """Constant 0"""
    return ['0']

def su(x):
    """Successor function"""
    if isinstance(x, list) and len(x) == 1 and x[0] == '0':
        return ['su', x]
    elif isinstance(x, list):
        return ['su', x]
    else:
        return ['su', x]

def eq(x, y):
    """Equality predicate"""
    # Direct equality check
    if x == y:
        return True
    
    # Try to simplify and compare
    x_simp = simplify(x)
    y_simp = simplify(y)
    
    if x_simp == y_simp:
        return True
    
    # If we can't determine, freeze as list
    return ['eq', x, y]

def add(x, y):
    """Addition function"""
    # Base case: add(0, y) = y
    if x == ['0']:
        return y
    
    # Recursive case: add(su(x'), y) = su(add(x', y))
    if isinstance(x, list) and len(x) == 2 and x[0] == 'su':
        x_pred = x[1]
        result = add(x_pred, y)
        if result is not False and result is not True:
            return su(result)
    
    # If we can't compute, freeze as list
    return ['add', x, y]

def mul(x, y):
    """Multiplication function"""
    # Base case: mul(0, y) = 0
    if x == ['0']:
        return ['0']
    
    # Recursive case: mul(su(x'), y) = add(y, mul(x', y))
    if isinstance(x, list) and len(x) == 2 and x[0] == 'su':
        x_pred = x[1]
        mul_result = mul(x_pred, y)
        if mul_result is not False and mul_result is not True:
            return add(y, mul_result)
    
    # If we can't compute, freeze as list
    return ['mul', x, y]

def forall(var, formula):
    """Universal quantifier"""
    # For now, freeze as list - full implementation would need variable substitution
    return ['forall', var, formula]

def exists(var, formula):
    """Existential quantifier"""
    return ['exists', var, formula]

def implies(p, q):
    """Implication"""
    if p is False:
        return True
    if p is True:
        return q
    if q is True:
        return True
    
    # Try to prove by checking axioms and theorems
    if check_known_facts(p, q):
        return True
    
    return ['implies', p, q]

def and_op(p, q):
    """Logical AND"""
    if p is True and q is True:
        return True
    if p is False or q is False:
        return False
    
    return ['and', p, q]

def or_op(p, q):
    """Logical OR"""
    if p is True or q is True:
        return True
    if p is False and q is False:
        return False
    
    return ['or', p, q]

def not_op(p):
    """Logical NOT"""
    if p is True:
        return False
    if p is False:
        return True
    
    return ['not', p]

def simplify(expr):
    """Simplify an expression by evaluating what we can"""
    if not isinstance(expr, list):
        return expr
    
    if len(expr) == 0:
        return expr
    
    op = expr[0]
    
    if op == '0':
        return expr
    
    elif op == 'su' and len(expr) == 2:
        inner = simplify(expr[1])
        return ['su', inner]
    
    elif op == 'eq' and len(expr) == 3:
        left = simplify(expr[1])
        right = simplify(expr[2])
        result = eq(left, right)
        if result is True or result is False:
            return result
        return ['eq', left, right]
    
    elif op == 'add' and len(expr) == 3:
        left = simplify(expr[1])
        right = simplify(expr[2])
        result = add(left, right)
        if result != ['add', left, right]:
            return simplify(result)
        return result
    
    elif op == 'mul' and len(expr) == 3:
        left = simplify(expr[1])
        right = simplify(expr[2])
        result = mul(left, right)
        if result != ['mul', left, right]:
            return simplify(result)
        return result
    
    elif op == 'implies' and len(expr) == 3:
        left = simplify(expr[1])
        right = simplify(expr[2])
        result = implies(left, right)
        if result is True or result is False:
            return result
        return ['implies', left, right]
    
    return expr

def substitute(expr, var, value):
    """Substitute variable with value in expression"""
    if expr == var:
        return value
    
    if not isinstance(expr, list):
        return expr
    
    return [substitute(item, var, value) for item in expr]

def prove_by_induction(formula, var):
    """Prove a formula by induction on variable var"""
    print(f"Proving by induction on {var}: {format_expr(formula)}")
    
    # Base case: prove formula[var := 0]
    base_case = substitute(formula, var, ['0'])
    base_simplified = simplify(base_case)
    
    print(f"Base case: {format_expr(base_simplified)}")
    
    if prove(base_simplified):
        print("Base case proved!")
        
        # Inductive step: assume formula[var := k], prove formula[var := su(k)]
        k = f"{var}_k"
        assumption = substitute(formula, var, [k])
        goal = substitute(formula, var, ['su', [k]])
        
        print(f"Inductive step - Assume: {format_expr(assumption)}")
        print(f"Inductive step - Prove: {format_expr(goal)}")
        
        # Add assumption to axioms temporarily
        old_axioms = axioms.copy()
        axioms.append(assumption)
        
        result = prove(goal)
        
        # Restore axioms
        axioms[:] = old_axioms
        
        if result:
            print("Inductive step proved!")
            return True
        else:
            print("Inductive step failed.")
            return False
    else:
        print("Base case failed.")
        return False

def check_known_facts(premise, conclusion):
    """Check if we can derive conclusion from premise using known axioms/theorems"""
    # Simple pattern matching for now
    for axiom in axioms + theorems:
        if axiom == conclusion:
            return True
        if isinstance(axiom, list) and len(axiom) == 3 and axiom[0] == 'implies':
            if axiom[1] == premise and axiom[2] == conclusion:
                return True
    return False

def prove(formula):
    """Main theorem proving function"""
    print(f"Trying to prove: {format_expr(formula)}")
    
    simplified = simplify(formula)
    
    if simplified is True:
        print("Proved directly!")
        return True
    
    if simplified is False:
        print("Disproved directly!")
        return False
    
    # Check if it's already known
    if simplified in axioms or simplified in theorems:
        print("Already known!")
        return True
    
    # Try induction if it's a universal quantifier
    if isinstance(simplified, list) and len(simplified) == 3 and simplified[0] == 'forall':
        var = simplified[1]
        inner_formula = simplified[2]
        return prove_by_induction(inner_formula, var)
    
    # Try to match against known implications
    for fact in axioms + theorems:
        if isinstance(fact, list) and len(fact) == 3 and fact[0] == 'implies':
            if prove(fact[1]):  # If we can prove the premise
                if fact[2] == simplified:  # And conclusion matches our goal
                    print(f"Proved using implication: {format_expr(fact)}")
                    return True
    
    print("Could not prove.")
    return False

def format_expr(expr):
    """Format expression for display"""
    if expr is True:
        return "True"
    if expr is False:
        return "False"
    if not isinstance(expr, list):
        return str(expr)
    
    if len(expr) == 0:
        return "[]"
    
    op = expr[0]
    
    if op == '0':
        return "0"
    elif op == 'su' and len(expr) == 2:
        return f"su({format_expr(expr[1])})"
    elif op == 'add' and len(expr) == 3:
        return f"add({format_expr(expr[1])}, {format_expr(expr[2])})"
    elif op == 'mul' and len(expr) == 3:
        return f"mul({format_expr(expr[1])}, {format_expr(expr[2])})"
    elif op == 'eq' and len(expr) == 3:
        return f"eq({format_expr(expr[1])}, {format_expr(expr[2])})"
    elif op == 'forall' and len(expr) == 3:
        return f"∀{expr[1]}.{format_expr(expr[2])}"
    elif op == 'implies' and len(expr) == 3:
        return f"({format_expr(expr[1])} → {format_expr(expr[2])})"
    else:
        return f"[{op}" + "".join(f", {format_expr(arg)}" for arg in expr[1:]) + "]"

def add_axiom(axiom):
    """Add an axiom to the knowledge base"""
    axioms.append(axiom)
    print(f"Added axiom: {format_expr(axiom)}")

def add_theorem(theorem):
    """Add a theorem to the knowledge base"""
    theorems.append(theorem)
    print(f"Added theorem: {format_expr(theorem)}")

# Setup basic Peano axioms
def setup_peano_axioms():
    """Initialize with basic Peano axioms"""
    # 0 is not the successor of any number
    add_axiom(['forall', 'x', ['not', ['eq', ['0'], ['su', 'x']]]])
    
    # Successor is injective
    add_axiom(['forall', 'x', ['forall', 'y', 
               ['implies', ['eq', ['su', 'x'], ['su', 'y']], ['eq', 'x', 'y']]]])
    
    # Addition axioms
    add_axiom(['forall', 'x', ['eq', ['add', ['0'], 'x'], 'x']])
    add_axiom(['forall', 'x', ['forall', 'y', 
               ['eq', ['add', ['su', 'x'], 'y'], ['su', ['add', 'x', 'y']]]]])
    
    # Multiplication axioms  
    add_axiom(['forall', 'x', ['eq', ['mul', ['0'], 'x'], ['0']]])
    add_axiom(['forall', 'x', ['forall', 'y',
               ['eq', ['mul', ['su', 'x'], 'y'], ['add', 'y', ['mul', 'x', 'y']]]]])

def demo():
    """Demonstrate the prover with some simple theorems"""
    print("=== Peano Arithmetic Prover Demo ===\n")
    
    setup_peano_axioms()
    
    print("\nAxioms loaded. Now testing some simple theorems:\n")
    
    # Test 1: 0 + 0 = 0
    print("Test 1: Proving 0 + 0 = 0")
    theorem1 = eq(add(zero(), zero()), zero())
    result1 = prove(theorem1)
    print(f"Result: {result1}\n")
    
    # Test 2: x + 0 = x (by induction)
    print("Test 2: Proving ∀x. x + 0 = x")
    theorem2 = forall('x', eq(add('x', zero()), 'x'))
    result2 = prove(theorem2)
    print(f"Result: {result2}\n")
    
    # Test 3: su(0) + su(0) = su(su(0))
    print("Test 3: Proving su(0) + su(0) = su(su(0))")
    one = su(zero())
    two = su(one)
    theorem3 = eq(add(one, one), two)
    result3 = prove(theorem3)
    print(f"Result: {result3}\n")

if __name__ == "__main__":
    demo()