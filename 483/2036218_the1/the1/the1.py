import cv2 #opencv contrIB PIP INSTALL
import numpy as np
from sklearn.cluster import MiniBatchKMeans # PIP INSTALL SKLEARN
import matplotlib.pyplot as plt
import os
from six.moves import cPickle as pickle

def save_dict(di_, filename_):
    with open(filename_, 'wb') as f:
        pickle.dump(di_, f)

def load_dict(filename_):
    with open(filename_, 'rb') as f:
        ret_di = pickle.load(f)
    return ret_di
	


"""
img = cv2.imread(os.getcwd() + '\\dataset\\' + 'uiSypscOYh.jpg')
	
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
sift = cv2.xfeatures2d.SIFT_create()

kp = []

for i in range(15, gray.shape[0], 15):
	for j in range(15, gray.shape[1], 15):
		kp.append(cv2.KeyPoint(float(j), float(i), float(15)))

img=cv2.drawKeypoints(gray,kp,img, flags=cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
cv2.imwrite('drawn'  + '.jpg',img)
"""



K= 256
METHOD = "DENSESIFT"
STEP = 20

paths =[]
all_descriptors = []
descriptor_dict = {}



outputfilename = '_' + METHOD +'_' + str(K)
if(METHOD =="DENSESIFT"):
	outputfilename += '_' + str(STEP)

intermediate = os.getcwd() + '\\intermediates\\' 
intermediate2 = os.getcwd() + '\\intermediates\\' 

if(METHOD=="DENSESIFT"):
	intermediate += 'descriptor_dense_' + str(STEP) + '.npy'
	intermediate2 += 'dictionary_dense_' + str(STEP) + '.npy'
	
else:
	intermediate += 'descriptor_sift.npy'
	intermediate2 += 'dictionary_sift.npy'

	
with open('images.dat') as f:
	for i in range(1,1492):
		line = f.readline().strip('\n')
		paths.append(line)



try:
	all_descriptors = load_dict(intermediate)
	descriptor_dict = load_dict(intermediate2)
except IOError:
	for path in paths:
		img = cv2.imread(os.getcwd() + '\\dataset\\' + path)
	
		gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
		sift = cv2.xfeatures2d.SIFT_create()
	
	
		kp = []
	
		if(METHOD == "SIFT"):
			kp = sift.detect(gray,None)
			if not kp:
				kp_step = 10
				for i in range(kp_step, gray.shape[0], kp_step):
					for j in range(kp_step, gray.shape[1], kp_step):
						kp.append(cv2.KeyPoint(float(j), float(i), float(kp_step)))
						
		elif(METHOD == "DENSESIFT"):
			for i in range(STEP, gray.shape[0], STEP):
				for j in range(STEP, gray.shape[1], STEP):
					kp.append(cv2.KeyPoint(float(j), float(i), float(STEP)))
	
	
	
		kp, des = sift.compute(gray, kp)
		all_descriptors.append(des)
		descriptor_dict[path] = des
		print('READING: ' + path)
		

	all_descriptors = np.concatenate(all_descriptors)
	save_dict(all_descriptors, intermediate)
	save_dict(descriptor_dict, intermediate2)



	
kmeans = MiniBatchKMeans(n_clusters=K)	
labels = kmeans.fit_predict(all_descriptors)


for path in paths:
	d = descriptor_dict.get(path)
	d = labels[0: len(d)]
	descriptor_dict[path] = d
	labels = labels[len(d):]


for path in paths:
	d = descriptor_dict.get(path)
	d = np.bincount(d, minlength=K) / len(d)
	descriptor_dict[path] = d
	


	
val_paths =[]
with open('test_queries.dat') as f:
	for i in range(1,101):
		line = f.readline().strip('\n')
		val_paths.append(line)
	

line = ""
for path1 in val_paths:
	d1 = descriptor_dict.get(path1)
	line += path1 + ':'
	for path2 in paths:		
		d2 = descriptor_dict.get(path2)
		dist = np.linalg.norm(d1 - d2)
		line += ' ' + str(dist) + ' ' + path2
	line += '\n'



with open(outputfilename + '.out', 'a') as f:
    f.write(line)

