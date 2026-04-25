class Array:
    """ Python matching implementation of the C++ Array class. """
    
    def __init__(self, size=0):
        self._length = size
        self._array = [0] * size

    def __len__(self):
        return self._length

    def size(self):
        return self._length

    def allocate(self, length):
        self._length = length
        self._array = [0] * length

    def clear(self):
        self._length = 0
        self._array = []

    def __getitem__(self, n):
        if not (0 <= n < self._length):
            raise IndexError("Array index out of range")
        return self._array[n]

    def __setitem__(self, n, val):
        if not (0 <= n < self._length):
            raise IndexError("Array index out of range")
        self._array[n] = val

    def at(self, n):
        return self.__getitem__(n)

    def copy(self):
        """ Returns a deep copy of the array. """
        new_arr = Array(self._length)
        for i in range(self._length):
            new_arr[i] = self._array[i]
        return new_arr
