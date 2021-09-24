import numpy as np
import torch
import os
import sys

args = sys.argv
# /models/1layer/100

device = torch.device('cpu')

val_vectors = np.load('valid.npy')
val_x = torch.tensor(val_vectors, device=device)

val_real = np.load('valid_gt.npy')

model = torch.load(os.getcwd() + args[1] + '5000')	
model.eval()
y1 = model(val_x)
y1 = (y1.data.numpy().reshape(-1))
for i in range(len(y1)):
	if(abs(y1[i] - val_real[i]) < 1 and val_real[i] < 30):
		print(i, y1[i], val_real[i], abs(y1[i] - val_real[i]))
print(y1)
print(val_real)	
