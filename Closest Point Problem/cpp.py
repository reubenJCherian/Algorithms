#Program will take a file with a set of coordinates that can be plotted on the Cartesian plane and determine the distance between the closest points.
#Output will be a real number rounded to the nearest thousandth

import math
import re
import ast

#Finds the distance between two points, inputted as a tuple or list
def dist(p1, p2):
    val = math.sqrt((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2)
    return val

#Brute forces to find the distance between the closest points, serves as the base case for the upcoming recursive function
def bruteForce(points): 
    min_dist = 100000000000000

    #Tests for edge cases
    if len(points) < 2: 
        return min_dist
    
    #Iterates through the points and finds the smallest distance between the points and stores it in a variable 'min_dist'
    for i in range(len(points)):
        for j in range(i+1, len(points)):
            d = dist(points[i], points[j])
            if d < min_dist: 
                min_dist = d

    #Returns the distance between the closest points
    return min_dist

#Function that recursively searches for the smallest distance between points
def cpp(Px, Py): 
    n = len(Px)

    #Base Case: Uses the bruteForce function to find the distance between the closest pair of points
    if n <= 3: 
        return bruteForce(Px)
    
    #Recursive Step: 

    #Step 1: Divide the points into left and right
    mid = n // 2
    PxL = Px[:mid]
    PxR = Px[mid:]

    medianX = PxL[-1][0]

    PyL = []
    PyR = []
    for p in Py:
        #Check the x-coordinate (p[0]) against the median to sort the list into left and right in sorted order by y-coordinates
        if p[0] <= medianX:
            PyL.append(p)
        else:
            PyR.append(p)

    #Step 2: Using recursion to find th eminimum distance in both sides
    distL = cpp(PxL, PyL)
    distR = cpp(PxR, PyR)

    #Finds the current minimum distance before accounting for the pairs within the median line
    minD = min(distL, distR)

    #Step 3: Searches within the median line to ensure that the minimum distance found was in fact the minimum distance
    #Creates a list of pairs that are within minD distance of the median line
    strip = []
    for p in Py: 
        if abs(p[0] - medianX) < minD: 
            strip.append(p)
    
    #Finds the smallest distance between the points closest to the median line
    for i in range(len(strip)):
        for j in range(i+1, min(i+8, len(strip))): 
            d = dist(strip[i], strip[j])
            minD = min(minD, d)
    
    return minD

#Function that handles all functions related to finding the pair of the closest points
def closestPoint(points): 
    #Converts everything to floats
    pointsNormalized = [(float(p[0]), float(p[1])) for p in points if len(p) == 2]

    n = len(pointsNormalized)
    if n < 2: 
        return 0.000
    
    #Sorts the list by both x and y-coordinates
    Px = sorted(pointsNormalized, key=lambda p: p[0])
    Py = sorted(pointsNormalized, key=lambda p: p[1])

    minD = cpp(Px, Py)
    return minD

#Function will read the file with the sets of numbers and convert it into a set of tuples
def readFile(filename):
    #Reads file
    with open(filename, 'r') as f: 
        fileString = f.read()

    #Removes all whitespace
    cleanString = fileString.replace(' ', '').replace('\n', '')

    #Looks for {1,2} pattern and replaces it with (1,2)
    cleanString = re.sub(r'\{([\d\s\.,-]+)\}', r'(\1)', cleanString)

    points = ast.literal_eval(cleanString)
    pointsList = list(points)

    return pointsList

#Function handles the rest of the other functions
def main(fileName):
    points = readFile(fileName)
    min_D = closestPoint(points)
    rounded = round(min_D, 3)

    return rounded

print(main(r"1.txt"))
print(main(r"2.txt"))
print(main(r"3.txt"))
print(main(r"4.txt"))
print(main(r"5.txt"))
print(main(r"6.txt"))
print(main(r"7.txt"))
print(main(r"8.txt"))
print(main(r"9.txt"))
print(main(r"10-3.txt"))




