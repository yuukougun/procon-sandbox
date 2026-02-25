import sys
import os

# Ensure the library directory is in the path
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from library.library import Array

def test_constructors():
    print("Testing constructors...")
    
    # Default constructor
    a1 = Array()
    assert len(a1) == 0
    
    # Parameterized constructor
    a2 = Array(10)
    assert len(a2) == 10
    for i in range(10):
        a2[i] = i * 2
        
    # Copy concept (explicit copy in Python)
    a3 = a2.copy()
    assert len(a3) == 10
    for i in range(10):
        assert a3[i] == i * 2
        
    # Verify deep copy
    a3[0] = 100
    assert a2[0] == 0
    
    print("Constructors test passed!")

def test_access():
    print("Testing element access...")
    
    a = Array(5)
    for i in range(5):
        a[i] = i * 10
        
    for i in range(5):
        assert a[i] == i * 10
        assert a.at(i) == i * 10
        
    print("Element access test passed!")

def test_assignment():
    print("Testing assignment (copy concept)...")
    
    a1 = Array(5)
    for i in range(5):
        a1[i] = i
        
    # Python assignment is by reference, so we use .copy() for "copy assignment" parity
    a2 = a1.copy()
    assert len(a2) == 5
    for i in range(5):
        assert a2[i] == i
        
    a2[0] = 99
    assert a1[0] == 0
    
    print("Assignment test passed!")

def test_memory_management():
    print("Testing memory management...")
    
    a = Array(10)
    assert len(a) == 10
    
    a.allocate(20)
    assert len(a) == 20
    
    a.clear()
    assert len(a) == 0
    
    a.allocate(5)
    assert len(a) == 5
    
    print("Memory management test passed!")

if __name__ == "__main__":
    try:
        test_constructors()
        test_access()
        test_assignment()
        test_memory_management()
        print("\nAll Python tests passed successfully!")
    except AssertionError as e:
        print(f"Test failed: Assertion error")
        sys.exit(1)
    except Exception as e:
        print(f"Test failed with exception: {e}")
        sys.exit(1)
