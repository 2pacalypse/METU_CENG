import numpy as np
import torch
import os
import sys
from skimage import io, color
from skimage.transform import rescale
import numpy as np
import numpy as np
import torch
import torch.utils.data
import os
from skimage import io, color
from skimage.transform import rescale
import numpy as np
from tensorboard_logger import configure, log_value
import cv2


def read_image(filename):
    img = io.imread(filename)
    if len(img.shape) == 2:
        img = np.stack([img, img, img], 2)
    return img

def cvt2Lab(image):
    Lab = color.rgb2lab(image)
    return Lab[:, :, 0], Lab[:, :, 1:]  # L, ab


def cvt2rgb(image):
    return color.lab2rgb(image)


def upsample(image):
    return rescale(image, 4, mode='constant', order=3)
	
valPath = "test.txt"
val_vectors = np.load(os.getcwd() +  '/' + 'estimations' + '/' + '200.npy')

i = 0
with open(valPath) as file:
	for line in file:
		cv2.imwrite(os.getcwd() + '/' + 'estimations' + '/' + line.strip(), val_vectors[i])		
		i += 1
		
"""
args = sys.argv
# device = torch.device('cpu')

valPath = "test.txt"
valFolder = "test_gray"

val_vectors = []


try:
    val_vectors = np.load("test_vectors.npy")
    print(type(val_vectors))
except IOError:
	with open(valPath) as file:
		for line in file:
			rgbimg = read_image(valFolder + "/" + line.strip())
			labimg = cvt2Lab(rgbimg)
			val_vectors.append([labimg[0]])
			print("Gettvng test vectors from: " + line)
	np.save("test_vectors.npy", val_vectors)
	val_vectors = np.array(val_vectors)
	print(type(val_vectors))

val_loader = torch.utils.data.DataLoader(val_vectors, batch_size = 10)



model = torch.load(os.getcwd() + '/' + 'models' + '/'  + args[1])	
model.eval()
output = []

for i, l in enumerate(val_loader):
	y1 = model(l)
	temp = (y1.data.numpy())
	for e in temp:
		output.append(e)
	print(i)

	
FINAL = []
for i in range(100):
	print (i)
	l = val_vectors[i][0]
	a = upsample(output[i][0])
	b = upsample(output[i][1])
	
	img = []
	for j in range(256):
		for k in range(256):
			img.append(l[j][k])
			img.append(a[j][k])
			img.append(b[j][k])
	x = 255*((cvt2rgb(np.reshape(img, (256,256,3)))))
	FINAL.append(x)
	
np.save(os.getcwd() + '/' + 'estimations' + '/' +  args[1] + 'test', (FINAL))
"""