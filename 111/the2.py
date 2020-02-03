##################################################
## Areas of rectangles and triangles have been calculated by "Shoelace formula."
## For each CM positions, "total moment/total mass" formula has been used. Holes were treated negative in total mass."
## The fixed reference point of moment is (0,0).
##################################################

def triangle(s,x1,y1,x2,y2,x3,y3): 
    mtri = s*abs(0.5*(x1*y2+x2*y3+x3*y1-x2*y1-x3*y2-x1*y3))# mass
    cmtri = [(x1+x2+x3)/3.0, (y1+y2+y3)/3.0]# center of mass --> [x,y]
    effect = [mtri*cmtri[0], mtri*cmtri[1]]# moment --> [x,y]
    return [mtri, effect]

def rectangle(s,x1,y1,x2,y2,x3,y3,x4,y4):
    mrec = s*abs(1.0*(x1*y2+x2*y3+x3*y1-x2*y1-x3*y2-x1*y3))
    cmrec = [(x1+x2+x3+x4)/4.0, (y1+y2+y3+y4)/4.0]
    effect = [mrec*cmrec[0], mrec*cmrec[1]]
    return [mrec, effect]

def circle(s,x1,y1,r):
    mcirc = s*3.1415926535897932*(r)**2
    effect = [mcirc*x1, mcirc*y1]
    return [mcirc, effect]

def iterateandcalculate(LIST,i, mass, effx, effy): #last three arguments are here not to use any global variables.
    if i < len(LIST):# base case
        length = len(LIST[i])
        if length == 7:
            x = triangle(*LIST[i])  ### http://stackoverflow.com/questions/4979542/python-use-list-as-function-parameters
            mass += x[0]
            effx += x[1][0]
            effy += x[1][1]
        elif length == 9:
            x = rectangle(*LIST[i])  ### https://docs.python.org/2/tutorial/controlflow.html#unpacking-argument-lists
            mass += x[0]
            effx += x[1][0]
            effy += x[1][1]
        else:
            x = circle(*LIST[i])
            mass += x[0]
            effx += x[1][0]
            effy += x[1][1]
        return iterateandcalculate(LIST, i+1, mass, effx, effy) #recursion
    else:
        return [effx/mass, effy/mass, mass]
        
def cm(L):
    return iterateandcalculate(L, 0, 0, 0, 0)