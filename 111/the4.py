def isleaf(tree): #helper function
    return len(tree) == 1

def lowercase(DICT): #make dictionary values lower case
    return dict((k.lower()," ".join(v).lower().split()) for k,v in DICT.iteritems())

def parsehelper(tree):
    if not tree: #base case
        return []
    elif isinstance(tree[0], str): #skip unnecessary list items
        return parsehelper(tree[1:])
    else:
        if isleaf(tree[0]):
            return tree[0] + parsehelper(tree[1:]) #if it is a list containing only one item.
        else:
            return parsehelper(tree[0]) + parsehelper(tree[1:]) #if it is a list containing more than one item.
        
def parse(tree):
    if isleaf(tree): # the tree can be a leaf as well
        return [tree[0].lower()]
    else:
        return " ".join(parsehelper(tree)).lower().split() #but most of the time, it is not.
        
def is_valid(T,D,S):
    x = parse(T)
    S = S.lower().split()
    D = lowercase(D)
    if len(x) != len(S): #comparing equality in terms of length.
        return False
    for i in range(len(S)):
        if S[i] not in D[x[i]]: #comparing one to one
            return False
    return True

T = ["S", ["NP", ["Pronoun"]], ["VP", ["Verb"], ["NP", ["Det"], ["Noun"], ["Noun"]]]]
D = {"Pronoun":["I", "He", "She"], "Det":["the", "a", "an"], "Verb":["like", "love", "attend"], "Noun":["CEng111", "CEng100", "morning", "evening", "lecture", "breeze", "course"]}
print True == is_valid(T, D, "I like the CEng111 course")
print False == is_valid(T, D, "I like the CEng111")
print True == is_valid(T, D, "I like the course course")
print False == is_valid(T, D, "I like the course course course")
print False == is_valid(T, D, "I like the CEng111 course CEng100")
print True == is_valid(T, D, "she like the CEng111 course")
print False == is_valid(T, D, "she likes the CENG111 course")