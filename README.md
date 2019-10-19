# Prisoner boxes riddle

The program has been written in order practice multi-threading and to verify this video:

https://www.youtube.com/watch?v=vIdStMTgNl0&t=2s

The way it works:

1) Fill a container ( a vector in this case ), with numbers respective to their container:
Number 1 in container 1, 10 in 10 and so on.

2) Scramble the contents of the containers: 1 in container 10, 5 in container 1, etc.

3) Check using the method presented in the video to see if all the numbers are found.

4) Check the same container again but using a randomness method.

# Explanation of the methods:

# General:
Every person has a number associated to them, person 1 has number 1, 2 has number 2, and so on. The numbers are put randomly in different containers, each with a number on them, but the number does not necessarily mean the same number is inside the container.

The point is for ALL the people to find their number. The number of guesses allowed for each person is of (total number of people)/2.
If all the people did not find their number, it is counted as failure.

Methods:

# Video

Every person starts at the box with their number, if their number is not inside. Go look inside the box from where that number is. So if number 5 is in box 1, go look in box 5, then repeat until either the number is found or the maximum number of boxes.

# Randomness

A path is created for every person by randomly generating a set of non-repeating numbers. This is a very resource intensive operation!
